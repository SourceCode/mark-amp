/// @file TracingService.h
/// @brief V9 Phase 33 – Request tracing, health checks, diagnostics, extension telemetry.

#pragma once

#include <chrono>
#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace markamp::core
{

// ════════════════════════════════════════════════════════════════════
// Request Tracing (PRD Task 6)
// ════════════════════════════════════════════════════════════════════

struct TraceRecord
{
    std::string trace_id;
    std::string event_name;
    std::string handler_name;
    int64_t start_us{0};
    int64_t duration_us{0};
    bool is_slow{false};
    std::string parent_trace_id;
};

class RequestTracer
{
public:
    /// Begin a trace for an event. Returns a trace ID.
    auto begin_trace(const std::string& event_name) -> std::string;

    /// Record a handler execution within a trace.
    void record_handler(const std::string& trace_id,
                        const std::string& handler_name,
                        int64_t duration_us);

    /// Get all records for a trace.
    [[nodiscard]] auto get_trace(const std::string& trace_id) const -> std::vector<TraceRecord>;

    /// Get all traces.
    [[nodiscard]] auto all_traces() const -> std::vector<TraceRecord>;

    /// Set the slow handler threshold (microseconds).
    void set_slow_threshold_us(int64_t threshold_us);

    /// Get the slow handler threshold.
    [[nodiscard]] auto slow_threshold_us() const -> int64_t;

    /// Clear all traces.
    void clear();

    /// Get slow handler records only.
    [[nodiscard]] auto slow_handlers() const -> std::vector<TraceRecord>;

private:
    mutable std::mutex mutex_;
    std::vector<TraceRecord> traces_;
    int64_t slow_threshold_us_{100'000}; ///< 100ms default
    int next_trace_id_{1};
};

// ════════════════════════════════════════════════════════════════════
// Health Check System (PRD Task 7)
// ════════════════════════════════════════════════════════════════════

enum class HealthStatus : uint8_t
{
    kHealthy,
    kDegraded,
    kUnhealthy,
};

struct SubsystemHealth
{
    std::string name;
    HealthStatus status{HealthStatus::kHealthy};
    std::string message;
    int64_t last_check_ms{0};
    std::string recovery_suggestion;
};

class HealthChecker
{
public:
    /// Register a subsystem for monitoring.
    void register_subsystem(const std::string& name);

    /// Update a subsystem's health status.
    void update_status(const std::string& name,
                       HealthStatus status,
                       const std::string& message = "",
                       const std::string& recovery = "");

    /// Get a subsystem's health.
    [[nodiscard]] auto get_health(const std::string& name) const -> SubsystemHealth;

    /// Get all subsystem health.
    [[nodiscard]] auto all_health() const -> std::vector<SubsystemHealth>;

    /// Get the overall system health (worst subsystem status).
    [[nodiscard]] auto overall_status() const -> HealthStatus;

    /// Get names of unhealthy/degraded subsystems.
    [[nodiscard]] auto unhealthy_subsystems() const -> std::vector<std::string>;

    /// Clear all registered subsystems.
    void clear();

private:
    mutable std::mutex mutex_;
    std::map<std::string, SubsystemHealth> subsystems_;
};

// ════════════════════════════════════════════════════════════════════
// Diagnostic Report (PRD Task 10)
// ════════════════════════════════════════════════════════════════════

struct DiagnosticSection
{
    std::string title;
    std::map<std::string, std::string> entries;
};

struct DiagnosticReport
{
    std::string app_version;
    std::string os_info;
    int64_t generated_at_ms{0};
    std::vector<DiagnosticSection> sections;

    /// Export the report as JSON.
    [[nodiscard]] auto to_json() const -> std::string;
};

class DiagnosticReportBuilder
{
public:
    /// Build a diagnostic report with system info.
    [[nodiscard]] static auto build(const std::string& app_version) -> DiagnosticReport;

    /// Add a custom section to a report.
    static void add_section(DiagnosticReport& report,
                            const std::string& title,
                            const std::map<std::string, std::string>& entries);
};

// ════════════════════════════════════════════════════════════════════
// Extension Telemetry (PRD Task 11)
// ════════════════════════════════════════════════════════════════════

struct ExtensionMetrics
{
    std::string extension_id;
    int64_t load_time_ms{0};
    int api_call_count{0};
    int error_count{0};
    int64_t memory_usage_bytes{0};
    bool is_slow{false};        ///< load > 2 seconds
    bool is_error_prone{false}; ///< > 5 errors/minute
};

class ExtensionTelemetry
{
public:
    /// Record extension load time.
    void record_load(const std::string& extension_id, int64_t load_time_ms);

    /// Record an API call from an extension.
    void record_api_call(const std::string& extension_id);

    /// Record an error from an extension.
    void record_error(const std::string& extension_id);

    /// Set memory usage for an extension.
    void set_memory_usage(const std::string& extension_id, int64_t bytes);

    /// Get metrics for a specific extension.
    [[nodiscard]] auto get_metrics(const std::string& extension_id) const -> ExtensionMetrics;

    /// Get all extension metrics.
    [[nodiscard]] auto all_metrics() const -> std::vector<ExtensionMetrics>;

    /// Get extensions flagged as problematic.
    [[nodiscard]] auto flagged_extensions() const -> std::vector<ExtensionMetrics>;

    /// Clear all telemetry data.
    void clear();

private:
    mutable std::mutex mutex_;
    std::map<std::string, ExtensionMetrics> metrics_;
    static constexpr int64_t kSlowLoadThresholdMs = 2000;
    static constexpr int kErrorProneThreshold = 5;
};

// ════════════════════════════════════════════════════════════════════
// Startup Diagnostic (PRD Task 14)
// ════════════════════════════════════════════════════════════════════

struct StartupInfo
{
    std::string os_name;
    std::string os_version;
    std::string architecture;
    std::string app_version;
    int display_count{1};
    double display_dpi{96.0};
    std::string workspace_path;
    int file_count{0};
    int extension_count{0};
    int extensions_loaded{0};
    int extensions_failed{0};
};

class StartupDiagnostic
{
public:
    /// Collect startup information about the system.
    [[nodiscard]] static auto collect() -> StartupInfo;

    /// Format startup info as a structured log string.
    [[nodiscard]] static auto format(const StartupInfo& info) -> std::string;
};

} // namespace markamp::core
