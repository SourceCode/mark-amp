#include "ui/PanelAreaModel.h"

#include <algorithm>

namespace markamp::ui
{

PanelAreaModel::PanelAreaModel(core::EventBus& event_bus)
    : event_bus_{event_bus}
{
}

void PanelAreaModel::register_panel(const PanelTabConfig& config)
{
    auto panel_id = config.id;
    registered_panels_[panel_id] = config;

    // Set first registered panel as active if none is set
    if (active_panel_id_.empty())
    {
        active_panel_id_ = panel_id;
    }

    event_bus_.publish_fast(core::events::PanelAreaTabsChangedEvent{});
}

void PanelAreaModel::unregister_panel(const std::string& panel_id)
{
    if (registered_panels_.erase(panel_id) > 0)
    {
        if (active_panel_id_ == panel_id)
        {
            auto visible_panels = panels();
            if (!visible_panels.empty())
            {
                active_panel_id_ = visible_panels.front().id;
            }
            else
            {
                active_panel_id_.clear();
            }
        }
        event_bus_.publish_fast(core::events::PanelAreaTabsChangedEvent{});
    }
}

void PanelAreaModel::set_active(const std::string& panel_id)
{
    if (active_panel_id_ != panel_id && registered_panels_.contains(panel_id))
    {
        active_panel_id_ = panel_id;
        event_bus_.publish_fast(core::events::PanelAreaTabsChangedEvent{});
    }
}

auto PanelAreaModel::active_panel() const -> std::string
{
    return active_panel_id_;
}

auto PanelAreaModel::panels() const -> std::vector<PanelTabConfig>
{
    std::vector<PanelTabConfig> result;
    result.reserve(registered_panels_.size());

    for (const auto& [panel_id_key, config] : registered_panels_)
    {
        if (config.visible)
        {
            result.push_back(config);
        }
    }

    std::sort(result.begin(),
              result.end(),
              [](const PanelTabConfig& panel_a, const PanelTabConfig& panel_b)
              { return panel_a.order < panel_b.order; });

    return result;
}

auto PanelAreaModel::has_panel(const std::string& panel_id) const -> bool
{
    return registered_panels_.contains(panel_id);
}

void PanelAreaModel::set_badge(const std::string& panel_id,
                               core::events::BadgeState state,
                               int count)
{
    if (auto iter = registered_panels_.find(panel_id); iter != registered_panels_.end())
    {
        if (iter->second.badge != state || iter->second.badge_count != count)
        {
            iter->second.badge = state;
            iter->second.badge_count = count;
            event_bus_.publish_fast(core::events::PanelAreaBadgeChangedEvent{});
        }
    }
}

void PanelAreaModel::set_order(const std::string& panel_id, int order)
{
    if (auto iter = registered_panels_.find(panel_id); iter != registered_panels_.end())
    {
        if (iter->second.order != order)
        {
            iter->second.order = order;
            event_bus_.publish_fast(core::events::PanelAreaTabsChangedEvent{});
        }
    }
}

void PanelAreaModel::set_visible(const std::string& panel_id, bool visible)
{
    if (auto iter = registered_panels_.find(panel_id); iter != registered_panels_.end())
    {
        if (iter->second.visible != visible)
        {
            iter->second.visible = visible;

            // Handle active panel switch if it's being hidden
            if (!visible && active_panel_id_ == panel_id)
            {
                auto visible_panels = panels(); // Note: panels() only returns visible ones
                if (!visible_panels.empty())
                {
                    active_panel_id_ = visible_panels.front().id;
                }
                else
                {
                    active_panel_id_.clear();
                }
            }

            event_bus_.publish_fast(core::events::PanelAreaTabsChangedEvent{});
        }
    }
}

} // namespace markamp::ui
