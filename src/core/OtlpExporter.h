/// OtlpExporter.h — V7 Phase 35: OTLP export & crash event integration
///
/// OTLP/gRPC and OTLP/HTTP export stubs (when opentelemetry-cpp linked).
/// Crash event export integration with CrashReporter.

#pragma once

#include "Result.h"
#include "TelemetryAbstraction.h"

#include <string>
#include <vector>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// OTLP Configuration
// ══════════════════════════════════════════════════════════════════════════════

/// Configuration for OTLP export.
struct OtlpConfig
{
    std::string endpoint; // e.g., "http://localhost:4317"
    bool use_grpc{true};  // gRPC vs HTTP
    int timeout_seconds{10};
    int batch_size{512};
    int flush_interval_seconds{5};
    bool enabled{false};
};

// ══════════════════════════════════════════════════════════════════════════════
// Crash Event
// ══════════════════════════════════════════════════════════════════════════════

/// A crash event for export.
struct CrashEvent
{
    std::string subsystem;
    std::string message;
    std::string stack_trace;
    int64_t timestamp_ms{0};
    ErrorCode error_code{ErrorCode::Unknown};
};

// ══════════════════════════════════════════════════════════════════════════════
// OTLP Exporter
// ══════════════════════════════════════════════════════════════════════════════

/// Exports telemetry data via OTLP protocol.
/// Currently a stub — real OTLP export requires the opentelemetry-cpp SDK.
class OtlpExporter
{
public:
    OtlpExporter() = default;

    /// Configure the exporter.
    void configure(const OtlpConfig& config);

    /// Get the current configuration.
    [[nodiscard]] auto config() const noexcept -> const OtlpConfig&
    {
        return config_;
    }

    /// Export collected metrics.
    [[nodiscard]] auto export_metrics(const std::vector<MetricRecord>& metrics) -> Result<size_t>;

    /// Export a crash event.
    [[nodiscard]] auto export_crash_event(const CrashEvent& event) -> Result<void>;

    /// Check if the exporter is enabled and configured.
    [[nodiscard]] auto is_enabled() const noexcept -> bool
    {
        return config_.enabled;
    }

    /// Get the number of successful exports.
    [[nodiscard]] auto export_count() const noexcept -> size_t
    {
        return export_count_;
    }

    /// Get the number of crash events exported.
    [[nodiscard]] auto crash_exports() const noexcept -> size_t
    {
        return crash_exports_;
    }

private:
    OtlpConfig config_;
    size_t export_count_{0};
    size_t crash_exports_{0};
};

} // namespace markamp::core
