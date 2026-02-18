/// TraceSpan.h — V7 Phase 34: Trace spans & metrics collection
///
/// RAII span scoping with nested spans, attribute recording,
/// and counter/histogram metrics.

#pragma once

#include "TelemetryAbstraction.h"

#include <chrono>
#include <string>
#include <vector>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// Span Builder
// ══════════════════════════════════════════════════════════════════════════════

/// Fluent builder for trace spans.
class SpanBuilder
{
public:
    explicit SpanBuilder(std::string name)
        : name_(std::move(name))
    {
    }

    /// Add an attribute.
    auto with_attribute(std::string key, std::string value) -> SpanBuilder&
    {
        attrs_[std::move(key)] = std::move(value);
        return *this;
    }

    /// Start the span and return a TracerScope.
    [[nodiscard]] auto start() -> TracerScope
    {
        return TracerScope(telemetry_provider(), name_, attrs_);
    }

    /// Start the span on a specific provider.
    [[nodiscard]] auto start_on(TelemetryProvider& provider) -> TracerScope
    {
        return TracerScope(provider, name_, attrs_);
    }

private:
    std::string name_;
    AttributeMap attrs_;
};

// ══════════════════════════════════════════════════════════════════════════════
// Metrics Helpers
// ══════════════════════════════════════════════════════════════════════════════

/// Increment a named counter.
inline void increment(std::string_view name, double value = 1.0, const AttributeMap& attrs = {})
{
    telemetry_provider().increment_counter(name, value, attrs);
}

/// Record a histogram observation.
inline void observe(std::string_view name, double value, const AttributeMap& attrs = {})
{
    telemetry_provider().record_histogram(name, value, attrs);
}

/// Record a duration observation in milliseconds.
inline void observe_duration(std::string_view name,
                             std::chrono::steady_clock::time_point start_time,
                             const AttributeMap& attrs = {})
{
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                        std::chrono::steady_clock::now() - start_time)
                        .count();
    telemetry_provider().record_histogram(name, static_cast<double>(duration), attrs);
}

// ══════════════════════════════════════════════════════════════════════════════
// Timed Scope
// ══════════════════════════════════════════════════════════════════════════════

/// RAII scope that measures and records its lifetime as a histogram observation.
class TimedScope
{
public:
    explicit TimedScope(std::string metric_name, AttributeMap attrs = {})
        : metric_name_(std::move(metric_name))
        , attrs_(std::move(attrs))
        , start_time_(std::chrono::steady_clock::now())
    {
    }

    ~TimedScope()
    {
        observe_duration(metric_name_, start_time_, attrs_);
    }

    // Non-copyable, non-movable
    TimedScope(const TimedScope&) = delete;
    auto operator=(const TimedScope&) -> TimedScope& = delete;
    TimedScope(TimedScope&&) = delete;
    auto operator=(TimedScope&&) -> TimedScope& = delete;

    /// Get elapsed time so far.
    [[nodiscard]] auto elapsed() const -> std::chrono::microseconds
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - start_time_);
    }

private:
    std::string metric_name_;
    AttributeMap attrs_;
    std::chrono::steady_clock::time_point start_time_;
};

} // namespace markamp::core
