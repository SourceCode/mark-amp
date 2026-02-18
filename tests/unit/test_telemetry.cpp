/// test_telemetry.cpp — V7 Phases 33-35: Telemetry, trace spans, OTLP export tests

#include "core/OtlpExporter.h"
#include "core/TelemetryAbstraction.h"
#include "core/TraceSpan.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ══════════════════════════════════════════════════════════════════════════════
// NoOp Provider
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("NoOpTelemetryProvider: all methods succeed", "[telemetry]")
{
    NoOpTelemetryProvider noop;
    noop.start_span("test");
    noop.end_span();
    noop.increment_counter("counter", 1.0);
    noop.record_histogram("histo", 42.0);
    auto result = noop.flush();
    REQUIRE(result.has_value());
}

// ══════════════════════════════════════════════════════════════════════════════
// InMemory Provider
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("InMemoryProvider: records spans", "[telemetry]")
{
    InMemoryTelemetryProvider provider;
    provider.start_span("span_a");
    provider.start_span("span_b");

    auto spans = provider.span_names();
    REQUIRE(spans.size() == 2);
    REQUIRE(spans[0] == "span_a");
    REQUIRE(spans[1] == "span_b");
}

TEST_CASE("InMemoryProvider: records counter metrics", "[telemetry]")
{
    InMemoryTelemetryProvider provider;
    provider.increment_counter("requests", 5.0, {{"method", "GET"}});

    auto metrics = provider.metrics();
    REQUIRE(metrics.size() == 1);
    REQUIRE(metrics[0].name == "requests");
    REQUIRE(metrics[0].type == MetricType::Counter);
    REQUIRE(metrics[0].value == 5.0);
}

TEST_CASE("InMemoryProvider: records histogram metrics", "[telemetry]")
{
    InMemoryTelemetryProvider provider;
    provider.record_histogram("latency", 123.0);

    REQUIRE(provider.metric_count() == 1);
    auto metrics = provider.metrics();
    REQUIRE(metrics[0].type == MetricType::Histogram);
    REQUIRE(metrics[0].value == 123.0);
}

TEST_CASE("InMemoryProvider: clear resets data", "[telemetry]")
{
    InMemoryTelemetryProvider provider;
    provider.start_span("s");
    provider.increment_counter("c");
    provider.clear();

    REQUIRE(provider.span_names().empty());
    REQUIRE(provider.metrics().empty());
}

// ══════════════════════════════════════════════════════════════════════════════
// TracerScope
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("TracerScope: RAII records span", "[telemetry]")
{
    InMemoryTelemetryProvider provider;
    {
        TracerScope scope(provider, "my_operation");
    }
    REQUIRE(provider.span_names().size() == 1);
    REQUIRE(provider.span_names()[0] == "my_operation");
}

// ══════════════════════════════════════════════════════════════════════════════
// SpanBuilder
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("SpanBuilder: fluent API records span", "[telemetry]")
{
    InMemoryTelemetryProvider provider;
    {
        auto scope = SpanBuilder("build_op").with_attribute("key", "val").start_on(provider);
    }
    REQUIRE(provider.span_names().size() == 1);
    REQUIRE(provider.span_names()[0] == "build_op");
}

// ══════════════════════════════════════════════════════════════════════════════
// TimedScope
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("TimedScope: captures elapsed time", "[telemetry]")
{
    auto provider = std::make_shared<InMemoryTelemetryProvider>();
    set_telemetry_provider(provider);

    {
        TimedScope timed("operation.duration");
        REQUIRE(timed.elapsed().count() >= 0);
    }

    REQUIRE(provider->metric_count() == 1);
    auto metrics = provider->metrics();
    REQUIRE(metrics[0].name == "operation.duration");
    REQUIRE(metrics[0].type == MetricType::Histogram);

    // Reset global provider
    set_telemetry_provider(nullptr);
}

// ══════════════════════════════════════════════════════════════════════════════
// OtlpExporter
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("OtlpExporter: disabled by default", "[telemetry]")
{
    OtlpExporter exporter;
    REQUIRE_FALSE(exporter.is_enabled());
}

TEST_CASE("OtlpExporter: export fails when disabled", "[telemetry]")
{
    OtlpExporter exporter;
    auto result = exporter.export_metrics({});
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::TelemetryError);
}

TEST_CASE("OtlpExporter: export succeeds when enabled", "[telemetry]")
{
    OtlpExporter exporter;
    exporter.configure(OtlpConfig{
        .endpoint = "http://localhost:4317",
        .use_grpc = true,
        .enabled = true,
    });

    REQUIRE(exporter.is_enabled());

    std::vector<MetricRecord> metrics{
        {.name = "test.counter", .type = MetricType::Counter, .value = 1.0},
        {.name = "test.histogram", .type = MetricType::Histogram, .value = 42.0},
    };

    auto result = exporter.export_metrics(metrics);
    REQUIRE(result.has_value());
    REQUIRE(result.value() == 2);
    REQUIRE(exporter.export_count() == 2);
}

TEST_CASE("OtlpExporter: crash event export", "[telemetry]")
{
    OtlpExporter exporter;
    exporter.configure(OtlpConfig{
        .endpoint = "http://localhost:4317",
        .enabled = true,
    });

    CrashEvent event{
        .subsystem = "Editor",
        .message = "Null pointer dereference",
    };

    auto result = exporter.export_crash_event(event);
    REQUIRE(result.has_value());
    REQUIRE(exporter.crash_exports() == 1);
}

TEST_CASE("OtlpExporter: config roundtrip", "[telemetry]")
{
    OtlpExporter exporter;
    exporter.configure(OtlpConfig{
        .endpoint = "http://collector:4317",
        .use_grpc = false,
        .timeout_seconds = 30,
        .batch_size = 256,
        .flush_interval_seconds = 10,
        .enabled = true,
    });

    auto cfg = exporter.config();
    REQUIRE(cfg.endpoint == "http://collector:4317");
    REQUIRE_FALSE(cfg.use_grpc);
    REQUIRE(cfg.timeout_seconds == 30);
    REQUIRE(cfg.batch_size == 256);
}
