/// @file test_phase33_observability.cpp
/// @brief V9 Phase 33 – Structured Logging & Observability tests.

#include "core/Events.h"
#include "core/ObservabilityCommands.h"
#include "core/ObservabilityService.h"
#include "core/TracingService.h"

#include <catch2/catch_test_macros.hpp>

#include <thread>

using namespace markamp::core;
using namespace markamp::core::events;

// ════════════════════════════════════════════════════════════════════
// LogLevelConfig (PRD Task 3)
// ════════════════════════════════════════════════════════════════════

TEST_CASE("LogLevelConfig — global default is INFO", "[observability][loglevel]")
{
    LogLevelConfig config;
    REQUIRE(config.global_level() == ConfigurableLogLevel::kInfo);
}

TEST_CASE("LogLevelConfig — set and get global level", "[observability][loglevel]")
{
    LogLevelConfig config;
    config.set_global_level(ConfigurableLogLevel::kDebug);
    REQUIRE(config.global_level() == ConfigurableLogLevel::kDebug);
}

TEST_CASE("LogLevelConfig — per-module override", "[observability][loglevel]")
{
    LogLevelConfig config;
    config.set_module_level("sync", ConfigurableLogLevel::kWarn);

    REQUIRE(config.effective_level("sync") == ConfigurableLogLevel::kWarn);
    REQUIRE(config.effective_level("editor") == ConfigurableLogLevel::kInfo); // global
}

TEST_CASE("LogLevelConfig — should_log filtering", "[observability][loglevel]")
{
    LogLevelConfig config;
    config.set_global_level(ConfigurableLogLevel::kWarn);

    REQUIRE_FALSE(config.should_log("editor", ConfigurableLogLevel::kDebug));
    REQUIRE_FALSE(config.should_log("editor", ConfigurableLogLevel::kInfo));
    REQUIRE(config.should_log("editor", ConfigurableLogLevel::kWarn));
    REQUIRE(config.should_log("editor", ConfigurableLogLevel::kError));
}

TEST_CASE("LogLevelConfig — remove module override", "[observability][loglevel]")
{
    LogLevelConfig config;
    config.set_module_level("sync", ConfigurableLogLevel::kDebug);
    config.remove_module_level("sync");

    REQUIRE(config.effective_level("sync") == config.global_level());
    REQUIRE(config.module_overrides().empty());
}

// ════════════════════════════════════════════════════════════════════
// MetricsCollector (PRD Task 5)
// ════════════════════════════════════════════════════════════════════

TEST_CASE("MetricsCollector — counter increment", "[observability][metrics]")
{
    MetricsCollector metrics;
    metrics.increment("file.opens");
    metrics.increment("file.opens");
    metrics.increment("file.opens", 3.0);

    const auto snap = metrics.get("file.opens");
    REQUIRE(snap.type == MetricType::kCounter);
    REQUIRE(snap.value == 5.0);
}

TEST_CASE("MetricsCollector — gauge set", "[observability][metrics]")
{
    MetricsCollector metrics;
    metrics.set("memory.usage", 1024.0);
    metrics.set("memory.usage", 2048.0);

    const auto snap = metrics.get("memory.usage");
    REQUIRE(snap.type == MetricType::kGauge);
    REQUIRE(snap.value == 2048.0);
}

TEST_CASE("MetricsCollector — histogram record", "[observability][metrics]")
{
    MetricsCollector metrics;
    metrics.record("frame.time", 16.0);
    metrics.record("frame.time", 17.5);
    metrics.record("frame.time", 15.0);

    const auto snap = metrics.get("frame.time");
    REQUIRE(snap.type == MetricType::kHistogram);
    REQUIRE(snap.histogram_values.size() == 3);
}

TEST_CASE("MetricsCollector — export JSON", "[observability][metrics]")
{
    MetricsCollector metrics;
    metrics.increment("test.counter");
    metrics.set("test.gauge", 42.0);

    const auto json = metrics.export_json();
    REQUIRE(json.find("\"metrics\"") != std::string::npos);
    REQUIRE(json.find("test.counter") != std::string::npos);
    REQUIRE(json.find("test.gauge") != std::string::npos);
}

TEST_CASE("MetricsCollector — reset and has", "[observability][metrics]")
{
    MetricsCollector metrics;
    metrics.increment("test.a");
    REQUIRE(metrics.has("test.a"));

    metrics.reset();
    REQUIRE_FALSE(metrics.has("test.a"));
    REQUIRE(metrics.all().empty());
}

// ════════════════════════════════════════════════════════════════════
// ErrorRateTracker (PRD Task 8)
// ════════════════════════════════════════════════════════════════════

TEST_CASE("ErrorRateTracker — record and count", "[observability][errorrate]")
{
    ErrorRateTracker tracker;
    tracker.record_error("editor");
    tracker.record_error("editor");
    tracker.record_error("sync");

    REQUIRE(tracker.error_count("editor") == 2);
    REQUIRE(tracker.error_count("sync") == 1);
    REQUIRE(tracker.error_count("canvas") == 0);
}

TEST_CASE("ErrorRateTracker — alert threshold", "[observability][errorrate]")
{
    ErrorRateTracker tracker;
    tracker.set_threshold(3);

    tracker.record_error("editor");
    tracker.record_error("editor");
    REQUIRE(tracker.check_alerts().empty());

    tracker.record_error("editor");
    const auto alerts = tracker.check_alerts();
    REQUIRE(alerts.size() == 1);
    REQUIRE(alerts[0].module_name == "editor");
    REQUIRE(alerts[0].count == 3);
}

TEST_CASE("ErrorRateTracker — reset preserves previous window", "[observability][errorrate]")
{
    ErrorRateTracker tracker;
    tracker.set_threshold(2);
    tracker.record_error("editor");
    tracker.record_error("editor");

    tracker.reset();
    REQUIRE(tracker.error_count("editor") == 0);
    REQUIRE(tracker.all_counts().empty());
}

// ════════════════════════════════════════════════════════════════════
// SpanTracker / PerformanceSpan (PRD Task 9)
// ════════════════════════════════════════════════════════════════════

TEST_CASE("SpanTracker — begin and end span", "[observability][spans]")
{
    SpanTracker tracker;
    auto idx = tracker.begin_span("test_operation");
    tracker.end_span(idx);

    const auto spans = tracker.completed_spans();
    REQUIRE(spans.size() == 1);
    REQUIRE(spans[0].name == "test_operation");
    REQUIRE(spans[0].duration_us >= 0);
}

TEST_CASE("SpanTracker — nested spans with parent", "[observability][spans]")
{
    SpanTracker tracker;
    auto parent = tracker.begin_span("parent_op");
    auto child = tracker.begin_span("child_op", "parent_op");
    tracker.end_span(child);
    tracker.end_span(parent);

    const auto spans = tracker.completed_spans();
    REQUIRE(spans.size() == 2);
    REQUIRE(spans[1].parent_name == "parent_op");
}

TEST_CASE("SpanTracker — slow span detection", "[observability][spans]")
{
    SpanTracker tracker;
    tracker.set_slow_threshold_us(0); // 0 microseconds — everything with duration > 0 is slow!
    auto idx = tracker.begin_span("slow_op");
    std::this_thread::sleep_for(std::chrono::microseconds(50));
    tracker.end_span(idx);

    const auto spans = tracker.completed_spans();
    REQUIRE(!spans.empty());
    REQUIRE(spans[0].is_slow);
}

TEST_CASE("PerformanceSpan — RAII auto end", "[observability][spans]")
{
    SpanTracker tracker;
    {
        PerformanceSpan span(tracker, "raii_op");
        // Span ends when PerformanceSpan goes out of scope
    }
    REQUIRE(tracker.completed_spans().size() == 1);
}

// ════════════════════════════════════════════════════════════════════
// LogBuffer (PRD Task 15)
// ════════════════════════════════════════════════════════════════════

TEST_CASE("LogBuffer — push and retrieve", "[observability][logbuffer]")
{
    LogBuffer buffer(10);
    buffer.push({"INFO", "test", "message 1", 100});
    buffer.push({"WARN", "test", "message 2", 200});

    REQUIRE(buffer.size() == 2);
    const auto entries = buffer.entries();
    REQUIRE(entries.size() == 2);
    REQUIRE(entries[0].message == "message 1");
    REQUIRE(entries[1].message == "message 2");
}

TEST_CASE("LogBuffer — circular overflow", "[observability][logbuffer]")
{
    LogBuffer buffer(3);
    buffer.push({"INFO", "a", "msg1", 1});
    buffer.push({"INFO", "a", "msg2", 2});
    buffer.push({"INFO", "a", "msg3", 3});
    buffer.push({"INFO", "a", "msg4", 4}); // overwrites msg1

    REQUIRE(buffer.size() == 3);
    const auto entries = buffer.entries();
    REQUIRE(entries.size() == 3);
    REQUIRE(entries[0].message == "msg2"); // oldest surviving
    REQUIRE(entries[2].message == "msg4"); // newest
}

TEST_CASE("LogBuffer — export JSON", "[observability][logbuffer]")
{
    LogBuffer buffer(10);
    buffer.push({"INFO", "test", "hello", 100});

    const auto json = buffer.export_json();
    REQUIRE(json.find("\"log_buffer\"") != std::string::npos);
    REQUIRE(json.find("hello") != std::string::npos);
}

TEST_CASE("LogBuffer — clear", "[observability][logbuffer]")
{
    LogBuffer buffer(10);
    buffer.push({"INFO", "a", "msg", 1});
    buffer.clear();
    REQUIRE(buffer.size() == 0);
}

// ════════════════════════════════════════════════════════════════════
// LogRedactor (PRD Task 17)
// ════════════════════════════════════════════════════════════════════

TEST_CASE("LogRedactor — redact file path", "[observability][redactor]")
{
    REQUIRE(LogRedactor::redact_path("/home/user/docs/secret.md") == "./secret.md");
    REQUIRE(LogRedactor::redact_path("C:\\Users\\test\\file.txt") == "./file.txt");
}

TEST_CASE("LogRedactor — redact credentials", "[observability][redactor]")
{
    const auto result = LogRedactor::redact_credentials("password=my_secret_pass");
    REQUIRE(result.find("my_secret_pass") == std::string::npos);
    REQUIRE(result.find("***") != std::string::npos);
}

TEST_CASE("LogRedactor — redact query", "[observability][redactor]")
{
    const auto result = LogRedactor::redact_query("secret search terms");
    REQUIRE(result.find("secret") == std::string::npos);
    REQUIRE(result.find("length=") != std::string::npos);
}

TEST_CASE("LogRedactor — contains_sensitive detection", "[observability][redactor]")
{
    REQUIRE(LogRedactor::contains_sensitive("my_password=test"));
    REQUIRE(LogRedactor::contains_sensitive("API_KEY: abc123"));
    REQUIRE_FALSE(LogRedactor::contains_sensitive("just a normal message"));
}

// ════════════════════════════════════════════════════════════════════
// RequestTracer (PRD Task 6)
// ════════════════════════════════════════════════════════════════════

TEST_CASE("RequestTracer — begin trace and record handler", "[observability][tracing]")
{
    RequestTracer tracer;
    const auto trace_id = tracer.begin_trace("FileOpenEvent");
    tracer.record_handler(trace_id, "EditorHandler", 5000);

    const auto records = tracer.get_trace(trace_id);
    REQUIRE(records.size() == 2); // trace start + handler
    REQUIRE(records[0].event_name == "FileOpenEvent");
    REQUIRE(records[1].handler_name == "EditorHandler");
}

TEST_CASE("RequestTracer — slow handler detection", "[observability][tracing]")
{
    RequestTracer tracer;
    tracer.set_slow_threshold_us(50'000); // 50ms
    const auto trace_id = tracer.begin_trace("SearchEvent");
    tracer.record_handler(trace_id, "SlowHandler", 200'000); // 200ms

    const auto slow = tracer.slow_handlers();
    REQUIRE(slow.size() == 1);
    REQUIRE(slow[0].handler_name == "SlowHandler");
    REQUIRE(slow[0].is_slow);
}

// ════════════════════════════════════════════════════════════════════
// HealthChecker (PRD Task 7)
// ════════════════════════════════════════════════════════════════════

TEST_CASE("HealthChecker — register and check subsystem", "[observability][health]")
{
    HealthChecker checker;
    checker.register_subsystem("EventBus");
    checker.register_subsystem("Config");

    const auto health = checker.get_health("EventBus");
    REQUIRE(health.name == "EventBus");
    REQUIRE(health.status == HealthStatus::kHealthy);
}

TEST_CASE("HealthChecker — update status and overall", "[observability][health]")
{
    HealthChecker checker;
    checker.register_subsystem("EventBus");
    checker.register_subsystem("Config");

    REQUIRE(checker.overall_status() == HealthStatus::kHealthy);

    checker.update_status("Config", HealthStatus::kDegraded, "High latency", "Restart config");
    REQUIRE(checker.overall_status() == HealthStatus::kDegraded);
    REQUIRE(checker.unhealthy_subsystems().size() == 1);
    REQUIRE(checker.unhealthy_subsystems()[0] == "Config");
}

TEST_CASE("HealthChecker — unhealthy status", "[observability][health]")
{
    HealthChecker checker;
    checker.register_subsystem("PluginManager");
    checker.update_status(
        "PluginManager", HealthStatus::kUnhealthy, "Crash loop", "Disable extensions");

    const auto health = checker.get_health("PluginManager");
    REQUIRE(health.status == HealthStatus::kUnhealthy);
    REQUIRE(health.recovery_suggestion == "Disable extensions");
}

// ════════════════════════════════════════════════════════════════════
// DiagnosticReport (PRD Task 10)
// ════════════════════════════════════════════════════════════════════

TEST_CASE("DiagnosticReport — build and serialize", "[observability][diagnostic]")
{
    auto report = DiagnosticReportBuilder::build("2.3.0");
    DiagnosticReportBuilder::add_section(report, "Extensions", {{"count", "5"}, {"failed", "1"}});

    REQUIRE(report.app_version == "2.3.0");
    REQUIRE(report.sections.size() >= 2); // System + Extensions

    const auto json = report.to_json();
    REQUIRE(json.find("\"version\":\"2.3.0\"") != std::string::npos);
    REQUIRE(json.find("Extensions") != std::string::npos);
}

// ════════════════════════════════════════════════════════════════════
// ExtensionTelemetry (PRD Task 11)
// ════════════════════════════════════════════════════════════════════

TEST_CASE("ExtensionTelemetry — record load time and flag slow", "[observability][exttel]")
{
    ExtensionTelemetry telemetry;
    telemetry.record_load("fast-ext", 500);
    telemetry.record_load("slow-ext", 3000);

    REQUIRE_FALSE(telemetry.get_metrics("fast-ext").is_slow);
    REQUIRE(telemetry.get_metrics("slow-ext").is_slow);
}

TEST_CASE("ExtensionTelemetry — error-prone detection", "[observability][exttel]")
{
    ExtensionTelemetry telemetry;
    for (int idx = 0; idx < 6; ++idx)
    {
        telemetry.record_error("bad-ext");
    }

    REQUIRE(telemetry.get_metrics("bad-ext").is_error_prone);

    const auto flagged = telemetry.flagged_extensions();
    REQUIRE(flagged.size() == 1);
    REQUIRE(flagged[0].extension_id == "bad-ext");
}

TEST_CASE("ExtensionTelemetry — API call and memory tracking", "[observability][exttel]")
{
    ExtensionTelemetry telemetry;
    telemetry.record_api_call("ext-a");
    telemetry.record_api_call("ext-a");
    telemetry.set_memory_usage("ext-a", 1024 * 1024);

    const auto ext_metrics = telemetry.get_metrics("ext-a");
    REQUIRE(ext_metrics.api_call_count == 2);
    REQUIRE(ext_metrics.memory_usage_bytes == 1024 * 1024);
}

// ════════════════════════════════════════════════════════════════════
// StartupDiagnostic (PRD Task 14)
// ════════════════════════════════════════════════════════════════════

TEST_CASE("StartupDiagnostic — collect and format", "[observability][startup]")
{
    auto info = StartupDiagnostic::collect();
    REQUIRE(!info.os_name.empty());
    REQUIRE(!info.architecture.empty());

    info.app_version = "2.3.0";
    info.extension_count = 10;
    info.extensions_loaded = 8;
    info.extensions_failed = 2;

    const auto formatted = StartupDiagnostic::format(info);
    REQUIRE(formatted.find("Startup Diagnostic") != std::string::npos);
    REQUIRE(formatted.find("2.3.0") != std::string::npos);
    REQUIRE(formatted.find("Extensions: 10") != std::string::npos);
}

// ════════════════════════════════════════════════════════════════════
// Module Name Extraction (PRD Task 2)
// ════════════════════════════════════════════════════════════════════

TEST_CASE("extract_module_name — strips path and extension", "[observability][module]")
{
    REQUIRE(extract_module_name("src/core/Editor.cpp") == "editor");
    REQUIRE(extract_module_name("/Users/dev/markamp/src/ui/MainFrame.cpp") == "mainframe");
    REQUIRE(extract_module_name("Config.h") == "config");
}

// ════════════════════════════════════════════════════════════════════
// LogChannels (PRD Task 4)
// ════════════════════════════════════════════════════════════════════

TEST_CASE("LogChannels — predefined channels", "[observability][channels]")
{
    const auto channels = LogChannels::all();
    REQUIRE(channels.size() == 8);
    REQUIRE(std::find(channels.begin(), channels.end(), "General") != channels.end());
    REQUIRE(std::find(channels.begin(), channels.end(), "Performance") != channels.end());
    REQUIRE(std::find(channels.begin(), channels.end(), "Health") != channels.end());
}

// ════════════════════════════════════════════════════════════════════
// UserActionLogger (PRD Task 12)
// ════════════════════════════════════════════════════════════════════

TEST_CASE("UserActionLogger — disabled by default", "[observability][actions]")
{
    UserActionLogger logger;
    REQUIRE_FALSE(logger.is_enabled());
    logger.log_action(UserActionType::kCommand, "test");
    REQUIRE(logger.recent_actions().empty());
}

TEST_CASE("UserActionLogger — enable and log actions", "[observability][actions]")
{
    UserActionLogger logger;
    logger.set_enabled(true);
    logger.log_action(UserActionType::kCommand, "editor.save");
    logger.log_action(UserActionType::kFileOpen, "readme.md");

    const auto actions = logger.recent_actions();
    REQUIRE(actions.size() == 2);
    REQUIRE(actions[0].type == UserActionType::kCommand);
    REQUIRE(actions[1].action_name == "readme.md");
}

TEST_CASE("UserActionLogger — export JSON", "[observability][actions]")
{
    UserActionLogger logger;
    logger.set_enabled(true);
    logger.log_action(UserActionType::kSearch, "find_text");

    const auto json = logger.export_json();
    REQUIRE(json.find("\"actions\"") != std::string::npos);
    REQUIRE(json.find("search") != std::string::npos);
}

// ════════════════════════════════════════════════════════════════════
// LogFilterService (PRD Task 13)
// ════════════════════════════════════════════════════════════════════

TEST_CASE("LogFilterService — filter by level", "[observability][filter]")
{
    const std::vector<std::string> lines = {
        "[INFO] Editor loaded",
        "[WARN] Config missing key",
        "[ERROR] Plugin crashed",
        "[INFO] File saved",
    };

    LogFilterCriteria criteria;
    criteria.level_filter = "WARN";
    const auto result = LogFilterService::filter(lines, criteria);
    REQUIRE(result.total_matches == 1);
    REQUIRE(result.lines[0].find("Config missing key") != std::string::npos);
}

TEST_CASE("LogFilterService — search text", "[observability][filter]")
{
    const std::vector<std::string> lines = {
        "[INFO] Editor loaded",
        "[INFO] Config loaded",
        "[WARN] Editor warning",
    };

    LogFilterCriteria criteria;
    criteria.search_text = "Editor";
    const auto result = LogFilterService::filter(lines, criteria);
    REQUIRE(result.total_matches == 2);
}

// ════════════════════════════════════════════════════════════════════
// ExternalLogShipper (PRD Task 16)
// ════════════════════════════════════════════════════════════════════

TEST_CASE("ExternalLogShipper — disabled by default", "[observability][shipper]")
{
    ExternalLogShipper shipper;
    REQUIRE_FALSE(shipper.is_enabled());

    shipper.ship("{\"msg\":\"test\"}");
    REQUIRE(shipper.buffered_count() == 0); // not enabled, not buffered
}

TEST_CASE("ExternalLogShipper — configure and ship", "[observability][shipper]")
{
    ExternalLogShipper shipper;
    LogShipConfig ship_config;
    ship_config.enabled = true;
    ship_config.target = LogShipTarget::kFile;
    ship_config.endpoint = "/tmp/logs.json";
    ship_config.buffer_size = 5;
    shipper.configure(ship_config);

    REQUIRE(shipper.is_enabled());
    shipper.ship("{\"msg\":\"entry1\"}");
    shipper.ship("{\"msg\":\"entry2\"}");
    REQUIRE(shipper.buffered_count() == 2);

    shipper.flush();
    REQUIRE(shipper.buffered_count() == 0);
}

// ════════════════════════════════════════════════════════════════════
// ObservabilityCommandProvider (PRD Task 18)
// ════════════════════════════════════════════════════════════════════

TEST_CASE("ObservabilityCommandProvider — commands list", "[observability][commands]")
{
    const auto cmds = ObservabilityCommandProvider::commands();
    REQUIRE(cmds.size() == 6);

    // Verify categories
    bool has_log = false;
    bool has_metrics = false;
    bool has_health = false;
    bool has_diagnostic = false;
    for (const auto& cmd : cmds)
    {
        if (cmd.category == "Log")
            has_log = true;
        if (cmd.category == "Metrics")
            has_metrics = true;
        if (cmd.category == "Health")
            has_health = true;
        if (cmd.category == "Diagnostic")
            has_diagnostic = true;
    }
    REQUIRE(has_log);
    REQUIRE(has_metrics);
    REQUIRE(has_health);
    REQUIRE(has_diagnostic);
}

// ════════════════════════════════════════════════════════════════════
// ObservabilitySettingsManager (PRD Task 19)
// ════════════════════════════════════════════════════════════════════

TEST_CASE("ObservabilitySettingsManager — defaults", "[observability][settings]")
{
    ObservabilitySettingsManager manager;
    const auto settings = manager.settings();
    REQUIRE(settings.global_log_level == "INFO");
    REQUIRE(settings.log_retention_days == 7);
    REQUIRE(settings.metrics_enabled);
    REQUIRE(settings.health_check_interval_seconds == 30);
    REQUIRE(settings.auto_diagnostic_on_crash);
}

TEST_CASE("ObservabilitySettingsManager — update and reset", "[observability][settings]")
{
    ObservabilitySettingsManager manager;

    ObservabilitySettings custom;
    custom.global_log_level = "DEBUG";
    custom.log_retention_days = 14;
    custom.metrics_enabled = false;
    custom.health_check_interval_seconds = 60;
    custom.auto_diagnostic_on_crash = false;
    manager.update(custom);

    REQUIRE(manager.settings().global_log_level == "DEBUG");
    REQUIRE(manager.settings().log_retention_days == 14);

    manager.reset_defaults();
    REQUIRE(manager.settings().global_log_level == "INFO");
}

// ════════════════════════════════════════════════════════════════════
// Phase 33 Events (PRD Task 20)
// ════════════════════════════════════════════════════════════════════

TEST_CASE("LogLevelChangedEvent", "[observability][events]")
{
    LogLevelChangedEvent event;
    event.module_name = "editor";
    event.new_level = "DEBUG";
    REQUIRE(event.module_name == "editor");
    REQUIRE(event.new_level == "DEBUG");
}

TEST_CASE("HealthStatusChangedEvent", "[observability][events]")
{
    HealthStatusChangedEvent event;
    event.subsystem = "Config";
    event.status = "degraded";
    event.message = "High latency";
    REQUIRE(event.subsystem == "Config");
}

TEST_CASE("MetricAlertEvent", "[observability][events]")
{
    MetricAlertEvent event;
    event.metric_name = "error.rate";
    event.value = 15.0;
    event.threshold = 10.0;
    REQUIRE(event.value > event.threshold);
}

TEST_CASE("DiagnosticReportGeneratedEvent", "[observability][events]")
{
    DiagnosticReportGeneratedEvent event;
    event.report_path = "/tmp/diagnostic.json";
    REQUIRE(!event.report_path.empty());
}

TEST_CASE("SlowSpanDetectedEvent", "[observability][events]")
{
    SlowSpanDetectedEvent event;
    event.span_name = "file.load";
    event.duration_us = 500'000;
    event.threshold_us = 100'000;
    REQUIRE(event.duration_us > event.threshold_us);
}
