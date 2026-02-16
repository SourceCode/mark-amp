/// PluginMemoryTracker.h — Phase 24: Per-plugin memory tracking
///
/// Tracks memory allocations per-plugin using MemoryBudget.
/// Reports over-budget conditions for individual plugins.

#pragma once

#include "MemoryBudget.h"

#include <cstddef>
#include <string>
#include <string_view>

namespace markamp::core
{

/// Per-plugin memory tracker wrapping MemoryBudget.
///
/// Usage:
///   PluginMemoryTracker tracker;
///   tracker.register_plugin("my-plugin", 2 * 1024 * 1024);  // 2MB cap
///   tracker.report_allocation("my-plugin", 1024);
///   if (tracker.is_over_budget("my-plugin")) { /* reject or trim */ }
class PluginMemoryTracker
{
public:
    PluginMemoryTracker() = default;

    /// Register a plugin with a memory budget in bytes.
    void register_plugin(std::string_view plugin_id, std::size_t budget_bytes)
    {
        budget_.register_subsystem(plugin_id, budget_bytes);
    }

    /// Report an allocation by a plugin (adds to current usage).
    void report_allocation(std::string_view plugin_id, std::size_t bytes)
    {
        auto current = budget_.current_usage(plugin_id);
        budget_.report(plugin_id, current + bytes);
    }

    /// Report a deallocation by a plugin (subtracts from current usage).
    void report_deallocation(std::string_view plugin_id, std::size_t bytes)
    {
        auto current = budget_.current_usage(plugin_id);
        auto new_usage = (bytes > current) ? 0UL : current - bytes;
        budget_.report(plugin_id, new_usage);
    }

    /// Set absolute usage for a plugin.
    void report_usage(std::string_view plugin_id, std::size_t total_bytes)
    {
        budget_.report(plugin_id, total_bytes);
    }

    /// Check if a plugin is exceeding its memory budget.
    [[nodiscard]] auto is_over_budget(std::string_view plugin_id) const -> bool
    {
        return budget_.is_over_budget(plugin_id);
    }

    /// Get current memory usage for a plugin.
    [[nodiscard]] auto current_usage(std::string_view plugin_id) const -> std::size_t
    {
        return budget_.current_usage(plugin_id);
    }

    /// Get a snapshot of all plugin memory usage.
    [[nodiscard]] auto snapshot() const -> std::vector<SubsystemUsage>
    {
        return budget_.snapshot();
    }

    /// Number of registered plugins.
    [[nodiscard]] auto plugin_count() const -> std::size_t
    {
        return budget_.subsystem_count();
    }

    /// Reset all usage counters.
    void reset()
    {
        budget_.reset_usage();
    }

private:
    MemoryBudget budget_;
};

} // namespace markamp::core
