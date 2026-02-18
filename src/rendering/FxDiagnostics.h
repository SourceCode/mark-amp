// ============================================================================
// File: src/rendering/FxDiagnostics.h
// Phase 28: FX Visual Effects System — Runtime FX Performance Diagnostics
// ============================================================================
#pragma once

#include "FxEngine.h"

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::rendering
{

/// Per-pass timing metrics.
struct PassMetrics
{
    std::string pass_name;
    float avg_time_ms{0.0F};
    float max_time_ms{0.0F};
    uint32_t execution_count{0};
    uint32_t skip_count{0};
};

/// Aggregate FX performance snapshot.
struct FxMetricsSnapshot
{
    float avg_frame_time_ms{0.0F};
    float max_frame_time_ms{0.0F};
    float min_frame_time_ms{0.0F};
    uint32_t total_frames{0};
    uint32_t degradation_count{0};
    std::size_t active_pass_count{0};
    std::vector<PassMetrics> pass_metrics;
};

/// Overall FX health assessment.
struct FxHealthReport
{
    enum class HealthLevel : uint8_t
    {
        kExcellent, ///< All within budget
        kGood,      ///< Minor occasional spikes
        kWarning,   ///< Frequent budget misses
        kCritical,  ///< Consistently over budget
    };

    HealthLevel level{HealthLevel::kExcellent};
    float budget_utilization{0.0F}; ///< 0.0–1.0+ (>1.0 = over budget)
    std::string summary;
    std::vector<std::string> recommendations;
};

/// Runtime FX performance metrics and diagnostics.
class FxDiagnostics
{
public:
    FxDiagnostics() = default;

    // ── Recording ──

    /// Record a frame result with its execution duration.
    auto record_frame(const FxFrameResult& result, float frame_duration_ms) -> void;

    /// Record timing for a specific pass.
    auto record_pass_timing(const std::string& pass_name, float timing_ms, bool was_skipped)
        -> void;

    // ── Queries ──

    /// Get the current metrics snapshot.
    [[nodiscard]] auto get_snapshot() const -> FxMetricsSnapshot;

    /// Get metrics for a specific pass.
    [[nodiscard]] auto get_pass_metrics(const std::string& pass_name) const
        -> std::optional<PassMetrics>;

    /// Generate a health report based on accumulated data.
    [[nodiscard]] auto get_health_report(float budget_ms = 16.0F) const -> FxHealthReport;

    /// Get the total number of recorded frames.
    [[nodiscard]] auto total_frames() const noexcept -> uint32_t;

    /// Get the average frame time over the recording window.
    [[nodiscard]] auto avg_frame_time() const noexcept -> float;

    // ── Management ──

    /// Reset all accumulated metrics.
    auto reset() -> void;

    /// Set the maximum recording window size (in frames).
    auto set_window_size(uint32_t max_frames) -> void;

    /// Get the current window size.
    [[nodiscard]] auto window_size() const noexcept -> uint32_t;

private:
    static constexpr uint32_t kDefaultWindowSize = 120;

    struct FrameRecord
    {
        float duration_ms{0.0F};
        std::size_t passes_executed{0};
        std::size_t passes_skipped{0};
    };

    std::vector<FrameRecord> frame_history_;
    std::unordered_map<std::string, PassMetrics> pass_data_;
    uint32_t max_window_size_{kDefaultWindowSize};
    uint32_t degradation_count_{0};
};

} // namespace markamp::rendering
