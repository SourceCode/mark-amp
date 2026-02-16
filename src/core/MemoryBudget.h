/// MemoryBudget.h — Phase 19: Per-subsystem memory budget tracker
///
/// Tracks memory usage by subsystem name and detects when a subsystem
/// exceeds its allocated budget. Thread-safe for concurrent reporting.
///
/// Pattern implemented: #24 Bounded container caps (budget enforcement)

#pragma once

#include <cstddef>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Snapshot of a single subsystem's memory usage.
struct SubsystemUsage
{
    std::string name;
    std::size_t budget_bytes{0};
    std::size_t current_bytes{0};

    [[nodiscard]] auto percentage() const noexcept -> double
    {
        if (budget_bytes == 0)
        {
            return 0.0;
        }
        return (static_cast<double>(current_bytes) / static_cast<double>(budget_bytes)) * 100.0;
    }

    [[nodiscard]] auto is_over_budget() const noexcept -> bool
    {
        return current_bytes > budget_bytes;
    }
};

/// Per-subsystem memory budget tracker.
///
/// Typical usage:
///   budget.register_subsystem("EventBus", 4 * 1024 * 1024);  // 4MB
///   budget.report("EventBus", event_bus.memory_usage());
///   if (budget.is_over_budget("EventBus")) { /* trim */ }
class MemoryBudget
{
public:
    MemoryBudget() = default;

    /// Register a subsystem with a byte budget.
    void register_subsystem(std::string_view name, std::size_t budget_bytes)
    {
        const std::lock_guard lock(mutex_);
        auto& entry = entries_[std::string(name)];
        entry.budget_bytes = budget_bytes;
    }

    /// Update the current memory usage for a subsystem.
    void report(std::string_view name, std::size_t current_bytes)
    {
        const std::lock_guard lock(mutex_);
        auto iter = entries_.find(std::string(name));
        if (iter != entries_.end())
        {
            iter->second.current_bytes = current_bytes;
        }
    }

    /// Check if a subsystem is exceeding its budget.
    [[nodiscard]] auto is_over_budget(std::string_view name) const -> bool
    {
        const std::lock_guard lock(mutex_);
        auto iter = entries_.find(std::string(name));
        if (iter == entries_.end())
        {
            return false;
        }
        return iter->second.current_bytes > iter->second.budget_bytes;
    }

    /// Get current usage for a subsystem.
    [[nodiscard]] auto current_usage(std::string_view name) const -> std::size_t
    {
        const std::lock_guard lock(mutex_);
        auto iter = entries_.find(std::string(name));
        if (iter == entries_.end())
        {
            return 0;
        }
        return iter->second.current_bytes;
    }

    /// Get a snapshot of all subsystem usage.
    [[nodiscard]] auto snapshot() const -> std::vector<SubsystemUsage>
    {
        const std::lock_guard lock(mutex_);
        std::vector<SubsystemUsage> result;
        result.reserve(entries_.size());
        for (const auto& [name, entry] : entries_)
        {
            result.push_back(SubsystemUsage{name, entry.budget_bytes, entry.current_bytes});
        }
        return result;
    }

    /// Total number of registered subsystems.
    [[nodiscard]] auto subsystem_count() const -> std::size_t
    {
        const std::lock_guard lock(mutex_);
        return entries_.size();
    }

    /// Reset all usage data (not budgets).
    void reset_usage()
    {
        const std::lock_guard lock(mutex_);
        for (auto& [name, entry] : entries_)
        {
            entry.current_bytes = 0;
        }
    }

private:
    struct BudgetEntry
    {
        std::size_t budget_bytes{0};
        std::size_t current_bytes{0};
    };

    mutable std::mutex mutex_;
    std::unordered_map<std::string, BudgetEntry> entries_;
};

} // namespace markamp::core
