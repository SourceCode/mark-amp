# Phase 33 -- Kanban Board

## Objective

Implement a Kanban board as a canvas construct: columns (KanbanColumn objects) containing cards (KanbanCard objects) with drag-drop reordering between columns. Includes a swimlane layout, card detail editing, and WIP (work-in-progress) limits per column.

## Prerequisites

- Phase 01 (CanvasObject)
- Phase 02 (CanvasRenderer, IObjectRenderer)
- Phase 07 (InlineTextEditor for card/column editing)
- Phase 05 (Board, UndoRedoStack)

## Feature References (PRD)

- PRD #70: Kanban Board

## Data Structures to Implement

### File: `src/canvas/KanbanObjects.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"

#include <optional>
#include <string>
#include <vector>

namespace markamp::canvas
{

class KanbanCard : public CanvasObject
{
public:
    KanbanCard();

    [[nodiscard]] auto title() const -> const std::string&;
    auto set_title(const std::string& title) -> void;
    [[nodiscard]] auto description() const -> const std::string&;
    auto set_description(const std::string& desc) -> void;
    [[nodiscard]] auto card_color() const -> const CanvasColor&;
    auto set_card_color(const CanvasColor& color) -> void;
    [[nodiscard]] auto assignee() const -> const std::string&;
    auto set_assignee(const std::string& name) -> void;
    [[nodiscard]] auto due_date() const -> const std::string&;
    auto set_due_date(const std::string& date) -> void;
    [[nodiscard]] auto priority() const -> int;
    auto set_priority(int priority) -> void;
    [[nodiscard]] auto column_id() const -> ObjectId;
    auto set_column_id(ObjectId id) -> void;
    [[nodiscard]] auto card_index() const -> int;
    auto set_card_index(int index) -> void;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::string title_{"New Card"};
    std::string description_;
    CanvasColor card_color_{255, 255, 255, 255};
    std::string assignee_;
    std::string due_date_;
    int priority_{0};
    ObjectId column_id_{kInvalidObjectId};
    int card_index_{0};
};

class KanbanColumn : public CanvasObject
{
public:
    KanbanColumn();

    [[nodiscard]] auto title() const -> const std::string&;
    auto set_title(const std::string& title) -> void;
    [[nodiscard]] auto column_color() const -> const CanvasColor&;
    auto set_column_color(const CanvasColor& color) -> void;
    [[nodiscard]] auto wip_limit() const -> int;
    auto set_wip_limit(int limit) -> void;
    [[nodiscard]] auto card_ids() const -> const std::vector<ObjectId>&;
    auto add_card(ObjectId id) -> void;
    auto remove_card(ObjectId id) -> void;
    auto insert_card(ObjectId id, int index) -> void;
    [[nodiscard]] auto card_count() const -> int;
    [[nodiscard]] auto column_width() const -> double;
    auto set_column_width(double w) -> void;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::string title_{"Column"};
    CanvasColor column_color_{240, 240, 240, 255};
    int wip_limit_{0}; // 0 = no limit
    std::vector<ObjectId> card_ids_;
    double column_width_{280.0};
};

} // namespace markamp::canvas
```

### File: `src/canvas/KanbanController.h`

```cpp
#pragma once

#include "CanvasTypes.h"

#include <string>

namespace markamp::canvas
{

class Board;
class UndoRedoStack;

class KanbanController
{
public:
    KanbanController(Board& board, UndoRedoStack& undo_stack);

    auto create_kanban(const Point2D& position, int column_count = 3) -> ObjectId;
    auto add_column(const std::string& title) -> ObjectId;
    auto remove_column(ObjectId column_id) -> void;
    auto add_card(ObjectId column_id, const std::string& title) -> ObjectId;
    auto move_card(ObjectId card_id, ObjectId to_column_id, int index) -> void;
    auto relayout() -> void;

private:
    Board& board_;
    UndoRedoStack& undo_stack_;
    std::vector<ObjectId> column_ids_;
    Point2D origin_;

    auto position_cards_in_column(ObjectId column_id) -> void;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `KanbanController::create_kanban()` -- Create N columns ("To Do", "In Progress", "Done"). Position side by side.

2. `KanbanController::move_card()` -- Remove card from source column, insert into target column at index. Relayout both columns.

3. `KanbanColumnRenderer::render()` -- Draw column header with title and card count / WIP limit indicator. Draw card stack below.

4. `KanbanCardRenderer::render()` -- Draw card with title, color stripe, assignee initials, due date, priority badge.

5. Drag-drop: Dragging a card between columns triggers move_card. Visual feedback shows insertion point.

## Test Cases

File: `tests/unit/test_kanban.cpp`

1. **Create kanban** -- 3 columns, verify.
2. **Add card** -- Add to column, verify card_ids.
3. **Move card** -- Move between columns, verify.
4. **WIP limit** -- Column with limit 3, add 4th, verify warning state.
5. **Card ordering** -- 3 cards, verify card_index.
6. **Column relayout** -- After card add, verify positions updated.
7. **Remove column** -- Remove column, verify cards deleted.
8. **JSON round-trip** -- Full kanban, serialize/deserialize.

## Acceptance Criteria

- [ ] KanbanColumn with title, WIP limit, card list
- [ ] KanbanCard with title, description, assignee, due date, priority
- [ ] Drag-drop cards between columns
- [ ] WIP limit visual indicator when exceeded
- [ ] Auto-layout of cards within columns
- [ ] KanbanController manages CRUD with undo
- [ ] All 8 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/KanbanObjects.h` | Kanban column + card models |
| CREATE | `src/canvas/KanbanObjects.cpp` | Implementation |
| CREATE | `src/canvas/KanbanController.h` | Kanban CRUD + layout |
| CREATE | `src/canvas/KanbanController.cpp` | Controller implementation |
| CREATE | `src/canvas/KanbanColumnRenderer.h` | Column rendering |
| CREATE | `src/canvas/KanbanColumnRenderer.cpp` | Column + header drawing |
| CREATE | `src/canvas/KanbanCardRenderer.h` | Card rendering |
| CREATE | `src/canvas/KanbanCardRenderer.cpp` | Card drawing |
| MODIFY | `src/canvas/BoardSerializer.cpp` | Register factories |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_kanban.cpp` | 8 Catch2 tests |

## Architecture Notes

- KanbanColumns and KanbanCards are separate CanvasObject subclasses. The column stores an ordered list of card IDs.
- The KanbanController manages the structural invariants and auto-layout. Direct manipulation of cards (outside the controller) could break the kanban structure.
- Drag-drop within the canvas reuses the existing SelectTool drag infrastructure, with custom drop-zone detection on columns.

## Estimated Complexity

**L** -- Two object types, controller with drag-drop logic, column/card renderers, 8 tests.
