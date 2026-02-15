# Phase 25 -- Kanban Board View

## Objective

Implement a Kanban board UI that renders structured Markdown as a drag-and-drop board with columns and cards. A Kanban document uses heading-defined columns with task items as cards. Supports drag-and-drop between columns (which modifies the source Markdown), card metadata display (priority, due date, tags), and configurable column styling.

## Prerequisites

- Phase 24 (TaskService -- task parsing and data model)
- Phase 02 (VaultService -- document access)
- Existing ThemeEngine

## Feature References (PRD)

- PRD #29: Kanban Boards

## Data Structures to Implement

### File: `src/ui/KanbanPanel.h`

```cpp
#pragma once

#include "core/TaskService.h"

#include <wx/panel.h>
#include <wx/scrolwin.h>

#include <string>
#include <vector>

namespace markamp::core { class EventBus; class ThemeEngine; class VaultService; }

namespace markamp::ui
{

struct KanbanColumn
{
    std::string title;
    std::string heading_text;     // Original markdown heading
    int line_number{0};           // Line in source document
    std::vector<markamp::core::Task> cards;
    wxColour color;               // Column header color
    int max_cards{0};             // 0 = unlimited (WIP limit)
};

struct KanbanBoard
{
    std::string document_id;
    std::string title;
    std::vector<KanbanColumn> columns;

    [[nodiscard]] auto total_cards() const -> int;
    [[nodiscard]] auto find_card(const std::string& task_id) const
        -> std::optional<std::pair<int, int>>;  // (column_idx, card_idx)
};

class KanbanPanel : public wxScrolledWindow
{
public:
    KanbanPanel(wxWindow* parent,
                markamp::core::EventBus& event_bus,
                markamp::core::VaultService& vault_service,
                markamp::core::ThemeEngine& theme_engine);

    /// Load a kanban board from a markdown document.
    auto load_document(const std::string& document_id) -> void;

    /// Refresh from the current document.
    auto refresh() -> void;

private:
    markamp::core::EventBus& event_bus_;
    markamp::core::VaultService& vault_service_;
    markamp::core::ThemeEngine& theme_engine_;

    KanbanBoard board_;

    auto parse_kanban_markdown(const std::string& markdown) -> KanbanBoard;
    auto render_board() -> void;
    auto render_column(const KanbanColumn& column, int col_index) -> wxPanel*;
    auto render_card(const markamp::core::Task& card) -> wxPanel*;

    auto on_card_drag_begin(const std::string& task_id, int col, int row) -> void;
    auto on_card_drop(const std::string& task_id, int target_col, int target_row) -> void;
    auto move_card_in_markdown(const std::string& task_id,
                                int from_col, int to_col, int to_row) -> void;
    auto on_card_clicked(const std::string& task_id) -> void;
    auto on_card_toggled(const std::string& task_id) -> void;
    auto apply_theme() -> void;

    markamp::core::Subscription content_changed_sub_;
    markamp::core::Subscription theme_changed_sub_;
};

} // namespace markamp::ui
```

## Key Functions to Implement

1. **`parse_kanban_markdown(markdown)`** -- Parse markdown where H2 headings define columns and task items (`- [ ]`, `- [x]`) are cards within each column. Extract metadata from task lines.

2. **`render_board()`** -- Clear and rebuild the UI. Create a horizontal scroll area. For each column, create a vertical column panel with header and card list. Apply theme colors.

3. **`render_card(card)`** -- Create a card panel showing: task text, priority indicator, due date badge, tag chips. Highlight overdue cards.

4. **`on_card_drop(task_id, target_col, target_row)`** -- Move the task line from its current column to the target column in the source Markdown. Save the document. Refresh the board.

5. **`move_card_in_markdown(task_id, from_col, to_col, to_row)`** -- Find the task line in the source markdown. Remove it from the current column's section. Insert it at the target position in the target column's section. Save via VaultService.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(KanbanCardMovedEvent)
std::string task_id;
std::string from_column;
std::string to_column;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(KanbanBoardLoadedEvent)
std::string document_id;
int column_count{0};
int card_count{0};
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_kanban_panel.cpp`

1. **Parse kanban markdown** -- 3 H2 headings with tasks. Verify 3 columns with correct card counts.
2. **Card metadata** -- Task with priority and due date. Verify card shows both.
3. **Move card between columns** -- Move task from col 1 to col 2. Verify markdown updated.
4. **Move card within column** -- Reorder card within same column. Verify order change.
5. **Card toggle** -- Toggle checkbox. Verify task status updates.
6. **Empty column** -- Column with no tasks. Verify renders with placeholder.
7. **WIP limit** -- Column with max_cards=3 and 3 cards. Verify limit indicator shown.
8. **Theme colors** -- Verify columns use theme-derived colors.
9. **Refresh on save** -- External edit to document. Verify board refreshes.
10. **Total card count** -- Board with 15 cards across 4 columns. total_cards() == 15.

## Acceptance Criteria

- [ ] Kanban board renders columns from H2 headings
- [ ] Task items render as cards with metadata
- [ ] Drag-and-drop moves cards between columns (updates source markdown)
- [ ] Card reordering within columns works
- [ ] Task toggle updates checkbox in source
- [ ] WIP limits displayed on columns
- [ ] Board refreshes when source document changes
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/ui/KanbanPanel.h` | KanbanPanel, KanbanBoard, KanbanColumn |
| CREATE | `src/ui/KanbanPanel.cpp` | Full implementation |
| MODIFY | `src/core/Events.h` | Add 2 kanban events |
| MODIFY | `src/CMakeLists.txt` | Add KanbanPanel.cpp |
| CREATE | `tests/unit/test_kanban_panel.cpp` | 10 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_kanban_panel target |

## Estimated Complexity

**L** -- Markdown-to-board parsing, drag-and-drop, markdown modification, card rendering, 10 tests.
