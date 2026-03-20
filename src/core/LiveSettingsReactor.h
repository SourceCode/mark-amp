/// @file LiveSettingsReactor.h
/// @brief P06-T03: Routes live setting changes to subsystem update handlers.
///
/// Subscribes to SettingChangedEvent and maps settings to their live-apply
/// adapters (theme, density, word wrap, etc.). Tracks which settings need restart.
#pragma once

#include "EventBus.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace markamp::core
{

/// Routes setting changes to live subsystem updates.
class LiveSettingsReactor
{
public:
    explicit LiveSettingsReactor(EventBus& bus);

    /// Register a live-apply handler for a setting key.
    using ApplyHandler = std::function<void(const std::string& value)>;
    void register_handler(const std::string& key, ApplyHandler handler);

    /// Mark a setting as requiring restart instead of live apply.
    void mark_restart_required(const std::string& key);

    /// Check if a setting requires restart.
    [[nodiscard]] auto requires_restart(const std::string& key) const -> bool;

    /// Get the count of registered handlers.
    [[nodiscard]] auto handler_count() const -> int
    {
        return static_cast<int>(handlers_.size());
    }

    /// Check if any restart-required settings were changed this session.
    [[nodiscard]] auto has_pending_restart() const -> bool
    {
        return !pending_restart_keys_.empty();
    }

private:
    EventBus& event_bus_;
    Subscription setting_changed_sub_;
    std::unordered_map<std::string, ApplyHandler> handlers_;
    std::unordered_set<std::string> restart_required_keys_;
    std::unordered_set<std::string> pending_restart_keys_;
};

} // namespace markamp::core
