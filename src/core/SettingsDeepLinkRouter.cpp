/// @file SettingsDeepLinkRouter.cpp
/// @brief P06-T01: Routes SettingsOpenRequestEvent to the active settings host.

#include "SettingsDeepLinkRouter.h"

#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

SettingsDeepLinkRouter::SettingsDeepLinkRouter(EventBus& bus)
    : event_bus_(bus)
    , settings_request_sub_(bus.subscribe<events::SettingsOpenRequestEvent>(
          [this](const events::SettingsOpenRequestEvent& evt)
          {
              route(evt.setting_id, evt.query, evt.scope);
          }))
{
}

void SettingsDeepLinkRouter::route(const std::string& setting_id, const std::string& query,
                                    const std::string& scope)
{
    if (!settings_available_)
    {
        // Settings host not yet created — open it first via activity bar
        MARKAMP_LOG_INFO("Settings host not available, requesting activity bar switch");
        const events::ActivityBarSelectionEvent activity_evt{events::ActivityBarItemId::kSettings};
        event_bus_.publish(activity_evt);
    }

    if (!setting_id.empty())
    {
        MARKAMP_LOG_INFO("Settings deep-link: setting_id={}, scope={}", setting_id, scope);
    }
    else if (!query.empty())
    {
        MARKAMP_LOG_INFO("Settings deep-link: query='{}', scope={}", query, scope);
    }
    else
    {
        MARKAMP_LOG_INFO("Settings deep-link: general open, scope={}",
                         scope.empty() ? "application" : scope);
    }
}

} // namespace markamp::core
