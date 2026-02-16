#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"
#include "canvas/TextBox.h" // Re-use TextStyle for embedded text.

#include <memory>
#include <string>

namespace markamp::canvas
{

/// The 12 built-in shape types.
enum class ShapeType : uint8_t
{
    kRectangle,
    kRoundedRectangle,
    kEllipse,
    kDiamond,
    kTriangle,
    kHexagon,
    kStar,
    kArrow,
    kCallout,
    kCylinder,
    kParallelogram,
    kTrapezoid
};

/// Dash pattern for shape strokes.
enum class DashPattern : uint8_t
{
    kSolid,
    kDashed,
    kDotted
};

/// Visual style properties for a shape.
struct ShapeStyle
{
    CanvasColor fill_color{255, 255, 255, 255};
    CanvasColor stroke_color{0, 0, 0, 255};
    double stroke_width{2.0};
    double corner_radius{8.0};
    DashPattern dash_pattern{DashPattern::kSolid};
};

/// A shape object on the canvas — one of 12 shape types with configurable style and embedded text.
class ShapeObject : public CanvasObject
{
public:
    ShapeObject();
    explicit ShapeObject(ShapeType type);

    // ── Shape Type ─────────────────────────────────────────────

    [[nodiscard]] auto shape_type() const -> ShapeType;
    auto set_shape_type(ShapeType type) -> void;

    // ── Style ──────────────────────────────────────────────────

    [[nodiscard]] auto style() const -> const ShapeStyle&;
    auto set_style(const ShapeStyle& style) -> void;

    // ── Dimensions ─────────────────────────────────────────────

    [[nodiscard]] auto width() const -> double;
    [[nodiscard]] auto height() const -> double;
    auto resize(double w, double h) -> void;

    // ── Embedded Text ──────────────────────────────────────────

    [[nodiscard]] auto text() const -> const std::string&;
    auto set_text(const std::string& text) -> void;

    [[nodiscard]] auto text_style() const -> const TextStyle&;
    auto set_text_style(const TextStyle& style) -> void;

    // ── Batch 9 (#49-51) ──────────────────────────────────────────

    /// Returns true if the shape contains embedded text.
    [[nodiscard]] auto has_text() const -> bool;

    /// Approximate area of the shape (width × height for bounding box).
    [[nodiscard]] auto area() const -> double;

    /// Approximate perimeter based on shape type.
    [[nodiscard]] auto perimeter_estimate() const -> double;

    // ── CanvasObject overrides ─────────────────────────────────

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;

private:
    ShapeType shape_type_{ShapeType::kRectangle};
    ShapeStyle style_;
    double width_{120.0};
    double height_{80.0};
    std::string text_;
    TextStyle text_style_;

    static constexpr double kMinSize = 10.0;
};

} // namespace markamp::canvas
