/// TelemetryAbstraction.cpp — V7 Phase 33: OpenTelemetry abstraction layer

#include "TelemetryAbstraction.h"

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// InMemory Provider
// ══════════════════════════════════════════════════════════════════════════════

void InMemoryTelemetryProvider::start_span(std::string_view name, const AttributeMap& /*attrs*/)
{
    span_names_.emplace_back(name);
}

void InMemoryTelemetryProvider::end_span()
{
    // No-op for in-memory; spans are already recorded by name
}

void InMemoryTelemetryProvider::increment_counter(std::string_view name,
                                                  double value,
                                                  const AttributeMap& attrs)
{
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::system_clock::now().time_since_epoch())
                      .count();

    metrics_.push_back(MetricRecord{
        .name = std::string(name),
        .type = MetricType::Counter,
        .value = value,
        .attributes = attrs,
        .timestamp_ms = now_ms,
    });
}

void InMemoryTelemetryProvider::record_histogram(std::string_view name,
                                                 double value,
                                                 const AttributeMap& attrs)
{
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::system_clock::now().time_since_epoch())
                      .count();

    metrics_.push_back(MetricRecord{
        .name = std::string(name),
        .type = MetricType::Histogram,
        .value = value,
        .attributes = attrs,
        .timestamp_ms = now_ms,
    });
}

void InMemoryTelemetryProvider::clear()
{
    span_names_.clear();
    metrics_.clear();
}

// ══════════════════════════════════════════════════════════════════════════════
// Tracer Scope
// ══════════════════════════════════════════════════════════════════════════════

TracerScope::TracerScope(TelemetryProvider& provider,
                         std::string_view name,
                         const AttributeMap& attrs)
    : provider_(provider)
{
    provider_.start_span(name, attrs);
}

TracerScope::~TracerScope()
{
    provider_.end_span();
}

// ══════════════════════════════════════════════════════════════════════════════
// Global Provider
// ══════════════════════════════════════════════════════════════════════════════

static std::shared_ptr<TelemetryProvider> g_telemetry_provider;

void set_telemetry_provider(std::shared_ptr<TelemetryProvider> provider)
{
    g_telemetry_provider = std::move(provider);
}

auto telemetry_provider() -> TelemetryProvider&
{
    if (!g_telemetry_provider)
    {
        static NoOpTelemetryProvider noop;
        return noop;
    }
    return *g_telemetry_provider;
}

} // namespace markamp::core
