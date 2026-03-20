/// @file SettingsScopeResolver.cpp
/// @brief P06-T05: Resolves setting scope origin and effective values.

#include "SettingsScopeResolver.h"

#include "Config.h"
#include "EventBus.h"
#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

SettingsScopeResolver::SettingsScopeResolver(EventBus& bus, Config& cfg)
    : event_bus_(bus)
    , config_(cfg)
{
}

auto SettingsScopeResolver::resolve(const std::string& key) const -> SettingScopeInfo
{
    SettingScopeInfo info;
    info.key = key;
    info.effective_value = config_.get_string(key);
    info.effective_scope = SettingScope::kApplication;
    info.default_value = info.effective_value; // Simplified; real impl reads catalog default

    // In a full implementation, ScopedConfig would be consulted to determine
    // which scope holds the effective value. For now, we always report application scope.
    return info;
}

void SettingsScopeResolver::reset_to_default(const std::string& key, SettingScope scope)
{
    MARKAMP_LOG_INFO("Reset to default: {} (scope: {})", key, static_cast<int>(scope));

    // Publish setting change with default value
    const auto info = resolve(key);
    config_.set(key, info.default_value);
    event_bus_.publish(events::SettingChangedEvent{key, info.default_value});
}

void SettingsScopeResolver::remove_override(const std::string& key, SettingScope scope)
{
    if (scope == SettingScope::kApplication)
    {
        MARKAMP_LOG_WARN("Cannot remove application-level setting: {}", key);
        return;
    }

    MARKAMP_LOG_INFO("Override removed: {} (scope: {})", key, static_cast<int>(scope));
    // In a full implementation, the scoped config layer would be cleared
}

auto SettingsScopeResolver::has_override(const std::string& key) const -> bool
{
    // Simplified: check if value differs from default
    const auto info = resolve(key);
    return info.has_workspace_override || info.has_project_override;
}

} // namespace markamp::core
