#pragma once

#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Phase 19 Task 14: Generic process runner for "Run Without Debugging".
/// Spawns a child process and routes output.
class ProcessRunner
{
public:
    ProcessRunner();
    ~ProcessRunner();

    /// Launch a program with arguments.
    void Launch(const std::string& program,
                const std::vector<std::string>& args,
                const std::string& working_dir);

    /// Terminate the running process.
    void Terminate();

    /// Check if process is running.
    [[nodiscard]] auto IsRunning() const -> bool;

    /// Get the last exit code (-1 if still running).
    [[nodiscard]] auto ExitCode() const -> int;

    // ── Callbacks ──

    using OutputCallback = std::function<void(const std::string& text, bool is_stderr)>;
    using ExitCallback = std::function<void(int exit_code)>;

    void SetOnOutput(OutputCallback callback);
    void SetOnExit(ExitCallback callback);

private:
    bool is_running_{false};
    int exit_code_{-1};
    OutputCallback on_output_;
    ExitCallback on_exit_;
    // Future: platform-specific process handle (pid_t / HANDLE)
};

} // namespace markamp::core
