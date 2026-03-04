// ============================================================================
// File: src/core/PerformanceCounters.cpp
// Phase 49: Performance Dashboard — Thread-safe metrics service
// ============================================================================
#include "PerformanceCounters.h"

#include <sstream>

namespace markamp::core
{

void PerformanceCounters::record_frame_time(double time_ms)
{
    frame_times_.push(time_ms);
}

auto PerformanceCounters::frame_time_average() const -> double
{
    return frame_times_.average();
}

auto PerformanceCounters::frame_time_p95() const -> double
{
    return frame_times_.percentile(0.95);
}

void PerformanceCounters::increment_event_count()
{
    event_count_.fetch_add(1, std::memory_order_relaxed);
}

void PerformanceCounters::record_event_throughput(double events_per_second)
{
    event_throughput_.push(events_per_second);
}

auto PerformanceCounters::event_throughput_average() const -> double
{
    return event_throughput_.average();
}

void PerformanceCounters::record_memory_usage(size_t bytes)
{
    memory_samples_.push(bytes);
}

auto PerformanceCounters::memory_usage_latest() const -> size_t
{
    return memory_samples_.latest();
}

auto PerformanceCounters::memory_delta_mb() const -> double
{
    if (memory_samples_.size() < 2)
    {
        return 0.0;
    }
    auto first = static_cast<double>(memory_samples_.at(0));
    auto last = static_cast<double>(memory_samples_.latest());
    return (last - first) / (1024.0 * 1024.0);
}

void PerformanceCounters::record_cache_hit()
{
    cache_hits_.fetch_add(1, std::memory_order_relaxed);
}

void PerformanceCounters::record_cache_miss()
{
    cache_misses_.fetch_add(1, std::memory_order_relaxed);
}

auto PerformanceCounters::cache_hit_rate() const -> double
{
    auto hits = cache_hits_.load(std::memory_order_relaxed);
    auto misses = cache_misses_.load(std::memory_order_relaxed);
    auto total = hits + misses;
    return total > 0 ? (static_cast<double>(hits) / static_cast<double>(total)) * 100.0 : 0.0;
}

void PerformanceCounters::record_startup_phase(const std::string& name,
                                               std::chrono::microseconds duration)
{
    startup_phases_.push_back({.name = name, .duration = duration});
}

auto PerformanceCounters::total_startup_time() const -> std::chrono::microseconds
{
    std::chrono::microseconds total{0};
    for (const auto& phase : startup_phases_)
    {
        total += phase.duration;
    }
    return total;
}

void PerformanceCounters::record_event_duration(double time_ms)
{
    if (time_ms > kSlowEventThresholdMs)
    {
        slow_events_.fetch_add(1, std::memory_order_relaxed);
    }
}

void PerformanceCounters::increment_reflow_count()
{
    reflow_count_.fetch_add(1, std::memory_order_relaxed);
}

void PerformanceCounters::increment_render_count()
{
    render_count_.fetch_add(1, std::memory_order_relaxed);
}

auto PerformanceCounters::detect_frame_regression(double baseline_avg) const -> bool
{
    if (baseline_avg <= 0.0 || frame_times_.size() == 0)
    {
        return false;
    }
    const double current_avg = frame_time_average();
    const double delta_percent = ((current_avg - baseline_avg) / baseline_avg) * 100.0;
    return delta_percent > kRegressionThresholdPercent;
}

auto PerformanceCounters::export_json() const -> std::string
{
    std::ostringstream json;
    json << "{\n";
    json << "  \"frame_time_avg_ms\": " << frame_time_average() << ",\n";
    json << "  \"frame_time_p95_ms\": " << frame_time_p95() << ",\n";
    json << "  \"event_count\": " << event_count_.load() << ",\n";
    json << "  \"cache_hit_rate\": " << cache_hit_rate() << ",\n";
    json << "  \"memory_latest_bytes\": " << memory_usage_latest() << ",\n";
    json << "  \"slow_events\": " << slow_events_.load() << ",\n";
    json << "  \"reflow_count\": " << reflow_count_.load() << ",\n";
    json << "  \"render_count\": " << render_count_.load() << ",\n";
    json << "  \"startup_phases\": [\n";
    for (size_t i = 0; i < startup_phases_.size(); ++i)
    {
        json << "    {\"name\": \"" << startup_phases_[i].name << "\", "
             << "\"duration_us\": " << startup_phases_[i].duration.count() << "}";
        if (i + 1 < startup_phases_.size())
        {
            json << ",";
        }
        json << "\n";
    }
    json << "  ]\n";
    json << "}";
    return json.str();
}

} // namespace markamp::core
