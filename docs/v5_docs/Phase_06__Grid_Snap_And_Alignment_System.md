# Phase 06 -- Grid, Snap and Alignment System

## Objective

Implement the grid rendering with configurable spacing, snap-to-grid during object placement and movement, smart alignment guides that appear when objects align with each other (edges, centers), and distribute/align actions for the selection. This makes precise layout effortless.

## Prerequisites

- Phase 01 (CanvasTypes, AABB, Point2D)
- Phase 02 (CanvasRenderer, grid rendering hook)
- Phase 03 (CanvasInputManager, tool system)
- Phase 04 (SelectionManager, move/resize operations)

## Feature References (PRD)

- PRD #40: Advanced Alignment (smart guides)
- PRD #41: Grid & Snap (grid + snap-to-grid)

## Data Structures to Implement

### File: `src/canvas/SnapEngine.h`

```cpp
#pragma once

#include "CanvasTypes.h"

#include <optional>
#include <vector>

namespace markamp::canvas
{

/// A guide line that appears when an object aligns with another.
struct AlignmentGuide
{
    enum class Orientation : uint8_t { Horizontal, Vertical };

    Orientation orientation;
    double position;     // Y for horizontal, X for vertical (world coords)
    double start;        // Start of the guide line (world coords)
    double end;          // End of the guide line (world coords)
    double snap_offset;  // How far the object would need to move to snap to this guide
};

/// Result of a snap computation.
struct SnapResult
{
    Point2D snapped_position;    // The position after snapping
    double snap_delta_x{0.0};   // How much X was adjusted
    double snap_delta_y{0.0};   // How much Y was adjusted
    std::vector<AlignmentGuide> active_guides; // Guides to render
    bool snapped_x{false};
    bool snapped_y{false};
};

/// Configuration for the snap/alignment system.
struct SnapConfig
{
    bool snap_to_grid{true};
    bool snap_to_objects{true};
    bool snap_to_guides{true};
    double grid_spacing{20.0};
    double snap_threshold{8.0}; // Max pixel distance to trigger snap
    bool show_alignment_guides{true};
    bool show_distribution_guides{true};
    bool show_spacing_indicators{true};
};

/// Engine that computes snap positions and alignment guides.
class SnapEngine
{
public:
    SnapEngine();

    /// Set the snap configuration.
    auto set_config(const SnapConfig& config) -> void;
    [[nodiscard]] auto config() const -> const SnapConfig&;

    /// Compute the snapped position for a moving object.
    /// @param moving_bounds  AABB of the object(s) being moved, at the proposed position
    /// @param other_bounds   AABBs of all other (non-selected) objects
    /// @param viewport_zoom  Current zoom level (for pixel-based threshold)
    [[nodiscard]] auto compute_snap(const AABB& moving_bounds,
                                     const std::vector<AABB>& other_bounds,
                                     double viewport_zoom) const -> SnapResult;

    /// Compute the snapped position for a single point (e.g., during creation).
    [[nodiscard]] auto snap_point(const Point2D& point, double viewport_zoom) const -> SnapResult;

    /// Snap to grid only (ignoring other objects).
    [[nodiscard]] auto snap_to_grid(const Point2D& point) const -> Point2D;

    // --- Alignment actions for selection ---

    /// Compute positions to align selected objects.
    enum class AlignAction
    {
        Left, CenterH, Right,
        Top, CenterV, Bottom,
        DistributeH, DistributeV,
        SpaceEvenlyH, SpaceEvenlyV
    };

    /// Compute new positions for objects after an alignment action.
    /// Returns a map of ObjectId -> new position (tx, ty).
    [[nodiscard]] auto compute_alignment(
        AlignAction action,
        const std::vector<std::pair<ObjectId, AABB>>& object_bounds) const
        -> std::vector<std::pair<ObjectId, Point2D>>;

private:
    SnapConfig config_;

    [[nodiscard]] auto compute_grid_snap(const AABB& bounds) const
        -> std::pair<double, double>;
    [[nodiscard]] auto compute_object_snap(const AABB& moving,
                                            const std::vector<AABB>& others,
                                            double threshold_world) const
        -> std::vector<AlignmentGuide>;
};

} // namespace markamp::canvas
```

### File: `src/canvas/AlignmentGuideRenderer.h`

```cpp
#pragma once

#include "CanvasTypes.h"
#include "SnapEngine.h"
#include "ViewportTransform.h"

class wxGraphicsContext;

namespace markamp::canvas
{

/// Renders alignment guides, spacing indicators, and grid snap visuals.
class AlignmentGuideRenderer
{
public:
    /// Draw active alignment guides.
    auto render_guides(wxGraphicsContext& gc,
                        const ViewportTransform& viewport,
                        const std::vector<AlignmentGuide>& guides) -> void;

    /// Draw spacing indicators between aligned objects.
    auto render_spacing_indicators(wxGraphicsContext& gc,
                                    const ViewportTransform& viewport,
                                    const std::vector<double>& spacings,
                                    const std::vector<AABB>& object_bounds) -> void;

    auto set_guide_color(const CanvasColor& color) -> void;
    auto set_spacing_color(const CanvasColor& color) -> void;

private:
    CanvasColor guide_color_{255, 0, 128, 200};     // Magenta
    CanvasColor spacing_color_{0, 180, 255, 180};    // Cyan
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `SnapEngine::compute_snap()` -- For the moving AABB, check alignment with each other AABB on 5 reference points per axis: left edge, center, right edge (horizontal); top edge, center, bottom edge (vertical). If any pair aligns within threshold, create an AlignmentGuide and compute the snap offset.

2. `SnapEngine::compute_grid_snap()` -- Round the AABB's top-left corner to the nearest grid intersection: `round(x / grid_spacing) * grid_spacing`.

3. `SnapEngine::compute_alignment(AlignAction::Left)` -- Find the minimum left-edge X among all selected objects. Move each object so its left edge aligns to that X.

4. `SnapEngine::compute_alignment(AlignAction::DistributeH)` -- Sort objects by X. Compute total space. Distribute evenly so gap between each pair is equal.

5. `AlignmentGuideRenderer::render_guides()` -- Draw dashed magenta lines at guide positions. Lines extend from guide.start to guide.end. Transform from world to screen coords.

6. Integration with SelectionManager::update_move(): After computing the proposed position, call `snap_engine_.compute_snap()` to get the snapped position and active guides. Apply snapped position. Store guides for rendering.

## Events to Add

```cpp
// In Events.h

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasSnapSettingsChangedEvent)
bool snap_to_grid{true};
bool snap_to_objects{true};
double grid_spacing{20.0};
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `canvas.snap_to_grid` | bool | `true` | Enable snap to grid |
| `canvas.snap_to_objects` | bool | `true` | Enable snap to other objects |
| `canvas.snap_threshold` | double | `8.0` | Snap distance in screen pixels |
| `canvas.show_alignment_guides` | bool | `true` | Show alignment guide lines |

## Shortcut Registrations

| Shortcut | Action | Context |
|----------|--------|---------|
| Cmd/Ctrl+Shift+G | Toggle grid visibility | canvas |
| Alt+G | Toggle snap to grid | canvas |

## Test Cases

File: `tests/unit/test_snap_engine.cpp`

1. **Snap to grid** -- Point at (23, 47), grid spacing 20. Verify snaps to (20, 40).
2. **Snap to grid disabled** -- snap_to_grid=false, verify no snapping.
3. **Object alignment -- left edge** -- Moving AABB left edge near another's left edge, verify horizontal guide and snap.
4. **Object alignment -- center** -- Moving AABB center near another's center, verify guide.
5. **Object alignment -- both axes** -- Snap X and Y simultaneously, verify both guides.
6. **No snap beyond threshold** -- Objects too far apart, verify no guides generated.
7. **Snap threshold respects zoom** -- At zoom=0.5, threshold in world coords is double the screen threshold.
8. **Align left** -- 3 objects at x=[100, 200, 150]. Align left. Verify all at x=100.
9. **Align center horizontal** -- 3 objects, verify all centered at average center X.
10. **Distribute horizontal** -- 3 objects, distribute, verify equal gaps.
11. **Distribute vertical** -- 3 objects, distribute vertically, verify equal gaps.
12. **Space evenly horizontal** -- 4 objects of different widths, space evenly, verify uniform gaps.

## Acceptance Criteria

- [ ] SnapEngine computes grid snap, object-to-object alignment snap
- [ ] Alignment guides generated when objects align on edges or centers
- [ ] Snap threshold is in screen pixels, scaled by viewport zoom
- [ ] AlignmentGuideRenderer draws dashed guide lines during move/resize
- [ ] Align actions: Left, CenterH, Right, Top, CenterV, Bottom
- [ ] Distribute actions: DistributeH, DistributeV, SpaceEvenlyH, SpaceEvenlyV
- [ ] Grid snap rounds to nearest grid intersection
- [ ] Snap settings configurable and persisted
- [ ] All 12 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/SnapEngine.h` | Snap computation + alignment actions |
| CREATE | `src/canvas/SnapEngine.cpp` | Snap logic implementation |
| CREATE | `src/canvas/AlignmentGuideRenderer.h` | Guide rendering |
| CREATE | `src/canvas/AlignmentGuideRenderer.cpp` | wxGraphicsContext drawing |
| MODIFY | `src/canvas/SelectionManager.cpp` | Integrate SnapEngine into move/resize |
| MODIFY | `src/canvas/CanvasRenderer.cpp` | Add alignment guide rendering pass |
| MODIFY | `src/core/Events.h` | Add CanvasSnapSettingsChangedEvent |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_snap_engine.cpp` | 12 Catch2 tests |

## Architecture Notes

- SnapEngine is a pure computation class with no EventBus or wxWidgets dependencies. It takes AABBs in, returns SnapResult out.
- The snap threshold is specified in screen pixels (constant visual distance regardless of zoom). The engine converts to world units internally using viewport_zoom.
- Alignment guides are computed per-frame during drag operations and passed to AlignmentGuideRenderer.
- The alignment/distribute actions produce position deltas that are executed as MoveObjectsCommands through the UndoRedoStack.
- Object-to-object snap checks all 3 reference points per axis (leading edge, center, trailing edge), resulting in up to 9 comparisons per object pair. For large boards, the Quadtree viewport filter limits the comparison set.

## Estimated Complexity

**M** -- Snap geometry calculations, alignment/distribute algorithms, guide rendering, integration with existing move system, 12 tests.
