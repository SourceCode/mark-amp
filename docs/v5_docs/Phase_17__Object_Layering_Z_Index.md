# Phase 17 -- Object Layering / Z-Index

## Objective

Implement a layer management panel and z-index control commands: bring to front, send to back, bring forward, send backward. Includes a visual layer panel showing all objects sorted by z-order, with drag-to-reorder, visibility toggles, and lock toggles per object.

## Prerequisites

- Phase 01 (CanvasObject with z_index)
- Phase 02 (CanvasRenderer with z-order sorting)
- Phase 05 (Board z-index management, UndoRedoStack)

## Feature References (PRD)

- PRD #58: Object Layering (Z-Index)

## Data Structures to Implement

### File: `src/canvas/LayerPanel.h`

```cpp
#pragma once

#include "CanvasTypes.h"

#include <functional>
#include <string>
#include <vector>

class wxGraphicsContext;

namespace markamp::canvas
{

struct LayerEntry
{
    ObjectId id{kInvalidObjectId};
    std::string name;
    CanvasObjectType type;
    int z_index{0};
    bool visible{true};
    bool locked{false};
    bool selected{false};
};

class LayerPanel
{
public:
    using OnReorderCallback = std::function<void(ObjectId id, int new_z_index)>;
    using OnVisibilityCallback = std::function<void(ObjectId id, bool visible)>;
    using OnLockCallback = std::function<void(ObjectId id, bool locked)>;
    using OnSelectCallback = std::function<void(ObjectId id)>;

    auto set_entries(const std::vector<LayerEntry>& entries) -> void;
    auto render(wxGraphicsContext& gc, double panel_x, double panel_y,
                 double panel_width, double panel_height) -> void;

    auto handle_click(double x, double y) -> bool;
    auto handle_drag(double x, double y) -> bool;
    auto handle_drop() -> bool;

    auto set_on_reorder(OnReorderCallback cb) -> void;
    auto set_on_visibility(OnVisibilityCallback cb) -> void;
    auto set_on_lock(OnLockCallback cb) -> void;
    auto set_on_select(OnSelectCallback cb) -> void;

    [[nodiscard]] auto is_visible() const -> bool;
    auto set_visible(bool visible) -> void;

private:
    std::vector<LayerEntry> entries_;
    bool visible_{false};
    int dragging_index_{-1};
    double scroll_offset_{0.0};
    OnReorderCallback on_reorder_;
    OnVisibilityCallback on_visibility_;
    OnLockCallback on_lock_;
    OnSelectCallback on_select_;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `Board::bring_to_front()` / `send_to_back()` / `bring_forward()` / `send_backward()` -- Already defined in Phase 05 Board class. Implement as ReorderZCommand.

2. `LayerPanel::render()` -- Draw a scrollable list of layer entries. Each row shows: visibility eye icon, lock icon, object type icon, name, z-index. Selected entries highlighted.

3. `LayerPanel::handle_drag()` -- Drag-to-reorder: move an entry to a new z-index position.

4. Right-click context menu on canvas objects: add z-ordering options (Bring to Front, Send to Back, Bring Forward, Send Backward).

## Shortcut Registrations

| Shortcut | Action | Context |
|----------|--------|---------|
| Cmd/Ctrl+] | Bring Forward | canvas |
| Cmd/Ctrl+[ | Send Backward | canvas |
| Cmd/Ctrl+Shift+] | Bring to Front | canvas |
| Cmd/Ctrl+Shift+[ | Send to Back | canvas |

## Test Cases

File: `tests/unit/test_layer_panel.cpp`

1. **Bring to front** -- Object at z=3, bring to front among [1,3,5]. Verify z > 5.
2. **Send to back** -- Object at z=3, send to back among [1,3,5]. Verify z < 1.
3. **Bring forward** -- Object at z=1, bring forward. Verify z > previous next object's z.
4. **Send backward** -- Object at z=5, send backward. Verify z < previous prior object's z.
5. **Layer entries** -- Verify entries are sorted by z-index descending (top first).
6. **Visibility toggle** -- Toggle visibility, verify callback fired.
7. **Lock toggle** -- Toggle lock, verify callback fired.
8. **Reorder undo** -- Reorder, undo, verify original z-order restored.

## Acceptance Criteria

- [ ] Bring to front / send to back / forward / backward commands
- [ ] LayerPanel shows all objects sorted by z-index with icons
- [ ] Drag-to-reorder in layer panel
- [ ] Visibility and lock toggles per object
- [ ] Right-click context menu with z-order options
- [ ] All z-order operations are undoable
- [ ] Keyboard shortcuts for z-ordering
- [ ] All 8 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/LayerPanel.h` | Layer panel UI |
| CREATE | `src/canvas/LayerPanel.cpp` | Layer panel implementation |
| MODIFY | `src/canvas/Board.cpp` | Implement z-order operations |
| MODIFY | `src/canvas/CanvasCommands.h` | Add ReorderZCommand |
| MODIFY | `src/canvas/CanvasCommands.cpp` | ReorderZCommand implementation |
| MODIFY | `src/ui/CanvasPanel.cpp` | Add LayerPanel rendering, context menu |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_layer_panel.cpp` | 8 Catch2 tests |

## Architecture Notes

- Z-index values are sparse (not contiguous). bring_forward swaps z-indices with the next higher object.
- The layer panel is a canvas-rendered overlay panel (not a wxWidgets panel), keeping it within the canvas coordinate system.
- Visibility toggles set the CanvasObject::visible flag. The renderer skips invisible objects.

## Estimated Complexity

**M** -- Layer panel rendering with drag-reorder, z-order commands, context menu integration, 8 tests.
