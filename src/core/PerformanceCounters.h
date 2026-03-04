// ============================================================================
// File: src/core/PerformanceCounters.h
// Phase 49: Performance Dashboard — Thread-safe metrics service
// ============================================================================
#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Ring buffer for time-series samples.
template <typename T, size_t N>
class MetricsRingBuffer
{
public:
    void push(T value)
    {
        buffer_[write_pos_ % N] = value;
        ++write_pos_;
        if (count_ < N)
        {
            ++count_;
        }
    }

    [[nodiscard]] auto size() const -> size_t
    {
        return count_;
    }
    [[nodiscard]] auto capacity() const -> size_t
    {
        return N;
    }

    [[nodiscard]] auto at(size_t index) const -> T
    {
        if (index >= count_)
        {
            return T{};
        }
        size_t actual = (write_pos_ - count_ + index) % N;
        return buffer_[actual];
    }

    [[nodiscard]] auto latest() const -> T
    {
        return count_ > 0 ? buffer_[(write_pos_ - 1) % N] : T{};
    }

    [[nodiscard]] auto average() const -> double
    {
        if (count_ == 0)
        {
            return 0.0;
        }
        double sum = 0.0;
        for (size_t i = 0; i < count_; ++i)
        {
            sum += static_cast<double>(at(i));
        }
        return sum / static_cast<double>(count_);
    }

    [[nodiscard]] auto percentile(double p) const -> T
    {
        if (count_ == 0)
        {
            return T{};
        }
        std::vector<T> sorted;
        sorted.reserve(count_);
        for (size_t i = 0; i < count_; ++i)
        {
            sorted.push_back(at(i));
        }
        std::ranges::sort(sorted);
        size_t idx = static_cast<size_t>(p * static_cast<double>(count_ - 1));
        return sorted[idx];
    }

private:
    std::array<T, N> buffer_{};
    size_t write_pos_{0};
    size_t count_{0};
};

/// A startup phase timing record.
struct StartupPhase
{
    std::string name;
    std::chrono::microseconds duration{0};
};

/// Thread-safe performance counters service.
class PerformanceCounters
{
public:
    static constexpr size_t kRingBufferSize = 300;
    static constexpr double kSlowEventThresholdMs = 5.0;
    static constexpr double kRegressionThresholdPercent = 20.0;

    PerformanceCounters() = default;

    // --- Frame Timing ---
    void record_frame_time(double time_ms);
    [[nodiscard]] auto frame_time_average() const -> double;
    [[nodiscard]] auto frame_time_p95() const -> double;
    [[nodiscard]] auto frame_time_buffer() const
        -> const MetricsRingBuffer<double, kRingBufferSize>&
    {
        return frame_times_;
    }

    // --- EventBus Throughput ---
    void increment_event_count();
    [[nodiscard]] auto event_count() const -> uint64_t
    {
        return event_count_.load();
    }
    void record_event_throughput(double events_per_second);
    [[nodiscard]] auto event_throughput_average() const -> double;

    // --- Memory Usage ---
    void record_memory_usage(size_t bytes);
    [[nodiscard]] auto memory_usage_latest() const -> size_t;
    [[nodiscard]] auto memory_delta_mb() const -> double;

    // --- File Cache ---
    void record_cache_hit();
    void record_cache_miss();
    [[nodiscard]] auto cache_hit_rate() const -> double;

    // --- Startup Timing ---
    void record_startup_phase(const std::string& name, std::chrono::microseconds duration);
    [[nodiscard]] auto startup_phases() const -> const std::vector<StartupPhase>&
    {
        return startup_phases_;
    }
    [[nodiscard]] auto total_startup_time() const -> std::chrono::microseconds;

    // --- Slow Event Detection ---
    void record_event_duration(double time_ms);
    [[nodiscard]] auto slow_event_count() const -> uint64_t
    {
        return slow_events_.load();
    }

    // --- Layout Reflow ---
    void increment_reflow_count();
    [[nodiscard]] auto reflow_count() const -> uint64_t
    {
        return reflow_count_.load();
    }

    // --- Render Calls ---
    void increment_render_count();
    [[nodiscard]] auto render_count() const -> uint64_t
    {
        return render_count_.load();
    }

    // --- Regression Detection ---
    [[nodiscard]] auto detect_frame_regression(double baseline_avg) const -> bool;

    // --- Report ---
    [[nodiscard]] auto export_json() const -> std::string;

private:
    MetricsRingBuffer<double, kRingBufferSize> frame_times_;
    MetricsRingBuffer<double, kRingBufferSize> event_throughput_;
    MetricsRingBuffer<size_t, kRingBufferSize> memory_samples_;

    std::atomic<uint64_t> event_count_{0};
    std::atomic<uint64_t> cache_hits_{0};
    std::atomic<uint64_t> cache_misses_{0};
    std::atomic<uint64_t> slow_events_{0};
    std::atomic<uint64_t> reflow_count_{0};
    std::atomic<uint64_t> render_count_{0};

    std::vector<StartupPhase> startup_phases_;
};

} // namespace markamp::core
