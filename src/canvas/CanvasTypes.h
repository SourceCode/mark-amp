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
};

/// 2D size (width, height).
struct Size2D
{
    double width{0.0};
    double height{0.0};
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

    /// Create from center + size.
    [[nodiscard]] static auto from_center_size(const Point2D& center, const Size2D& size) -> AABB;
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
    CrossBoardLink
};

} // namespace markamp::canvas
