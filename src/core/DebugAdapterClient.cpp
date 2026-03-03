#include "DebugAdapterClient.h"

#include "DebugSessionManager.h"
#include "Logger.h"

namespace markamp::core
{

DebugAdapterClient::DebugAdapterClient() = default;
DebugAdapterClient::~DebugAdapterClient() = default;

void DebugAdapterClient::Initialize()
{
    MARKAMP_LOG_INFO("DAP: Initialize request sent");
    // Future: send initialize request over stdin/stdout pipe
}

void DebugAdapterClient::Launch(const LaunchConfig& config)
{
    MARKAMP_LOG_INFO("DAP: Launching program: {}", config.program);
    is_running_ = true;
    // Future: spawn debug adapter process, send launch request
}

void DebugAdapterClient::Attach(const LaunchConfig& config)
{
    MARKAMP_LOG_INFO("DAP: Attaching to port: {}", config.port);
    is_running_ = true;
}

void DebugAdapterClient::Disconnect()
{
    MARKAMP_LOG_INFO("DAP: Disconnect");
    is_running_ = false;
    if (on_terminated_)
    {
        on_terminated_();
    }
}

// ── Execution control ──

void DebugAdapterClient::Continue()
{
    MARKAMP_LOG_INFO("DAP: Continue");
}

void DebugAdapterClient::Pause()
{
    MARKAMP_LOG_INFO("DAP: Pause");
}

void DebugAdapterClient::StepOver()
{
    MARKAMP_LOG_INFO("DAP: Step Over");
}

void DebugAdapterClient::StepInto()
{
    MARKAMP_LOG_INFO("DAP: Step Into");
}

void DebugAdapterClient::StepOut()
{
    MARKAMP_LOG_INFO("DAP: Step Out");
}

void DebugAdapterClient::Terminate()
{
    MARKAMP_LOG_INFO("DAP: Terminate");
    is_running_ = false;
    if (on_terminated_)
    {
        on_terminated_();
    }
}

void DebugAdapterClient::Restart()
{
    MARKAMP_LOG_INFO("DAP: Restart");
}

// ── Breakpoints ──

void DebugAdapterClient::SetBreakpoints(const std::string& file,
                                        const std::vector<DAPBreakpoint>& breakpoints)
{
    MARKAMP_LOG_INFO("DAP: Setting {} breakpoints in {}", breakpoints.size(), file);
}

void DebugAdapterClient::SetExceptionBreakpoints(const std::vector<std::string>& filters)
{
    MARKAMP_LOG_INFO("DAP: Setting {} exception breakpoint filters", filters.size());
}

void DebugAdapterClient::SetDataBreakpoint(const std::string& data_id,
                                           const std::string& access_type)
{
    MARKAMP_LOG_INFO("DAP: Data breakpoint on {} ({})", data_id, access_type);
}

// ── Inspection ──

auto DebugAdapterClient::GetCallStack(int thread_id) -> std::vector<StackFrame>
{
    MARKAMP_LOG_INFO("DAP: Getting call stack for thread {}", thread_id);
    // Future: send stackTrace request, parse response
    return {};
}

auto DebugAdapterClient::GetVariables(int scope_id) -> std::vector<DebugVariable>
{
    MARKAMP_LOG_INFO("DAP: Getting variables for scope {}", scope_id);
    // Future: send variables request, parse response
    return {};
}

auto DebugAdapterClient::Evaluate(const std::string& expression, int frame_id) -> std::string
{
    MARKAMP_LOG_INFO("DAP: Evaluate '{}' in frame {}", expression, frame_id);
    // Future: send evaluate request, return result
    return "";
}

// ── Event callbacks ──

void DebugAdapterClient::SetOnStopped(StoppedCallback callback)
{
    on_stopped_ = std::move(callback);
}

void DebugAdapterClient::SetOnOutput(OutputCallback callback)
{
    on_output_ = std::move(callback);
}

void DebugAdapterClient::SetOnTerminated(TerminatedCallback callback)
{
    on_terminated_ = std::move(callback);
}

auto DebugAdapterClient::IsRunning() const -> bool
{
    return is_running_;
}

} // namespace markamp::core
