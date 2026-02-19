#include "ShapeModel.h"

#include <algorithm>
#include <cmath>

namespace markamp::canvas
{

void ShapeModel::set_type(ShapeType type)
{
    type_ = type;
}
auto ShapeModel::type() const -> ShapeType
{
    return type_;
}

void ShapeModel::set_bounds(Bounds bounds)
{
    bounds_ = bounds;
}
auto ShapeModel::bounds() const -> const Bounds&
{
    return bounds_;
}

void ShapeModel::set_corner_radius(double radius)
{
    const double max_radius = std::min(bounds_.width, bounds_.height) / 2.0;
    style_.corner_radius = std::clamp(radius, 0.0, max_radius);
}

auto ShapeModel::corner_radius() const -> double
{
    return style_.corner_radius;
}

void ShapeModel::constrain_proportions()
{
    const double side = std::min(bounds_.width, bounds_.height);
    bounds_.width = side;
    bounds_.height = side;
}

void ShapeModel::set_style(ShapeStyle style)
{
    style_ = std::move(style);
}
auto ShapeModel::style() const -> const ShapeStyle&
{
    return style_;
}

void ShapeModel::commit_style_as_default()
{
    default_style_ = style_;
}
auto ShapeModel::default_style() const -> const ShapeStyle&
{
    return default_style_;
}
void ShapeModel::reset_default_style()
{
    default_style_ = ShapeStyle{};
}

void ShapeModel::set_constrained(bool constrained)
{
    constrained_ = constrained;
}
auto ShapeModel::is_constrained() const -> bool
{
    return constrained_;
}

auto ShapeModel::constrained_bounds() const -> Bounds
{
    if (!constrained_)
    {
        return bounds_;
    }
    const double side = std::min(bounds_.width, bounds_.height);
    return {bounds_.x, bounds_.y, side, side};
}

} // namespace markamp::canvas
