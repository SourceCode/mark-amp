# Phase 23 -- Attribute View Relations & Rollups

**Priority:** High (most complex AV feature)
**Estimated Scope:** ~8 new files, ~6 modified files
**Dependencies:** Phase 17 (Data Model), Phase 18 (Column Types), Phase 19 (Table Renderer)

## Objective

Implement the Relation and Rollup column types -- the most architecturally complex features in the Attribute View system. Relations link rows across different attribute views (or within the same AV), creating a graph of connections between records. Rollups aggregate values from related rows using configurable calculations (sum, average, count, min, max, etc.).

Key capabilities:
- **Relations:** A Relation column in AV-A links to one or more rows in AV-B. The relation stores a list of block IDs pointing to the target rows. The cell displays the target rows' titles as clickable chips.
- **Two-way Relations:** When creating a relation from AV-A to AV-B, optionally create a reciprocal "back-relation" column in AV-B that automatically links back to the AV-A row. When AV-A row X links to AV-B row Y, AV-B row Y's back-relation column automatically shows AV-A row X.
- **Rollups:** A Rollup column references a Relation column and a target field in the related AV. It computes an aggregate over the target field values of all related rows. Supports 20 calculation types (Sum, Average, Count, Min, Max, Median, Range, date aggregations, checkbox aggregations, etc.).

## Prerequisites

- **Phase 17** -- `AttributeView`, `AVKey`, `AVValue`, `AVRelationConfig`, `AVRollupConfig`, `AVRollupCalc`, `AVValueRelation`, `AVValueRollup`, `AttributeViewStore`
- **Phase 18** -- `AVColumnTypeRegistry`, `RelationColumnHandler`, `RollupColumnHandler`
- **Phase 19** -- `AVTablePanel`, `AVCellRenderer`, `RelationCellRenderer`, `RollupCellRenderer`

## SiYuan Source Reference

| SiYuan File | Purpose | Key Structures |
|---|---|---|
| `kernel/av/relation.go` | Relation resolution, two-way sync | Relation types, block ID resolution |
| `kernel/av/rollup.go` | Rollup calculation dispatch | Rollup calc functions per type |
| `kernel/model/attribute_view.go` | CRUD for relations/rollups, two-way management | `addAttributeViewRelation`, `removeAttributeViewRelation`, rollup recalculation |
| `kernel/av/value.go` | `ValueRelation`, `ValueRollup` structures | Relation content, rollup content |

## MarkAmp Integration Points

### New Files to Create

| File | Namespace | Purpose |
|---|---|---|
| `src/core/av/AVRelationService.h` | `markamp::core::av` | Relation CRUD, two-way sync, resolution |
| `src/core/av/AVRelationService.cpp` | `markamp::core::av` | Relation service implementation |
| `src/core/av/AVRollupEngine.h` | `markamp::core::av` | Rollup calculation engine |
| `src/core/av/AVRollupEngine.cpp` | `markamp::core::av` | All 20 rollup calculations |
| `src/ui/av/AVRelationEditor.h` | `markamp::ui::av` | Relation cell editor (block picker popup) |
| `src/ui/av/AVRelationEditor.cpp` | `markamp::ui::av` | Block search, selection, chip display |
| `src/ui/av/AVRollupConfigDialog.h` | `markamp::ui::av` | Rollup column config dialog |
| `src/ui/av/AVRollupConfigDialog.cpp` | `markamp::ui::av` | Relation picker, target field picker, calc picker |
| `tests/unit/test_av_relations_rollups.cpp` | (anonymous) | Catch2 tests |

### Files to Modify

| File | Change |
|---|---|
| `src/core/av/AVColumnType.cpp` | Enhance `RelationColumnHandler` and `RollupColumnHandler` |
| `src/ui/av/AVCellRenderer.cpp` | Enhance `RelationCellRenderer` and `RollupCellRenderer` |
| `src/ui/av/AVCellEditor.cpp` | Register `AVRelationEditor` for Relation type |
| `src/core/Events.h` | Add relation/rollup events |
| `src/CMakeLists.txt` | Add new `.cpp` sources |
| `tests/CMakeLists.txt` | Add `test_av_relations_rollups` test target |

## Data Structures to Implement

### File: `src/core/av/AVRelationService.h`

```cpp
#pragma once

#include "AttributeView.h"
#include "AttributeViewStore.h"
#include "AVTypes.h"
#include "AVValue.h"

#include <expected>
#include <string>
#include <vector>

namespace markamp::core::av
{

// ============================================================================
// Resolved Relation: a relation with full content for display
// ============================================================================

struct AVResolvedRelation
{
    std::string source_av_id;
    std::string source_block_id;
    std::string target_av_id;
    std::string target_block_id;
    std::string target_block_content;  // Denormalized title for display
};

// ============================================================================
// Relation Service
// Manages cross-AV relations including two-way sync
// ============================================================================

class AVRelationService
{
public:
    explicit AVRelationService(AttributeViewStore* store);

    // ── Single relation operations ──

    /// Add a relation link from source row to target row.
    /// If the relation key is two-way, also creates the back-link.
    [[nodiscard]] auto set_relation(
        const std::string& source_av_id,
        const std::string& source_block_id,
        const std::string& relation_key_id,
        const std::vector<std::string>& target_block_ids)
        -> std::expected<void, std::string>;

    /// Add a single target to an existing relation (append).
    [[nodiscard]] auto add_relation_target(
        const std::string& source_av_id,
        const std::string& source_block_id,
        const std::string& relation_key_id,
        const std::string& target_block_id)
        -> std::expected<void, std::string>;

    /// Remove a single target from a relation.
    [[nodiscard]] auto remove_relation_target(
        const std::string& source_av_id,
        const std::string& source_block_id,
        const std::string& relation_key_id,
        const std::string& target_block_id)
        -> std::expected<void, std::string>;

    // ── Query operations ──

    /// Get all rows in target_av that are linked from source row via relation_key.
    [[nodiscard]] auto get_related_rows(
        const std::string& source_av_id,
        const std::string& source_block_id,
        const std::string& relation_key_id) const
        -> std::expected<std::vector<AVResolvedRelation>, std::string>;

    /// Get all rows in any AV that link TO a specific block (reverse lookup).
    [[nodiscard]] auto get_incoming_relations(
        const std::string& target_av_id,
        const std::string& target_block_id) const
        -> std::expected<std::vector<AVResolvedRelation>, std::string>;

    /// Resolve relation block IDs to their content strings (denormalization).
    [[nodiscard]] auto resolve_relation_contents(
        const std::string& target_av_id,
        const std::vector<std::string>& target_block_ids) const
        -> std::expected<std::vector<AVRelationContent>, std::string>;

    // ── Two-way relation management ──

    /// Create a two-way relation between two AVs.
    /// Creates a Relation key in source AV pointing to target AV,
    /// and a back-Relation key in target AV pointing to source AV.
    /// Links them via back_key_id references.
    [[nodiscard]] auto create_two_way_relation(
        const std::string& source_av_id,
        const std::string& source_key_name,
        const std::string& target_av_id,
        const std::string& target_key_name)
        -> std::expected<std::pair<std::string, std::string>, std::string>;
        // Returns (source_key_id, target_back_key_id)

    /// Break a two-way relation (convert to one-way or remove back-relation).
    [[nodiscard]] auto break_two_way_relation(
        const std::string& av_id,
        const std::string& relation_key_id)
        -> std::expected<void, std::string>;

    // ── Sync operations ──

    /// Synchronize back-relations after a forward relation change.
    /// Called automatically by set_relation/add/remove when is_two_way is true.
    [[nodiscard]] auto sync_back_relations(
        const std::string& source_av_id,
        const std::string& source_block_id,
        const std::string& relation_key_id,
        const std::vector<std::string>& old_target_ids,
        const std::vector<std::string>& new_target_ids)
        -> std::expected<void, std::string>;

    // ── Validation ──

    /// Check for circular relation chains (A -> B -> C -> A).
    /// Returns true if adding the proposed link would create a cycle.
    [[nodiscard]] auto would_create_cycle(
        const std::string& source_av_id,
        const std::string& target_av_id,
        int max_depth = 10) const -> bool;

private:
    AttributeViewStore* store_{nullptr};

    /// Internal: get the Key for a relation column, loading the AV if needed.
    [[nodiscard]] auto get_relation_key(
        const std::string& av_id,
        const std::string& key_id)
        -> std::expected<std::pair<AttributeView*, AVKey*>, std::string>;
};

} // namespace markamp::core::av
```

### File: `src/core/av/AVRollupEngine.h`

```cpp
#pragma once

#include "AVRelationService.h"
#include "AVTypes.h"
#include "AVValue.h"
#include "AttributeView.h"
#include "AttributeViewStore.h"

#include <expected>
#include <string>
#include <vector>

namespace markamp::core::av
{

// ============================================================================
// Rollup Result
// ============================================================================

struct AVRollupResult
{
    AVRollupCalc calc{AVRollupCalc::None};
    AVValueVariant computed_value;       // The aggregated result
    std::string display_string;          // Formatted result for display
    int source_value_count{0};           // Number of values aggregated
};

// ============================================================================
// Rollup Engine
// Computes rollup aggregations over related row values
// ============================================================================

class AVRollupEngine
{
public:
    AVRollupEngine(AttributeViewStore* store,
                   AVRelationService* relation_service);

    /// Calculate a rollup for a specific row.
    ///
    /// 1. Follow the relation_key to get related block_ids
    /// 2. In the target AV, get the target_key values for those blocks
    /// 3. Apply the calc aggregation function
    /// 4. Return the result
    [[nodiscard]] auto calculate(
        const std::string& source_av_id,
        const std::string& source_block_id,
        const AVRollupConfig& config) const
        -> std::expected<AVRollupResult, std::string>;

    /// Recalculate all rollup values in an AV (batch operation).
    /// Call after data changes that might affect rollup results.
    [[nodiscard]] auto recalculate_all(const std::string& av_id)
        -> std::expected<int, std::string>;
        // Returns count of rollup values updated

    /// Recalculate rollups for a specific row.
    [[nodiscard]] auto recalculate_row(
        const std::string& av_id,
        const std::string& block_id)
        -> std::expected<int, std::string>;

    // ── Individual aggregation functions ──
    // Each operates on a vector of AVValues (the related rows' target field values)

    [[nodiscard]] static auto calc_count_all(
        const std::vector<AVValue>& values) -> AVRollupResult;

    [[nodiscard]] static auto calc_count_values(
        const std::vector<AVValue>& values) -> AVRollupResult;

    [[nodiscard]] static auto calc_count_unique(
        const std::vector<AVValue>& values) -> AVRollupResult;

    [[nodiscard]] static auto calc_count_empty(
        const std::vector<AVValue>& values) -> AVRollupResult;

    [[nodiscard]] static auto calc_count_not_empty(
        const std::vector<AVValue>& values) -> AVRollupResult;

    [[nodiscard]] static auto calc_percent_empty(
        const std::vector<AVValue>& values) -> AVRollupResult;

    [[nodiscard]] static auto calc_percent_not_empty(
        const std::vector<AVValue>& values) -> AVRollupResult;

    [[nodiscard]] static auto calc_sum(
        const std::vector<AVValue>& values) -> AVRollupResult;

    [[nodiscard]] static auto calc_average(
        const std::vector<AVValue>& values) -> AVRollupResult;

    [[nodiscard]] static auto calc_median(
        const std::vector<AVValue>& values) -> AVRollupResult;

    [[nodiscard]] static auto calc_min(
        const std::vector<AVValue>& values) -> AVRollupResult;

    [[nodiscard]] static auto calc_max(
        const std::vector<AVValue>& values) -> AVRollupResult;

    [[nodiscard]] static auto calc_range(
        const std::vector<AVValue>& values) -> AVRollupResult;

    [[nodiscard]] static auto calc_date_earliest(
        const std::vector<AVValue>& values) -> AVRollupResult;

    [[nodiscard]] static auto calc_date_latest(
        const std::vector<AVValue>& values) -> AVRollupResult;

    [[nodiscard]] static auto calc_checked(
        const std::vector<AVValue>& values) -> AVRollupResult;

    [[nodiscard]] static auto calc_unchecked(
        const std::vector<AVValue>& values) -> AVRollupResult;

    [[nodiscard]] static auto calc_percent_checked(
        const std::vector<AVValue>& values) -> AVRollupResult;

    [[nodiscard]] static auto calc_percent_unchecked(
        const std::vector<AVValue>& values) -> AVRollupResult;

    /// Dispatch to the correct calc function based on AVRollupCalc enum.
    [[nodiscard]] static auto dispatch_calc(
        AVRollupCalc calc,
        const std::vector<AVValue>& values) -> AVRollupResult;

private:
    AttributeViewStore* store_{nullptr};
    AVRelationService* relation_service_{nullptr};

    /// Extract numeric values from a vector of AVValues (ignoring non-numeric).
    [[nodiscard]] static auto extract_numbers(
        const std::vector<AVValue>& values) -> std::vector<double>;

    /// Extract timestamps from a vector of AVValues (ignoring non-date).
    [[nodiscard]] static auto extract_timestamps(
        const std::vector<AVValue>& values) -> std::vector<int64_t>;

    /// Extract checkbox states from a vector of AVValues.
    [[nodiscard]] static auto extract_checkboxes(
        const std::vector<AVValue>& values) -> std::vector<bool>;
};

} // namespace markamp::core::av
```

### File: `src/ui/av/AVRelationEditor.h`

```cpp
#pragma once

#include "AVCellEditor.h"
#include "core/av/AVRelationService.h"
#include "core/av/AVTypes.h"
#include "core/av/AVValue.h"
#include "core/av/AttributeViewStore.h"

#include <string>
#include <vector>
#include <wx/panel.h>
#include <wx/textctrl.h>

namespace markamp::ui::av
{

// ============================================================================
// Relation Editor: popup for selecting related blocks
// ============================================================================

class AVRelationEditor : public IAVCellEditor
{
public:
    explicit AVRelationEditor(core::av::AttributeViewStore* store,
                              core::av::AVRelationService* relation_service);

    void begin_edit(wxWindow* parent,
                    const wxRect& cell_rect,
                    const core::av::AVValue& current_value,
                    const core::av::AVKey& key,
                    AVCellEditCallback on_commit,
                    AVCellCancelCallback on_cancel) override;
    void commit() override;
    void cancel() override;
    [[nodiscard]] auto is_active() const -> bool override;

private:
    void on_search_text_changed(wxCommandEvent& event);
    void on_item_selected(int index);
    void on_item_deselected(int index);
    void rebuild_suggestions(const std::string& search_text);
    void rebuild_selected_chips();

    core::av::AttributeViewStore* store_{nullptr};
    core::av::AVRelationService* relation_service_{nullptr};

    wxPanel* popup_{nullptr};
    wxTextCtrl* search_input_{nullptr};
    wxPanel* suggestions_panel_{nullptr};
    wxPanel* selected_chips_panel_{nullptr};

    AVCellEditCallback on_commit_;
    AVCellCancelCallback on_cancel_;

    std::string target_av_id_;
    std::vector<std::string> selected_block_ids_;
    std::vector<std::pair<std::string, std::string>> suggestions_; // (block_id, content)
};

} // namespace markamp::ui::av
```

### File: `src/ui/av/AVRollupConfigDialog.h`

```cpp
#pragma once

#include "core/av/AVTypes.h"

#include <functional>
#include <string>
#include <vector>
#include <wx/dialog.h>

namespace markamp::ui::av
{

// ============================================================================
// Rollup Column Configuration Dialog
// ============================================================================

struct AVRollupConfigState
{
    std::string relation_key_id;       // Which Relation column to follow
    std::string target_key_id;         // Which field in the related AV to aggregate
    core::av::AVRollupCalc calc{core::av::AVRollupCalc::None};
};

using AVRollupConfigCallback = std::function<void(const AVRollupConfigState& config)>;

class AVRollupConfigDialog : public wxDialog
{
public:
    AVRollupConfigDialog(
        wxWindow* parent,
        const AVRollupConfigState& current_state,
        const std::vector<core::av::AVKey>& relation_keys,    // Available relation columns
        const std::vector<core::av::AVKey>& target_keys,      // Columns in the target AV
        AVRollupConfigCallback on_apply);

    [[nodiscard]] auto get_state() const -> AVRollupConfigState;

private:
    void on_apply(wxCommandEvent& event);
    void on_cancel(wxCommandEvent& event);
    void on_relation_changed(wxCommandEvent& event);
    void build_ui();
    void update_target_keys();
    void update_calc_options();

    AVRollupConfigState state_;
    std::vector<core::av::AVKey> relation_keys_;
    std::vector<core::av::AVKey> target_keys_;
    AVRollupConfigCallback on_apply_callback_;

    wxChoice* relation_choice_{nullptr};
    wxChoice* target_field_choice_{nullptr};
    wxChoice* calc_choice_{nullptr};
};

} // namespace markamp::ui::av
```

## Key Functions to Implement

### AVRelationService.cpp

1. **`set_relation(source_av_id, source_block_id, relation_key_id, target_block_ids)`** -- Load source AV. Find the relation key. Get old target IDs. Set the new relation value with block IDs and resolved content. If `key.relation_config.is_two_way`, call `sync_back_relations()`. Save source AV.

2. **`add_relation_target(source_av_id, source_block_id, relation_key_id, target_block_id)`** -- Load source AV. Get current relation value. Append target_block_id (if not already present). Resolve content. If two-way, add back-link in target AV. Save.

3. **`remove_relation_target(source_av_id, source_block_id, relation_key_id, target_block_id)`** -- Load source AV. Get current relation value. Remove target_block_id. If two-way, remove back-link from target AV. Save.

4. **`sync_back_relations(source_av_id, source_block_id, relation_key_id, old_targets, new_targets)`** -- Compute added = new_targets - old_targets. Compute removed = old_targets - new_targets. For each added target: load target AV, find back-relation key, add source_block_id to that row's back-relation. For each removed target: remove source_block_id from back-relation. Save target AV.

5. **`create_two_way_relation(source_av_id, source_key_name, target_av_id, target_key_name)`** -- Load both AVs. Create a Relation key in source with `AVRelationConfig{target_av_id, true, ""}`. Create a Relation key in target with `AVRelationConfig{source_av_id, true, ""}`. Set `back_key_id` on each key to point to the other. Save both.

6. **`break_two_way_relation(av_id, relation_key_id)`** -- Load source AV. Get relation config. Load target AV. Remove back-relation key from target. Set `is_two_way = false` and clear `back_key_id` on source key. Save both.

7. **`resolve_relation_contents(target_av_id, target_block_ids)`** -- Load target AV. For each block_id, find the Block column value and extract content text. Return vector of `AVRelationContent{block_id, content}`.

8. **`would_create_cycle(source_av_id, target_av_id, max_depth)`** -- BFS/DFS from target_av_id following all relation keys. If source_av_id is reachable within max_depth hops, return true (cycle detected).

### AVRollupEngine.cpp

9. **`calculate(source_av_id, source_block_id, config)`** -- Load source AV. Get the relation value for `config.relation_key_id` on the source row. Extract target block IDs. Load target AV (from `relation_config.dest_av_id`). For each target block_id, get the value at `config.target_key_id`. Collect into a vector. Call `dispatch_calc(config.calc, values)`. Return result.

10. **`dispatch_calc(calc, values)`** -- Switch on `AVRollupCalc` enum. Call the appropriate `calc_*()` static method. Return its result.

11. **`calc_sum(values)`** -- Call `extract_numbers()`. Sum all values. Return `AVRollupResult{Sum, AVValueNumber{sum, true, ...}, to_string(sum), count}`.

12. **`calc_average(values)`** -- Call `extract_numbers()`. Compute mean. Return formatted result.

13. **`calc_median(values)`** -- Call `extract_numbers()`. Sort. Return middle value (or average of two middle values for even count).

14. **`calc_min(values)` / `calc_max(values)`** -- Call `extract_numbers()`. Return min/max element.

15. **`calc_range(values)`** -- Return max - min.

16. **`calc_date_earliest(values)` / `calc_date_latest(values)`** -- Call `extract_timestamps()`. Return min/max timestamp.

17. **`calc_checked(values)` / `calc_unchecked(values)`** -- Call `extract_checkboxes()`. Count true/false values.

18. **`calc_percent_checked(values)` / `calc_percent_unchecked(values)`** -- Compute `checked_count / total_count * 100`.

19. **`calc_count_unique(values)`** -- Convert values to strings. Use a `std::unordered_set` to count unique non-empty values.

20. **`recalculate_all(av_id)`** -- Load AV. For each Rollup key, for each row, call `calculate()` and update the value. Save AV. Return count of values updated.

### AVRelationEditor.cpp

21. **`begin_edit()`** -- Create popup panel below cell. Show search input at top. Show currently selected block chips. Show suggestion list below search. Load target AV rows as initial suggestions.

22. **`rebuild_suggestions(search_text)`** -- Load target AV. Filter rows whose Block content contains `search_text` (case-insensitive). Exclude already-selected block IDs. Display matching rows in suggestion list.

23. **`on_item_selected(index)`** -- Add the selected block_id to `selected_block_ids_`. Add a chip to the selected panel. Remove from suggestions.

24. **`commit()`** -- Build `AVValueRelation` from `selected_block_ids_` and resolved contents. Call `on_commit_()`.

## Events to Add

```cpp
// ============================================================================
// Attribute View relation/rollup events (Phase 23)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVRelationChangedEvent)
std::string source_av_id;
std::string source_block_id;
std::string relation_key_id;
int target_count;                  // Number of related blocks
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVTwoWayRelationCreatedEvent)
std::string source_av_id;
std::string source_key_id;
std::string target_av_id;
std::string target_key_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVRollupRecalculatedEvent)
std::string av_id;
int values_updated;
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|---|---|---|---|
| `av.relation.max_targets` | int | `100` | Maximum number of related blocks per relation cell |
| `av.relation.cycle_detection_depth` | int | `10` | Max depth for cycle detection in relation graphs |
| `av.relation.auto_resolve_content` | bool | `true` | Auto-resolve block content for relation display |
| `av.rollup.auto_recalculate` | bool | `true` | Auto-recalculate rollups when source data changes |

## Test Cases

File: `tests/unit/test_av_relations_rollups.cpp`

All tests use Catch2 with `[av][relations_rollups]` tags.

1. **Create one-way relation** -- Create AV-A and AV-B. Add a Relation key in AV-A pointing to AV-B. Add a row to AV-A and set the relation to point to 2 rows in AV-B. Verify `get_related_rows()` returns 2 results with correct block IDs and content.

2. **Two-way relation auto-creates back column** -- Call `create_two_way_relation(av_a_id, "Projects", av_b_id, "Tasks")`. Verify AV-A has a Relation key "Projects" with `is_two_way=true`. Verify AV-B has a Relation key "Tasks" with `is_two_way=true` and `back_key_id` pointing to AV-A's key.

3. **Two-way relation sync on add** -- Create two-way relation between AV-A and AV-B. In AV-A, set row-1's relation to [AV-B row-X]. Verify AV-B row-X's back-relation column now contains AV-A row-1.

4. **Two-way relation sync on remove** -- From test 3 state, remove AV-B row-X from AV-A row-1's relation. Verify AV-B row-X's back-relation is now empty.

5. **Rollup: Sum of related numbers** -- AV-A has a Relation to AV-B and a Rollup (Sum) of AV-B's "Score" column. AV-A row links to AV-B rows with scores [10, 20, 30]. Calculate rollup. Verify result is 60.

6. **Rollup: Count of related rows** -- Same setup but with `CountAll` calc. Verify result is 3.

7. **Rollup: Average** -- Scores [10, 20, 30]. Average calc. Verify result is 20.0.

8. **Rollup: DateEarliest** -- AV-B has a Date column. Related rows have dates [Jan 15, Mar 1, Feb 10]. DateEarliest calc. Verify result is Jan 15.

9. **Relation across AVs: content resolution** -- Create relation from AV-A to AV-B. AV-B rows have block content ["Project Alpha", "Project Beta"]. Resolve contents. Verify returned contents match.

10. **Circular relation detection** -- AV-A relates to AV-B, AV-B relates to AV-C. Check `would_create_cycle(AV-C, AV-A)`. Verify returns true (would create A->B->C->A cycle). Check `would_create_cycle(AV-A, AV-D)`. Verify returns false.

11. **Two-way relation delete cascade** -- Create two-way relation. Break it via `break_two_way_relation()`. Verify the back-relation key is removed from the target AV. Verify the source key's `is_two_way` is now false.

12. **Rollup recalculation on data change** -- Set up rollup Sum. Initial related scores [10, 20]. Rollup = 30. Change one related score from 20 to 50. Call `recalculate_row()`. Verify rollup is now 60.

13. **Rollup on empty relation** -- Row has no related blocks. All rollup calcs should return sensible defaults: Sum=0, Average=0, Count=0, DateEarliest=empty, Checked=0, PercentChecked=0%.

14. **Large dataset rollup performance** -- Create AV-A with 100 rows, each relating to 50 rows in AV-B. AV-B has 5000 rows total. Calculate rollups for all 100 rows. Verify completes in under 2 seconds on debug builds.

## Acceptance Criteria

- [ ] Relation cells display target block titles as clickable chips
- [ ] Setting a relation value correctly updates the `AVValueRelation` variant
- [ ] Two-way relations auto-create back-relation columns in target AV
- [ ] Two-way relation sync: adding/removing a forward link updates the back-link
- [ ] Breaking a two-way relation removes the back column and clears `is_two_way`
- [ ] Circular relation detection prevents infinite loops
- [ ] All 20 rollup calculations produce correct results
- [ ] Rollup recalculation triggers when source data changes (if auto_recalculate enabled)
- [ ] Relation editor popup allows searching and selecting target rows
- [ ] Rollup config dialog allows choosing relation, target field, and calculation
- [ ] All 14 test cases pass
- [ ] No use of `catch(...)` -- all exceptions typed
- [ ] All query methods have `[[nodiscard]]`

## Files to Create

- `/Users/ryanrentfro/code/markamp/src/core/av/AVRelationService.h`
- `/Users/ryanrentfro/code/markamp/src/core/av/AVRelationService.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/av/AVRollupEngine.h`
- `/Users/ryanrentfro/code/markamp/src/core/av/AVRollupEngine.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVRelationEditor.h`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVRelationEditor.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVRollupConfigDialog.h`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVRollupConfigDialog.cpp`
- `/Users/ryanrentfro/code/markamp/tests/unit/test_av_relations_rollups.cpp`

## Files to Modify

- `/Users/ryanrentfro/code/markamp/src/core/av/AVColumnType.cpp` -- Enhance `RelationColumnHandler` and `RollupColumnHandler` with full logic
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVCellRenderer.cpp` -- Enhance `RelationCellRenderer` (clickable chips) and `RollupCellRenderer` (formatted aggregate display)
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVCellEditor.cpp` -- Register `AVRelationEditor` in `create_cell_editor()` for `AVKeyType::Relation`
- `/Users/ryanrentfro/code/markamp/src/core/Events.h` -- Add 3 new relation/rollup events
- `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` -- Add 4 new `.cpp` source files
- `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt` -- Add `test_av_relations_rollups` test target
- `/Users/ryanrentfro/code/markamp/resources/config_defaults.json` -- Add 4 new `av.relation.*` / `av.rollup.*` config keys
