/// TelemetryAbstraction.h — V7 Phase 33: OpenTelemetry abstraction layer
///
/// Conditional compilation: real OpenTelemetry or no-op stubs.
/// Provides TelemetryProvider interface and scope guards.

#pragma once

#include "Result.h"

#include <chrono>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// Telemetry Types
// ══════════════════════════════════════════════════════════════════════════════

/// Attribute value for spans and metrics.
using AttributeMap = std::unordered_map<std::string, std::string>;

/// Metric type enumeration.
enum class MetricType : uint8_t
{
    Counter = 0,
    Histogram = 1,
    Gauge = 2,
};

/// A recorded metric data point.
struct MetricRecord
{
    std::string name;
    MetricType type{MetricType::Counter};
    double value{0.0};
    AttributeMap attributes;
    int64_t timestamp_ms{0};
};

// ══════════════════════════════════════════════════════════════════════════════
// Telemetry Provider Interface
// ══════════════════════════════════════════════════════════════════════════════

/// Abstract interface for telemetry backends.
class TelemetryProvider
{
public:
    virtual ~TelemetryProvider() = default;

    /// Start a trace span.
    virtual void start_span(std::string_view name, const AttributeMap& attrs = {}) = 0;

    /// End the current trace span.
    virtual void end_span() = 0;

    /// Record a counter increment.
    virtual void increment_counter(std::string_view name,
                                   double value = 1.0,
                                   const AttributeMap& attrs = {}) = 0;

    /// Record a histogram observation.
    virtual void
    record_histogram(std::string_view name, double value, const AttributeMap& attrs = {}) = 0;

    /// Flush all pending telemetry.
    virtual auto flush() -> Result<void> = 0;
};

// ══════════════════════════════════════════════════════════════════════════════
// No-Op Provider (default)
// ══════════════════════════════════════════════════════════════════════════════

/// No-op implementation for when OTel is not linked.
class NoOpTelemetryProvider final : public TelemetryProvider
{
public:
    void start_span(std::string_view /*name*/, const AttributeMap& /*attrs*/ = {}) override {}
    void end_span() override {}
    void increment_counter(std::string_view /*name*/,
                           double /*value*/ = 1.0,
                           const AttributeMap& /*attrs*/ = {}) override
    {
    }
    void record_histogram(std::string_view /*name*/,
                          double /*value*/,
                          const AttributeMap& /*attrs*/ = {}) override
    {
    }
    auto flush() -> Result<void> override
    {
        return {};
    }
};

// ══════════════════════════════════════════════════════════════════════════════
// In-Memory Provider (for testing)
// ══════════════════════════════════════════════════════════════════════════════

/// Captures telemetry data in memory for testing.
class InMemoryTelemetryProvider final : public TelemetryProvider
{
public:
    void start_span(std::string_view name, const AttributeMap& attrs = {}) override;
    void end_span() override;
    void increment_counter(std::string_view name,
                           double value = 1.0,
                           const AttributeMap& attrs = {}) override;
    void
    record_histogram(std::string_view name, double value, const AttributeMap& attrs = {}) override;
    auto flush() -> Result<void> override
    {
        return {};
    }

    /// Get recorded spans.
    [[nodiscard]] auto span_names() const -> std::vector<std::string>
    {
        return span_names_;
    }

    /// Get recorded metrics.
    [[nodiscard]] auto metrics() const -> std::vector<MetricRecord>
    {
        return metrics_;
    }

    /// Get total metrics count.
    [[nodiscard]] auto metric_count() const -> size_t
    {
        return metrics_.size();
    }

    /// Clear all recorded data.
    void clear();

private:
    std::vector<std::string> span_names_;
    std::vector<MetricRecord> metrics_;
};

// ══════════════════════════════════════════════════════════════════════════════
// Tracer Scope (RAII)
// ══════════════════════════════════════════════════════════════════════════════

/// RAII guard that starts a span on construction and ends it on destruction.
class TracerScope
{
public:
    explicit TracerScope(TelemetryProvider& provider,
                         std::string_view name,
                         const AttributeMap& attrs = {});
    ~TracerScope();

    // Non-copyable, non-movable
    TracerScope(const TracerScope&) = delete;
    auto operator=(const TracerScope&) -> TracerScope& = delete;
    TracerScope(TracerScope&&) = delete;
    auto operator=(TracerScope&&) -> TracerScope& = delete;

private:
    TelemetryProvider& provider_;
};

// ══════════════════════════════════════════════════════════════════════════════
// Global Provider
// ══════════════════════════════════════════════════════════════════════════════

/// Set the global telemetry provider.
void set_telemetry_provider(std::shared_ptr<TelemetryProvider> provider);

/// Get the global telemetry provider.
[[nodiscard]] auto telemetry_provider() -> TelemetryProvider&;

} // namespace markamp::core
