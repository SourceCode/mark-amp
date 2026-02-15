# Phase 04 -- Object Selection and Transform

## Objective

Implement the interactive object manipulation system: selection handles (resize/rotate), drag-to-move with snapping, multi-select group transforms, and the visual feedback layer (selection outlines, resize handles, rotation widget). This phase makes canvas objects fully interactive.

## Prerequisites

- Phase 01 (CanvasTypes, Transform2D, AABB)
- Phase 02 (CanvasPanel, CanvasRenderer)
- Phase 03 (SelectTool, CanvasInputManager, hit-testing)

## Feature References (PRD)

- PRD #43: Bulk Select/Actions (multi-select transforms)
- PRD #95: Multi-Select Transform Tools (resize/rotate group)
- PRD #40: Advanced Alignment (selection-based alignment)

## Data Structures to Implement

### File: `src/canvas/SelectionManager.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"

#include <functional>
#include <unordered_set>
#include <vector>

namespace markamp::core
{
class EventBus;
} // namespace markamp::core

namespace markamp::canvas
{

/// Handle identifier for resize/rotate grips.
enum class HandleType : uint8_t
{
    TopLeft, TopCenter, TopRight,
    MiddleLeft, MiddleRight,
    BottomLeft, BottomCenter, BottomRight,
    Rotation,  // Circle handle above top-center
    None
};

/// Describes a single interactive handle on the selection box.
struct SelectionHandle
{
    HandleType type{HandleType::None};
    Point2D screen_pos;     // Center of handle in screen coords
    double size{8.0};       // Handle radius in screen pixels
    bool is_hovered{false};
};

/// Manages the selection state and interactive transform operations
/// for selected canvas objects.
class SelectionManager
{
public:
    explicit SelectionManager(core::EventBus& event_bus);

    // --- Selection ---
    auto select(ObjectId id) -> void;
    auto add_to_selection(ObjectId id) -> void;
    auto remove_from_selection(ObjectId id) -> void;
    auto toggle_selection(ObjectId id) -> void;
    auto select_all(const std::vector<ObjectId>& ids) -> void;
    auto clear_selection() -> void;

    [[nodiscard]] auto selected_ids() const -> const std::unordered_set<ObjectId>&;
    [[nodiscard]] auto is_selected(ObjectId id) const -> bool;
    [[nodiscard]] auto selection_count() const -> size_t;
    [[nodiscard]] auto has_selection() const -> bool;

    /// Compute the combined AABB of all selected objects (world space).
    [[nodiscard]] auto selection_bounds(
        const std::unordered_map<ObjectId, std::unique_ptr<CanvasObject>>& objects) const -> AABB;

    // --- Handles ---
    /// Compute handle positions for the current selection bounds.
    [[nodiscard]] auto compute_handles(const AABB& selection_bounds,
                                        const ViewportTransform& viewport) const
        -> std::vector<SelectionHandle>;

    /// Hit-test a screen point against selection handles.
    [[nodiscard]] auto hit_test_handle(const Point2D& screen_pos,
                                        const std::vector<SelectionHandle>& handles) const
        -> HandleType;

    // --- Transform Operations ---

    /// Begin a drag-move operation on selected objects.
    auto begin_move(const Point2D& start_world) -> void;
    /// Update the move operation with a new world position.
    auto update_move(const Point2D& current_world,
                     std::unordered_map<ObjectId, std::unique_ptr<CanvasObject>>& objects) -> void;
    /// Commit the move operation.
    auto end_move() -> void;

    /// Begin a resize operation via a specific handle.
    auto begin_resize(HandleType handle, const Point2D& start_world,
                      const AABB& original_bounds) -> void;
    auto update_resize(const Point2D& current_world,
                       std::unordered_map<ObjectId, std::unique_ptr<CanvasObject>>& objects) -> void;
    auto end_resize() -> void;

    /// Begin a rotation operation.
    auto begin_rotate(const Point2D& start_world, const Point2D& pivot) -> void;
    auto update_rotate(const Point2D& current_world,
                       std::unordered_map<ObjectId, std::unique_ptr<CanvasObject>>& objects) -> void;
    auto end_rotate() -> void;

    /// Cancel the current operation, restoring original positions.
    auto cancel_operation() -> void;

    [[nodiscard]] auto is_operating() const -> bool;

private:
    core::EventBus& event_bus_;
    std::unordered_set<ObjectId> selected_ids_;

    // Operation state
    enum class OpType { None, Move, Resize, Rotate };
    OpType current_op_{OpType::None};
    Point2D op_start_world_;
    Point2D op_pivot_;
    HandleType resize_handle_{HandleType::None};
    AABB original_bounds_;

    // Snapshot of original transforms for undo during operation.
    std::unordered_map<ObjectId, Transform2D> original_transforms_;

    auto snapshot_transforms(
        const std::unordered_map<ObjectId, std::unique_ptr<CanvasObject>>& objects) -> void;
    auto publish_selection_event() -> void;
};

} // namespace markamp::canvas
```

### File: `src/canvas/SelectionRenderer.h`

```cpp
#pragma once

#include "CanvasTypes.h"
#include "SelectionManager.h"
#include "ViewportTransform.h"

class wxGraphicsContext;

namespace markamp::canvas
{

/// Renders selection visuals: outlines, handles, rotation widget, marquee.
class SelectionRenderer
{
public:
    /// Draw selection outline around each selected object.
    auto render_selection_outlines(wxGraphicsContext& gc,
                                    const ViewportTransform& viewport,
                                    const std::vector<AABB>& object_bounds) -> void;

    /// Draw the combined selection box with resize/rotate handles.
    auto render_selection_handles(wxGraphicsContext& gc,
                                   const std::vector<SelectionHandle>& handles,
                                   const AABB& selection_bounds_screen) -> void;

    /// Draw the marquee selection rectangle.
    auto render_marquee(wxGraphicsContext& gc,
                         const Point2D& start_screen,
                         const Point2D& end_screen) -> void;

    /// Draw rotation angle indicator while rotating.
    auto render_rotation_indicator(wxGraphicsContext& gc,
                                    const Point2D& pivot_screen,
                                    double angle_radians) -> void;

    // --- Styling ---
    auto set_selection_color(const CanvasColor& color) -> void;
    auto set_handle_color(const CanvasColor& color) -> void;
    auto set_handle_size(double size) -> void;

private:
    CanvasColor selection_color_{0, 120, 255, 200};
    CanvasColor handle_color_{255, 255, 255, 255};
    CanvasColor handle_border_color_{0, 120, 255, 255};
    CanvasColor marquee_fill_color_{0, 120, 255, 30};
    CanvasColor marquee_border_color_{0, 120, 255, 180};
    double handle_size_{8.0};
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `SelectionManager::begin_move()` -- Snapshot all selected objects' transforms into original_transforms_. Record start world point.

2. `SelectionManager::update_move()` -- Compute delta = current_world - op_start_world_. For each selected object: set transform.tx = original.tx + delta.x, transform.ty = original.ty + delta.y.

3. `SelectionManager::begin_resize()` -- Record which handle is being dragged, the original selection bounds, and snapshot transforms. The resize handle determines which edges move.

4. `SelectionManager::update_resize()` -- Compute the new selection bounds from the drag handle position. Compute scale factors: new_width/old_width, new_height/old_height. Scale each selected object's position relative to the anchor corner (opposite the drag handle) and scale its local size.

5. `SelectionManager::begin_rotate()` -- Record the pivot (center of selection bounds) and the start angle from pivot to mouse.

6. `SelectionManager::update_rotate()` -- Compute angle delta. Rotate each selected object around the pivot: translate to pivot, rotate, translate back.

7. `SelectionRenderer::render_selection_handles()` -- Draw white squares with blue borders at each handle position. Draw the rotation handle as a circle above the top-center.

8. `SelectionRenderer::render_marquee()` -- Draw a semi-transparent blue filled rectangle with a blue dashed border.

9. Integration with SelectTool: On mouse_down, if a handle is hit, begin the corresponding operation. On mouse_move during operation, call update. On mouse_up, call end.

## Events to Add

```cpp
// In Events.h

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasObjectMovedEvent)
std::vector<uint64_t> object_ids;
double delta_x{0.0};
double delta_y{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasObjectResizedEvent)
std::vector<uint64_t> object_ids;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasObjectRotatedEvent)
std::vector<uint64_t> object_ids;
double angle_radians{0.0};
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_selection_manager.cpp`

1. **Single select** -- Select one ID, verify selected_ids contains it.
2. **Multi-select** -- select_all with 3 IDs, verify all present.
3. **Toggle selection** -- Select, toggle same ID, verify deselected.
4. **Clear selection** -- Select several, clear, verify empty.
5. **Selection bounds** -- Select objects at known positions, verify combined AABB.
6. **Handle computation** -- Compute handles for a known AABB, verify 9 handles at correct positions.
7. **Handle hit-test** -- Point near TopLeft handle, verify HandleType::TopLeft returned.
8. **Move operation** -- begin_move, update_move with delta (10, 20), verify objects translated.
9. **Move cancel** -- begin_move, update_move, cancel_operation, verify objects at original positions.
10. **Resize operation** -- begin_resize with BottomRight handle, drag outward, verify objects scaled.
11. **Resize preserves aspect** -- Hold Shift during resize (future), verify aspect ratio maintained.
12. **Rotate operation** -- begin_rotate, update_rotate by 45 degrees, verify object positions rotated around pivot.
13. **Selection event** -- Select object, verify CanvasSelectionChangedEvent published.
14. **Move event** -- Complete a move, verify CanvasObjectMovedEvent published.

## Acceptance Criteria

- [ ] SelectionManager tracks selected object IDs with add/remove/toggle/clear
- [ ] Selection bounds computed as union AABB of all selected objects
- [ ] 9 resize handles + 1 rotation handle computed at correct screen positions
- [ ] Handle hit-testing identifies the correct handle within handle radius
- [ ] Move operation translates all selected objects by mouse delta
- [ ] Resize operation scales selected objects relative to the anchor corner
- [ ] Rotate operation rotates selected objects around the selection center
- [ ] Cancel operation restores original transforms
- [ ] SelectionRenderer draws outlines, handles, marquee, and rotation indicator
- [ ] Events published for selection change, move, resize, rotate
- [ ] All 14 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/SelectionManager.h` | Selection state + transform operations |
| CREATE | `src/canvas/SelectionManager.cpp` | Move/resize/rotate implementation |
| CREATE | `src/canvas/SelectionRenderer.h` | Selection visual rendering |
| CREATE | `src/canvas/SelectionRenderer.cpp` | Drawing outlines, handles, marquee |
| MODIFY | `src/canvas/SelectTool.h` | Integrate SelectionManager |
| MODIFY | `src/canvas/SelectTool.cpp` | Delegate to SelectionManager for transforms |
| MODIFY | `src/canvas/CanvasRenderer.cpp` | Add selection rendering pass after objects |
| MODIFY | `src/core/Events.h` | Add move/resize/rotate events |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_selection_manager.cpp` | 14 Catch2 tests |

## Architecture Notes

- SelectionManager is a non-UI class. It operates on ObjectId sets and Transform2D values. It does not depend on wxWidgets.
- SelectionRenderer is the UI layer that draws the selection visuals using wxGraphicsContext.
- The move/resize/rotate operations use a snapshot+delta pattern: snapshot original transforms on begin, apply incremental deltas on update, commit on end. This enables cancel to restore originals.
- The resize operation must handle corner, edge, and center handles differently. Corner handles resize both dimensions. Edge handles resize one dimension.
- Aspect-ratio-locked resize (Shift held) and center-anchored resize (Alt held) are computed in update_resize by adjusting the anchor point.
- Object-level constraints (minimum size, rotation snap to 15-degree increments when Shift held) are applied in the update functions.

## Estimated Complexity

**L** -- Transform math (resize scaling, rotation around pivot), handle hit-testing, operation state machine, snapshot/restore pattern, selection rendering, 14 tests.
