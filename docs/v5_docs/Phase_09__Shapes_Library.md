# Phase 09 -- Shapes Library

## Objective

Implement a library of geometric shapes (rectangles, rounded rectangles, circles, ellipses, triangles, diamonds, arrows, stars, pentagons, hexagons) as canvas objects with configurable fill, stroke, and optional text labels. Includes a shape picker palette and a shape creation tool.

## Prerequisites

- Phase 01 (CanvasObject, CanvasTypes, Transform2D)
- Phase 02 (CanvasRenderer, IObjectRenderer)
- Phase 03 (CanvasInputManager, tool system)
- Phase 07 (InlineTextEditor for shape labels)

## Feature References (PRD)

- PRD #4: Shapes Library

## Data Structures to Implement

### File: `src/canvas/ShapeObject.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"

#include <optional>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Supported shape types.
enum class ShapeType : uint8_t
{
    Rectangle,
    RoundedRectangle,
    Circle,
    Ellipse,
    Triangle,
    Diamond,       // Rhombus
    ArrowRight,
    ArrowLeft,
    ArrowUp,
    ArrowDown,
    Star,
    Pentagon,
    Hexagon,
    Octagon,
    Parallelogram,
    Trapezoid,
    Cross,
    Heart,
    Cloud,
    Callout,       // Speech bubble
    Cylinder,
    Custom         // User-defined path
};

/// Line/stroke cap style.
enum class StrokeCap : uint8_t { Butt, Round, Square };

/// Line/stroke join style.
enum class StrokeJoin : uint8_t { Miter, Round, Bevel };

/// A geometric shape on the canvas with fill, stroke, and optional text label.
class ShapeObject : public CanvasObject
{
public:
    explicit ShapeObject(ShapeType shape_type = ShapeType::Rectangle);

    // --- Shape ---
    [[nodiscard]] auto shape_type() const -> ShapeType;
    auto set_shape_type(ShapeType type) -> void;

    // --- Dimensions ---
    [[nodiscard]] auto width() const -> double;
    [[nodiscard]] auto height() const -> double;
    auto set_dimensions(double w, double h) -> void;

    // --- Fill ---
    [[nodiscard]] auto fill_color() const -> const CanvasColor&;
    auto set_fill_color(const CanvasColor& color) -> void;
    [[nodiscard]] auto has_fill() const -> bool;
    auto set_has_fill(bool enabled) -> void;

    // --- Stroke ---
    [[nodiscard]] auto stroke_color() const -> const CanvasColor&;
    auto set_stroke_color(const CanvasColor& color) -> void;
    [[nodiscard]] auto stroke_width() const -> double;
    auto set_stroke_width(double w) -> void;
    [[nodiscard]] auto stroke_cap() const -> StrokeCap;
    auto set_stroke_cap(StrokeCap cap) -> void;
    [[nodiscard]] auto stroke_join() const -> StrokeJoin;
    auto set_stroke_join(StrokeJoin join) -> void;
    [[nodiscard]] auto stroke_dash_pattern() const -> const std::vector<double>&;
    auto set_stroke_dash_pattern(const std::vector<double>& pattern) -> void;

    // --- Label ---
    [[nodiscard]] auto label() const -> const std::string&;
    auto set_label(const std::string& label) -> void;
    [[nodiscard]] auto label_font_size() const -> double;
    auto set_label_font_size(double size) -> void;

    // --- Rounded rectangle corner radius ---
    [[nodiscard]] auto corner_radius() const -> double;
    auto set_corner_radius(double r) -> void;

    // --- Star points ---
    [[nodiscard]] auto star_points() const -> int;
    auto set_star_points(int n) -> void;
    [[nodiscard]] auto star_inner_radius_ratio() const -> double;
    auto set_star_inner_radius_ratio(double ratio) -> void;

    // --- Connection points (for connectors) ---
    /// Get connection point positions on the shape perimeter.
    [[nodiscard]] auto connection_points() const -> std::vector<Point2D>;

    // --- CanvasObject overrides ---
    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    ShapeType shape_type_;
    double width_{100.0};
    double height_{100.0};
    CanvasColor fill_color_{66, 133, 244, 255};    // Blue
    CanvasColor stroke_color_{50, 100, 200, 255};
    double stroke_width_{2.0};
    bool has_fill_{true};
    StrokeCap stroke_cap_{StrokeCap::Round};
    StrokeJoin stroke_join_{StrokeJoin::Round};
    std::vector<double> stroke_dash_pattern_; // Empty = solid
    std::string label_;
    double label_font_size_{14.0};
    double corner_radius_{12.0};
    int star_points_{5};
    double star_inner_radius_ratio_{0.4};
};

} // namespace markamp::canvas
```

### File: `src/canvas/ShapeRenderer.h`

```cpp
#pragma once

#include "CanvasRenderer.h"
#include "ShapeObject.h"

class wxGraphicsPath;

namespace markamp::canvas
{

class ShapeRenderer : public IObjectRenderer
{
public:
    auto render(wxGraphicsContext& gc, const CanvasObject& obj,
                const ViewportTransform& vp) -> void override;
    [[nodiscard]] auto handles_type() const -> CanvasObjectType override;

    /// Build the wxGraphicsPath for a given ShapeType and dimensions.
    [[nodiscard]] static auto build_shape_path(wxGraphicsContext& gc,
                                                ShapeType type,
                                                double width, double height,
                                                double corner_radius,
                                                int star_points,
                                                double star_inner_ratio)
        -> wxGraphicsPath;

private:
    auto apply_stroke_style(wxGraphicsContext& gc, const ShapeObject& shape,
                             double zoom) -> void;
    auto draw_label(wxGraphicsContext& gc, const ShapeObject& shape,
                     const AABB& screen_bounds, double zoom) -> void;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `ShapeRenderer::build_shape_path()` -- Switch on ShapeType. Rectangle: AddRectangle. RoundedRectangle: AddRoundedRectangle. Circle: AddEllipse with equal width/height. Triangle: MoveTo/AddLineToPoint for 3 vertices. Star: compute inner/outer vertices in a loop. Hexagon: 6 evenly spaced vertices.

2. `ShapeObject::connection_points()` -- Return 4 cardinal points (top, right, bottom, left center of bounding box) plus optional midpoints. These are used by connectors in Phase 10.

3. Shape creation tool: Extend the tool system with a ShapeTool that shows a shape type picker and lets users drag to define size. The currently selected shape type is stored in the tool.

4. `apply_stroke_style()` -- Set pen width, color, dash pattern, cap, and join on the wxGraphicsContext before stroking the path.

## Test Cases

File: `tests/unit/test_shape_object.cpp`

1. **Default construction** -- Verify Rectangle type, 100x100, blue fill.
2. **Shape type change** -- Set to Circle, verify getter.
3. **Fill/stroke** -- Set fill color, stroke width, verify.
4. **Dash pattern** -- Set dash pattern [5, 3], verify.
5. **Label** -- Set label text, verify.
6. **Star points** -- Set star_points=6, verify.
7. **Connection points** -- Rectangle 100x100 at origin. Verify 4 connection points at (50,0), (100,50), (50,100), (0,50).
8. **JSON round-trip** -- All fields, serialize/deserialize, verify.
9. **Clone** -- Clone shape, verify independent.
10. **All shape types** -- Iterate all ShapeType values, create each, verify no crash.
11. **Path building** -- Build path for each ShapeType, verify non-empty path (basic smoke test).

## Acceptance Criteria

- [ ] ShapeObject supports 22 shape types with fill, stroke, dash patterns, labels
- [ ] ShapeRenderer builds wxGraphicsPath for each shape type
- [ ] Connection points computed for connector integration
- [ ] Shape picker palette in toolbar for type selection
- [ ] Drag-to-create with shape tool
- [ ] Stroke styling: width, color, dash pattern, cap, join
- [ ] Label text centered on shape
- [ ] JSON serialization round-trips all fields
- [ ] All 11 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/ShapeObject.h` | Shape data model |
| CREATE | `src/canvas/ShapeObject.cpp` | Shape implementation |
| CREATE | `src/canvas/ShapeRenderer.h` | Shape rendering |
| CREATE | `src/canvas/ShapeRenderer.cpp` | Path building + drawing |
| CREATE | `src/canvas/ShapeTool.h` | Shape creation tool |
| CREATE | `src/canvas/ShapeTool.cpp` | Tool implementation |
| MODIFY | `src/canvas/BoardSerializer.cpp` | Register ShapeObject factory |
| MODIFY | `src/canvas/CanvasInputManager.cpp` | Register ShapeTool |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_shape_object.cpp` | 11 Catch2 tests |

## Architecture Notes

- ShapeRenderer::build_shape_path() is a static method so it can be reused by other renderers (e.g., diagram shapes in Phase 34).
- Connection points are defined in local coordinates, then transformed to world space. Connectors (Phase 10) query these points.
- The shape picker palette is rendered as a canvas overlay (not a wxWidgets popup) for consistency with the infinite canvas UX.
- Custom shapes (ShapeType::Custom) will be added in a future phase with user-defined SVG paths.

## Estimated Complexity

**L** -- 22 shape types with path geometry, renderer with fill/stroke/dash/label, connection points, creation tool, 11 tests.
