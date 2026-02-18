// ============================================================================
// File: src/canvas/CanvasPerformanceMonitor.h
// Phase 11: Canvas Workbench Shell — FPS, object count, render budget
// ============================================================================
#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// A single render-budget violation.
struct BudgetViolation
{
    std::string metric_name; ///< e.g. "frame_time_ms", "object_count"
    double current_value{0.0};
    double threshold{0.0};
    std::string suggestion; ///< Remediation hint
};

/// Performance budget thresholds.
struct CanvasPerformanceBudget
{
    double target_fps{60.0};
    double frame_budget_ms{16.67}; ///< 1000 / target_fps
    size_t max_visible_objects{5000};
    size_t max_total_objects{50000};
    double max_memory_mb{512.0};
    double max_render_time_ms{12.0}; ///< Leave 4ms headroom for input
    size_t max_undo_stack_depth{200};
};

/// Snapshot of current performance metrics.
struct CanvasPerformanceSnapshot
{
    double fps{0.0};
    double frame_time_ms{0.0};
    double render_time_ms{0.0};
    size_t total_objects{0};
    size_t visible_objects{0};
    double estimated_memory_mb{0.0};
    size_t undo_stack_depth{0};
    uint64_t frame_number{0};
    std::chrono::steady_clock::time_point timestamp;
};

/// Health status classification.
enum class CanvasHealthStatus : uint8_t
{
    kHealthy, ///< All metrics within budget
    kWarning, ///< Close to threshold
    kCritical ///< Exceeding budget
};

/// Monitors canvas performance and checks budgets.
///
/// Call `begin_frame()` / `end_frame()` around each render pass
/// and `check_budget()` periodically to detect performance issues.
class CanvasPerformanceMonitor
{
public:
    CanvasPerformanceMonitor();
    explicit CanvasPerformanceMonitor(const CanvasPerformanceBudget& budget);

    // ── Frame Timing ───────────────────────────────────────────────

    /// Call at the start of each render frame.
    auto begin_frame() -> void;

    /// Call at the end of each render frame.
    auto end_frame() -> void;

    /// Mark the start of the actual render pass (within a frame).
    auto begin_render() -> void;

    /// Mark the end of the actual render pass.
    auto end_render() -> void;

    // ── Metrics Update ─────────────────────────────────────────────

    /// Update object counts (call after board changes).
    auto set_object_counts(size_t total, size_t visible) -> void;

    /// Update estimated memory usage in MB.
    auto set_memory_estimate(double megabytes) -> void;

    /// Update undo stack depth.
    auto set_undo_depth(size_t depth) -> void;

    // ── Queries ────────────────────────────────────────────────────

    /// Current FPS (smoothed over the last N frames).
    [[nodiscard]] auto fps() const -> double;

    /// Last frame time in milliseconds.
    [[nodiscard]] auto frame_time_ms() const -> double;

    /// Last render time in milliseconds.
    [[nodiscard]] auto render_time_ms() const -> double;

    /// Current performance snapshot.
    [[nodiscard]] auto snapshot() const -> CanvasPerformanceSnapshot;

    /// Overall health status.
    [[nodiscard]] auto health_status() const -> CanvasHealthStatus;

    /// Check all budgets and return any violations.
    [[nodiscard]] auto check_budget() const -> std::vector<BudgetViolation>;

    // ── Budget Configuration ───────────────────────────────────────

    [[nodiscard]] auto budget() const -> const CanvasPerformanceBudget&;
    auto set_budget(const CanvasPerformanceBudget& new_budget) -> void;

    // ── History ────────────────────────────────────────────────────

    /// Frame count since monitor was created.
    [[nodiscard]] auto frame_count() const -> uint64_t;

    /// Reset all accumulated metrics.
    auto reset() -> void;

private:
    CanvasPerformanceBudget budget_;

    // Frame timing
    std::chrono::steady_clock::time_point frame_start_;
    std::chrono::steady_clock::time_point render_start_;
    double last_frame_ms_{0.0};
    double last_render_ms_{0.0};

    // FPS smoothing (exponential moving average)
    double smoothed_fps_{60.0};
    static constexpr double kFpsSmoothingAlpha = 0.1;

    // Object metrics
    size_t total_objects_{0};
    size_t visible_objects_{0};
    double memory_mb_{0.0};
    size_t undo_depth_{0};
    uint64_t frame_count_{0};
};

} // namespace markamp::canvas
