# Phase 16 -- Object Grouping and Nesting

## Objective

Implement explicit object grouping: select multiple objects and group them into a single compound entity that can be moved, resized, and rotated as one unit. Groups can be nested (group of groups). Double-clicking a group enters the group for editing individual members.

## Prerequisites

- Phase 01 (CanvasObject, parent_id)
- Phase 04 (SelectionManager, multi-select)
- Phase 05 (Board, UndoRedoStack)

## Feature References (PRD)

- PRD #39: Sticky Note Grouping
- PRD #43: Bulk Select/Actions

## Data Structures to Implement

### File: `src/canvas/GroupObject.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"

#include <vector>

namespace markamp::canvas
{

class GroupObject : public CanvasObject
{
public:
    GroupObject();

    [[nodiscard]] auto child_ids() const -> const std::vector<ObjectId>&;
    auto add_child(ObjectId id) -> void;
    auto remove_child(ObjectId id) -> void;
    auto set_children(const std::vector<ObjectId>& ids) -> void;

    /// Recompute bounds from children (call after children change).
    auto recompute_bounds(
        const std::unordered_map<ObjectId, std::unique_ptr<CanvasObject>>& all_objects) -> void;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::vector<ObjectId> child_ids_;
    AABB cached_bounds_;
};

} // namespace markamp::canvas
```

### File: `src/canvas/GroupCommands.h`

```cpp
#pragma once

#include "CanvasCommands.h"
#include "GroupObject.h"

namespace markamp::canvas
{

class GroupObjectsCommand : public ICanvasCommand
{
public:
    GroupObjectsCommand(Board& board, const std::vector<ObjectId>& object_ids);
    auto execute() -> void override;
    auto undo() -> void override;
    [[nodiscard]] auto description() const -> std::string override;

private:
    Board& board_;
    std::vector<ObjectId> object_ids_;
    ObjectId group_id_{kInvalidObjectId};
};

class UngroupObjectsCommand : public ICanvasCommand
{
public:
    UngroupObjectsCommand(Board& board, ObjectId group_id);
    auto execute() -> void override;
    auto undo() -> void override;
    [[nodiscard]] auto description() const -> std::string override;

private:
    Board& board_;
    ObjectId group_id_;
    std::vector<ObjectId> child_ids_;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `GroupObjectsCommand::execute()` -- Create a GroupObject. Set each selected object's parent_id to the group. Store child IDs in the group. Compute group bounds. Add group to board.

2. `UngroupObjectsCommand::execute()` -- Remove the GroupObject. Reset each child's parent_id to kInvalidObjectId.

3. Group selection: Clicking a grouped object selects the group. Double-clicking enters the group (selects the specific child). Pressing Escape exits the group context.

4. Group transforms: Moving/resizing/rotating a group applies the transform to all children relative to the group's pivot.

## Shortcut Registrations

| Shortcut | Action | Context |
|----------|--------|---------|
| Cmd/Ctrl+G | Group selected objects | canvas |
| Cmd/Ctrl+Shift+G | Ungroup | canvas |

## Test Cases

File: `tests/unit/test_group_object.cpp`

1. **Group creation** -- Group 3 objects, verify group has 3 children.
2. **Group bounds** -- Group objects at known positions, verify bounds enclose all.
3. **Ungroup** -- Group then ungroup, verify children have no parent.
4. **Nested groups** -- Group A contains B, B contains objects. Verify hierarchy.
5. **Group move** -- Move group, verify children move.
6. **Group JSON** -- Serialize/deserialize group with children.
7. **Group undo** -- Group, undo, verify no group exists.
8. **Ungroup undo** -- Ungroup, undo, verify group restored.

## Acceptance Criteria

- [ ] GroupObject stores child IDs, computes bounds from children
- [ ] Group/Ungroup commands with full undo/redo
- [ ] Clicking a group selects the group, double-click enters the group
- [ ] Group transforms propagate to children
- [ ] Nested groups supported
- [ ] Cmd+G / Cmd+Shift+G shortcuts
- [ ] All 8 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/GroupObject.h` | Group data model |
| CREATE | `src/canvas/GroupObject.cpp` | Group implementation |
| CREATE | `src/canvas/GroupCommands.h` | Group/ungroup commands |
| CREATE | `src/canvas/GroupCommands.cpp` | Command implementations |
| MODIFY | `src/canvas/SelectionManager.cpp` | Group-aware selection logic |
| MODIFY | `src/canvas/BoardSerializer.cpp` | Register GroupObject factory |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_group_object.cpp` | 8 Catch2 tests |

## Architecture Notes

- GroupObject is a CanvasObject subclass that holds child IDs. It does not own the children -- they remain in the Board's object map.
- Group transforms are applied by modifying children's transforms relative to the group's transform origin.
- Selection enters a "group context" stack: clicking selects the outermost group, double-click pushes deeper, Escape pops back.

## Estimated Complexity

**M** -- Group object, commands, selection context management, nested group transforms, 8 tests.
