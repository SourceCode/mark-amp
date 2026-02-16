/// HealthPanel.h — Phase 32: Health Monitoring Panel
///
/// Collects real-time health metrics: frame latency, memory usage,
/// plugin count, event queue depth. Provides a snapshot for UI display.
///
/// Pattern implemented: #29 Health monitoring dashboard

#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// A single health metric entry.
struct HealthMetric
{
    std::string name;
    double value{0.0};
    std::string unit;
    bool is_warning{false};
    bool is_critical{false};
};

/// Snapshot of all health metrics at a point in time.
struct HealthSnapshot
{
    // Frame performance
    double frame_time_ms{0.0};
    double frame_rate_fps{0.0};
    uint64_t total_frames{0};
    uint64_t dropped_frames{0};

    // Memory
    std::size_t memory_used_bytes{0};
    std::size_t memory_budget_bytes{0};
    double memory_usage_percent{0.0};

    // Plugins
    uint32_t active_plugins{0};
    uint32_t plugins_over_budget{0};

    // Event system
    uint64_t events_dispatched{0};
    uint32_t queue_depth{0};

    // General
    int64_t uptime_ms{0};

    /// All metrics as a flat list for UI rendering.
    [[nodiscard]] auto to_metrics() const -> std::vector<HealthMetric>
    {
        std::vector<HealthMetric> metrics;
        metrics.reserve(10);

        metrics.push_back(
            {"Frame Time", frame_time_ms, "ms", frame_time_ms > 16.0, frame_time_ms > 33.0});
        metrics.push_back(
            {"FPS", frame_rate_fps, "fps", frame_rate_fps < 30.0, frame_rate_fps < 15.0});
        metrics.push_back({"Memory",
                           static_cast<double>(memory_used_bytes) / (1024.0 * 1024.0),
                           "MB",
                           memory_usage_percent > 80.0,
                           memory_usage_percent > 95.0});
        metrics.push_back({"Memory Usage",
                           memory_usage_percent,
                           "%",
                           memory_usage_percent > 80.0,
                           memory_usage_percent > 95.0});
        metrics.push_back(
            {"Active Plugins", static_cast<double>(active_plugins), "", false, false});
        metrics.push_back({"Plugins Over Budget",
                           static_cast<double>(plugins_over_budget),
                           "",
                           plugins_over_budget > 0,
                           plugins_over_budget > 2});
        metrics.push_back(
            {"Events Dispatched", static_cast<double>(events_dispatched), "", false, false});
        metrics.push_back({"Queue Depth",
                           static_cast<double>(queue_depth),
                           "",
                           queue_depth > 100,
                           queue_depth > 500});
        metrics.push_back({"Dropped Frames",
                           static_cast<double>(dropped_frames),
                           "",
                           dropped_frames > 0,
                           dropped_frames > 10});

        return metrics;
    }
};

/// Collects and exposes health metrics for the monitoring panel.
///
/// Usage:
///   HealthPanel panel;
///   panel.record_frame(elapsed_us);
///   panel.set_memory(used, budget);
///   auto snap = panel.snapshot();
class HealthPanel
{
public:
    HealthPanel()
        : start_time_(std::chrono::steady_clock::now())
    {
    }

    /// Record a completed frame with its duration.
    void record_frame(std::chrono::microseconds elapsed)
    {
        last_frame_us_ = elapsed;
        ++total_frames_;
        if (elapsed > std::chrono::microseconds(16667)) // > 60fps budget
        {
            ++dropped_frames_;
        }
    }

    /// Update memory usage.
    void set_memory(std::size_t used_bytes, std::size_t budget_bytes)
    {
        memory_used_ = used_bytes;
        memory_budget_ = budget_bytes;
    }

    /// Update plugin counts.
    void set_plugins(uint32_t active_count, uint32_t over_budget_count)
    {
        active_plugins_ = active_count;
        plugins_over_budget_ = over_budget_count;
    }

    /// Update event system stats.
    void set_events(uint64_t dispatched, uint32_t queue_depth_val)
    {
        events_dispatched_ = dispatched;
        queue_depth_ = queue_depth_val;
    }

    /// Take a snapshot of current health metrics.
    [[nodiscard]] auto snapshot() const -> HealthSnapshot
    {
        HealthSnapshot snap;

        auto frame_us = static_cast<double>(last_frame_us_.count());
        snap.frame_time_ms = frame_us / 1000.0;
        snap.frame_rate_fps = (frame_us > 0.0) ? (1000000.0 / frame_us) : 0.0;
        snap.total_frames = total_frames_;
        snap.dropped_frames = dropped_frames_;

        snap.memory_used_bytes = memory_used_;
        snap.memory_budget_bytes = memory_budget_;
        snap.memory_usage_percent =
            (memory_budget_ > 0)
                ? (static_cast<double>(memory_used_) / static_cast<double>(memory_budget_)) * 100.0
                : 0.0;

        snap.active_plugins = active_plugins_;
        snap.plugins_over_budget = plugins_over_budget_;

        snap.events_dispatched = events_dispatched_;
        snap.queue_depth = queue_depth_;

        auto elapsed = std::chrono::steady_clock::now() - start_time_;
        snap.uptime_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

        return snap;
    }

private:
    std::chrono::steady_clock::time_point start_time_;
    std::chrono::microseconds last_frame_us_{0};
    uint64_t total_frames_{0};
    uint64_t dropped_frames_{0};
    std::size_t memory_used_{0};
    std::size_t memory_budget_{0};
    uint32_t active_plugins_{0};
    uint32_t plugins_over_budget_{0};
    uint64_t events_dispatched_{0};
    uint32_t queue_depth_{0};
};

} // namespace markamp::core
