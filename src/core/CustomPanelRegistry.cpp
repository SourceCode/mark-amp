/// @file CustomPanelRegistry.cpp
/// @brief V9 Phase 41 — CustomPanelRegistry implementation.

#include "CustomPanelRegistry.h"

#include <algorithm>

namespace markamp::core
{

void CustomPanelRegistry::register_panel(CustomPanel panel)
{
    if (panel.panel_id.empty())
    {
        panel.panel_id = "panel_" + std::to_string(panels_.size());
    }
    panels_.push_back(std::move(panel));
}

auto CustomPanelRegistry::unregister_panel(const std::string& panel_id) -> bool
{
    auto iter =
        std::remove_if(panels_.begin(),
                       panels_.end(),
                       [&](const CustomPanel& panel) { return panel.panel_id == panel_id; });
    if (iter == panels_.end())
    {
        return false;
    }
    panels_.erase(iter, panels_.end());
    return true;
}

auto CustomPanelRegistry::find_panel(const std::string& panel_id) const -> const CustomPanel*
{
    for (const auto& panel : panels_)
    {
        if (panel.panel_id == panel_id)
        {
            return &panel;
        }
    }
    return nullptr;
}

auto CustomPanelRegistry::panel_count() const -> int
{
    return static_cast<int>(panels_.size());
}

auto CustomPanelRegistry::panels_at_location(PanelLocation location) const
    -> std::vector<const CustomPanel*>
{
    std::vector<const CustomPanel*> result;
    for (const auto& panel : panels_)
    {
        if (panel.location == location && panel.visible)
        {
            result.push_back(&panel);
        }
    }
    // Sort by priority
    std::sort(result.begin(),
              result.end(),
              [](const CustomPanel* lhs, const CustomPanel* rhs)
              { return lhs->priority > rhs->priority; });
    return result;
}

auto CustomPanelRegistry::move_panel(const std::string& panel_id, PanelLocation location) -> bool
{
    auto* panel = find_mut(panel_id);
    if (panel == nullptr)
    {
        return false;
    }
    panel->location = location;
    return true;
}

auto CustomPanelRegistry::show_panel(const std::string& panel_id) -> bool
{
    auto* panel = find_mut(panel_id);
    if (panel == nullptr)
    {
        return false;
    }
    panel->visible = true;
    return true;
}

auto CustomPanelRegistry::hide_panel(const std::string& panel_id) -> bool
{
    auto* panel = find_mut(panel_id);
    if (panel == nullptr)
    {
        return false;
    }
    panel->visible = false;
    return true;
}

auto CustomPanelRegistry::pin_panel(const std::string& panel_id) -> bool
{
    auto* panel = find_mut(panel_id);
    if (panel == nullptr)
    {
        return false;
    }
    panel->pinned = true;
    return true;
}

auto CustomPanelRegistry::unpin_panel(const std::string& panel_id) -> bool
{
    auto* panel = find_mut(panel_id);
    if (panel == nullptr)
    {
        return false;
    }
    panel->pinned = false;
    return true;
}

auto CustomPanelRegistry::all_panels() const -> std::vector<const CustomPanel*>
{
    std::vector<const CustomPanel*> result;
    result.reserve(panels_.size());
    for (const auto& panel : panels_)
    {
        result.push_back(&panel);
    }
    return result;
}

auto CustomPanelRegistry::visible_panels() const -> std::vector<const CustomPanel*>
{
    std::vector<const CustomPanel*> result;
    for (const auto& panel : panels_)
    {
        if (panel.visible)
        {
            result.push_back(&panel);
        }
    }
    return result;
}

auto CustomPanelRegistry::pinned_panels() const -> std::vector<const CustomPanel*>
{
    std::vector<const CustomPanel*> result;
    for (const auto& panel : panels_)
    {
        if (panel.pinned)
        {
            result.push_back(&panel);
        }
    }
    return result;
}

auto CustomPanelRegistry::panels_by_extension(const std::string& extension_id) const
    -> std::vector<const CustomPanel*>
{
    std::vector<const CustomPanel*> result;
    for (const auto& panel : panels_)
    {
        if (panel.extension_id == extension_id)
        {
            result.push_back(&panel);
        }
    }
    return result;
}

void CustomPanelRegistry::clear_all()
{
    panels_.clear();
}

auto CustomPanelRegistry::find_mut(const std::string& panel_id) -> CustomPanel*
{
    for (auto& panel : panels_)
    {
        if (panel.panel_id == panel_id)
        {
            return &panel;
        }
    }
    return nullptr;
}

} // namespace markamp::core
