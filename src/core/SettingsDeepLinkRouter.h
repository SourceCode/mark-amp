/// @file SettingsDeepLinkRouter.h
/// @brief P06-T01: Routes SettingsOpenRequestEvent to the active settings host.
///
/// Mediates between deep-link events and the canonical settings UI surface.
/// Handles setting ID, query, and scope routing with fallback behavior.
#pragma once

#include "EventBus.h"

#include <string>

namespace markamp::core
{

/// Routes settings deep-links to the correct settings surface.
class SettingsDeepLinkRouter
{
public:
    explicit SettingsDeepLinkRouter(EventBus& bus);

    /// Manually route a deep-link to settings.
    void route(const std::string& setting_id, const std::string& query,
               const std::string& scope);

    /// Check if the settings host is available.
    [[nodiscard]] auto is_settings_available() const -> bool { return settings_available_; }

    /// Set settings host availability (called by settings panel on creation/destruction).
    void set_settings_available(bool available) { settings_available_ = available; }

private:
    EventBus& event_bus_;
    Subscription settings_request_sub_;
    bool settings_available_{false};
};

} // namespace markamp::core
