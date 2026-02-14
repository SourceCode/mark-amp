# Phase 20 -- Attribute View Filtering & Sorting Engine

**Priority:** High (core table interactivity)
**Estimated Scope:** ~8 new files, ~4 modified files
**Dependencies:** Phase 17 (Data Model), Phase 18 (Column Types), Phase 19 (Table Renderer)

## Objective

Implement the filtering and sorting engines for attribute views. Filters allow multi-condition queries with AND/OR compound logic over any column type. Sorting supports multi-level ordering by any column with type-aware comparison. Together, these engines transform the raw row data into a filtered, ordered result set that the Table, Gallery, and Kanban views all consume.

This phase implements:
- A `FilterEngine` that evaluates filter conditions against rows
- A `SortEngine` that orders rows by one or more columns
- A filter bar UI component for adding, editing, and removing filter conditions
- Sort indicators in column headers with multi-level sort configuration
- Persistent filter/sort configurations stored per-view in the AV JSON

## Prerequisites

- **Phase 17** -- `AttributeView`, `AVKey`, `AVValue`, `AVValueVariant`
- **Phase 18** -- `AVColumnTypeRegistry`, `IAVColumnTypeHandler::filter()`, `IAVColumnTypeHandler::compare()`, `AVFilterOperator`, `AVSortOrder`
- **Phase 19** -- `AVTablePanel`, `AVTableController`, `AVColumnHeader`

## SiYuan Source Reference

| SiYuan File | Purpose | Key Structures |
|---|---|---|
| `kernel/av/filter.go` | Filter struct, operator definitions, per-type evaluation | `AVFilter`, `FilterOperator`, filter evaluation functions |
| `kernel/av/sort.go` | Sort struct, per-type comparison | `AVSort`, comparison dispatch |
| `kernel/av/av.go` | View-level filter/sort configuration | `View.Filters`, `View.Sorts` |
| `app/src/protyle/render/av/filter.ts` | Filter bar UI, operator dropdowns, value inputs | Frontend filter UI components |

## MarkAmp Integration Points

### New Files to Create

| File | Namespace | Purpose |
|---|---|---|
| `src/core/av/AVFilter.h` | `markamp::core::av` | Filter condition structs, compound logic |
| `src/core/av/AVFilter.cpp` | `markamp::core::av` | Filter evaluation engine |
| `src/core/av/AVSort.h` | `markamp::core::av` | Sort specification structs |
| `src/core/av/AVSort.cpp` | `markamp::core::av` | Sort execution engine |
| `src/core/av/AVQueryEngine.h` | `markamp::core::av` | Combined filter+sort pipeline |
| `src/core/av/AVQueryEngine.cpp` | `markamp::core::av` | Pipeline implementation |
| `src/ui/av/AVFilterBar.h` | `markamp::ui::av` | Filter bar wxPanel UI |
| `src/ui/av/AVFilterBar.cpp` | `markamp::ui::av` | Filter bar rendering and interaction |
| `tests/unit/test_av_filter_sort.cpp` | (anonymous) | Catch2 tests |

### Files to Modify

| File | Change |
|---|---|
| `src/core/av/AVTypes.h` | Add filter/sort config structs to `AVView` |
| `src/ui/av/AVTablePanel.h` | Integrate filter bar, apply filter/sort pipeline |
| `src/ui/av/AVTablePanel.cpp` | Wire filter/sort into row rendering |
| `src/CMakeLists.txt` | Add new `.cpp` sources |
| `tests/CMakeLists.txt` | Add `test_av_filter_sort` test target |

## Data Structures to Implement

### File: `src/core/av/AVFilter.h`

```cpp
#pragma once

#include "AVColumnType.h"
#include "AVTypes.h"
#include "AVValue.h"

#include <string>
#include <variant>
#include <vector>

namespace markamp::core::av
{

// ============================================================================
// Compound Logic
// ============================================================================

enum class AVFilterLogic
{
    And,
    Or
};

// ============================================================================
// Relative Date Specifier (for DateIsRelative operator)
// ============================================================================

enum class AVRelativeDateRange
{
    Today,
    Yesterday,
    Tomorrow,
    PastWeek,
    PastMonth,
    PastYear,
    ThisWeek,
    ThisMonth,
    ThisYear,
    NextWeek,
    NextMonth,
    NextYear
};

[[nodiscard]] auto relative_date_range_to_string(AVRelativeDateRange range) -> std::string;
[[nodiscard]] auto string_to_relative_date_range(const std::string& str) -> AVRelativeDateRange;

// ============================================================================
// Filter Operand Value
// The value to compare against in a filter condition.
// Uses a variant to hold type-appropriate operands.
// ============================================================================

using AVFilterOperand = std::variant<
    std::monostate,                    // For IsEmpty/IsNotEmpty/IsTrue/IsFalse (no operand)
    std::string,                       // For text/URL/email/phone operators
    double,                            // For number operators
    int64_t,                           // For date operators (timestamp)
    std::pair<int64_t, int64_t>,       // For DateBetween (start, end)
    AVRelativeDateRange,               // For DateIsRelative
    AVSelectOption,                    // For SelectIs/SelectIsNot
    std::vector<std::string>           // For RelationContains (block IDs)
>;

// ============================================================================
// Filter Condition
// A single filter rule: column + operator + operand
// ============================================================================

struct AVFilterCondition
{
    std::string id;                    // Unique condition ID (for UI tracking)
    std::string key_id;                // Column to filter on
    AVFilterOperator op{AVFilterOperator::IsNotEmpty};
    AVFilterOperand operand;

    [[nodiscard]] auto operator==(const AVFilterCondition& other) const -> bool = default;
};

// ============================================================================
// Filter Group
// A set of conditions combined with AND or OR logic
// ============================================================================

struct AVFilterGroup
{
    AVFilterLogic logic{AVFilterLogic::And};
    std::vector<AVFilterCondition> conditions;

    [[nodiscard]] auto is_empty() const -> bool
    {
        return conditions.empty();
    }
};

// ============================================================================
// Filter Engine
// Evaluates filter conditions against rows
// ============================================================================

class FilterEngine
{
public:
    explicit FilterEngine(const AVColumnTypeRegistry* type_registry);

    /// Apply a filter group to a set of rows.
    /// Returns indices of rows that pass all filter conditions.
    [[nodiscard]] auto apply_filters(
        const AttributeView& av,
        const AVFilterGroup& filter_group,
        const std::vector<int>& row_indices) const -> std::vector<int>;

    /// Evaluate a single filter condition against a single value.
    [[nodiscard]] auto evaluate_condition(
        const AVValue& value,
        const AVKey& key,
        const AVFilterCondition& condition) const -> bool;

    /// Get the list of supported operators for a given key type.
    [[nodiscard]] auto supported_operators_for_type(AVKeyType type) const
        -> std::vector<AVFilterOperator>;

    /// Get a human-readable label for a filter operator.
    [[nodiscard]] static auto operator_display_label(AVFilterOperator op) -> std::string;

private:
    const AVColumnTypeRegistry* type_registry_{nullptr};

    /// Evaluate the IsEmpty/IsNotEmpty universal operators.
    [[nodiscard]] auto evaluate_empty_check(
        const AVValue& value,
        AVFilterOperator op) const -> bool;

    /// Resolve a relative date range to a concrete timestamp range.
    [[nodiscard]] static auto resolve_relative_date(
        AVRelativeDateRange range,
        int64_t now_timestamp) -> std::pair<int64_t, int64_t>;
};

} // namespace markamp::core::av
```

### File: `src/core/av/AVSort.h`

```cpp
#pragma once

#include "AVColumnType.h"
#include "AVTypes.h"
#include "AVValue.h"

#include <string>
#include <vector>

namespace markamp::core::av
{

// ============================================================================
// Sort Specification
// One level of sort: column + direction
// ============================================================================

struct AVSortSpec
{
    std::string key_id;                // Column to sort by
    AVSortOrder order{AVSortOrder::Ascending};

    [[nodiscard]] auto operator==(const AVSortSpec& other) const -> bool = default;
};

// ============================================================================
// Sort Configuration
// Multi-level sort: first by column A, then by column B, etc.
// ============================================================================

struct AVSortConfig
{
    std::vector<AVSortSpec> sorts;     // Ordered list of sort levels

    [[nodiscard]] auto is_empty() const -> bool
    {
        return sorts.empty();
    }

    /// Add a sort level. If the column already has a sort, toggle its direction.
    void toggle_sort(const std::string& key_id);

    /// Remove all sort levels for a column.
    void remove_sort(const std::string& key_id);

    /// Clear all sort levels.
    void clear();
};

// ============================================================================
// Sort Engine
// Sorts rows by one or more columns using type-aware comparison
// ============================================================================

class SortEngine
{
public:
    explicit SortEngine(const AVColumnTypeRegistry* type_registry);

    /// Sort row indices according to the sort configuration.
    /// Returns a new vector of indices in sorted order.
    [[nodiscard]] auto apply_sorts(
        const AttributeView& av,
        const AVSortConfig& sort_config,
        const std::vector<int>& row_indices) const -> std::vector<int>;

    /// Compare two rows by a single sort specification.
    /// Returns <0, 0, or >0.
    [[nodiscard]] auto compare_rows(
        const AttributeView& av,
        int row_a,
        int row_b,
        const AVSortSpec& sort_spec) const -> int;

    /// Compare two rows by the full multi-level sort config.
    [[nodiscard]] auto compare_rows_multi(
        const AttributeView& av,
        int row_a,
        int row_b,
        const AVSortConfig& sort_config) const -> int;

private:
    const AVColumnTypeRegistry* type_registry_{nullptr};
};

} // namespace markamp::core::av
```

### File: `src/core/av/AVQueryEngine.h`

```cpp
#pragma once

#include "AVFilter.h"
#include "AVSort.h"
#include "AttributeView.h"

#include <vector>

namespace markamp::core::av
{

// ============================================================================
// Query Result
// The output of applying filters + sorts to an AV
// ============================================================================

struct AVQueryResult
{
    std::vector<int> row_indices;      // Filtered and sorted row indices
    int total_unfiltered_count{0};     // Total rows before filtering
    int filtered_count{0};             // Rows remaining after filtering

    /// Get a page of results.
    [[nodiscard]] auto page(int page_number, int page_size) const -> std::vector<int>;

    /// Get total number of pages for a given page size.
    [[nodiscard]] auto total_pages(int page_size) const -> int;
};

// ============================================================================
// Query Engine
// Combines FilterEngine + SortEngine into a single pipeline
// ============================================================================

class AVQueryEngine
{
public:
    AVQueryEngine(const AVColumnTypeRegistry* type_registry);

    /// Execute a query: filter, then sort, then return result.
    [[nodiscard]] auto execute(
        const AttributeView& av,
        const AVFilterGroup& filters,
        const AVSortConfig& sorts) const -> AVQueryResult;

    /// Execute with just filtering (no sort).
    [[nodiscard]] auto filter_only(
        const AttributeView& av,
        const AVFilterGroup& filters) const -> AVQueryResult;

    /// Execute with just sorting (no filter).
    [[nodiscard]] auto sort_only(
        const AttributeView& av,
        const AVSortConfig& sorts) const -> AVQueryResult;

    /// Access the underlying engines for direct use.
    [[nodiscard]] auto filter_engine() const -> const FilterEngine&;
    [[nodiscard]] auto sort_engine() const -> const SortEngine&;

private:
    FilterEngine filter_engine_;
    SortEngine sort_engine_;

    /// Build the initial row index list [0, 1, 2, ..., n-1].
    [[nodiscard]] static auto all_row_indices(const AttributeView& av) -> std::vector<int>;
};

} // namespace markamp::core::av
```

### File: `src/ui/av/AVFilterBar.h`

```cpp
#pragma once

#include "core/av/AVFilter.h"
#include "core/av/AVColumnType.h"
#include "core/av/AVTypes.h"
#include "core/ThemeEngine.h"

#include <functional>
#include <string>
#include <vector>
#include <wx/panel.h>

namespace markamp::ui::av
{

// ============================================================================
// Filter Condition Row: UI for one filter condition
// ============================================================================

struct AVFilterConditionUI
{
    std::string condition_id;
    std::string selected_key_id;
    core::av::AVFilterOperator selected_op{core::av::AVFilterOperator::IsNotEmpty};
    std::string operand_text;          // User-entered operand value
    bool enabled{true};
};

// ============================================================================
// Filter Bar Callbacks
// ============================================================================

using AVFilterChangedCallback = std::function<void(const core::av::AVFilterGroup& filters)>;

// ============================================================================
// AVFilterBar: horizontal filter bar panel
// Shown between column headers and data rows when filters are active.
// ============================================================================

class AVFilterBar : public wxPanel
{
public:
    AVFilterBar(wxWindow* parent,
                core::IThemeEngine* theme_engine,
                const core::av::AVColumnTypeRegistry* type_registry);

    /// Set the available columns for filter dropdowns.
    void set_columns(const std::vector<core::av::AVKey>& keys);

    /// Set the current filter group (e.g. loaded from saved view config).
    void set_filter_group(const core::av::AVFilterGroup& group);

    /// Get the current filter group from the UI state.
    [[nodiscard]] auto get_filter_group() const -> core::av::AVFilterGroup;

    /// Set callback invoked when any filter condition changes.
    void set_on_filter_changed(AVFilterChangedCallback callback);

    /// Add a new empty filter condition row.
    void add_condition();

    /// Remove a filter condition by ID.
    void remove_condition(const std::string& condition_id);

    /// Clear all filter conditions.
    void clear_all();

    /// Returns true if there are any active filter conditions.
    [[nodiscard]] auto has_active_filters() const -> bool;

    /// Toggle AND/OR logic.
    void toggle_logic();

    /// Get the current logic mode.
    [[nodiscard]] auto logic() const -> core::av::AVFilterLogic;

private:
    void on_paint(wxPaintEvent& event);
    void on_size(wxSizeEvent& event);
    void rebuild_ui();
    void notify_changed();

    core::IThemeEngine* theme_engine_{nullptr};
    const core::av::AVColumnTypeRegistry* type_registry_{nullptr};

    std::vector<core::av::AVKey> available_keys_;
    std::vector<AVFilterConditionUI> conditions_;
    core::av::AVFilterLogic logic_{core::av::AVFilterLogic::And};
    AVFilterChangedCallback on_changed_;
};

} // namespace markamp::ui::av
```

### Additions to `src/core/av/AVTypes.h` (AVView struct)

Add filter/sort configuration fields to the existing `AVView` struct:

```cpp
// Add to the existing AVView struct in AVTypes.h:

struct AVView
{
    // ... existing fields ...

    // ── Filter & Sort Configuration (Phase 20) ──
    AVFilterGroup filters;         // Active filter conditions for this view
    AVSortConfig sorts;            // Active sort configuration for this view
};
```

This requires forward-declaring or including `AVFilter.h` and `AVSort.h` from `AVTypes.h`, or restructuring the include graph. The recommended approach is to declare `AVFilterGroup` and `AVSortConfig` in `AVFilter.h` and `AVSort.h` respectively, and include both from `AVTypes.h`.

## Key Functions to Implement

### AVFilter.cpp

1. **`FilterEngine::apply_filters(av, filter_group, row_indices)`** -- Iterate `row_indices`. For each row, evaluate all conditions. If `logic == And`, row passes if ALL conditions pass. If `logic == Or`, row passes if ANY condition passes. Return indices of passing rows.

2. **`FilterEngine::evaluate_condition(value, key, condition)`** -- If the operator is `IsEmpty` or `IsNotEmpty`, call `evaluate_empty_check()`. Otherwise, delegate to `type_registry_->evaluate_filter(value, condition.op, operand_as_value, key.type)`. The `operand_as_value` must be constructed from the `AVFilterOperand` variant.

3. **`FilterEngine::evaluate_empty_check(value, op)`** -- Check `value.is_empty()`. For `IsEmpty`, return true if empty. For `IsNotEmpty`, return true if not empty. Special cases: empty string counts as empty for text types. Zero with `is_not_empty == false` counts as empty for number types.

4. **`FilterEngine::resolve_relative_date(range, now)`** -- Switch on range enum. Compute start/end timestamps for the range. E.g., `PastWeek`: start = `now - 7*86400`, end = `now`. `ThisMonth`: start = first day of current month, end = last day of current month. Return `{start, end}`.

5. **`FilterEngine::supported_operators_for_type(type)`** -- Delegate to `type_registry_->get_handler(type)->supported_operators()`.

6. **`FilterEngine::operator_display_label(op)`** -- Static switch: `Contains` -> "contains", `IsEqual` -> "is", `GreaterThan` -> ">", `DateBefore` -> "is before", `IsTrue` -> "is checked", etc.

### AVSort.cpp

7. **`SortEngine::apply_sorts(av, sort_config, row_indices)`** -- Copy `row_indices`. Sort using `std::stable_sort` with a comparator that calls `compare_rows_multi()`.

8. **`SortEngine::compare_rows(av, row_a, row_b, sort_spec)`** -- Get values for both rows at the sort column. Delegate to `type_registry_->compare_values(val_a, val_b, key.type)`. Multiply result by -1 if `order == Descending`. Handle empty values: empty values always sort to the end regardless of direction.

9. **`SortEngine::compare_rows_multi(av, row_a, row_b, sort_config)`** -- Iterate sort specs in order. For each, call `compare_rows()`. Return the first non-zero result. If all are zero, return 0 (equal by all criteria).

10. **`AVSortConfig::toggle_sort(key_id)`** -- If column has no sort, add Ascending. If Ascending, change to Descending. If Descending, remove the sort.

### AVQueryEngine.cpp

11. **`AVQueryEngine::execute(av, filters, sorts)`** -- Build initial indices `[0, n)`. Apply `filter_engine_.apply_filters()`. Apply `sort_engine_.apply_sorts()`. Return `AVQueryResult` with indices, counts.

12. **`AVQueryResult::page(page_number, page_size)`** -- Compute `start = page_number * page_size`. Compute `end = min(start + page_size, row_indices.size())`. Return `vector(row_indices.begin() + start, row_indices.begin() + end)`.

13. **`AVQueryResult::total_pages(page_size)`** -- Return `ceil(filtered_count / page_size)`. Minimum 1 page even for empty results.

### AVFilterBar.cpp

14. **`AVFilterBar::add_condition()`** -- Append a new `AVFilterConditionUI` with a generated ID, the first available column selected, and `IsNotEmpty` as default operator. Call `rebuild_ui()` and `notify_changed()`.

15. **`AVFilterBar::set_filter_group(group)`** -- Convert `AVFilterGroup` conditions to `AVFilterConditionUI` entries. Set `logic_`. Call `rebuild_ui()`.

16. **`AVFilterBar::get_filter_group()`** -- Convert UI state back to `AVFilterGroup`. Parse operand text into `AVFilterOperand` based on the selected column type and operator.

17. **`AVFilterBar::rebuild_ui()`** -- Clear child windows. For each condition: create a column dropdown (`wxChoice`), an operator dropdown (`wxChoice` populated from `supported_operators_for_type()`), a value input (type-appropriate: `wxTextCtrl` for text, `wxSpinCtrl` for numbers, `wxDatePickerCtrl` for dates, `wxChoice` for select options), and a remove button. Add AND/OR toggle button. Add "Add filter" button.

18. **`AVFilterBar::toggle_logic()`** -- Switch between `And` and `Or`. Update the toggle button label. Call `notify_changed()`.

## Events to Add

```cpp
// ============================================================================
// Attribute View filter/sort events (Phase 20)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVFilterChangedEvent)
std::string av_id;
std::string view_id;
int active_filter_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVSortChangedEvent)
std::string av_id;
std::string view_id;
int active_sort_count{0};
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|---|---|---|---|
| `av.filter.show_bar` | bool | `true` | Show filter bar when filters are active |
| `av.filter.default_logic` | string | `"and"` | Default compound logic: "and" or "or" |
| `av.sort.empty_values_last` | bool | `true` | Sort empty values to the end regardless of direction |

## Test Cases

File: `tests/unit/test_av_filter_sort.cpp`

All tests use Catch2 with `[av][filter_sort]` tags.

1. **Filter: text Contains** -- Create AV with Text column. 5 rows: "Apple", "Banana", "Apricot", "Cherry", "Avocado". Filter `Contains("Ap")`. Verify result contains rows 0, 2 ("Apple", "Apricot").

2. **Filter: number GreaterThan** -- Create AV with Number column. 5 rows: 10, 25, 5, 30, 15. Filter `GreaterThan(20)`. Verify result contains rows 1, 3 (values 25, 30).

3. **Filter: date DateBefore** -- Create AV with Date column. 3 rows: Jan 1 2024, Jun 15 2024, Dec 31 2024. Filter `DateBefore(Jul 1 2024)`. Verify result contains rows 0, 1 (Jan 1 and Jun 15).

4. **Filter: select SelectIs** -- Create AV with Select column, options ["Red", "Blue", "Green"]. 4 rows: Red, Blue, Green, Blue. Filter `SelectIs("Blue")`. Verify result contains rows 1, 3.

5. **Filter: checkbox IsTrue** -- Create AV with Checkbox column. 4 rows: true, false, true, false. Filter `IsTrue`. Verify result contains rows 0, 2.

6. **Filter: compound AND logic** -- Create AV with Text and Number columns. 4 rows: ("Alice", 30), ("Bob", 25), ("Alice", 20), ("Charlie", 35). Filter AND: Text Contains "Alice" AND Number GreaterThan 25. Verify result contains only row 0 ("Alice", 30).

7. **Filter: compound OR logic** -- Same data. Filter OR: Text Contains "Bob" OR Number GreaterThan 30. Verify result contains rows 1 ("Bob", 25) and 3 ("Charlie", 35).

8. **Sort: single column ascending** -- Create AV with Number column. 5 rows: 30, 10, 50, 20, 40. Sort ascending by number. Verify order: 10, 20, 30, 40, 50.

9. **Sort: multi-level sort** -- Create AV with Text and Number columns. 4 rows: ("B", 2), ("A", 3), ("B", 1), ("A", 1). Sort by Text ascending, then Number descending. Verify order: ("A", 3), ("A", 1), ("B", 2), ("B", 1).

10. **Sort: empty values sort to end** -- Create AV with Number column. 5 rows: 10, empty, 30, empty, 20. Sort ascending. Verify order: 10, 20, 30, empty, empty. Sort descending. Verify order: 30, 20, 10, empty, empty.

11. **Query pipeline: filter then sort** -- Create AV with Text and Number columns. 6 rows. Apply filter (Number > 10) then sort (Text ascending). Verify the result is filtered first, then sorted. Verify pagination: page 0 with page_size=2 returns first 2 results.

12. **Filter: IsEmpty and IsNotEmpty** -- Create AV with Text column. 3 rows: "Hello", "", "World". Filter `IsEmpty`. Verify result contains row 1. Filter `IsNotEmpty`. Verify result contains rows 0, 2.

## Acceptance Criteria

- [ ] `FilterEngine` correctly evaluates all operator types per column type
- [ ] `FilterEngine` supports AND and OR compound logic
- [ ] `FilterEngine` handles `DateIsRelative` with correct time range computation
- [ ] `SortEngine` sorts rows using type-aware comparison from `AVColumnTypeRegistry`
- [ ] `SortEngine` supports multi-level sorting with stable sort semantics
- [ ] Empty values sort to end regardless of sort direction
- [ ] `AVQueryEngine` combines filter + sort into a single pipeline
- [ ] `AVQueryResult` provides correct pagination
- [ ] `AVFilterBar` renders filter conditions with type-appropriate inputs
- [ ] Filter/sort configurations persist in `AVView` JSON
- [ ] All 12 test cases pass (note: these test only the engine, not the UI)
- [ ] No use of `catch(...)` -- all exceptions typed
- [ ] All query methods have `[[nodiscard]]`

## Files to Create

- `/Users/ryanrentfro/code/markamp/src/core/av/AVFilter.h`
- `/Users/ryanrentfro/code/markamp/src/core/av/AVFilter.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/av/AVSort.h`
- `/Users/ryanrentfro/code/markamp/src/core/av/AVSort.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/av/AVQueryEngine.h`
- `/Users/ryanrentfro/code/markamp/src/core/av/AVQueryEngine.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVFilterBar.h`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVFilterBar.cpp`
- `/Users/ryanrentfro/code/markamp/tests/unit/test_av_filter_sort.cpp`

## Files to Modify

- `/Users/ryanrentfro/code/markamp/src/core/av/AVTypes.h` -- Add `AVFilterGroup filters` and `AVSortConfig sorts` fields to `AVView`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.h` -- Add `AVFilterBar*` member, `AVQueryEngine*` member
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp` -- Use `AVQueryEngine::execute()` instead of raw row iteration; show/hide filter bar
- `/Users/ryanrentfro/code/markamp/src/core/Events.h` -- Add `AVFilterChangedEvent`, `AVSortChangedEvent`
- `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` -- Add 4 new `.cpp` source files
- `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt` -- Add `test_av_filter_sort` test target
- `/Users/ryanrentfro/code/markamp/resources/config_defaults.json` -- Add 3 new `av.filter.*` / `av.sort.*` config keys
