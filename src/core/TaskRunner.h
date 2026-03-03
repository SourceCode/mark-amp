#pragma once

#include <expected>
#include <functional>
#include <string>

namespace markamp::core
{

/// Phase 19 Task 4: Pre-launch task runner.
/// Executes build tasks before starting the debugger.
class TaskRunner
{
public:
    /// Execute a named build task and call back with success/failure.
    /// @param task_name The build task identifier (e.g. "cmake-build-debug")
    /// @param working_dir The working directory for the command
    /// @param on_complete Called with true on success (exit code 0), false on failure
    /// @param on_output Called with each line of build output
    void ExecuteTask(const std::string& task_name,
                     const std::string& working_dir,
                     std::function<void(bool success)> on_complete,
                     std::function<void(const std::string& line)> on_output = nullptr);

    /// Resolve a build task name to a command.
    /// Known tasks: "cmake-build-debug", "cmake-build-release", "make", etc.
    [[nodiscard]] static auto ResolveBuildCommand(const std::string& task_name)
        -> std::expected<std::string, std::string>;

    /// Cancel a running task.
    void Cancel();

    /// Check if a task is currently running.
    [[nodiscard]] auto IsRunning() const -> bool;

private:
    bool is_running_{false};
    // Future: process handle for cancellation
};

} // namespace markamp::core
