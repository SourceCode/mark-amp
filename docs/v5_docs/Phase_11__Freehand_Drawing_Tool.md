# Phase 11 -- Freehand Drawing Tool

## Objective

Implement pen/brush freehand drawing on the canvas with path smoothing, variable stroke width (pressure sensitivity when available), configurable pen color and size, and an eraser tool. Freehand strokes are stored as CanvasObject instances and participate in selection, serialization, and undo/redo.

## Prerequisites

- Phase 01 (CanvasObject, Point2D, AABB)
- Phase 02 (CanvasRenderer, IObjectRenderer)
- Phase 03 (CanvasInputManager, ICanvasTool)
- Phase 05 (UndoRedoStack)

## Feature References (PRD)

- PRD #13: Freehand Drawing

## Data Structures to Implement

### File: `src/canvas/FreehandPathObject.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"

#include <vector>

namespace markamp::canvas
{

/// A single sample point in a freehand stroke.
struct PathSample
{
    double x{0.0};
    double y{0.0};
    double pressure{1.0};  // 0.0-1.0, from pen tablet or default 1.0 for mouse
    double timestamp{0.0}; // Milliseconds from stroke start
};

/// A freehand-drawn path on the canvas.
class FreehandPathObject : public CanvasObject
{
public:
    FreehandPathObject();

    // --- Path data ---
    [[nodiscard]] auto samples() const -> const std::vector<PathSample>&;
    auto add_sample(const PathSample& sample) -> void;
    auto set_samples(const std::vector<PathSample>& samples) -> void;

    /// Get a smoothed version of the path (Catmull-Rom or similar).
    [[nodiscard]] auto smoothed_points(double smoothing_factor = 0.5) const
        -> std::vector<Point2D>;

    /// Simplify the path using Ramer-Douglas-Peucker algorithm.
    auto simplify(double tolerance = 1.0) -> void;

    // --- Stroke style ---
    [[nodiscard]] auto stroke_color() const -> const CanvasColor&;
    auto set_stroke_color(const CanvasColor& color) -> void;
    [[nodiscard]] auto stroke_width() const -> double;
    auto set_stroke_width(double w) -> void;
    [[nodiscard]] auto use_pressure() const -> bool;
    auto set_use_pressure(bool enabled) -> void;
    [[nodiscard]] auto smoothing() const -> double;
    auto set_smoothing(double factor) -> void;

    /// Pen opacity (separate from object opacity).
    [[nodiscard]] auto pen_opacity() const -> double;
    auto set_pen_opacity(double opacity) -> void;

    // --- CanvasObject overrides ---
    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::vector<PathSample> samples_;
    CanvasColor stroke_color_{0, 0, 0, 255};
    double stroke_width_{3.0};
    bool use_pressure_{true};
    double smoothing_{0.5};
    double pen_opacity_{1.0};
};

} // namespace markamp::canvas
```

### File: `src/canvas/FreehandPathRenderer.h`

```cpp
#pragma once

#include "CanvasRenderer.h"

namespace markamp::canvas
{

class FreehandPathRenderer : public IObjectRenderer
{
public:
    auto render(wxGraphicsContext& gc, const CanvasObject& obj,
                const ViewportTransform& vp) -> void override;
    [[nodiscard]] auto handles_type() const -> CanvasObjectType override;

private:
    /// Draw a variable-width stroke using triangulated quads along the path.
    auto draw_variable_width_stroke(wxGraphicsContext& gc,
                                     const std::vector<PathSample>& samples,
                                     double base_width, double zoom,
                                     const CanvasColor& color) -> void;

    /// Draw a constant-width stroke using wxGraphicsPath.
    auto draw_constant_width_stroke(wxGraphicsContext& gc,
                                     const std::vector<Point2D>& points,
                                     double width, double zoom,
                                     const CanvasColor& color) -> void;
};

} // namespace markamp::canvas
```

### File: `src/canvas/DrawTool.h`

```cpp
#pragma once

#include "CanvasTool.h"
#include "CanvasTypes.h"
#include "FreehandPathObject.h"

#include <memory>

namespace markamp::canvas
{

class Board;
class UndoRedoStack;

/// Freehand drawing tool. Mouse down starts a stroke, mouse move adds samples,
/// mouse up commits the stroke as a FreehandPathObject.
class DrawTool : public ICanvasTool
{
public:
    DrawTool(Board& board, UndoRedoStack& undo_stack);

    [[nodiscard]] auto tool_mode() const -> ToolMode override { return ToolMode::Draw; }
    [[nodiscard]] auto tool_name() const -> std::string override { return "Draw"; }
    [[nodiscard]] auto cursor_name() const -> std::string override { return "crosshair"; }

    auto on_activate() -> void override;
    auto on_deactivate() -> void override;
    auto on_mouse_down(const CanvasInputEvent& event) -> bool override;
    auto on_mouse_move(const CanvasInputEvent& event) -> bool override;
    auto on_mouse_up(const CanvasInputEvent& event) -> bool override;
    auto on_mouse_double_click(const CanvasInputEvent& event) -> bool override;
    auto on_scroll(const CanvasInputEvent& event) -> bool override;
    auto on_key_down(int key_code, int modifiers) -> bool override;
    auto on_key_up(int key_code, int modifiers) -> bool override;
    auto render_overlay(wxGraphicsContext& gc, const ViewportTransform& viewport) -> void override;

    // --- Configuration ---
    auto set_pen_color(const CanvasColor& color) -> void;
    auto set_pen_width(double width) -> void;
    auto set_smoothing(double factor) -> void;
    auto set_use_pressure(bool enabled) -> void;

private:
    Board& board_;
    UndoRedoStack& undo_stack_;
    std::unique_ptr<FreehandPathObject> current_stroke_;
    CanvasColor pen_color_{0, 0, 0, 255};
    double pen_width_{3.0};
    double smoothing_{0.5};
    bool use_pressure_{true};
};

} // namespace markamp::canvas
```

### File: `src/canvas/EraserTool.h`

```cpp
#pragma once

#include "CanvasTool.h"

namespace markamp::canvas
{

class Board;
class UndoRedoStack;
class Quadtree;

/// Eraser tool that removes or splits freehand strokes under the cursor.
class EraserTool : public ICanvasTool
{
public:
    EraserTool(Board& board, UndoRedoStack& undo_stack, Quadtree& spatial_index);

    [[nodiscard]] auto tool_mode() const -> ToolMode override { return ToolMode::Eraser; }
    [[nodiscard]] auto tool_name() const -> std::string override { return "Eraser"; }
    [[nodiscard]] auto cursor_name() const -> std::string override { return "eraser"; }

    auto on_activate() -> void override;
    auto on_deactivate() -> void override;
    auto on_mouse_down(const CanvasInputEvent& event) -> bool override;
    auto on_mouse_move(const CanvasInputEvent& event) -> bool override;
    auto on_mouse_up(const CanvasInputEvent& event) -> bool override;
    auto on_mouse_double_click(const CanvasInputEvent& event) -> bool override;
    auto on_scroll(const CanvasInputEvent& event) -> bool override;
    auto on_key_down(int key_code, int modifiers) -> bool override;
    auto on_key_up(int key_code, int modifiers) -> bool override;
    auto render_overlay(wxGraphicsContext& gc, const ViewportTransform& viewport) -> void override;

    auto set_eraser_size(double size) -> void;

private:
    Board& board_;
    UndoRedoStack& undo_stack_;
    Quadtree& spatial_index_;
    double eraser_size_{20.0};
    bool erasing_{false};
    std::vector<ObjectId> erased_ids_; // Accumulate for compound undo
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `FreehandPathObject::smoothed_points()` -- Apply Catmull-Rom spline interpolation to raw samples. Insert interpolated points between each pair of samples for a smooth curve.

2. `FreehandPathObject::simplify()` -- Ramer-Douglas-Peucker: recursively remove points that are within `tolerance` of the line between their neighbors. Reduces point count while preserving shape.

3. `FreehandPathRenderer::draw_variable_width_stroke()` -- For each segment, compute perpendicular offsets based on pressure * base_width. Build a triangle strip (quad per segment) with varying width. Fill the strip.

4. `DrawTool::on_mouse_down()` -- Create new FreehandPathObject with current pen settings. Add first sample.

5. `DrawTool::on_mouse_move()` -- Add sample to current_stroke_ with world position and pressure. Trigger redraw.

6. `DrawTool::on_mouse_up()` -- Simplify the stroke. Add to board via AddObjectCommand. Clear current_stroke_.

7. `DrawTool::render_overlay()` -- Render the in-progress stroke directly (before it's committed to the board).

8. `EraserTool::on_mouse_move()` -- Query Quadtree for objects near cursor (within eraser_size). For FreehandPathObjects, remove them. For other objects, skip. Accumulate into compound undo.

## Shortcut Registrations

| Shortcut | Action | Context |
|----------|--------|---------|
| P | Switch to Draw tool | canvas |
| E | Switch to Eraser tool | canvas |
| [ | Decrease pen/eraser size | canvas |
| ] | Increase pen/eraser size | canvas |

## Test Cases

File: `tests/unit/test_freehand_path.cpp`

1. **Default construction** -- Verify black color, 3px width, empty samples.
2. **Add samples** -- Add 10 samples, verify count.
3. **Smoothed points** -- Add zigzag samples, verify smoothed version has reduced sharp angles.
4. **Simplify** -- Add 100 colinear points, simplify, verify reduced to ~2 points.
5. **Simplify preserves shape** -- Add curve samples, simplify with low tolerance, verify shape preserved.
6. **Variable width** -- Add samples with varying pressure, verify use_pressure flag.
7. **Local bounds** -- Add samples, verify bounds enclose all points with stroke width margin.
8. **JSON round-trip** -- Full path with 50 samples, serialize/deserialize, verify.
9. **Clone** -- Clone path, verify independent copy.
10. **Pen opacity** -- Set pen_opacity=0.5, verify getter.

## Acceptance Criteria

- [ ] FreehandPathObject stores raw samples with position, pressure, timestamp
- [ ] Path smoothing via Catmull-Rom spline interpolation
- [ ] Path simplification via Ramer-Douglas-Peucker algorithm
- [ ] Variable-width stroke rendering based on pressure
- [ ] DrawTool captures mouse movement as stroke samples
- [ ] EraserTool removes freehand strokes under cursor
- [ ] Pen size adjustable via [ and ] keys
- [ ] Stroke committed to board with undo/redo support
- [ ] In-progress stroke rendered as overlay before commit
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/FreehandPathObject.h` | Freehand path data model |
| CREATE | `src/canvas/FreehandPathObject.cpp` | Path implementation with smoothing/simplification |
| CREATE | `src/canvas/FreehandPathRenderer.h` | Path rendering |
| CREATE | `src/canvas/FreehandPathRenderer.cpp` | Variable-width stroke drawing |
| CREATE | `src/canvas/DrawTool.h` | Drawing tool |
| CREATE | `src/canvas/DrawTool.cpp` | Tool implementation |
| CREATE | `src/canvas/EraserTool.h` | Eraser tool |
| CREATE | `src/canvas/EraserTool.cpp` | Eraser implementation |
| MODIFY | `src/canvas/BoardSerializer.cpp` | Register FreehandPathObject factory |
| MODIFY | `src/canvas/CanvasInputManager.cpp` | Register DrawTool, EraserTool |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_freehand_path.cpp` | 10 Catch2 tests |

## Architecture Notes

- Path smoothing and simplification are O(n) and O(n log n) respectively, suitable for real-time use.
- For very long strokes (>1000 points), simplification runs automatically on mouse_up to reduce storage and rendering cost.
- The variable-width stroke is rendered as a filled polygon (triangle strip), not as a thick line. This allows smooth width transitions.
- The EraserTool uses proximity testing rather than exact path intersection. Objects whose AABB overlaps the eraser circle are candidates; for FreehandPaths, a more precise point-to-path distance check may be added as a refinement.

## Estimated Complexity

**L** -- Spline interpolation, Ramer-Douglas-Peucker algorithm, variable-width stroke rendering, two tools, 10 tests.
