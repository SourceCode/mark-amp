/// OtlpExporter.cpp — V7 Phase 35: OTLP export stubs

#include "OtlpExporter.h"

namespace markamp::core
{

void OtlpExporter::configure(const OtlpConfig& config)
{
    config_ = config;
}

auto OtlpExporter::export_metrics(const std::vector<MetricRecord>& metrics) -> Result<size_t>
{
    if (!config_.enabled)
    {
        return std::unexpected(make_error(
            ErrorCode::TelemetryError, SubsystemId::Telemetry, "OTLP exporter is not enabled"));
    }

    if (config_.endpoint.empty())
    {
        return std::unexpected(make_error(
            ErrorCode::TelemetryError, SubsystemId::Telemetry, "OTLP endpoint not configured"));
    }

    // In production, this would serialize to OTLP protobuf and send via gRPC/HTTP.
    // For now, count as exported.
    export_count_ += metrics.size();
    return metrics.size();
}

auto OtlpExporter::export_crash_event(const CrashEvent& event) -> Result<void>
{
    if (!config_.enabled)
    {
        return std::unexpected(make_error(
            ErrorCode::TelemetryError, SubsystemId::Telemetry, "OTLP exporter is not enabled"));
    }

    // In production, serialize crash event to OTLP span with error status.
    // For now, just count it.
    (void)event;
    crash_exports_++;
    return {};
}

} // namespace markamp::core
