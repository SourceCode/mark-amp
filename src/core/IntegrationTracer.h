/// @file IntegrationTracer.h
/// @brief P09-T04: Integration-focused instrumentation for UI chains.
///
/// Scoped trace points for command execution, mode switches, session restore,
/// and prompt lifecycle. Debug-only verbosity.
#pragma once

#include <chrono>
#include <string>

namespace markamp::core
{

/// A trace record for an integration event.
struct TraceRecord
{
    std::string trace_id;
    std::string surface;       ///< "editor", "canvas", "graph", "notebook"
    std::string action;        ///< "command.execute", "mode.switch", "session.restore"
    std::string target;        ///< Resource identifier
    std::string result;        ///< "success", "failure", "cancelled"
    int duration_ms{0};
};

/// Integration tracer for diagnosing UI chain failures.
class IntegrationTracer
{
public:
    IntegrationTracer() = default;

    /// Begin a trace for a scoped operation.
    void begin_trace(const std::string& trace_id, const std::string& surface,
                     const std::string& action);

    /// End a trace with a result.
    void end_trace(const std::string& trace_id, const std::string& result);

    /// Record a single-shot trace event.
    void record(const TraceRecord& record);

    /// Get the total trace count.
    [[nodiscard]] auto trace_count() const -> int { return trace_count_; }

private:
    int trace_count_{0};
    std::chrono::steady_clock::time_point last_begin_;
};

} // namespace markamp::core
