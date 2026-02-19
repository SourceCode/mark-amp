#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// UX metric type.
enum class MetricType : uint8_t
{
    kCommandLatency,    ///< Time to complete a command
    kMisclickRate,      ///< Accidental clicks detected
    kUndoAfterAction,   ///< Undo triggered right after action
    kSearchAbandon,     ///< Search started but not completed
    kErrorRecoveryTime, ///< Time from error to resolution
};

/// A recorded UX metric event.
struct MetricEvent
{
    std::string event_id;
    MetricType type{MetricType::kCommandLatency};
    double value{0.0};   ///< Metric-specific value (ms, rate, etc.)
    std::string surface; ///< Which surface generated it
};

/// Testable model for UX Instrumentation & Metrics (Phase 38).
///
/// Encapsulates:
/// - Metric event recording and retrieval
/// - Per-type aggregation (average, count)
/// - Telemetry opt-in/out
/// - Event emission when enabled
class UxMetricsModel
{
public:
    // ── Recording ───────────────────────────────────────────────────

    void record(MetricEvent event);
    [[nodiscard]] auto events() const -> const std::vector<MetricEvent>&;
    void clear_events();

    // ── Aggregation ─────────────────────────────────────────────────

    [[nodiscard]] auto count_by_type(MetricType type) const -> int;
    [[nodiscard]] auto average_by_type(MetricType type) const -> double;

    // ── Telemetry ───────────────────────────────────────────────────

    void set_telemetry_enabled(bool enabled);
    [[nodiscard]] auto telemetry_enabled() const -> bool;

    /// Returns true if the event should be emitted (telemetry on + not excluded).
    [[nodiscard]] auto should_emit(MetricType type) const -> bool;

    void exclude_metric(MetricType type);
    void include_metric(MetricType type);

private:
    std::vector<MetricEvent> events_;
    bool telemetry_enabled_{false};
    std::vector<MetricType> excluded_types_;
};

} // namespace markamp::ui
