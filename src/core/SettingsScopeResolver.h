/// @file SettingsScopeResolver.h
/// @brief P06-T05: Resolves setting scope origin and effective values.
///
/// Exposes the scope chain (app/workspace/project) for each setting,
/// determines which scope owns the effective value, and provides
/// reset/remove-override actions.
#pragma once

#include <string>

namespace markamp::core
{
class Config;
class EventBus;

/// Setting scope levels.
enum class SettingScope
{
    kApplication, ///< Global application default
    kWorkspace,   ///< Workspace-level override
    kProject,     ///< Project-level override
};

/// Information about a setting's effective scope.
struct SettingScopeInfo
{
    std::string key;
    std::string effective_value;
    SettingScope effective_scope{SettingScope::kApplication};
    bool has_workspace_override{false};
    bool has_project_override{false};
    std::string default_value;
};

/// Resolves setting scope and provides override controls.
class SettingsScopeResolver
{
public:
    SettingsScopeResolver(EventBus& bus, Config& cfg);

    /// Resolve the scope info for a setting.
    [[nodiscard]] auto resolve(const std::string& key) const -> SettingScopeInfo;

    /// Reset a setting to its default value in the given scope.
    void reset_to_default(const std::string& key, SettingScope scope);

    /// Remove a workspace or project override for a setting.
    void remove_override(const std::string& key, SettingScope scope);

    /// Check if a setting has any overrides above application level.
    [[nodiscard]] auto has_override(const std::string& key) const -> bool;

private:
    EventBus& event_bus_;
    Config& config_;
};

} // namespace markamp::core
