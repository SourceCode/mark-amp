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
};

/// Geometry bounds.
struct Bounds
{
    double x{0.0};
    double y{0.0};
    double width{100.0};
    double height{100.0};
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
};

} // namespace markamp::canvas
