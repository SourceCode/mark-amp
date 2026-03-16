#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Shape primitive type.
enum class ShapeType : uint8_t
{
    kRectangle,
    kEllipse,
    kTriangle,
    kDiamond,
    kStar,
    kArrow,
    kLine,
    kPolygon,
};

/// Shape style for inheritance.
struct ShapeStyle
{
    std::string fill_color{"#FFFFFF"};
    std::string stroke_color{"#000000"};
    double stroke_width{2.0};
    double corner_radius{0.0};
    double opacity{1.0};

    // ── Round 5 Batch 8 (#76-79) ────────────────────────────────

    /// (#76) Whether a fill color is set.
    [[nodiscard]] auto has_fill() const noexcept -> bool
    {
        return !fill_color.empty();
    }

    /// (#77) Whether a stroke color is set.
    [[nodiscard]] auto has_stroke() const noexcept -> bool
    {
        return !stroke_color.empty();
    }

    /// (#78) Whether corner radius is set.
    [[nodiscard]] auto has_radius() const noexcept -> bool
    {
        return corner_radius > 0.0;
    }

    /// (#79) Whether opacity is full.
    [[nodiscard]] auto is_fully_opaque() const noexcept -> bool
    {
        return opacity == 1.0;
    }
};

/// Geometry bounds.
struct Bounds
{
    double x{0.0};
    double y{0.0};
    double width{100.0};
    double height{100.0};

    // ── Round 5 Batch 8 (#80) ───────────────────────────────────

    /// (#80) Area of the bounds.
    [[nodiscard]] auto area() const noexcept -> double
    {
        return width * height;
    }
};

/// Testable model for Shape Tools & Geometry Editing (Phase 43).
///
/// Encapsulates:
/// - Shape primitive set with type and bounds
/// - Geometry handle editing (resize, corner radius)
/// - Constrained draw mode (shift-constrain)
/// - Style inheritance (last-used style carries over)
class ShapeModel
{
public:
    // ── Shape creation ──────────────────────────────────────────────

    void set_type(ShapeType type);
    [[nodiscard]] auto type() const -> ShapeType;

    void set_bounds(Bounds bounds);
    [[nodiscard]] auto bounds() const -> const Bounds&;

    // ── Geometry editing ────────────────────────────────────────────

    void set_corner_radius(double radius);
    [[nodiscard]] auto corner_radius() const -> double;

    /// Apply constrained resize (equal width/height).
    void constrain_proportions();

    // ── Style ───────────────────────────────────────────────────────

    void set_style(ShapeStyle style);
    [[nodiscard]] auto style() const -> const ShapeStyle&;

    /// Record as last-used style for inheritance.
    void commit_style_as_default();
    [[nodiscard]] auto default_style() const -> const ShapeStyle&;
    void reset_default_style();

    // ── Constrained drawing ─────────────────────────────────────────

    void set_constrained(bool constrained);
    [[nodiscard]] auto is_constrained() const -> bool;

    /// Apply constraint to current bounds.
    [[nodiscard]] auto constrained_bounds() const -> Bounds;

private:
    ShapeType type_{ShapeType::kRectangle};
    Bounds bounds_;
    ShapeStyle style_;
    ShapeStyle default_style_;
    bool constrained_{false};

    // ── Round 5 Batch 9 (#81-84) ────────────────────────────────

    /// (#81) Whether shape is a rectangle.
    [[nodiscard]] auto is_rectangle() const noexcept -> bool
    {
        return type_ == ShapeType::kRectangle;
    }

    /// (#82) Whether shape is an ellipse.
    [[nodiscard]] auto is_ellipse() const noexcept -> bool
    {
        return type_ == ShapeType::kEllipse;
    }

    /// (#83) Whether shape is a line.
    [[nodiscard]] auto is_line() const noexcept -> bool
    {
        return type_ == ShapeType::kLine;
    }

    /// (#84) Whether the shape has a visible corner radius.
    [[nodiscard]] auto has_corner_radius() const noexcept -> bool
    {
        return bounds_.width > 0.0 && style_.corner_radius > 0.0;
    }
};

} // namespace markamp::canvas
