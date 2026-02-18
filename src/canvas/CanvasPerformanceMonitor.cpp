// ============================================================================
// File: src/canvas/CanvasPerformanceMonitor.cpp
// Phase 11: Canvas Workbench Shell — performance monitor implementation
// ============================================================================
#include "CanvasPerformanceMonitor.h"

#include <algorithm>

namespace markamp::canvas
{

CanvasPerformanceMonitor::CanvasPerformanceMonitor()
    : frame_start_(std::chrono::steady_clock::now())
    , render_start_(std::chrono::steady_clock::now())
{
}

CanvasPerformanceMonitor::CanvasPerformanceMonitor(const CanvasPerformanceBudget& budget)
    : budget_(budget)
    , frame_start_(std::chrono::steady_clock::now())
    , render_start_(std::chrono::steady_clock::now())
{
}

// ── Frame Timing ───────────────────────────────────────────────────

auto CanvasPerformanceMonitor::begin_frame() -> void
{
    frame_start_ = std::chrono::steady_clock::now();
}

auto CanvasPerformanceMonitor::end_frame() -> void
{
    const auto now = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration<double, std::milli>(now - frame_start_);
    last_frame_ms_ = elapsed.count();

    // Exponential moving average for FPS
    const double current_fps = (last_frame_ms_ > 0.001) ? (1000.0 / last_frame_ms_) : 0.0;
    smoothed_fps_ = kFpsSmoothingAlpha * current_fps + (1.0 - kFpsSmoothingAlpha) * smoothed_fps_;

    ++frame_count_;
}

auto CanvasPerformanceMonitor::begin_render() -> void
{
    render_start_ = std::chrono::steady_clock::now();
}

auto CanvasPerformanceMonitor::end_render() -> void
{
    const auto now = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration<double, std::milli>(now - render_start_);
    last_render_ms_ = elapsed.count();
}

// ── Metrics Update ─────────────────────────────────────────────────

auto CanvasPerformanceMonitor::set_object_counts(size_t total, size_t visible) -> void
{
    total_objects_ = total;
    visible_objects_ = visible;
}

auto CanvasPerformanceMonitor::set_memory_estimate(double megabytes) -> void
{
    memory_mb_ = megabytes;
}

auto CanvasPerformanceMonitor::set_undo_depth(size_t depth) -> void
{
    undo_depth_ = depth;
}

// ── Queries ────────────────────────────────────────────────────────

auto CanvasPerformanceMonitor::fps() const -> double
{
    return smoothed_fps_;
}

auto CanvasPerformanceMonitor::frame_time_ms() const -> double
{
    return last_frame_ms_;
}

auto CanvasPerformanceMonitor::render_time_ms() const -> double
{
    return last_render_ms_;
}

auto CanvasPerformanceMonitor::snapshot() const -> CanvasPerformanceSnapshot
{
    CanvasPerformanceSnapshot snap;
    snap.fps = smoothed_fps_;
    snap.frame_time_ms = last_frame_ms_;
    snap.render_time_ms = last_render_ms_;
    snap.total_objects = total_objects_;
    snap.visible_objects = visible_objects_;
    snap.estimated_memory_mb = memory_mb_;
    snap.undo_stack_depth = undo_depth_;
    snap.frame_number = frame_count_;
    snap.timestamp = std::chrono::steady_clock::now();
    return snap;
}

auto CanvasPerformanceMonitor::health_status() const -> CanvasHealthStatus
{
    const auto violations = check_budget();
    if (violations.empty())
    {
        return CanvasHealthStatus::kHealthy;
    }

    // If any violation is critical (>150% of threshold), report critical
    for (const auto& violation : violations)
    {
        if (violation.threshold > 0.0 && violation.current_value > violation.threshold * 1.5)
        {
            return CanvasHealthStatus::kCritical;
        }
    }

    return CanvasHealthStatus::kWarning;
}

auto CanvasPerformanceMonitor::check_budget() const -> std::vector<BudgetViolation>
{
    std::vector<BudgetViolation> violations;

    if (last_frame_ms_ > budget_.frame_budget_ms)
    {
        violations.push_back(
            {"frame_time_ms",
             last_frame_ms_,
             budget_.frame_budget_ms,
             "Frame time exceeds budget; reduce visible objects or simplify rendering"});
    }

    if (last_render_ms_ > budget_.max_render_time_ms)
    {
        violations.push_back({"render_time_ms",
                              last_render_ms_,
                              budget_.max_render_time_ms,
                              "Render time exceeds budget; enable tile caching or reduce detail"});
    }

    if (visible_objects_ > budget_.max_visible_objects)
    {
        violations.push_back({"visible_objects",
                              static_cast<double>(visible_objects_),
                              static_cast<double>(budget_.max_visible_objects),
                              "Too many visible objects; use frustum culling or level-of-detail"});
    }

    if (total_objects_ > budget_.max_total_objects)
    {
        violations.push_back({"total_objects",
                              static_cast<double>(total_objects_),
                              static_cast<double>(budget_.max_total_objects),
                              "Total object count exceeds budget; consider board partitioning"});
    }

    if (memory_mb_ > budget_.max_memory_mb)
    {
        violations.push_back({"memory_mb",
                              memory_mb_,
                              budget_.max_memory_mb,
                              "Memory usage exceeds budget; unload off-screen assets"});
    }

    if (undo_depth_ > budget_.max_undo_stack_depth)
    {
        violations.push_back({"undo_depth",
                              static_cast<double>(undo_depth_),
                              static_cast<double>(budget_.max_undo_stack_depth),
                              "Undo stack too deep; trim oldest entries"});
    }

    return violations;
}

// ── Budget Configuration ───────────────────────────────────────────

auto CanvasPerformanceMonitor::budget() const -> const CanvasPerformanceBudget&
{
    return budget_;
}

auto CanvasPerformanceMonitor::set_budget(const CanvasPerformanceBudget& new_budget) -> void
{
    budget_ = new_budget;
}

// ── History ────────────────────────────────────────────────────────

auto CanvasPerformanceMonitor::frame_count() const -> uint64_t
{
    return frame_count_;
}

auto CanvasPerformanceMonitor::reset() -> void
{
    last_frame_ms_ = 0.0;
    last_render_ms_ = 0.0;
    smoothed_fps_ = budget_.target_fps;
    total_objects_ = 0;
    visible_objects_ = 0;
    memory_mb_ = 0.0;
    undo_depth_ = 0;
    frame_count_ = 0;
}

} // namespace markamp::canvas
