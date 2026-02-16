/// ExtensionResourceTracker.h — Phase 33: Per-Extension Resource Tracking
///
/// Tracks CPU time, memory usage, and event latency per extension.
/// Provides ranked snapshots for identifying resource-heavy extensions.
///
/// Pattern implemented: #30 Per-extension resource tracking

#pragma once

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Per-extension resource usage snapshot.
struct ExtensionResourceUsage
{
    std::string extension_id;
    int64_t cpu_time_us{0};
    std::size_t memory_bytes{0};
    int64_t avg_event_latency_us{0};
    uint64_t event_count{0};
    uint64_t error_count{0};
};

/// Tracks resource usage for each registered extension.
///
/// Usage:
///   ExtensionResourceTracker tracker;
///   tracker.register_extension("ext.mermaid");
///   tracker.record_cpu_time("ext.mermaid", duration);
///   tracker.record_memory("ext.mermaid", bytes);
///   auto ranking = tracker.ranked_by_cpu();
class ExtensionResourceTracker
{
public:
    /// Register an extension for tracking.
    void register_extension(std::string_view ext_id)
    {
        const std::lock_guard<std::mutex> guard(mutex_);
        entries_[std::string(ext_id)] = Entry{};
    }

    /// Record CPU time consumed by an extension.
    void record_cpu_time(std::string_view ext_id, std::chrono::microseconds duration)
    {
        const std::lock_guard<std::mutex> guard(mutex_);
        auto iter = entries_.find(std::string(ext_id));
        if (iter != entries_.end())
        {
            iter->second.cpu_time_us += duration.count();
        }
    }

    /// Record memory usage for an extension.
    void record_memory(std::string_view ext_id, std::size_t bytes)
    {
        const std::lock_guard<std::mutex> guard(mutex_);
        auto iter = entries_.find(std::string(ext_id));
        if (iter != entries_.end())
        {
            iter->second.memory_bytes = bytes;
        }
    }

    /// Record an event processing latency.
    void record_event_latency(std::string_view ext_id, std::chrono::microseconds latency)
    {
        const std::lock_guard<std::mutex> guard(mutex_);
        auto iter = entries_.find(std::string(ext_id));
        if (iter != entries_.end())
        {
            iter->second.total_event_latency_us += latency.count();
            ++iter->second.event_count;
        }
    }

    /// Record an error from an extension.
    void record_error(std::string_view ext_id)
    {
        const std::lock_guard<std::mutex> guard(mutex_);
        auto iter = entries_.find(std::string(ext_id));
        if (iter != entries_.end())
        {
            ++iter->second.error_count;
        }
    }

    /// Get all extension usage snapshots.
    [[nodiscard]] auto snapshot() const -> std::vector<ExtensionResourceUsage>
    {
        const std::lock_guard<std::mutex> guard(mutex_);
        std::vector<ExtensionResourceUsage> result;
        result.reserve(entries_.size());

        for (const auto& [ext_id, entry] : entries_)
        {
            ExtensionResourceUsage usage;
            usage.extension_id = ext_id;
            usage.cpu_time_us = entry.cpu_time_us;
            usage.memory_bytes = entry.memory_bytes;
            usage.event_count = entry.event_count;
            usage.error_count = entry.error_count;
            usage.avg_event_latency_us =
                (entry.event_count > 0)
                    ? entry.total_event_latency_us / static_cast<int64_t>(entry.event_count)
                    : 0;
            result.push_back(usage);
        }
        return result;
    }

    /// Get extensions ranked by CPU time (descending).
    [[nodiscard]] auto ranked_by_cpu() const -> std::vector<ExtensionResourceUsage>
    {
        auto result = snapshot();
        std::sort(result.begin(),
                  result.end(),
                  [](const ExtensionResourceUsage& lhs, const ExtensionResourceUsage& rhs)
                  { return lhs.cpu_time_us > rhs.cpu_time_us; });
        return result;
    }

    /// Get extensions ranked by memory (descending).
    [[nodiscard]] auto ranked_by_memory() const -> std::vector<ExtensionResourceUsage>
    {
        auto result = snapshot();
        std::sort(result.begin(),
                  result.end(),
                  [](const ExtensionResourceUsage& lhs, const ExtensionResourceUsage& rhs)
                  { return lhs.memory_bytes > rhs.memory_bytes; });
        return result;
    }

    /// Number of tracked extensions.
    [[nodiscard]] auto extension_count() const -> std::size_t
    {
        const std::lock_guard<std::mutex> guard(mutex_);
        return entries_.size();
    }

    /// Reset all tracking data.
    void reset()
    {
        const std::lock_guard<std::mutex> guard(mutex_);
        for (auto& [ext_id, entry] : entries_)
        {
            entry = Entry{};
        }
    }

private:
    struct Entry
    {
        int64_t cpu_time_us{0};
        std::size_t memory_bytes{0};
        int64_t total_event_latency_us{0};
        uint64_t event_count{0};
        uint64_t error_count{0};
    };

    mutable std::mutex mutex_;
    std::unordered_map<std::string, Entry> entries_;
};

} // namespace markamp::core
