// ============================================================================
// File: src/rendering/FxDiagnostics.cpp
// Phase 28: FX Visual Effects System — Runtime FX Performance Diagnostics
// ============================================================================

#include "FxDiagnostics.h"

#include <algorithm>
#include <numeric>

namespace markamp::rendering
{

auto FxDiagnostics::record_frame(const FxFrameResult& result, float frame_duration_ms) -> void
{
    FrameRecord record;
    record.duration_ms = frame_duration_ms;
    record.passes_executed = result.passes_executed;
    record.passes_skipped = result.passes_skipped;

    frame_history_.push_back(record);

    // Maintain window size.
    if (frame_history_.size() > max_window_size_)
    {
        frame_history_.erase(frame_history_.begin());
    }
}

auto FxDiagnostics::record_pass_timing(const std::string& pass_name,
                                       float timing_ms,
                                       bool was_skipped) -> void
{
    auto& metrics = pass_data_[pass_name];
    metrics.pass_name = pass_name;

    if (was_skipped)
    {
        ++metrics.skip_count;
        return;
    }

    ++metrics.execution_count;
    metrics.max_time_ms = std::max(metrics.max_time_ms, timing_ms);

    // Running average.
    const auto exec_count = static_cast<float>(metrics.execution_count);
    metrics.avg_time_ms = metrics.avg_time_ms + (timing_ms - metrics.avg_time_ms) / exec_count;
}

auto FxDiagnostics::get_snapshot() const -> FxMetricsSnapshot
{
    FxMetricsSnapshot snapshot;
    snapshot.total_frames = static_cast<uint32_t>(frame_history_.size());
    snapshot.degradation_count = degradation_count_;

    if (frame_history_.empty())
    {
        return snapshot;
    }

    float total_time = 0.0F;
    float min_time = frame_history_.front().duration_ms;
    float max_time = 0.0F;
    std::size_t max_passes = 0;

    for (const auto& frame : frame_history_)
    {
        total_time += frame.duration_ms;
        min_time = std::min(min_time, frame.duration_ms);
        max_time = std::max(max_time, frame.duration_ms);
        max_passes = std::max(max_passes, frame.passes_executed);
    }

    snapshot.avg_frame_time_ms = total_time / static_cast<float>(frame_history_.size());
    snapshot.min_frame_time_ms = min_time;
    snapshot.max_frame_time_ms = max_time;
    snapshot.active_pass_count = max_passes;

    // Collect pass metrics.
    for (const auto& [name, metrics] : pass_data_)
    {
        snapshot.pass_metrics.push_back(metrics);
    }

    return snapshot;
}

auto FxDiagnostics::get_pass_metrics(const std::string& pass_name) const
    -> std::optional<PassMetrics>
{
    auto iter = pass_data_.find(pass_name);
    if (iter != pass_data_.end())
    {
        return iter->second;
    }
    return std::nullopt;
}

auto FxDiagnostics::get_health_report(float budget_ms) const -> FxHealthReport
{
    FxHealthReport report;

    if (frame_history_.empty())
    {
        report.level = FxHealthReport::HealthLevel::kExcellent;
        report.summary = "No FX frames recorded yet";
        return report;
    }

    const auto snapshot = get_snapshot();
    report.budget_utilization = snapshot.avg_frame_time_ms / budget_ms;

    // Count violations.
    uint32_t violations = 0;
    for (const auto& frame : frame_history_)
    {
        if (frame.duration_ms > budget_ms)
        {
            ++violations;
        }
    }
    const float violation_rate =
        static_cast<float>(violations) / static_cast<float>(frame_history_.size());

    // Determine health level.
    if (violation_rate < 0.05F)
    {
        report.level = FxHealthReport::HealthLevel::kExcellent;
        report.summary = "FX performance is excellent — well within budget";
    }
    else if (violation_rate < 0.15F)
    {
        report.level = FxHealthReport::HealthLevel::kGood;
        report.summary = "FX performance is good — occasional budget spikes";
    }
    else if (violation_rate < 0.35F)
    {
        report.level = FxHealthReport::HealthLevel::kWarning;
        report.summary = "FX performance has warnings — frequent budget misses";
        report.recommendations.emplace_back("Consider lowering quality tier");
        report.recommendations.emplace_back("Disable expensive passes");
    }
    else
    {
        report.level = FxHealthReport::HealthLevel::kCritical;
        report.summary = "FX performance is critical — consistently over budget";
        report.recommendations.emplace_back("Lower quality tier immediately");
        report.recommendations.emplace_back("Disable motion effects");
        report.recommendations.emplace_back("Enable low power mode");
    }

    return report;
}

auto FxDiagnostics::total_frames() const noexcept -> uint32_t
{
    return static_cast<uint32_t>(frame_history_.size());
}

auto FxDiagnostics::avg_frame_time() const noexcept -> float
{
    if (frame_history_.empty())
    {
        return 0.0F;
    }
    float total = 0.0F;
    for (const auto& frame : frame_history_)
    {
        total += frame.duration_ms;
    }
    return total / static_cast<float>(frame_history_.size());
}

auto FxDiagnostics::reset() -> void
{
    frame_history_.clear();
    pass_data_.clear();
    degradation_count_ = 0;
}

auto FxDiagnostics::set_window_size(uint32_t max_frames) -> void
{
    max_window_size_ = max_frames;
    while (frame_history_.size() > max_window_size_)
    {
        frame_history_.erase(frame_history_.begin());
    }
}

auto FxDiagnostics::window_size() const noexcept -> uint32_t
{
    return max_window_size_;
}

} // namespace markamp::rendering
