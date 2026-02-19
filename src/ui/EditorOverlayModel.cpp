#include "EditorOverlayModel.h"

#include <algorithm>
#include <set>

namespace markamp::ui
{

void EditorOverlayModel::set_actions(std::vector<OverlayAction> actions)
{
    actions_ = std::move(actions);
}

auto EditorOverlayModel::actions() const -> const std::vector<OverlayAction>&
{
    return actions_;
}

auto EditorOverlayModel::actions_by_group(const std::string& group) const
    -> std::vector<OverlayAction>
{
    std::vector<OverlayAction> result;
    for (const auto& action : actions_)
    {
        if (action.group == group)
        {
            result.push_back(action);
        }
    }
    return result;
}

auto EditorOverlayModel::groups() const -> std::vector<std::string>
{
    std::set<std::string> unique;
    for (const auto& action : actions_)
    {
        unique.insert(action.group);
    }
    return {unique.begin(), unique.end()};
}

auto EditorOverlayModel::clamped_position(
    int desired_x, int desired_y, int overlay_w, int overlay_h, ViewportBounds viewport) -> Position
{
    int clamped_x = desired_x;
    int clamped_y = desired_y;

    // Clamp right edge
    if (clamped_x + overlay_w > viewport.width)
    {
        clamped_x = viewport.width - overlay_w;
    }
    // Clamp left edge
    if (clamped_x < 0)
    {
        clamped_x = 0;
    }
    // Clamp bottom edge
    if (clamped_y + overlay_h > viewport.height)
    {
        clamped_y = viewport.height - overlay_h;
    }
    // Clamp top edge
    if (clamped_y < 0)
    {
        clamped_y = 0;
    }

    return {clamped_x, clamped_y};
}

void EditorOverlayModel::set_anchor(OverlayAnchor anchor)
{
    anchor_ = anchor;
}

auto EditorOverlayModel::anchor() const -> OverlayAnchor
{
    return anchor_;
}

void EditorOverlayModel::set_visible(bool visible)
{
    visible_ = visible;
}

auto EditorOverlayModel::is_visible() const -> bool
{
    return visible_;
}

void EditorOverlayModel::toggle_action(const std::string& action_id)
{
    for (auto& action : actions_)
    {
        if (action.action_id == action_id)
        {
            action.is_active = !action.is_active;
            return;
        }
    }
}

void EditorOverlayModel::set_action_enabled(const std::string& action_id, bool enabled)
{
    for (auto& action : actions_)
    {
        if (action.action_id == action_id)
        {
            action.is_enabled = enabled;
            return;
        }
    }
}

} // namespace markamp::ui
