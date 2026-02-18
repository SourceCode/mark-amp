/// @file TracingService.cpp
/// @brief V9 Phase 33 – Tracing, health, diagnostics implementation.

#include "TracingService.h"

#include <sstream>

namespace markamp::core
{

// ════════════════════════════════════════════════════════════════════
// RequestTracer
// ════════════════════════════════════════════════════════════════════

auto RequestTracer::begin_trace(const std::string& event_name) -> std::string
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::string trace_id = "trace-" + std::to_string(next_trace_id_++);

    TraceRecord record;
    record.trace_id = trace_id;
    record.event_name = event_name;
    record.start_us = std::chrono::duration_cast<std::chrono::microseconds>(
                          std::chrono::steady_clock::now().time_since_epoch())
                          .count();
    traces_.push_back(record);
    return trace_id;
}

void RequestTracer::record_handler(const std::string& trace_id,
                                   const std::string& handler_name,
                                   int64_t duration_us)
{
    std::lock_guard<std::mutex> lock(mutex_);
    TraceRecord record;
    record.trace_id = trace_id;
    record.handler_name = handler_name;
    record.duration_us = duration_us;
    record.is_slow = (duration_us > slow_threshold_us_);
    traces_.push_back(record);
}

auto RequestTracer::get_trace(const std::string& trace_id) const -> std::vector<TraceRecord>
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<TraceRecord> result;
    for (const auto& record : traces_)
    {
        if (record.trace_id == trace_id)
        {
            result.push_back(record);
        }
    }
    return result;
}

auto RequestTracer::all_traces() const -> std::vector<TraceRecord>
{
    std::lock_guard<std::mutex> lock(mutex_);
    return traces_;
}

void RequestTracer::set_slow_threshold_us(int64_t threshold_us)
{
    slow_threshold_us_ = threshold_us;
}

auto RequestTracer::slow_threshold_us() const -> int64_t
{
    return slow_threshold_us_;
}

void RequestTracer::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    traces_.clear();
}

auto RequestTracer::slow_handlers() const -> std::vector<TraceRecord>
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<TraceRecord> result;
    for (const auto& record : traces_)
    {
        if (record.is_slow)
        {
            result.push_back(record);
        }
    }
    return result;
}

// ════════════════════════════════════════════════════════════════════
// HealthChecker
// ════════════════════════════════════════════════════════════════════

void HealthChecker::register_subsystem(const std::string& name)
{
    std::lock_guard<std::mutex> lock(mutex_);
    SubsystemHealth health;
    health.name = name;
    health.status = HealthStatus::kHealthy;
    health.last_check_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                               std::chrono::system_clock::now().time_since_epoch())
                               .count();
    subsystems_[name] = health;
}

void HealthChecker::update_status(const std::string& name,
                                  HealthStatus status,
                                  const std::string& message,
                                  const std::string& recovery)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = subsystems_.find(name);
    if (iter != subsystems_.end())
    {
        iter->second.status = status;
        iter->second.message = message;
        iter->second.recovery_suggestion = recovery;
        iter->second.last_check_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                         std::chrono::system_clock::now().time_since_epoch())
                                         .count();
    }
}

auto HealthChecker::get_health(const std::string& name) const -> SubsystemHealth
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = subsystems_.find(name);
    if (iter != subsystems_.end())
    {
        return iter->second;
    }
    return {};
}

auto HealthChecker::all_health() const -> std::vector<SubsystemHealth>
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<SubsystemHealth> result;
    result.reserve(subsystems_.size());
    for (const auto& [sub_name, health] : subsystems_)
    {
        result.push_back(health);
    }
    return result;
}

auto HealthChecker::overall_status() const -> HealthStatus
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto worst = HealthStatus::kHealthy;
    for (const auto& [sub_name, health] : subsystems_)
    {
        if (health.status > worst)
        {
            worst = health.status;
        }
    }
    return worst;
}

auto HealthChecker::unhealthy_subsystems() const -> std::vector<std::string>
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> result;
    for (const auto& [sub_name, health] : subsystems_)
    {
        if (health.status != HealthStatus::kHealthy)
        {
            result.push_back(sub_name);
        }
    }
    return result;
}

void HealthChecker::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    subsystems_.clear();
}

// ════════════════════════════════════════════════════════════════════
// DiagnosticReport
// ════════════════════════════════════════════════════════════════════

auto DiagnosticReport::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"version\":\"" << app_version << "\",\"os\":\"" << os_info
        << "\",\"generated_at\":" << generated_at_ms << ",\"sections\":[";

    bool first_section = true;
    for (const auto& section : sections)
    {
        if (!first_section)
        {
            oss << ',';
        }
        first_section = false;
        oss << "{\"title\":\"" << section.title << "\",\"entries\":{";

        bool first_entry = true;
        for (const auto& [entry_key, entry_value] : section.entries)
        {
            if (!first_entry)
            {
                oss << ',';
            }
            first_entry = false;
            oss << "\"" << entry_key << "\":\"" << entry_value << "\"";
        }
        oss << "}}";
    }
    oss << "]}";
    return oss.str();
}

auto DiagnosticReportBuilder::build(const std::string& app_version) -> DiagnosticReport
{
    DiagnosticReport report;
    report.app_version = app_version;
    report.generated_at_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                 std::chrono::system_clock::now().time_since_epoch())
                                 .count();

    // System info section
#if defined(__APPLE__)
    report.os_info = "macOS";
#elif defined(_WIN32) || defined(_WIN64)
    report.os_info = "Windows";
#else
    report.os_info = "Linux";
#endif

    DiagnosticSection sys_section;
    sys_section.title = "System";
    sys_section.entries["os"] = report.os_info;
    sys_section.entries["app_version"] = app_version;
#if defined(__aarch64__) || defined(_M_ARM64)
    sys_section.entries["architecture"] = "arm64";
#elif defined(__x86_64__) || defined(_M_X64)
    sys_section.entries["architecture"] = "x86_64";
#else
    sys_section.entries["architecture"] = "unknown";
#endif
    report.sections.push_back(sys_section);

    return report;
}

void DiagnosticReportBuilder::add_section(DiagnosticReport& report,
                                          const std::string& title,
                                          const std::map<std::string, std::string>& entries)
{
    DiagnosticSection section;
    section.title = title;
    section.entries = entries;
    report.sections.push_back(section);
}

// ════════════════════════════════════════════════════════════════════
// ExtensionTelemetry
// ════════════════════════════════════════════════════════════════════

void ExtensionTelemetry::record_load(const std::string& extension_id, int64_t load_time_ms)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto& ext_metrics = metrics_[extension_id];
    ext_metrics.extension_id = extension_id;
    ext_metrics.load_time_ms = load_time_ms;
    ext_metrics.is_slow = (load_time_ms > kSlowLoadThresholdMs);
}

void ExtensionTelemetry::record_api_call(const std::string& extension_id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto& ext_metrics = metrics_[extension_id];
    ext_metrics.extension_id = extension_id;
    ext_metrics.api_call_count++;
}

void ExtensionTelemetry::record_error(const std::string& extension_id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto& ext_metrics = metrics_[extension_id];
    ext_metrics.extension_id = extension_id;
    ext_metrics.error_count++;
    ext_metrics.is_error_prone = (ext_metrics.error_count >= kErrorProneThreshold);
}

void ExtensionTelemetry::set_memory_usage(const std::string& extension_id, int64_t bytes)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto& ext_metrics = metrics_[extension_id];
    ext_metrics.extension_id = extension_id;
    ext_metrics.memory_usage_bytes = bytes;
}

auto ExtensionTelemetry::get_metrics(const std::string& extension_id) const -> ExtensionMetrics
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = metrics_.find(extension_id);
    if (iter != metrics_.end())
    {
        return iter->second;
    }
    ExtensionMetrics empty;
    empty.extension_id = extension_id;
    return empty;
}

auto ExtensionTelemetry::all_metrics() const -> std::vector<ExtensionMetrics>
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ExtensionMetrics> result;
    result.reserve(metrics_.size());
    for (const auto& [ext_id, ext_metrics] : metrics_)
    {
        result.push_back(ext_metrics);
    }
    return result;
}

auto ExtensionTelemetry::flagged_extensions() const -> std::vector<ExtensionMetrics>
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ExtensionMetrics> result;
    for (const auto& [ext_id, ext_metrics] : metrics_)
    {
        if (ext_metrics.is_slow || ext_metrics.is_error_prone)
        {
            result.push_back(ext_metrics);
        }
    }
    return result;
}

void ExtensionTelemetry::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_.clear();
}

// ════════════════════════════════════════════════════════════════════
// StartupDiagnostic
// ════════════════════════════════════════════════════════════════════

auto StartupDiagnostic::collect() -> StartupInfo
{
    StartupInfo info;

#if defined(__APPLE__)
    info.os_name = "macOS";
#elif defined(_WIN32) || defined(_WIN64)
    info.os_name = "Windows";
#else
    info.os_name = "Linux";
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
    info.architecture = "arm64";
#elif defined(__x86_64__) || defined(_M_X64)
    info.architecture = "x86_64";
#else
    info.architecture = "unknown";
#endif

    info.display_count = 1;
    info.display_dpi = 96.0;

    return info;
}

auto StartupDiagnostic::format(const StartupInfo& info) -> std::string
{
    std::ostringstream oss;
    oss << "Startup Diagnostic:\n";
    oss << "  OS: " << info.os_name;
    if (!info.os_version.empty())
    {
        oss << " " << info.os_version;
    }
    oss << " (" << info.architecture << ")\n";
    oss << "  App: " << info.app_version << "\n";
    oss << "  Displays: " << info.display_count << " @ " << info.display_dpi << " DPI\n";
    if (!info.workspace_path.empty())
    {
        oss << "  Workspace: " << info.workspace_path << " (" << info.file_count << " files)\n";
    }
    oss << "  Extensions: " << info.extension_count << " total, " << info.extensions_loaded
        << " loaded, " << info.extensions_failed << " failed\n";
    return oss.str();
}

} // namespace markamp::core
