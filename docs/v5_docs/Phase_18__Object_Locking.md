# Phase 18 -- Object Locking

## Objective

Implement object locking: a toggle that prevents an object from being moved, resized, rotated, or edited. Locked objects display a visual indicator (lock icon overlay) and are excluded from drag operations but can still be selected for inspection.

## Prerequisites

- Phase 01 (CanvasObject with is_locked field)
- Phase 03 (CanvasInputManager, hit-testing)
- Phase 04 (SelectionManager)
- Phase 05 (UndoRedoStack)

## Feature References (PRD)

- PRD #57: Object Locking

## Data Structures to Implement

### File: `src/canvas/LockCommands.h`

```cpp
#pragma once

#include "CanvasCommands.h"

namespace markamp::canvas
{

class LockObjectsCommand : public ICanvasCommand
{
public:
    LockObjectsCommand(Board& board, const std::vector<ObjectId>& ids, bool lock);
    auto execute() -> void override;
    auto undo() -> void override;
    [[nodiscard]] auto description() const -> std::string override;

private:
    Board& board_;
    std::vector<ObjectId> ids_;
    bool lock_;
    std::vector<bool> previous_states_;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `SelectionManager::update_move()` -- Check if any selected object is locked. If so, skip it (or block the entire operation if all are locked).

2. Lock icon rendering: After rendering an object, if it is locked, draw a small lock icon in the corner of its bounds.

3. Right-click context menu: Add "Lock" / "Unlock" option.

4. InlineTextEditor: Prevent editing if the target object is locked.

## Shortcut Registrations

| Shortcut | Action | Context |
|----------|--------|---------|
| Cmd/Ctrl+L | Toggle lock on selection | canvas |

## Test Cases

File: `tests/unit/test_object_locking.cpp`

1. **Lock toggle** -- Lock object, verify is_locked=true.
2. **Locked object blocks move** -- Lock object, attempt move via SelectionManager, verify position unchanged.
3. **Locked object blocks resize** -- Lock, attempt resize, verify dimensions unchanged.
4. **Locked object blocks edit** -- Lock, attempt InlineTextEditor activation, verify blocked.
5. **Locked object is selectable** -- Lock, click, verify selected.
6. **Lock command undo** -- Lock, undo, verify unlocked.
7. **Batch lock** -- Lock multiple objects, verify all locked.

## Acceptance Criteria

- [ ] Lock toggle on any canvas object
- [ ] Locked objects cannot be moved, resized, rotated, or edited
- [ ] Locked objects can be selected for inspection
- [ ] Lock icon overlay on locked objects
- [ ] Lock/unlock via context menu and Cmd+L
- [ ] Lock command is undoable
- [ ] All 7 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/LockCommands.h` | Lock/unlock command |
| CREATE | `src/canvas/LockCommands.cpp` | Command implementation |
| MODIFY | `src/canvas/SelectionManager.cpp` | Skip locked objects in transforms |
| MODIFY | `src/canvas/InlineTextEditor.cpp` | Block editing on locked objects |
| MODIFY | `src/canvas/CanvasRenderer.cpp` | Draw lock icon overlay |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_object_locking.cpp` | 7 Catch2 tests |

## Architecture Notes

- Locking is enforced in the SelectionManager (for transforms) and InlineTextEditor (for text editing), not at the CanvasObject level.
- The lock icon is rendered as a small padlock SVG/path at the top-right corner of the object's screen bounds, at a fixed screen size regardless of zoom.

## Estimated Complexity

**S** -- Lock command, guard logic in selection/editor, lock icon rendering, 7 tests.
