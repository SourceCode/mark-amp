// ============================================================================
// File: src/core/PerformanceProfiler.h
// Phase 30: Performance Optimization — Unified performance monitoring facade
// ============================================================================
#pragma once

#include "AdaptiveThrottle.h"
#include "StartupPhase.h"
#include "StartupTimer.h"
#include "Watchdog.h"

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>

namespace markamp::core
{

/// Per-frame timing statistics.
struct FrameStats
{
    double avg_frame_us{0.0};
    double min_frame_us{0.0};
    double max_frame_us{0.0};
    double p95_frame_us{0.0};
    uint64_t frame_count{0};
    uint64_t overbudget_count{0};
};

/// Complete performance snapshot at a point in time.
struct PerfSnapshot
{
    FrameStats frame_stats;
    double startup_ms{0.0};
    ActivityMode activity_mode{ActivityMode::Idle};
    double memory_usage_bytes{0.0};
    uint64_t watchdog_stall_count{0};
    int64_t timestamp_ms{0};
};

/// Unified performance monitoring coordinator.
///
/// Ties together StartupTimer, AdaptiveThrottle, and Watchdog into a single
/// facade for frame profiling, startup measurement, and stall detection.
///
/// Phase 30 Tasks 1, 2, 11, 17: Startup profiling + frame timing + watchdog.
class PerformanceProfiler
{
public:
    static constexpr size_t kFrameHistorySize = 64;          // power of 2 for efficiency
    static constexpr double kDefaultFrameBudgetUs = 16667.0; // ~60fps

    PerformanceProfiler();

    // ── Startup Profiling ──────────────────────────────────────────────

    /// Record a startup phase checkpoint.
    void checkpoint(StartupPhase phase);

    /// Record a named checkpoint (freeform).
    void checkpoint(const std::string& name);

    /// Total startup time elapsed.
    [[nodiscard]] auto startup_elapsed_ms() const -> double;

    /// Number of checkpoints recorded.
    [[nodiscard]] auto checkpoint_count() const -> size_t;

    // ── Frame Profiling ────────────────────────────────────────────────

    /// Call at the start of each frame.
    void begin_frame();

    /// Call at the end of each frame. Records the frame duration.
    void end_frame();

    /// Get current frame statistics (rolling window).
    [[nodiscard]] auto frame_stats() const -> FrameStats;

    /// Current frame budget in microseconds.
    [[nodiscard]] auto frame_budget_us() const -> double;

    /// Set frame budget in microseconds.
    void set_frame_budget(double budget_us);

    /// Whether frame profiling capture is active.
    [[nodiscard]] auto is_profiling() const -> bool;

    /// Begin capturing N frames for a profiling report.
    void begin_profile_capture(uint64_t frame_count);

    /// Whether profile capture has completed.
    [[nodiscard]] auto profile_capture_complete() const -> bool;

    // ── Adaptive Throttle ──────────────────────────────────────────────

    /// Signal user activity (delegates to AdaptiveThrottle).
    void update_activity();

    /// Current activity mode.
    [[nodiscard]] auto activity_mode() const -> ActivityMode;

    /// Current frame budget from throttle.
    [[nodiscard]] auto throttle_budget() const -> std::chrono::microseconds;

    // ── Watchdog ───────────────────────────────────────────────────────

    /// Start the watchdog monitor thread.
    void start_watchdog();

    /// Stop the watchdog.
    void stop_watchdog();

    /// Watchdog stall count.
    [[nodiscard]] auto stall_count() const -> uint64_t;

    // ── Snapshot & Reporting ───────────────────────────────────────────

    /// Take a complete performance snapshot.
    [[nodiscard]] auto take_snapshot() const -> PerfSnapshot;

    /// Export snapshot as JSON string.
    [[nodiscard]] auto to_json() const -> std::string;

    /// Dump startup report to log.
    void dump_startup_report() const;

private:
    StartupTimer startup_timer_;
    AdaptiveThrottle throttle_;
    Watchdog watchdog_;

    // Frame timing ring buffer
    std::array<double, kFrameHistorySize> frame_times_{};
    size_t frame_write_pos_{0};
    uint64_t total_frames_{0};
    uint64_t overbudget_frames_{0};
    double frame_budget_us_{kDefaultFrameBudgetUs};

    // Frame start time
    std::chrono::high_resolution_clock::time_point frame_start_;

    // Profile capture state
    uint64_t capture_target_{0};
    uint64_t capture_count_{0};
    bool capturing_{false};
};

} // namespace markamp::core
