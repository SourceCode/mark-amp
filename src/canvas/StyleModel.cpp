#include "StyleModel.h"

#include <algorithm>

namespace markamp::canvas
{

void StyleModel::set_stroke_width(double width)
{
    stroke_width_ = std::max(0.0, width);
}
auto StyleModel::stroke_width() const -> double
{
    return stroke_width_;
}

void StyleModel::set_dash(DashPattern pattern)
{
    dash_ = pattern;
}
auto StyleModel::dash() const -> DashPattern
{
    return dash_;
}

void StyleModel::set_line_cap(LineCap cap)
{
    line_cap_ = cap;
}
auto StyleModel::line_cap() const -> LineCap
{
    return line_cap_;
}

void StyleModel::set_line_join(LineJoin join)
{
    line_join_ = join;
}
auto StyleModel::line_join() const -> LineJoin
{
    return line_join_;
}

void StyleModel::set_fill_type(FillType type)
{
    fill_type_ = type;
}
auto StyleModel::fill_type() const -> FillType
{
    return fill_type_;
}

void StyleModel::set_gradient_stops(std::vector<std::string> stops)
{
    gradient_stops_ = std::move(stops);
}
auto StyleModel::gradient_stops() const -> const std::vector<std::string>&
{
    return gradient_stops_;
}

void StyleModel::set_opacity(double opacity)
{
    opacity_ = std::clamp(opacity, 0.0, 1.0);
}
auto StyleModel::opacity() const -> double
{
    return opacity_;
}

void StyleModel::set_shadow(ShadowStyle shadow)
{
    shadow_ = std::move(shadow);
}
auto StyleModel::shadow() const -> const ShadowStyle&
{
    return shadow_;
}

} // namespace markamp::canvas
