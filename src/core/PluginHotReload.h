/// PluginHotReload.h — V9 Phase 04 Task 5: Plugin hot-reload support
///
/// Provides live reload capability for plugins without full app restart.
/// Deactivates → unregisters → re-registers → re-activates a plugin,
/// tracking reload counts and publishing PluginReloadedEvent.

#pragma once

#include <string>
#include <unordered_map>

namespace markamp::core
{

class PluginManager;
class EventBus;

/// Manages plugin hot-reload lifecycle.
class PluginHotReload
{
public:
    explicit PluginHotReload(PluginManager& plugin_manager, EventBus& event_bus)
        : plugin_manager_(plugin_manager)
        , event_bus_(event_bus)
    {
    }

    /// Attempt to hot-reload a plugin by ID.
    /// Returns true if the reload succeeded.
    auto reload_plugin(const std::string& plugin_id) -> bool;

    /// Get the number of times a plugin has been reloaded.
    [[nodiscard]] auto reload_count(const std::string& plugin_id) const -> int;

    /// Get the total number of reloads across all plugins.
    [[nodiscard]] auto total_reloads() const -> int
    {
        return total_reloads_;
    }

    /// Reset all reload counters.
    void reset_counters();

private:
    PluginManager& plugin_manager_;
    EventBus& event_bus_;
    std::unordered_map<std::string, int> reload_counts_;
    int total_reloads_{0};
};

} // namespace markamp::core
