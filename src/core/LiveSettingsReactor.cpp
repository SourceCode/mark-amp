/// @file LiveSettingsReactor.cpp
/// @brief P06-T03: Routes live setting changes to subsystem update handlers.

#include "LiveSettingsReactor.h"

#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

LiveSettingsReactor::LiveSettingsReactor(EventBus& bus)
    : event_bus_(bus)
    , setting_changed_sub_(bus.subscribe<events::SettingChangedEvent>(
          [this](const events::SettingChangedEvent& evt)
          {
              // Check restart required
              if (restart_required_keys_.contains(evt.key))
              {
                  pending_restart_keys_.insert(evt.key);
                  event_bus_.publish(events::NotificationEvent{
                      "Setting '" + evt.key + "' requires restart to take effect.",
                      events::NotificationLevel::Info, 4000});
                  MARKAMP_LOG_INFO("Setting '{}' changed but requires restart", evt.key);
                  return;
              }

              // Try live apply
              auto iter = handlers_.find(evt.key);
              if (iter != handlers_.end())
              {
                  iter->second(evt.value);
                  MARKAMP_LOG_DEBUG("Setting '{}' live-applied: {}", evt.key, evt.value);
              }
              else
              {
                  MARKAMP_LOG_DEBUG("Setting '{}' changed, no live handler", evt.key);
              }
          }))
{
    // Register restart-required settings
    mark_restart_required("ai.provider");
}

void LiveSettingsReactor::register_handler(const std::string& key, ApplyHandler handler)
{
    handlers_[key] = std::move(handler);
    MARKAMP_LOG_DEBUG("Live handler registered for: {}", key);
}

void LiveSettingsReactor::mark_restart_required(const std::string& key)
{
    restart_required_keys_.insert(key);
}

auto LiveSettingsReactor::requires_restart(const std::string& key) const -> bool
{
    return restart_required_keys_.contains(key);
}

} // namespace markamp::core
