/// @file StatusBarAdapter.cpp
/// @brief P09-T02: Extension status bar adapter.

#include "StatusBarAdapter.h"

#include "Events.h"
#include "Logger.h"

#include <algorithm>

namespace markamp::core
{

StatusBarAdapter::StatusBarAdapter(EventBus& bus)
    : event_bus_(bus)
    , plugin_deactivated_sub_(bus.subscribe<events::PluginDeactivatedEvent>(
          [this](const events::PluginDeactivatedEvent& evt)
          {
              // Remove items from deactivated plugin
              auto iter = std::remove_if(items_.begin(), items_.end(),
                                         [&evt](const ProjectedStatusItem& item)
                                         { return item.from_extension && item.item_id.find(evt.plugin_id) != std::string::npos; });
              items_.erase(iter, items_.end());
              MARKAMP_LOG_DEBUG("Status bar items cleaned for plugin: {}", evt.plugin_id);
          }))
{
}

void StatusBarAdapter::refresh()
{
    sort_items();
    MARKAMP_LOG_DEBUG("Status bar refreshed: {} items", items_.size());
}

void StatusBarAdapter::add_item(const ProjectedStatusItem& item)
{
    if (static_cast<int>(items_.size()) >= kMaxItems)
    {
        MARKAMP_LOG_WARN("Status bar item cap reached, ignoring: {}", item.item_id);
        return;
    }
    items_.push_back(item);
    sort_items();
}

void StatusBarAdapter::remove_item(const std::string& item_id)
{
    auto iter = std::remove_if(items_.begin(), items_.end(),
                               [&item_id](const ProjectedStatusItem& item)
                               { return item.item_id == item_id; });
    items_.erase(iter, items_.end());
}

void StatusBarAdapter::handle_click(const std::string& item_id)
{
    for (const auto& item : items_)
    {
        if (item.item_id == item_id && !item.command.empty())
        {
            event_bus_.publish(events::NotificationEvent{
                "Status bar: " + item.command,
                events::NotificationLevel::Info, 0});
            MARKAMP_LOG_INFO("Status bar click: {} -> {}", item_id, item.command);
            return;
        }
    }
}

void StatusBarAdapter::sort_items()
{
    std::stable_sort(items_.begin(), items_.end(),
                     [](const ProjectedStatusItem& left, const ProjectedStatusItem& right)
                     { return left.priority > right.priority; });
}

} // namespace markamp::core
