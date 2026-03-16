#pragma once

#include <cmath>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Unique identifier for canvas objects. 64-bit monotonic counter.
using ObjectId = uint64_t;
constexpr ObjectId kInvalidObjectId = 0;

/// 2D point in world or screen coordinates.
struct Point2D
{
    double x{0.0};
    double y{0.0};

    [[nodiscard]] auto distance_to(const Point2D& other) const -> double
    {
        const double dx = x - other.x;
        const double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    auto operator+(const Point2D& rhs) const -> Point2D
    {
        return {x + rhs.x, y + rhs.y};
    }
    auto operator-(const Point2D& rhs) const -> Point2D
    {
        return {x - rhs.x, y - rhs.y};
    }
    auto operator*(double s) const -> Point2D
    {
        return {x * s, y * s};
    }
    auto operator==(const Point2D& rhs) const -> bool = default;

    // ── Batch 1 (#1-5) ────────────────────────────────────────────

    /// (#1) Distance from origin.
    [[nodiscard]] auto length() const noexcept -> double
    {
        return std::sqrt(x * x + y * y);
    }

    /// (#2) Unit vector (returns zero vector if length is ~0).
    [[nodiscard]] auto normalized() const noexcept -> Point2D
    {
        const double len = length();
        if (len < 1e-12) return {0.0, 0.0};
        return {x / len, y / len};
    }

    /// (#3) Dot product with another point/vector.
    [[nodiscard]] auto dot(const Point2D& other) const noexcept -> double
    {
        return x * other.x + y * other.y;
    }

    /// (#4) Midpoint between this and another point.
    [[nodiscard]] auto midpoint(const Point2D& other) const noexcept -> Point2D
    {
        return {(x + other.x) * 0.5, (y + other.y) * 0.5};
    }

    /// (#5) Whether both components are zero.
    [[nodiscard]] auto is_zero() const noexcept -> bool
    {
        return x == 0.0 && y == 0.0;
    }
};

/// 2D size (width, height).
struct Size2D
{
    double width{0.0};
    double height{0.0};

    /// Total area (width × height).
    [[nodiscard]] auto area() const noexcept -> double
    {
        return width * height;
    }

    /// Whether either dimension is zero or negative.
    [[nodiscard]] auto is_empty() const noexcept -> bool
    {
        return width <= 0.0 || height <= 0.0;
    }
};

/// 2D affine transform: translation, rotation (radians), uniform scale.
struct Transform2D
{
    double tx{0.0};       // Translation X (world space)
    double ty{0.0};       // Translation Y (world space)
    double rotation{0.0}; // Rotation in radians
    double scale_x{1.0};
    double scale_y{1.0};

    /// Apply this transform to a point.
    [[nodiscard]] auto apply(const Point2D& p) const -> Point2D;

    /// Inverse-transform a point (screen -> local).
    [[nodiscard]] auto inverse(const Point2D& p) const -> Point2D;

    /// Compose with another transform (this * other).
    [[nodiscard]] auto compose(const Transform2D& other) const -> Transform2D;

    /// Identity transform.
    [[nodiscard]] static auto identity() -> Transform2D;

    /// Whether this transform is the identity (no translation, rotation, or non-unit scale).
    [[nodiscard]] auto is_identity() const noexcept -> bool
    {
        return tx == 0.0 && ty == 0.0 && rotation == 0.0 && scale_x == 1.0 && scale_y == 1.0;
    }

    /// Whether rotation is non-zero.
    [[nodiscard]] auto has_rotation() const noexcept -> bool
    {
        return rotation != 0.0;
    }

    // ── Batch 1 (#8-9) ────────────────────────────────────────────

    /// (#8) Whether any scale factor differs from 1.0.
    [[nodiscard]] auto has_scale() const noexcept -> bool
    {
        return scale_x != 1.0 || scale_y != 1.0;
    }

    /// (#9) Translation as a Point2D.
    [[nodiscard]] auto translation() const noexcept -> Point2D
    {
        return {tx, ty};
    }
};

/// Axis-aligned bounding box.
struct AABB
{
    double min_x{std::numeric_limits<double>::max()};
    double min_y{std::numeric_limits<double>::max()};
    double max_x{std::numeric_limits<double>::lowest()};
    double max_y{std::numeric_limits<double>::lowest()};

    [[nodiscard]] auto width() const -> double
    {
        return max_x - min_x;
    }
    [[nodiscard]] auto height() const -> double
    {
        return max_y - min_y;
    }
    [[nodiscard]] auto center() const -> Point2D;
    [[nodiscard]] auto contains(const Point2D& p) const -> bool;
    [[nodiscard]] auto intersects(const AABB& other) const -> bool;
    [[nodiscard]] auto merged(const AABB& other) const -> AABB;
    [[nodiscard]] auto expanded(double margin) const -> AABB;
    [[nodiscard]] auto is_valid() const -> bool;

    /// Total area of the bounding box.
    [[nodiscard]] auto area() const noexcept -> double
    {
        return width() * height();
    }

    /// Create from center + size.
    [[nodiscard]] static auto from_center_size(const Point2D& center, const Size2D& size) -> AABB;

    // ── Batch 1 (#6-7) ────────────────────────────────────────────

    /// (#6) Clamp a point to lie inside this AABB.
    [[nodiscard]] auto clamp_point(const Point2D& p) const noexcept -> Point2D
    {
        return {std::fmax(min_x, std::fmin(p.x, max_x)),
                std::fmax(min_y, std::fmin(p.y, max_y))};
    }

    /// (#7) Aspect ratio (width / height). Returns 0 if height is ~0.
    [[nodiscard]] auto aspect_ratio() const noexcept -> double
    {
        const double h = height();
        if (h < 1e-12) return 0.0;
        return width() / h;
    }
};

/// RGBA color for canvas objects (0-255 per channel).
struct CanvasColor
{
    uint8_t r{0};
    uint8_t g{0};
    uint8_t b{0};
    uint8_t a{255};

    [[nodiscard]] static auto from_hex(const std::string& hex) -> CanvasColor;
    [[nodiscard]] auto to_hex() const -> std::string;
    auto operator==(const CanvasColor& rhs) const -> bool = default;

    /// Whether the color is fully opaque (alpha == 255).
    [[nodiscard]] auto is_opaque() const noexcept -> bool
    {
        return a == 255;
    }

    /// Whether the color is fully transparent (alpha == 0).
    [[nodiscard]] auto is_transparent() const noexcept -> bool
    {
        return a == 0;
    }

    // ── Batch 1 (#10) ─────────────────────────────────────────────

    /// (#10) Return a copy with a different alpha value.
    [[nodiscard]] auto with_alpha(uint8_t new_alpha) const noexcept -> CanvasColor
    {
        return {r, g, b, new_alpha};
    }
};

/// Enumeration of all canvas object types.
enum class CanvasObjectType : uint8_t
{
    StickyNote,
    TextBox,
    Shape,
    Connector,
    FreehandPath,
    Image,
    Frame,
    Section,
    Group,
    Table,
    MindMapNode,
    KanbanColumn,
    KanbanCard,
    BookmarkCard,
    VideoEmbed,
    PDFPage,
    Icon,
    Comment,
    DiagramShape,
    CrossBoardLink,
    AppWidget // V8 Phase 8: In-canvas widget host
};

} // namespace markamp::canvas
