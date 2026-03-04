#include "GridGuideController.h"

#include <algorithm>
#include <cmath>

namespace markamp::canvas
{

auto GridConfig::style_name() const -> std::string
{
    switch (style)
    {
        case GridStyle::kDots:
            return "dots";
        case GridStyle::kLines:
            return "lines";
        case GridStyle::kCrosshairs:
            return "crosshairs";
    }
    return "unknown";
}

void GridGuideController::set_grid(const GridConfig& config)
{
    grid_ = config;
}

auto GridGuideController::grid() const -> const GridConfig&
{
    return grid_;
}

void GridGuideController::set_grid_visible(bool visible)
{
    grid_.visible = visible;
}

auto GridGuideController::grid_visible() const -> bool
{
    return grid_.visible;
}

auto GridGuideController::snap_to_grid(double value) const -> double
{
    if (grid_.spacing <= 0.0)
    {
        return value;
    }
    return std::round(value / grid_.spacing) * grid_.spacing;
}

void GridGuideController::add_guide(const GuideLine& guide)
{
    guides_.push_back(guide);
}

void GridGuideController::remove_guide(const std::string& guide_id)
{
    guides_.erase(std::remove_if(guides_.begin(),
                                 guides_.end(),
                                 [&guide_id](const GuideLine& guide)
                                 { return guide.guide_id == guide_id; }),
                  guides_.end());
}

auto GridGuideController::guide_count() const -> int
{
    return static_cast<int>(guides_.size());
}

auto GridGuideController::guides() const -> const std::vector<GuideLine>&
{
    return guides_;
}

void GridGuideController::set_guide_locked(const std::string& guide_id, bool locked)
{
    for (auto& guide : guides_)
    {
        if (guide.guide_id == guide_id)
        {
            guide.locked = locked;
            return;
        }
    }
}

void GridGuideController::clear_guides()
{
    guides_.clear();
}

} // namespace markamp::canvas
