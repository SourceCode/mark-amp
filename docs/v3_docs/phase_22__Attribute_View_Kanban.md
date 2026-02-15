# Phase 22 -- Attribute View Kanban View

**Priority:** Medium (alternative view layout)
**Estimated Scope:** ~6 new files, ~4 modified files
**Dependencies:** Phase 17 (Data Model), Phase 18 (Column Types), Phase 20 (Filtering & Sorting)

## Objective

Implement the Kanban View for attribute views -- a horizontal board with vertical lanes where each lane represents a Select/MSelect option value. Cards within lanes can be dragged between lanes, which changes the underlying Select column value for that row. This provides a project management / workflow visualization over AV data.

Key capabilities:
- Horizontal scrolling board with one lane per Select option value (plus a "No Value" lane)
- Cards within each lane display title and configurable fields (similar to Gallery cards)
- Drag-and-drop cards between lanes changes the row's Select/MSelect value
- Lane headers show the option name, color indicator, and card count
- Lanes can be collapsed, expanded, reordered, and hidden when empty
- "Add card" button per lane creates a new row with that lane's Select value pre-set
- Same filtering/sorting from Phase 20 applies before lane grouping
- Right-click context menu on cards for edit/delete/duplicate

## Prerequisites

- **Phase 17** -- `AttributeView`, `AVKanbanView`, `AVSelectOption`
- **Phase 18** -- `AVColumnTypeRegistry`, Select/MSelect type handlers
- **Phase 20** -- `AVQueryEngine`, `FilterEngine`, `SortEngine`

## SiYuan Source Reference

| SiYuan File | Purpose |
|---|---|
| `kernel/sql/av_kanban.go` | Kanban view query generation, lane grouping |
| `kernel/av/av.go` | `KanbanView` struct definition |
| `app/src/protyle/render/av/` | Kanban board rendering in the frontend |

## MarkAmp Integration Points

### New Files to Create

| File | Namespace | Purpose |
|---|---|---|
| `src/ui/av/AVKanbanPanel.h` | `markamp::ui::av` | Main kanban board wxScrolledWindow |
| `src/ui/av/AVKanbanPanel.cpp` | `markamp::ui::av` | Lane layout, scroll, drag-and-drop |
| `src/ui/av/AVKanbanLane.h` | `markamp::ui::av` | Single lane rendering component |
| `src/ui/av/AVKanbanLane.cpp` | `markamp::ui::av` | Lane header, card list, collapse |
| `src/ui/av/AVKanbanCard.h` | `markamp::ui::av` | Kanban card rendering |
| `src/ui/av/AVKanbanCard.cpp` | `markamp::ui::av` | Card painting: title + fields |
| `tests/unit/test_av_kanban.cpp` | (anonymous) | Catch2 tests |

### Files to Modify

| File | Change |
|---|---|
| `src/core/Events.h` | Add kanban-specific events |
| `src/CMakeLists.txt` | Add new `.cpp` sources |
| `tests/CMakeLists.txt` | Add `test_av_kanban` test target |
| `src/core/av/AVTypes.h` | Ensure `AVKanbanView` has collapse state |

## Data Structures to Implement

### File: `src/ui/av/AVKanbanPanel.h`

```cpp
#pragma once

#include "AVKanbanLane.h"
#include "core/av/AttributeView.h"
#include "core/av/AVColumnType.h"
#include "core/av/AVQueryEngine.h"
#include "core/EventBus.h"
#include "core/ThemeEngine.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <wx/scrolwin.h>

namespace markamp::ui::av
{

// ============================================================================
// Kanban Layout Configuration
// ============================================================================

struct AVKanbanLayout
{
    int lane_width{280};               // Width of each lane in pixels
    int lane_gap{12};                  // Gap between lanes
    int padding{16};                   // Outer padding
    int header_height{40};             // Lane header height
    int card_height{100};              // Default card height
    int card_gap{8};                   // Gap between cards within a lane
    int card_padding{8};               // Internal card padding
    int add_card_button_height{36};    // Height of "Add card" button at bottom of lane
};

// ============================================================================
// Lane Data: grouped rows for one Select option value
// ============================================================================

struct AVKanbanLaneData
{
    std::string option_name;           // Select option name (empty for "No Value" lane)
    std::string option_color;          // CSS color string
    std::vector<int> row_indices;      // Indices into the AV's row list
    bool collapsed{false};
    bool is_no_value_lane{false};      // True for the special "No Value" group
};

// ============================================================================
// Drag State
// ============================================================================

struct AVKanbanDragState
{
    bool active{false};
    int source_lane{-1};               // Lane index the card is being dragged from
    int source_card{-1};               // Card index within the source lane
    int target_lane{-1};               // Lane index the card is hovering over
    int target_insert_pos{-1};         // Insertion position within the target lane
    std::string block_id;              // Block ID of the card being dragged
    wxPoint drag_offset;               // Offset from mouse to card top-left
};

// ============================================================================
// AVKanbanPanel: Horizontal scrolling kanban board
// ============================================================================

class AVKanbanPanel : public wxScrolledWindow
{
public:
    AVKanbanPanel(wxWindow* parent,
                  core::EventBus* event_bus,
                  core::IThemeEngine* theme_engine,
                  core::av::AVColumnTypeRegistry* type_registry);

    ~AVKanbanPanel() override;

    // ── Data binding ──

    /// Bind to an attribute view and kanban view.
    void set_attribute_view(core::av::AttributeView* av,
                            const std::string& view_id);

    /// Refresh lanes and cards from current data.
    void refresh_data();

    // ── Lane operations ──

    /// Collapse or expand a lane.
    void set_lane_collapsed(int lane_index, bool collapsed);

    /// Toggle collapse state.
    void toggle_lane_collapsed(int lane_index);

    /// Hide all lanes that have zero cards.
    void hide_empty_lanes();

    /// Show all lanes (including empty ones).
    void show_all_lanes();

    /// Get the number of lanes.
    [[nodiscard]] auto lane_count() const -> int;

    /// Get lane data by index.
    [[nodiscard]] auto lane_data(int index) const -> const AVKanbanLaneData&;

    // ── Card operations ──

    /// Add a new card to a specific lane (creates a row with the lane's select value).
    void add_card_to_lane(int lane_index);

    // ── Configuration ──

    /// Get the current layout configuration.
    [[nodiscard]] auto layout() const -> const AVKanbanLayout&;

    /// Set the group-by key ID (must be a Select or MSelect column).
    void set_group_by_key(const std::string& key_id);

private:
    void on_paint(wxPaintEvent& event);
    void on_size(wxSizeEvent& event);
    void on_mouse_left_down(wxMouseEvent& event);
    void on_mouse_left_up(wxMouseEvent& event);
    void on_mouse_motion(wxMouseEvent& event);
    void on_mouse_left_dclick(wxMouseEvent& event);
    void on_key_down(wxKeyEvent& event);
    void on_context_menu(wxContextMenuEvent& event);

    // ── Layout ──
    void recalculate_layout();
    void build_lanes();
    [[nodiscard]] auto lane_at_point(wxPoint pt) const -> int;
    [[nodiscard]] auto card_at_point(wxPoint pt) const
        -> std::pair<int, int>; // (lane_index, card_index)
    [[nodiscard]] auto lane_rect(int lane_index) const -> wxRect;
    [[nodiscard]] auto card_rect(int lane_index, int card_index) const -> wxRect;
    [[nodiscard]] auto drop_indicator_y(int lane_index, int insert_pos) const -> int;

    // ── Rendering ──
    void render_lanes(wxDC& dc);
    void render_lane(wxDC& dc, int lane_index);
    void render_lane_header(wxDC& dc, int lane_index, wxRect rect);
    void render_lane_cards(wxDC& dc, int lane_index, wxRect rect);
    void render_card(wxDC& dc, int lane_index, int card_index, wxRect rect);
    void render_add_card_button(wxDC& dc, int lane_index, wxRect rect);
    void render_drag_indicator(wxDC& dc);
    void render_dragged_card(wxDC& dc);

    // ── Drag and drop ──
    void start_drag(int lane_index, int card_index, wxPoint mouse_pos);
    void update_drag(wxPoint mouse_pos);
    void finish_drag();
    void cancel_drag();

    /// Move a card from one lane to another, updating the Select value.
    void move_card(const std::string& block_id,
                   const std::string& old_option_name,
                   const std::string& new_option_name);

    // ── Data ──
    core::av::AttributeView* av_{nullptr};
    core::av::AVView* active_view_{nullptr};
    core::EventBus* event_bus_{nullptr};
    core::IThemeEngine* theme_engine_{nullptr};
    core::av::AVColumnTypeRegistry* type_registry_{nullptr};
    std::unique_ptr<core::av::AVQueryEngine> query_engine_;

    // ── Lane data ──
    std::vector<AVKanbanLaneData> lanes_;
    std::string group_by_key_id_;
    AVKanbanLayout layout_;
    std::unordered_set<std::string> hidden_lanes_; // Option names of hidden lanes

    // ── Drag state ──
    AVKanbanDragState drag_state_;

    // ── Subscriptions ──
    std::vector<core::Subscription> subscriptions_;
};

} // namespace markamp::ui::av
```

### File: `src/ui/av/AVKanbanLane.h`

```cpp
#pragma once

#include "core/av/AVTypes.h"

#include <string>
#include <vector>

namespace markamp::ui::av
{

// ============================================================================
// Lane Header Rendering Info
// ============================================================================

struct AVKanbanLaneHeaderInfo
{
    std::string option_name;
    std::string option_color;
    int card_count{0};
    bool collapsed{false};
    bool is_no_value{false};
};

// ============================================================================
// Lane Metrics
// ============================================================================

struct AVKanbanLaneMetrics
{
    int total_height{0};               // Total lane content height
    int visible_card_count{0};
    int header_height{40};
    int cards_start_y{0};              // Y offset where cards begin (after header)
};

/// Calculate lane metrics given card count and layout parameters.
[[nodiscard]] auto calculate_lane_metrics(
    int card_count,
    int card_height,
    int card_gap,
    int header_height,
    int add_button_height,
    bool collapsed) -> AVKanbanLaneMetrics;

} // namespace markamp::ui::av
```

### File: `src/ui/av/AVKanbanCard.h`

```cpp
#pragma once

#include "core/av/AVTypes.h"
#include "core/av/AVValue.h"
#include "core/ThemeEngine.h"

#include <string>
#include <vector>
#include <wx/dc.h>
#include <wx/gdicmn.h>

namespace markamp::ui::av
{

// ============================================================================
// Kanban Card Data
// ============================================================================

struct AVKanbanCardData
{
    std::string block_id;
    std::string title;                 // Primary block content
    std::vector<std::pair<std::string, std::string>> fields; // (label, formatted_value)
    bool selected{false};
    bool hovered{false};
    bool dragging{false};
};

// ============================================================================
// Kanban Card Renderer
// ============================================================================

class AVKanbanCardRenderer
{
public:
    /// Render a kanban card within the given rectangle.
    void render(wxDC& dc,
                const wxRect& rect,
                const AVKanbanCardData& data,
                core::IThemeEngine* theme);

    /// Calculate the required height for a card given its data.
    [[nodiscard]] auto calculate_height(wxDC& dc,
                                         int width,
                                         const AVKanbanCardData& data) const -> int;

private:
    static constexpr int kTitleHeight = 24;
    static constexpr int kFieldHeight = 18;
    static constexpr int kPadding = 8;
    static constexpr int kCornerRadius = 6;
};

} // namespace markamp::ui::av
```

## Key Functions to Implement

### AVKanbanPanel.cpp

1. **`AVKanbanPanel::set_attribute_view(av, view_id)`** -- Store AV pointer. Find kanban view. Extract `group_by_key_id`. Initialize query engine. Call `build_lanes()` then `recalculate_layout()`.

2. **`AVKanbanPanel::build_lanes()`** -- Get the group-by key. Get its Select options from `key.options`. Execute query (filter + sort). Group resulting row indices by their Select value for the group-by column. Create one `AVKanbanLaneData` per option. Add a "No Value" lane for rows with empty Select values. Respect `hide_empty_groups` setting.

3. **`AVKanbanPanel::recalculate_layout()`** -- Compute total width: `padding + (lane_count * lane_width) + ((lane_count - 1) * lane_gap) + padding`. Compute total height: max lane height across all lanes. Set virtual scroll size.

4. **`AVKanbanPanel::render_lanes(wxDC&)`** -- For each lane, compute its x-position. If not collapsed, render full lane. If collapsed, render just the header with a collapsed indicator.

5. **`AVKanbanPanel::render_lane_header(wxDC&, lane_index, rect)`** -- Draw colored bar at left edge (option color, 4px wide). Draw option name text. Draw card count badge (e.g. "(5)"). Draw collapse/expand chevron. Draw "+" add card button if expanded.

6. **`AVKanbanPanel::render_card(wxDC&, lane_index, card_index, rect)`** -- Build `AVKanbanCardData` from the AV row. Delegate to `AVKanbanCardRenderer::render()`.

7. **`AVKanbanPanel::start_drag(lane_index, card_index, mouse_pos)`** -- Set `drag_state_.active = true`. Record source lane, card, block_id. Compute drag offset. Capture mouse.

8. **`AVKanbanPanel::update_drag(mouse_pos)`** -- Determine which lane the mouse is over. Compute insertion position within that lane based on Y coordinate. Update drag state. Call `Refresh()` to show drag indicator.

9. **`AVKanbanPanel::finish_drag()`** -- If source_lane != target_lane, call `move_card()`. Release mouse capture. Clear drag state. Rebuild lanes. Refresh.

10. **`AVKanbanPanel::move_card(block_id, old_option, new_option)`** -- Get the row's value for the group-by key. If Select type: set value to `AVValueSelect{AVSelectOption{new_option, color}}`. If MSelect type: remove `old_option` from selected, add `new_option`. Save to AV. Publish `AVKanbanCardMovedEvent`. Mark dirty.

11. **`AVKanbanPanel::add_card_to_lane(lane_index)`** -- Generate a new block_id. Call `av_->add_row(block_id)`. Set the group-by column value to the lane's Select option. Publish `AVRowAddedEvent`. Rebuild lanes.

12. **`AVKanbanPanel::render_drag_indicator(wxDC&)`** -- At the target lane and insertion position, draw a horizontal colored line (2px thick, accent color) indicating where the card will be inserted.

13. **`AVKanbanPanel::render_dragged_card(wxDC&)`** -- At the mouse position (offset by drag_offset), render a semi-transparent copy of the card being dragged.

### AVKanbanLane.cpp

14. **`calculate_lane_metrics(card_count, ...)`** -- If collapsed: total_height = header_height. Otherwise: total_height = header_height + card_count * (card_height + card_gap) + add_button_height.

### AVKanbanCard.cpp

15. **`AVKanbanCardRenderer::render(dc, rect, data, theme)`** -- Draw card background (white/dark with rounded corners and subtle shadow). If `data.dragging`, reduce opacity. Draw title text bold. Draw each field as "label: value" in smaller font. If selected, draw accent border.

16. **`AVKanbanCardRenderer::calculate_height(dc, width, data)`** -- Return `kPadding + kTitleHeight + data.fields.size() * kFieldHeight + kPadding`.

## Events to Add

```cpp
// ============================================================================
// Attribute View kanban events (Phase 22)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVKanbanCardMovedEvent)
std::string av_id;
std::string block_id;
std::string old_value;     // Old Select option name
std::string new_value;     // New Select option name
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVKanbanLaneCollapsedEvent)
std::string av_id;
std::string option_name;
bool collapsed;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVKanbanCardClickedEvent)
std::string av_id;
std::string block_id;
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|---|---|---|---|
| `av.kanban.lane_width` | int | `280` | Width of each kanban lane in pixels |
| `av.kanban.card_corner_radius` | int | `6` | Card corner radius |
| `av.kanban.show_card_shadow` | bool | `true` | Show drop shadow on cards |
| `av.kanban.hide_empty_groups` | bool | `false` | Hide lanes with no cards |
| `av.kanban.drag_opacity` | double | `0.7` | Opacity of card while being dragged |

## Test Cases

File: `tests/unit/test_av_kanban.cpp`

All tests use Catch2 with `[av][kanban]` tags.

1. **Lane building: groups by Select value** -- Create AV with a Select column ("Status") having options ["Todo", "In Progress", "Done"]. 6 rows: 2 Todo, 3 In Progress, 1 Done. Build lanes. Verify 3 lanes. Verify card counts: Todo=2, In Progress=3, Done=1.

2. **Lane building: "No Value" lane for empty select** -- Same AV as test 1 but add 2 rows with no Status value. Build lanes. Verify 4 lanes (3 options + "No Value"). Verify No Value lane has 2 cards.

3. **Lane building: hide empty groups** -- Create AV with 3 options but only 2 have rows. With `hide_empty_groups=false`, verify 3 lanes. With `hide_empty_groups=true`, verify 2 lanes (empty one hidden).

4. **Card move: changes Select value** -- Create AV with a Select column. Row has value "Todo". Call `move_card(block_id, "Todo", "Done")`. Verify the row's Select value is now "Done". Verify the lane rebuild puts the card in the "Done" lane.

5. **Card move: MSelect type adds/removes option** -- Create AV with MSelect column. Row has values ["Tag1", "Tag2"]. Move from "Tag1" lane to "Tag3" lane. Verify row's MSelect is now ["Tag2", "Tag3"] (Tag1 removed, Tag3 added).

6. **Lane collapse: collapsed lane shows only header** -- Set lane 0 collapsed. Verify `calculate_lane_metrics()` returns `total_height == header_height`. Verify cards are not rendered for collapsed lanes.

7. **Add card to lane: pre-sets Select value** -- Call `add_card_to_lane(1)` where lane 1 is "In Progress". Verify a new row was added. Verify the new row's Status value is "In Progress".

8. **Drag state management** -- Start drag from lane 0, card 1. Verify `drag_state_.active == true`. Update drag position over lane 2. Verify `drag_state_.target_lane == 2`. Cancel drag. Verify `drag_state_.active == false` and no data changed.

9. **Event publishing on card move** -- Set up EventBus subscriber for `AVKanbanCardMovedEvent`. Move a card. Verify the event was published with correct `old_value` and `new_value`.

10. **Kanban with filters applied** -- Create AV with 20 rows. Apply filter matching 10 rows. Build kanban lanes. Verify total cards across all lanes equals 10 (not 20).

## Acceptance Criteria

- [ ] Kanban board renders lanes for each Select option value
- [ ] "No Value" lane appears for rows without a Select value
- [ ] Cards display title and configurable fields
- [ ] Drag-and-drop moves cards between lanes and updates the Select value
- [ ] Lane headers show option name, color indicator, and card count
- [ ] Lanes can be collapsed and expanded
- [ ] Empty lanes can be hidden
- [ ] "Add card" button creates new row with lane's Select value
- [ ] `AVKanbanCardMovedEvent` publishes on successful move
- [ ] Filtering from Phase 20 applies before lane grouping
- [ ] All 10 test cases pass
- [ ] No use of `catch(...)` -- all exceptions typed

## Files to Create

- `/Users/ryanrentfro/code/markamp/src/ui/av/AVKanbanPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVKanbanPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVKanbanLane.h`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVKanbanLane.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVKanbanCard.h`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVKanbanCard.cpp`
- `/Users/ryanrentfro/code/markamp/tests/unit/test_av_kanban.cpp`

## Files to Modify

- `/Users/ryanrentfro/code/markamp/src/core/Events.h` -- Add 3 kanban events
- `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` -- Add 3 new `.cpp` source files
- `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt` -- Add `test_av_kanban` test target
- `/Users/ryanrentfro/code/markamp/resources/config_defaults.json` -- Add 5 new `av.kanban.*` config keys
