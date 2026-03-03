#include "TaskRunner.h"

#include "Logger.h"

namespace markamp::core
{

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

    MARKAMP_LOG_INFO("TaskRunner: Executing '{}' in {}", resolved.value(), working_dir);
    is_running_ = true;

    // Future: spawn process, pipe stdout to on_output, await exit code
    if (on_output)
    {
        on_output("[TaskRunner] Running: " + resolved.value());
    }

    // Stub: simulate success
    is_running_ = false;
    if (on_complete)
    {
        on_complete(true);
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
}

auto TaskRunner::IsRunning() const -> bool
{
    return is_running_;
}

} // namespace markamp::core
