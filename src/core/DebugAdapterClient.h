#pragma once

#include "LaunchConfig.h"

#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

struct StackFrame;
struct DebugVariable;

/// A DAP breakpoint request.
struct DAPBreakpoint
{
    int line{0};
    std::string condition;
    std::string log_message;
    std::string hit_condition;
};

/// Phase 19 Task 5: Debug Adapter Protocol client.
/// Communicates with debug adapters (lldb-vscode, cppdbg) over stdin/stdout
/// using the DAP JSON-RPC protocol.
class DebugAdapterClient
{
public:
    DebugAdapterClient();
    ~DebugAdapterClient();

    /// Initialize the DAP session.
    void Initialize();

    /// Launch a program for debugging.
    void Launch(const LaunchConfig& config);

    /// Attach to a running process.
    void Attach(const LaunchConfig& config);

    /// Disconnect from the debug adapter.
    void Disconnect();

    // ── Execution control ──

    void Continue();
    void Pause();
    void StepOver();
    void StepInto();
    void StepOut();
    void Terminate();
    void Restart();

    // ── Breakpoints ──

    /// Set line breakpoints for a file.
    void SetBreakpoints(const std::string& file, const std::vector<DAPBreakpoint>& breakpoints);

    /// Set exception breakpoint filters (e.g. "uncaught", "caught").
    void SetExceptionBreakpoints(const std::vector<std::string>& filters);

    /// Set a data breakpoint that triggers on value change.
    void SetDataBreakpoint(const std::string& data_id, const std::string& access_type);

    // ── Inspection ──

    /// Get call stack for a thread.
    [[nodiscard]] auto GetCallStack(int thread_id) -> std::vector<StackFrame>;

    /// Get variables for a given scope reference.
    [[nodiscard]] auto GetVariables(int scope_id) -> std::vector<DebugVariable>;

    /// Evaluate an expression in the current context.
    [[nodiscard]] auto Evaluate(const std::string& expression, int frame_id) -> std::string;

    // ── Event callbacks ──

    using StoppedCallback =
        std::function<void(int line, const std::string& reason, const std::string& file)>;
    using OutputCallback =
        std::function<void(const std::string& category, const std::string& text)>;
    using TerminatedCallback = std::function<void()>;

    void SetOnStopped(StoppedCallback callback);
    void SetOnOutput(OutputCallback callback);
    void SetOnTerminated(TerminatedCallback callback);

    /// Check if the debug adapter process is running.
    [[nodiscard]] auto IsRunning() const -> bool;

private:
    struct Impl;
    // Future: will hold process handle, message queue, etc.
    bool is_running_{false};

    StoppedCallback on_stopped_;
    OutputCallback on_output_;
    TerminatedCallback on_terminated_;
};

} // namespace markamp::core
