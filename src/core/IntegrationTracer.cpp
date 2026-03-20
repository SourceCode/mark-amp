/// @file IntegrationTracer.cpp
/// @brief P09-T04: Integration-focused instrumentation for UI chains.

#include "IntegrationTracer.h"

#include "Logger.h"

namespace markamp::core
{

void IntegrationTracer::begin_trace(const std::string& trace_id,
                                     const std::string& surface,
                                     const std::string& action)
{
    last_begin_ = std::chrono::steady_clock::now();
    MARKAMP_LOG_DEBUG("[TRACE:BEGIN] {} surface={} action={}", trace_id, surface, action);
}

void IntegrationTracer::end_trace(const std::string& trace_id,
                                   const std::string& result)
{
    const auto elapsed = std::chrono::steady_clock::now() - last_begin_;
    const auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    ++trace_count_;

    MARKAMP_LOG_DEBUG("[TRACE:END] {} result={} duration={}ms", trace_id, result, duration_ms);
}

void IntegrationTracer::record(const TraceRecord& record)
{
    ++trace_count_;
    MARKAMP_LOG_DEBUG("[TRACE] {} surface={} action={} target={} result={} duration={}ms",
                      record.trace_id, record.surface, record.action,
                      record.target, record.result, record.duration_ms);
}

} // namespace markamp::core
