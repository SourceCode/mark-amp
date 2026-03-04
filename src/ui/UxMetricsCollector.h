#pragma once

/**
 * @file UxMetricsCollector.h
 * @brief Phase 38 Task 1-2: UX metric types, event recording, aggregation.
 *
 * Defines UX metrics (misclick, latency, undo-after-action, command frequency),
 * records lightweight events, and aggregates (count, average, percentile).
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// UX metric type.
enum class UxMetricType : uint8_t
{
    kMisclick,         ///< Click on non-interactive area
    kCommandLatency,   ///< Time from invocation to completion
    kUndoAfterAction,  ///< Undo within N seconds of action
    kCommandFrequency, ///< How often a command is used
    kDragCancelRate,   ///< Drag operations cancelled vs completed
    kKeyboardNavRate,  ///< Keyboard vs mouse navigation ratio
};

/// A single metric event.
struct MetricEvent
{
    UxMetricType type{UxMetricType::kMisclick};
    std::string context; ///< Where it happened (e.g. "toolbar", "editor")
    double value{0.0};   ///< Metric value (ms for latency, count for freq)
    int timestamp_ms{0};

    /// Get type as string.
    [[nodiscard]] auto type_name() const -> std::string;
};

/// Aggregated metric summary.
struct MetricSummary
{
    UxMetricType type{UxMetricType::kMisclick};
    int count{0};
    double total{0.0};
    double min_value{0.0};
    double max_value{0.0};

    /// Get the average value.
    [[nodiscard]] auto average() const -> double;
};

/**
 * @brief Collects and aggregates UX quality metrics.
 */
class UxMetricsCollector
{
public:
    UxMetricsCollector() = default;

    // ── Recording ──────────────────────────────────────────────────

    /// Record a metric event.
    void record(const MetricEvent& event);

    /// Record a simple metric.
    void record(UxMetricType type, double value, const std::string& context = "");

    /// Get total events recorded.
    [[nodiscard]] auto event_count() const -> int;

    /// Get events by type.
    [[nodiscard]] auto events_of_type(UxMetricType type) const -> std::vector<MetricEvent>;

    // ── Aggregation ────────────────────────────────────────────────

    /// Get summary for a metric type.
    [[nodiscard]] auto summary(UxMetricType type) const -> MetricSummary;

    /// Get all summaries.
    [[nodiscard]] auto all_summaries() const -> std::vector<MetricSummary>;

    // ── Configuration ──────────────────────────────────────────────

    /// Enable/disable collection.
    void set_enabled(bool enabled);

    /// Check if collection is enabled.
    [[nodiscard]] auto is_enabled() const -> bool;

    /// Set max events to retain.
    void set_max_events(int max_events);

    /// Get max events.
    [[nodiscard]] auto max_events() const -> int;

    /// Clear all recorded events.
    void clear();

private:
    std::vector<MetricEvent> events_;
    bool enabled_{true};
    int max_events_{10000};
};

} // namespace markamp::ui
