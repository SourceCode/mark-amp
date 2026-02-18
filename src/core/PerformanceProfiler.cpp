// ============================================================================
// File: src/core/PerformanceProfiler.cpp
// Phase 30: Performance Optimization — Unified performance monitoring facade
// ============================================================================

#include "PerformanceProfiler.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>
#include <vector>

namespace markamp::core
{

PerformanceProfiler::PerformanceProfiler()
{
    frame_times_.fill(0.0);
}

// ── Startup Profiling ──────────────────────────────────────────────────────

void PerformanceProfiler::checkpoint(StartupPhase phase)
{
    startup_timer_.checkpoint(std::string(phase_name(phase)));
}

void PerformanceProfiler::checkpoint(const std::string& name)
{
    startup_timer_.checkpoint(name);
}

auto PerformanceProfiler::startup_elapsed_ms() const -> double
{
    return startup_timer_.elapsed_ms();
}

auto PerformanceProfiler::checkpoint_count() const -> size_t
{
    return startup_timer_.checkpoint_count();
}

// ── Frame Profiling ────────────────────────────────────────────────────────

void PerformanceProfiler::begin_frame()
{
    frame_start_ = std::chrono::high_resolution_clock::now();
}

void PerformanceProfiler::end_frame()
{
    auto elapsed = std::chrono::high_resolution_clock::now() - frame_start_;
    const double frame_us =
        static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count()) /
        1000.0;

    frame_times_.at(frame_write_pos_ % kFrameHistorySize) = frame_us;
    ++frame_write_pos_;
    ++total_frames_;

    if (frame_us > frame_budget_us_)
    {
        ++overbudget_frames_;
    }

    // Heartbeat the watchdog on each frame
    watchdog_.heartbeat();

    // Update capture state
    if (capturing_)
    {
        ++capture_count_;
        if (capture_count_ >= capture_target_)
        {
            capturing_ = false;
        }
    }
}

auto PerformanceProfiler::frame_stats() const -> FrameStats
{
    FrameStats stats;
    stats.frame_count = total_frames_;
    stats.overbudget_count = overbudget_frames_;

    if (total_frames_ == 0)
    {
        return stats;
    }

    // Determine how many valid samples we have
    const size_t sample_count = std::min(static_cast<size_t>(total_frames_), kFrameHistorySize);

    // Gather valid samples
    std::vector<double> samples;
    samples.reserve(sample_count);
    for (size_t idx = 0; idx < sample_count; ++idx)
    {
        // Read from the most recent entries backwards
        const size_t ring_idx = (frame_write_pos_ - sample_count + idx) % kFrameHistorySize;
        samples.push_back(frame_times_.at(ring_idx));
    }

    // Compute stats
    stats.avg_frame_us =
        std::accumulate(samples.begin(), samples.end(), 0.0) / static_cast<double>(sample_count);

    auto minmax = std::minmax_element(samples.begin(), samples.end());
    stats.min_frame_us = *minmax.first;
    stats.max_frame_us = *minmax.second;

    // P95: sort and pick 95th percentile
    std::sort(samples.begin(), samples.end());
    const size_t p95_idx =
        static_cast<size_t>(std::ceil(0.95 * static_cast<double>(sample_count))) - 1;
    stats.p95_frame_us = samples[std::min(p95_idx, sample_count - 1)];

    return stats;
}

auto PerformanceProfiler::frame_budget_us() const -> double
{
    return frame_budget_us_;
}

void PerformanceProfiler::set_frame_budget(double budget_us)
{
    frame_budget_us_ = budget_us;
}

auto PerformanceProfiler::is_profiling() const -> bool
{
    return capturing_;
}

void PerformanceProfiler::begin_profile_capture(uint64_t frame_count)
{
    capture_target_ = frame_count;
    capture_count_ = 0;
    capturing_ = true;
}

auto PerformanceProfiler::profile_capture_complete() const -> bool
{
    return !capturing_ && capture_target_ > 0;
}

// ── Adaptive Throttle ──────────────────────────────────────────────────────

void PerformanceProfiler::update_activity()
{
    throttle_.update_activity();
}

auto PerformanceProfiler::activity_mode() const -> ActivityMode
{
    return throttle_.current_mode();
}

auto PerformanceProfiler::throttle_budget() const -> std::chrono::microseconds
{
    return throttle_.current_budget();
}

// ── Watchdog ───────────────────────────────────────────────────────────────

void PerformanceProfiler::start_watchdog()
{
    watchdog_.start();
}

void PerformanceProfiler::stop_watchdog()
{
    watchdog_.stop();
}

auto PerformanceProfiler::stall_count() const -> uint64_t
{
    return watchdog_.stall_count();
}

// ── Snapshot & Reporting ───────────────────────────────────────────────────

auto PerformanceProfiler::take_snapshot() const -> PerfSnapshot
{
    PerfSnapshot snap;
    snap.frame_stats = frame_stats();
    snap.startup_ms = startup_timer_.total_ms();
    snap.activity_mode = throttle_.current_mode();
    snap.watchdog_stall_count = watchdog_.stall_count();
    snap.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::system_clock::now().time_since_epoch())
                            .count();
    return snap;
}

auto PerformanceProfiler::to_json() const -> std::string
{
    auto snap = take_snapshot();
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"timestamp_ms\": " << snap.timestamp_ms << ",\n";
    oss << "  \"startup_ms\": " << snap.startup_ms << ",\n";
    oss << "  \"activity_mode\": \""
        << (snap.activity_mode == ActivityMode::Typing ? "typing" : "idle") << "\",\n";
    oss << "  \"watchdog_stall_count\": " << snap.watchdog_stall_count << ",\n";
    oss << "  \"frame_stats\": {\n";
    oss << "    \"frame_count\": " << snap.frame_stats.frame_count << ",\n";
    oss << "    \"avg_frame_us\": " << snap.frame_stats.avg_frame_us << ",\n";
    oss << "    \"min_frame_us\": " << snap.frame_stats.min_frame_us << ",\n";
    oss << "    \"max_frame_us\": " << snap.frame_stats.max_frame_us << ",\n";
    oss << "    \"p95_frame_us\": " << snap.frame_stats.p95_frame_us << ",\n";
    oss << "    \"overbudget_count\": " << snap.frame_stats.overbudget_count << "\n";
    oss << "  }\n";
    oss << "}";
    return oss.str();
}

void PerformanceProfiler::dump_startup_report() const
{
    startup_timer_.dump_to_log();
}

} // namespace markamp::core
