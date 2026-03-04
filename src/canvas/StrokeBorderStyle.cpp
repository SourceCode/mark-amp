#include "StrokeBorderStyle.h"

namespace markamp::canvas
{

auto StrokeBorderStyle::dash_name() const -> std::string
{
    switch (dash)
    {
        case DashPattern::kSolid:
            return "solid";
        case DashPattern::kDashed:
            return "dashed";
        case DashPattern::kDotted:
            return "dotted";
        case DashPattern::kDashDot:
            return "dash-dot";
    }
    return "unknown";
}

auto StrokeBorderStyle::cap_name() const -> std::string
{
    switch (cap)
    {
        case LineCap::kButt:
            return "butt";
        case LineCap::kRound:
            return "round";
        case LineCap::kSquare:
            return "square";
    }
    return "unknown";
}

auto StrokeBorderStyle::join_name() const -> std::string
{
    switch (join)
    {
        case LineJoin::kMiter:
            return "miter";
        case LineJoin::kRound:
            return "round";
        case LineJoin::kBevel:
            return "bevel";
    }
    return "unknown";
}

auto StrokeBorderStyle::has_side(BorderSide side) const -> bool
{
    return (sides & side) == side;
}

auto StrokeBorderStyle::dash_array() const -> std::vector<double>
{
    switch (dash)
    {
        case DashPattern::kSolid:
            return {};
        case DashPattern::kDashed:
            return {width * 4.0, width * 2.0};
        case DashPattern::kDotted:
            return {width, width * 1.5};
        case DashPattern::kDashDot:
            return {width * 4.0, width * 1.5, width, width * 1.5};
    }
    return {};
}

} // namespace markamp::canvas
