#include "AccessibilitySemanticsController.h"

#include <algorithm>
#include <cmath>

namespace markamp::ui
{

auto ControlSemantics::has_state(AccessibleState flag) const -> bool
{
    return (state_flags & static_cast<uint8_t>(flag)) != 0;
}

auto ControlSemantics::role_name() const -> std::string
{
    switch (role)
    {
        case AccessibleRole::kNone:
            return "none";
        case AccessibleRole::kButton:
            return "button";
        case AccessibleRole::kCheckbox:
            return "checkbox";
        case AccessibleRole::kRadio:
            return "radio";
        case AccessibleRole::kTab:
            return "tab";
        case AccessibleRole::kTabPanel:
            return "tabpanel";
        case AccessibleRole::kToolbar:
            return "toolbar";
        case AccessibleRole::kMenu:
            return "menu";
        case AccessibleRole::kMenuItem:
            return "menuitem";
        case AccessibleRole::kTree:
            return "tree";
        case AccessibleRole::kTreeItem:
            return "treeitem";
        case AccessibleRole::kList:
            return "list";
        case AccessibleRole::kListItem:
            return "listitem";
        case AccessibleRole::kSlider:
            return "slider";
        case AccessibleRole::kProgressBar:
            return "progressbar";
        case AccessibleRole::kDialog:
            return "dialog";
        case AccessibleRole::kAlert:
            return "alert";
        case AccessibleRole::kStatus:
            return "status";
        case AccessibleRole::kRegion:
            return "region";
    }
    return "unknown";
}

void AccessibilitySemanticsController::register_control(const ControlSemantics& semantics)
{
    // Replace if exists
    remove_control(semantics.control_id);
    controls_.push_back(semantics);
}

void AccessibilitySemanticsController::update_state(const std::string& control_id,
                                                    uint8_t state_flags)
{
    for (auto& ctrl : controls_)
    {
        if (ctrl.control_id == control_id)
        {
            ctrl.state_flags = state_flags;
            return;
        }
    }
}

void AccessibilitySemanticsController::remove_control(const std::string& control_id)
{
    controls_.erase(std::remove_if(controls_.begin(),
                                   controls_.end(),
                                   [&control_id](const ControlSemantics& ctrl)
                                   { return ctrl.control_id == control_id; }),
                    controls_.end());
}

auto AccessibilitySemanticsController::semantics_for(const std::string& control_id) const
    -> const ControlSemantics*
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

auto AccessibilitySemanticsController::control_count() const -> int
{
    return static_cast<int>(controls_.size());
}

void AccessibilitySemanticsController::set_high_contrast(bool enabled)
{
    high_contrast_ = enabled;
}

auto AccessibilitySemanticsController::is_high_contrast() const -> bool
{
    return high_contrast_;
}

void AccessibilitySemanticsController::set_reduced_complexity(bool enabled)
{
    reduced_complexity_ = enabled;
}

auto AccessibilitySemanticsController::is_reduced_complexity() const -> bool
{
    return reduced_complexity_;
}

auto AccessibilitySemanticsController::check_contrast(double luminance_fg, double luminance_bg)
    -> ContrastResult
{
    double lighter = std::max(luminance_fg, luminance_bg);
    double darker = std::min(luminance_fg, luminance_bg);
    double ratio = (lighter + 0.05) / (darker + 0.05);

    ContrastResult result;
    result.ratio = ratio;
    result.meets_aa = ratio >= 4.5;
    result.meets_aaa = ratio >= 7.0;
    return result;
}

void AccessibilitySemanticsController::set_focus_visible(bool enabled)
{
    focus_visible_ = enabled;
}

auto AccessibilitySemanticsController::is_focus_visible() const -> bool
{
    return focus_visible_;
}

} // namespace markamp::ui
