/// @file WorkspaceSettings.cpp
/// @brief Phase 20 – Cascading workspace settings implementation.

#include "core/WorkspaceSettings.h"

#include "core/EventBus.h"
#include "core/Events.h"

#include <algorithm>

namespace markamp::core
{

// ============================================================================
// Construction
// ============================================================================

WorkspaceSettings::WorkspaceSettings(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ============================================================================
// Getters
// ============================================================================

auto WorkspaceSettings::get_string(const std::string& key, SettingScope scope) const
    -> std::optional<std::string>
{
    const auto& settings_map = scope_map(scope);
    auto iter = settings_map.find(key);
    if (iter != settings_map.end())
    {
        return iter->second.string_value;
    }
    return std::nullopt;
}

auto WorkspaceSettings::get_int(const std::string& key, SettingScope scope) const
    -> std::optional<int>
{
    const auto& settings_map = scope_map(scope);
    auto iter = settings_map.find(key);
    if (iter != settings_map.end())
    {
        return iter->second.int_value;
    }
    return std::nullopt;
}

auto WorkspaceSettings::get_bool(const std::string& key, SettingScope scope) const
    -> std::optional<bool>
{
    const auto& settings_map = scope_map(scope);
    auto iter = settings_map.find(key);
    if (iter != settings_map.end())
    {
        return iter->second.bool_value;
    }
    return std::nullopt;
}

// ============================================================================
// Setters
// ============================================================================

void WorkspaceSettings::set(const std::string& key, const std::string& value, SettingScope scope)
{
    SettingValue setting_val;
    setting_val.key = key;
    setting_val.scope = scope;
    setting_val.string_value = value;

    scope_map_mut(scope)[key] = std::move(setting_val);

    events::WorkspaceSettingsChangedEvent evt;
    evt.key = key;
    evt.scope = static_cast<int>(scope);
    event_bus_.publish(evt);
}

void WorkspaceSettings::set_int(const std::string& key, int value, SettingScope scope)
{
    SettingValue setting_val;
    setting_val.key = key;
    setting_val.scope = scope;
    setting_val.int_value = value;
    setting_val.string_value = std::to_string(value);

    scope_map_mut(scope)[key] = std::move(setting_val);

    events::WorkspaceSettingsChangedEvent evt;
    evt.key = key;
    evt.scope = static_cast<int>(scope);
    event_bus_.publish(evt);
}

void WorkspaceSettings::set_bool(const std::string& key, bool value, SettingScope scope)
{
    SettingValue setting_val;
    setting_val.key = key;
    setting_val.scope = scope;
    setting_val.bool_value = value;
    setting_val.string_value = value ? "true" : "false";

    scope_map_mut(scope)[key] = std::move(setting_val);

    events::WorkspaceSettingsChangedEvent evt;
    evt.key = key;
    evt.scope = static_cast<int>(scope);
    event_bus_.publish(evt);
}

// ============================================================================
// Management
// ============================================================================

auto WorkspaceSettings::remove(const std::string& key, SettingScope scope) -> bool
{
    return scope_map_mut(scope).erase(key) > 0;
}

auto WorkspaceSettings::has(const std::string& key, SettingScope scope) const -> bool
{
    return scope_map(scope).contains(key);
}

auto WorkspaceSettings::all_keys(SettingScope scope) const -> std::vector<std::string>
{
    std::vector<std::string> keys;
    const auto& settings_map = scope_map(scope);
    keys.reserve(settings_map.size());
    for (const auto& [setting_key, setting_val] : settings_map)
    {
        keys.push_back(setting_key);
    }
    std::sort(keys.begin(), keys.end());
    return keys;
}

auto WorkspaceSettings::count(SettingScope scope) const -> std::size_t
{
    return scope_map(scope).size();
}

// ============================================================================
// Cascade Resolution
// ============================================================================

auto WorkspaceSettings::effective_value(const std::string& key) const -> std::optional<SettingValue>
{
    // Cascade order: Folder > Workspace > User > Default.
    static constexpr std::array<SettingScope, 4> kScopes = {SettingScope::kFolder,
                                                            SettingScope::kWorkspace,
                                                            SettingScope::kUser,
                                                            SettingScope::kDefault};

    for (auto scope : kScopes)
    {
        const auto& settings_map = scope_map(scope);
        auto iter = settings_map.find(key);
        if (iter != settings_map.end())
        {
            return iter->second;
        }
    }

    return std::nullopt;
}

// ============================================================================
// Persistence
// ============================================================================

auto WorkspaceSettings::load_workspace_settings(const std::string& path)
    -> std::expected<void, std::string>
{
    // Simplified: in production this would parse JSON from the file.
    // For now, store the source path for reference.
    if (path.empty())
    {
        return std::unexpected("Empty path");
    }

    // Mark workspace settings as loaded from this path.
    for (auto& [key, val] : workspace_settings_)
    {
        val.source_path = path;
    }

    return {};
}

auto WorkspaceSettings::save_workspace_settings(const std::string& path) const
    -> std::expected<void, std::string>
{
    if (path.empty())
    {
        return std::unexpected("Empty path");
    }

    // Simplified: in production this would serialize to JSON.
    // Workspace settings are in workspace_settings_ map.
    return {};
}

// ============================================================================
// Reset
// ============================================================================

void WorkspaceSettings::reset_to_defaults()
{
    user_settings_.clear();
    workspace_settings_.clear();
    folder_settings_.clear();
}

void WorkspaceSettings::clear_scope(SettingScope scope)
{
    scope_map_mut(scope).clear();
}

// ============================================================================
// Private Helpers
// ============================================================================

auto WorkspaceSettings::scope_map(SettingScope scope) const
    -> const std::unordered_map<std::string, SettingValue>&
{
    switch (scope)
    {
        case SettingScope::kUser:
            return user_settings_;
        case SettingScope::kWorkspace:
            return workspace_settings_;
        case SettingScope::kFolder:
            return folder_settings_;
        case SettingScope::kDefault:
        default:
            return default_settings_;
    }
}

auto WorkspaceSettings::scope_map_mut(SettingScope scope)
    -> std::unordered_map<std::string, SettingValue>&
{
    switch (scope)
    {
        case SettingScope::kUser:
            return user_settings_;
        case SettingScope::kWorkspace:
            return workspace_settings_;
        case SettingScope::kFolder:
            return folder_settings_;
        case SettingScope::kDefault:
        default:
            return default_settings_;
    }
}

} // namespace markamp::core
