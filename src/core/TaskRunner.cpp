#include "TaskRunner.h"

#include "Events.h"
#include "Logger.h"

#include <algorithm>
#include <array>
#include <cstdlib>

namespace markamp::core
{

TaskRunner::TaskRunner(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

void TaskRunner::ExecuteTask(const std::string& task_name,
                             const std::string& working_dir,
                             std::function<void(bool success)> on_complete,
                             std::function<void(const std::string& line)> on_output)
{
    auto resolved = ResolveBuildCommand(task_name);
    if (!resolved)
    {
        MARKAMP_LOG_ERROR("TaskRunner: Unknown task '{}': {}", task_name, resolved.error());
        if (on_complete)
        {
            on_complete(false);
        }
        return;
    }

    // Wrap into TaskConfig-style execution
    RunCommand(
        resolved.value(),
        working_dir,
        task_name,
        "build",
        [on_complete](const TaskExecutionResult& result)
        {
            if (on_complete)
            {
                on_complete(result.success);
            }
        },
        on_output);
}

void TaskRunner::ExecuteTaskConfig(
    const TaskDefinition& task, std::function<void(const TaskExecutionResult& result)> on_complete)
{
    const std::string command = task.full_command();
    const std::string working_dir = task.working_directory.empty() ? "." : task.working_directory;
    auto group_to_string = [](TaskGroup g) -> std::string {
        switch (g) {
            case TaskGroup::kBuild: return "build";
            case TaskGroup::kTest: return "test";
            case TaskGroup::kDeploy: return "deploy";
            case TaskGroup::kOther: return "other";
        }
        return "other";
    };
    const std::string task_type = group_to_string(task.group);
    const std::string task_name = task.label.empty() ? task.name : task.label;

    RunCommand(command, working_dir, task_name, task_type, on_complete, nullptr);
}

void TaskRunner::RunCommand(const std::string& command,
                            const std::string& working_dir,
                            const std::string& task_name,
                            const std::string& task_type,
                            std::function<void(const TaskExecutionResult& result)> on_complete,
                            std::function<void(const std::string& line)> on_output)
{
    if (is_running_)
    {
        MARKAMP_LOG_WARN("TaskRunner: Task already running, ignoring '{}'", task_name);
        return;
    }

    MARKAMP_LOG_INFO("TaskRunner: Executing '{}' in {}", command, working_dir);
    is_running_ = true;
    start_time_ = std::chrono::steady_clock::now();

    // Publish start event
    events::TaskExecutionStartedEvent start_evt;
    start_evt.task_name = task_name;
    start_evt.task_type = task_type;
    start_evt.command = command;
    event_bus_.publish(start_evt);

    // Build the full shell command with cd
    const std::string full_command = "cd \"" + working_dir + "\" && " + command + " 2>&1";

    // Execute via popen and collect output
    TaskExecutionResult result;
    result.success = false;
    result.exit_code = -1;

    CompilerErrorParser error_parser;

    // Notify output listener with command info
    if (on_output)
    {
        on_output("[TaskRunner] Running: " + command);
        on_output("[TaskRunner] Working dir: " + working_dir);
    }

    FILE* pipe = popen(full_command.c_str(), "r");
    if (pipe == nullptr)
    {
        MARKAMP_LOG_ERROR("TaskRunner: Failed to start process for '{}'", command);
        is_running_ = false;

        if (on_output)
        {
            on_output("[TaskRunner] ERROR: Failed to start process");
        }

        result.success = false;
        last_result_ = result;

        // Publish finish event
        events::TaskExecutionFinishedEvent finish_evt;
        finish_evt.task_name = task_name;
        finish_evt.success = false;
        finish_evt.exit_code = -1;
        finish_evt.duration_seconds = 0.0F;
        event_bus_.publish(finish_evt);

        if (on_complete)
        {
            on_complete(result);
        }
        return;
    }

    // Read output line by line
    std::array<char, 4096> buffer{};
    std::string line_buffer;

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr)
    {
        line_buffer += buffer.data();

        // Process complete lines
        std::size_t newline_pos = 0;
        while ((newline_pos = line_buffer.find('\n')) != std::string::npos)
        {
            std::string line = line_buffer.substr(0, newline_pos);
            line_buffer.erase(0, newline_pos + 1);

            // Strip trailing \r
            if (!line.empty() && line.back() == '\r')
            {
                line.pop_back();
            }

            result.output_lines.push_back(line);

            // Parse for compiler errors
            const auto parsed = error_parser.parse_line(line);
            for (const auto& err : parsed)
            {
                result.errors.push_back(err);
                if (err.severity == DiagnosticSeverity::kError)
                {
                    result.error_count++;
                }
                else if (err.severity == DiagnosticSeverity::kWarning)
                {
                    result.warning_count++;
                }
            }

            // Publish build output event
            events::BuildOutputEvent output_evt;
            output_evt.text = line;
            output_evt.is_stderr = false;
            event_bus_.publish(output_evt);

            // Call output listener
            if (on_output)
            {
                on_output(line);
            }
        }
    }

    // Process any remaining partial line
    if (!line_buffer.empty())
    {
        result.output_lines.push_back(line_buffer);

        const auto parsed = error_parser.parse_line(line_buffer);
        for (const auto& err : parsed)
        {
            result.errors.push_back(err);
            if (err.severity == DiagnosticSeverity::kError)
            {
                result.error_count++;
            }
            else if (err.severity == DiagnosticSeverity::kWarning)
            {
                result.warning_count++;
            }
        }

        if (on_output)
        {
            on_output(line_buffer);
        }
    }

    const int status = pclose(pipe);
    result.exit_code = WEXITSTATUS(status);
    result.success = (result.exit_code == 0);

    const auto end_time = std::chrono::steady_clock::now();
    const auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_);
    result.duration_seconds = static_cast<float>(duration.count()) / 1000.0F;

    is_running_ = false;
    last_result_ = result;

    MARKAMP_LOG_INFO(
        "TaskRunner: '{}' finished with exit code {} ({:.2f}s, {} errors, {} warnings)",
        task_name,
        result.exit_code,
        result.duration_seconds,
        result.error_count,
        result.warning_count);

    if (on_output)
    {
        on_output("[TaskRunner] Finished with exit code " + std::to_string(result.exit_code) +
                  " (" + std::to_string(result.error_count) + " errors, " +
                  std::to_string(result.warning_count) + " warnings)");
    }

    // Publish finish event
    events::TaskExecutionFinishedEvent finish_evt;
    finish_evt.task_name = task_name;
    finish_evt.success = result.success;
    finish_evt.exit_code = result.exit_code;
    finish_evt.duration_seconds = result.duration_seconds;
    event_bus_.publish(finish_evt);

    if (on_complete)
    {
        on_complete(result);
    }
}

auto TaskRunner::ResolveBuildCommand(const std::string& task_name)
    -> std::expected<std::string, std::string>
{
    if (task_name == "cmake-build-debug")
    {
        return "cmake --build build/debug";
    }
    if (task_name == "cmake-build-release")
    {
        return "cmake --build build/release";
    }
    if (task_name == "make")
    {
        return "make -j$(nproc)";
    }
    if (task_name.empty())
    {
        return std::unexpected("Empty task name");
    }

    // Treat unknown tasks as literal shell commands
    return task_name;
}

void TaskRunner::Cancel()
{
    MARKAMP_LOG_INFO("TaskRunner: Cancel requested");
    is_running_ = false;
    // Future: kill the process via pid
}

auto TaskRunner::IsRunning() const -> bool
{
    return is_running_;
}

auto TaskRunner::last_result() const -> const TaskExecutionResult&
{
    return last_result_;
}

} // namespace markamp::core
