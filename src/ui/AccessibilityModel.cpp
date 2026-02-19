#include "AccessibilityModel.h"

namespace markamp::ui
{

void AccessibilityModel::set_controls(std::vector<AccessibleControl> controls)
{
    controls_ = std::move(controls);
}

auto AccessibilityModel::controls() const -> const std::vector<AccessibleControl>&
{
    return controls_;
}

auto AccessibilityModel::control_by_id(const std::string& control_id) const
    -> const AccessibleControl*
{
    for (const auto& ctrl : controls_)
    {
        if (ctrl.control_id == control_id)
        {
            return &ctrl;
        }
    }
    return nullptr;
}

auto AccessibilityModel::unnamed_controls() const -> std::vector<AccessibleControl>
{
    std::vector<AccessibleControl> result;
    for (const auto& ctrl : controls_)
    {
        if (ctrl.accessible_name.empty())
        {
            result.push_back(ctrl);
        }
    }
    return result;
}

auto AccessibilityModel::non_focusable() const -> std::vector<AccessibleControl>
{
    std::vector<AccessibleControl> result;
    for (const auto& ctrl : controls_)
    {
        if (!ctrl.is_focusable)
        {
            result.push_back(ctrl);
        }
    }
    return result;
}

void AccessibilityModel::announce(const std::string& message, bool assertive)
{
    announcements_.push_back({message, assertive});
}

auto AccessibilityModel::pending_announcements() const -> const std::vector<Announcement>&
{
    return announcements_;
}

void AccessibilityModel::clear_announcements()
{
    announcements_.clear();
}

void AccessibilityModel::set_high_contrast(bool enabled)
{
    high_contrast_ = enabled;
}
auto AccessibilityModel::high_contrast() const -> bool
{
    return high_contrast_;
}

void AccessibilityModel::set_reduced_complexity(bool enabled)
{
    reduced_complexity_ = enabled;
}
auto AccessibilityModel::reduced_complexity() const -> bool
{
    return reduced_complexity_;
}

} // namespace markamp::ui
