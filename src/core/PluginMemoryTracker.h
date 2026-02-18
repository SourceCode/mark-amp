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

    // ── V9 Phase 04 Task 8: Per-plugin resource limits ──

    /// Configurable resource limits per plugin.
    struct ResourceLimits
    {
        std::size_t memory_bytes{0}; ///< Max memory in bytes (0 = unlimited)
        int64_t cpu_time_ms{0};      ///< Max CPU time in milliseconds (0 = unlimited)
        int event_subscriptions{0};  ///< Max event subscriptions (0 = unlimited)
    };

    /// A single resource violation.
    struct ResourceViolation
    {
        std::string plugin_id;
        std::string resource_type; ///< "memory", "cpu_time", or "event_subscriptions"
        std::string message;
    };

    /// Set resource limits for a specific plugin.
    void set_limits(const std::string& plugin_id, const ResourceLimits& limits)
    {
        resource_limits_[plugin_id] = limits;
    }

    /// Report CPU time usage for a plugin.
    void report_cpu_time(const std::string& plugin_id, int64_t cpu_ms)
    {
        cpu_time_usage_[plugin_id] = cpu_ms;
    }

    /// Report event subscription count for a plugin.
    void report_event_subscriptions(const std::string& plugin_id, int count)
    {
        event_sub_counts_[plugin_id] = count;
    }

    /// Check all resource limits for a specific plugin.
    [[nodiscard]] auto check_limits(const std::string& plugin_id) const
        -> std::vector<ResourceViolation>
    {
        std::vector<ResourceViolation> violations;
        auto limits_it = resource_limits_.find(plugin_id);
        if (limits_it == resource_limits_.end())
        {
            return violations; // No limits configured
        }
        const auto& limits = limits_it->second;

        // Check memory
        if (limits.memory_bytes > 0 && budget_.current_usage(plugin_id) > limits.memory_bytes)
        {
            violations.push_back({plugin_id, "memory", "Memory usage exceeds limit"});
        }

        // Check CPU time
        if (limits.cpu_time_ms > 0)
        {
            auto cpu_it = cpu_time_usage_.find(plugin_id);
            if (cpu_it != cpu_time_usage_.end() && cpu_it->second > limits.cpu_time_ms)
            {
                violations.push_back({plugin_id, "cpu_time", "CPU time exceeds limit"});
            }
        }

        // Check event subscriptions
        if (limits.event_subscriptions > 0)
        {
            auto sub_it = event_sub_counts_.find(plugin_id);
            if (sub_it != event_sub_counts_.end() && sub_it->second > limits.event_subscriptions)
            {
                violations.push_back(
                    {plugin_id, "event_subscriptions", "Event subscription count exceeds limit"});
            }
        }
        return violations;
    }

private:
    MemoryBudget budget_;

    // Task 8: Extended resource tracking
    std::unordered_map<std::string, ResourceLimits> resource_limits_;
    std::unordered_map<std::string, int64_t> cpu_time_usage_;
    std::unordered_map<std::string, int> event_sub_counts_;
};

} // namespace markamp::core
