#pragma once

#include "CompilerErrorParser.h"
#include "EventBus.h"

#include <chrono>
#include <expected>
#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Task group classification (build infrastructure).
enum class TaskGroup
{
    kBuild,
    kTest,
    kDeploy,
    kOther,
};

/// Task type (build infrastructure).
enum class TaskType
{
    kShell,
    kProcess,
};

/// Presentation options for task output.
struct TaskPresentation
{
    bool reveal{true};
    bool focus{false};
    bool clear_before{false};
    bool show_rerun{true};
};

/// A single task definition for build execution.
struct TaskDefinition
{
    std::string name;
    std::string label;
    std::string command;
    std::vector<std::string> args;
    std::string working_directory;
    TaskGroup group{TaskGroup::kOther};
    TaskType type{TaskType::kShell};
    TaskPresentation presentation;
    std::string problem_matcher;
    bool is_default{false};
    bool is_background{false};

    [[nodiscard]] auto full_command() const -> std::string
    {
        std::string result = command;
        for (const auto& arg : args)
        {
            result += " " + arg;
        }
        return result;
    }
};


/// Result of a task execution.
struct TaskExecutionResult
{
    bool success{false};
    int exit_code{-1};
    float duration_seconds{0.0F};
    int error_count{0};
    int warning_count{0};
    std::vector<ParsedCompilerError> errors;
    std::vector<std::string> output_lines;
};

/// Phase 25 Task 4: Enhanced task runner with EventBus integration,
/// CompilerErrorParser piping, and TaskConfig-based execution.
class TaskRunner
{
public:
    explicit TaskRunner(EventBus& event_bus);

    /// Execute a named build task and call back with success/failure.
    /// @param task_name The build task identifier (e.g. "cmake-build-debug")
    /// @param working_dir The working directory for the command
    /// @param on_complete Called with true on success (exit code 0), false on failure
    /// @param on_output Called with each line of build output
    void ExecuteTask(const std::string& task_name,
                     const std::string& working_dir,
                     std::function<void(bool success)> on_complete,
                     std::function<void(const std::string& line)> on_output = nullptr);

    /// Execute a TaskConfig entry.
    /// @param task Task configuration from tasks.json
    /// @param on_complete Called with the full execution result
    void ExecuteTaskConfig(const TaskDefinition& task,
                           std::function<void(const TaskExecutionResult& result)> on_complete);

    /// Resolve a build task name to a command.
    /// Known tasks: "cmake-build-debug", "cmake-build-release", "make", etc.
    [[nodiscard]] static auto ResolveBuildCommand(const std::string& task_name)
        -> std::expected<std::string, std::string>;

    /// Cancel a running task.
    void Cancel();

    /// Check if a task is currently running.
    [[nodiscard]] auto IsRunning() const -> bool;

    /// Get the last execution result.
    [[nodiscard]] auto last_result() const -> const TaskExecutionResult&;

private:
    EventBus& event_bus_;
    bool is_running_{false};
    TaskExecutionResult last_result_;
    std::chrono::steady_clock::time_point start_time_;

    /// Run a command line string, collecting output and parsing errors.
    void RunCommand(const std::string& command,
                    const std::string& working_dir,
                    const std::string& task_name,
                    const std::string& task_type,
                    std::function<void(const TaskExecutionResult& result)> on_complete,
                    std::function<void(const std::string& line)> on_output);
};

} // namespace markamp::core
