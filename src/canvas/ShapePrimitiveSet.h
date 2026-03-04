#pragma once

/**
 * @file ShapePrimitiveSet.h
 * @brief Phase 43 Task 1,4: Shape primitives, defaults, and style inheritance.
 *
 * 8 shape types, defaults (fill/stroke/corner radius), bounding box,
 * center point, and last-used style inheritance for new shapes.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Shape primitive type.
enum class ShapeType : uint8_t
{
    kRect,
    kEllipse,
    kTriangle,
    kDiamond,
    kArrow,
    kLine,
    kPolygon,
    kStar,
};

/// Shape visual defaults.
struct ShapeDefaults
{
    uint32_t fill_color{0xFFFFFFFF};
    uint32_t stroke_color{0x000000FF};
    double stroke_width{2.0};
    double corner_radius{0.0};
    double opacity{1.0};
};

/// A shape primitive with bounds.
struct ShapePrimitive
{
    std::string shape_id;
    ShapeType type{ShapeType::kRect};
    double pos_x{0.0};
    double pos_y{0.0};
    double width{100.0};
    double height{100.0};
    ShapeDefaults defaults;

    /// Get type as string.
    [[nodiscard]] auto type_name() const -> std::string;

    /// Get center point.
    [[nodiscard]] auto center_x() const -> double;
    [[nodiscard]] auto center_y() const -> double;

    /// Get area.
    [[nodiscard]] auto area() const -> double;
};

/**
 * @brief Manages shape primitives and style inheritance.
 */
class ShapePrimitiveSet
{
public:
    ShapePrimitiveSet() = default;

    // ── Shape management ───────────────────────────────────────────

    /// Add a shape.
    void add_shape(const ShapePrimitive& shape);

    /// Remove a shape by ID.
    void remove_shape(const std::string& shape_id);

    /// Find a shape by ID.
    [[nodiscard]] auto find_shape(const std::string& shape_id) const -> const ShapePrimitive*;

    /// Get shape count.
    [[nodiscard]] auto shape_count() const -> int;

    // ── Style inheritance ──────────────────────────────────────────

    /// Set last-used style (for new shape creation).
    void set_last_style(const ShapeDefaults& style);

    /// Get last-used style.
    [[nodiscard]] auto last_style() const -> const ShapeDefaults&;

    /// Create a shape with inherited style.
    [[nodiscard]] auto create_with_style(const std::string& shape_id,
                                         ShapeType type,
                                         double pos_x,
                                         double pos_y,
                                         double width,
                                         double height) const -> ShapePrimitive;

    // ── Queries ────────────────────────────────────────────────────

    /// Get shapes of a given type.
    [[nodiscard]] auto shapes_of_type(ShapeType type) const -> std::vector<const ShapePrimitive*>;

private:
    std::vector<ShapePrimitive> shapes_;
    ShapeDefaults last_style_;
};

} // namespace markamp::canvas
