#include "SnappingModel.h"

#include <algorithm>
#include <cmath>

namespace markamp::canvas
{

void SnappingModel::set_grid_spacing(double spacing)
{
    grid_spacing_ = std::max(1.0, spacing);
}
auto SnappingModel::grid_spacing() const -> double
{
    return grid_spacing_;
}

void SnappingModel::set_grid_style(GridStyle style)
{
    grid_style_ = style;
}
auto SnappingModel::grid_style() const -> GridStyle
{
    return grid_style_;
}

void SnappingModel::set_major_interval(int interval)
{
    major_interval_ = std::max(1, interval);
}
auto SnappingModel::major_interval() const -> int
{
    return major_interval_;
}

void SnappingModel::set_grid_visible(bool visible)
{
    grid_visible_ = visible;
}
auto SnappingModel::grid_visible() const -> bool
{
    return grid_visible_;
}

void SnappingModel::set_snap_enabled(bool enabled)
{
    snap_enabled_ = enabled;
}
auto SnappingModel::snap_enabled() const -> bool
{
    return snap_enabled_;
}

void SnappingModel::set_snap_tolerance(double pixels)
{
    snap_tolerance_ = std::max(1.0, pixels);
}
auto SnappingModel::snap_tolerance() const -> double
{
    return snap_tolerance_;
}

void SnappingModel::enable_target(SnapTarget target)
{
    if (std::find(enabled_targets_.begin(), enabled_targets_.end(), target) ==
        enabled_targets_.end())
    {
        enabled_targets_.push_back(target);
    }
}

void SnappingModel::disable_target(SnapTarget target)
{
    enabled_targets_.erase(std::remove(enabled_targets_.begin(), enabled_targets_.end(), target),
                           enabled_targets_.end());
}

auto SnappingModel::is_target_enabled(SnapTarget target) const -> bool
{
    return std::find(enabled_targets_.begin(), enabled_targets_.end(), target) !=
           enabled_targets_.end();
}

auto SnappingModel::snap_to_grid(double x_val, double y_val) const -> SnapResult
{
    SnapResult result;
    result.snapped_x = x_val;
    result.snapped_y = y_val;

    if (!snap_enabled_ || !is_target_enabled(SnapTarget::kGrid))
    {
        return result;
    }

    const double nearest_x = std::round(x_val / grid_spacing_) * grid_spacing_;
    const double nearest_y = std::round(y_val / grid_spacing_) * grid_spacing_;

    if (std::abs(x_val - nearest_x) <= snap_tolerance_)
    {
        result.snapped_x = nearest_x;
        result.did_snap_x = true;
        result.target_x = SnapTarget::kGrid;
    }

    if (std::abs(y_val - nearest_y) <= snap_tolerance_)
    {
        result.snapped_y = nearest_y;
        result.did_snap_y = true;
        result.target_y = SnapTarget::kGrid;
    }

    return result;
}

} // namespace markamp::canvas
