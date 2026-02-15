#include "CanvasTypes.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace markamp::canvas
{

// ============================================================================
// Transform2D
// ============================================================================

auto Transform2D::apply(const Point2D& p) const -> Point2D
{
    // First rotate, then scale, then translate.
    const double cos_r = std::cos(rotation);
    const double sin_r = std::sin(rotation);

    const double rotated_x = cos_r * p.x - sin_r * p.y;
    const double rotated_y = sin_r * p.x + cos_r * p.y;

    return {rotated_x * scale_x + tx, rotated_y * scale_y + ty};
}

auto Transform2D::inverse(const Point2D& p) const -> Point2D
{
    // Reverse: un-translate, un-scale, un-rotate.
    const double unscaled_x = (p.x - tx) / scale_x;
    const double unscaled_y = (p.y - ty) / scale_y;

    const double cos_r = std::cos(-rotation);
    const double sin_r = std::sin(-rotation);

    return {cos_r * unscaled_x - sin_r * unscaled_y, sin_r * unscaled_x + cos_r * unscaled_y};
}

auto Transform2D::compose(const Transform2D& other) const -> Transform2D
{
    // Apply other first, then this.
    // The result produces the same effect as applying other.apply(p) then this.apply(result).
    Transform2D result;
    result.scale_x = scale_x * other.scale_x;
    result.scale_y = scale_y * other.scale_y;
    result.rotation = rotation + other.rotation;

    // Transform other's translation through this transform's rotation and scale.
    const double cos_r = std::cos(rotation);
    const double sin_r = std::sin(rotation);
    result.tx = tx + (cos_r * other.tx - sin_r * other.ty) * scale_x;
    result.ty = ty + (sin_r * other.tx + cos_r * other.ty) * scale_y;

    return result;
}

auto Transform2D::identity() -> Transform2D
{
    return {0.0, 0.0, 0.0, 1.0, 1.0};
}

// ============================================================================
// AABB
// ============================================================================

auto AABB::center() const -> Point2D
{
    return {(min_x + max_x) / 2.0, (min_y + max_y) / 2.0};
}

auto AABB::contains(const Point2D& p) const -> bool
{
    return p.x >= min_x && p.x <= max_x && p.y >= min_y && p.y <= max_y;
}

auto AABB::intersects(const AABB& other) const -> bool
{
    // No separation on either axis means overlap.
    return !(max_x < other.min_x || min_x > other.max_x || max_y < other.min_y ||
             min_y > other.max_y);
}

auto AABB::merged(const AABB& other) const -> AABB
{
    return {std::min(min_x, other.min_x),
            std::min(min_y, other.min_y),
            std::max(max_x, other.max_x),
            std::max(max_y, other.max_y)};
}

auto AABB::expanded(double margin) const -> AABB
{
    return {min_x - margin, min_y - margin, max_x + margin, max_y + margin};
}

auto AABB::is_valid() const -> bool
{
    return min_x <= max_x && min_y <= max_y;
}

auto AABB::from_center_size(const Point2D& center, const Size2D& size) -> AABB
{
    const double half_w = size.width / 2.0;
    const double half_h = size.height / 2.0;
    return {center.x - half_w, center.y - half_h, center.x + half_w, center.y + half_h};
}

// ============================================================================
// CanvasColor
// ============================================================================

auto CanvasColor::from_hex(const std::string& hex) -> CanvasColor
{
    std::string h = hex;
    if (!h.empty() && h[0] == '#')
    {
        h = h.substr(1);
    }

    if (h.length() != 6 && h.length() != 8)
    {
        return {0, 0, 0, 255};
    }

    auto parse_byte = [](const std::string& s, size_t offset) -> uint8_t
    {
        unsigned int val = 0;
        std::istringstream iss(s.substr(offset, 2));
        iss >> std::hex >> val;
        return static_cast<uint8_t>(val);
    };

    CanvasColor color;
    color.r = parse_byte(h, 0);
    color.g = parse_byte(h, 2);
    color.b = parse_byte(h, 4);
    color.a = (h.length() == 8) ? parse_byte(h, 6) : static_cast<uint8_t>(255);

    return color;
}

auto CanvasColor::to_hex() const -> std::string
{
    std::ostringstream oss;
    oss << '#' << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(r)
        << std::setw(2) << static_cast<unsigned int>(g) << std::setw(2)
        << static_cast<unsigned int>(b);
    return oss.str();
}

} // namespace markamp::canvas
