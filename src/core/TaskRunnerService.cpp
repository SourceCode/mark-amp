#include "TaskRunnerService.h"

namespace markamp::core
{

auto TaskRunnerService::register_task_provider(
    const std::string& type, std::function<std::vector<TaskDefinition>()> provider) -> bool
{
    providers_[type] = std::move(provider);
    return true;
}

auto TaskRunnerService::execute_task(const std::string& task_name) -> bool
{
    for (const auto& task : tasks_)
    {
        if (task.name == task_name)
        {
            TaskState state;
            state.task_name = task_name;
            state.status = TaskStatus::kRunning;
            state.start_time = std::chrono::steady_clock::now();
            task_states_[task_name] = std::move(state);
            return true;
        }
    }
    return false;
}

auto TaskRunnerService::cancel_task(const std::string& task_name) -> bool
{
    auto iter = task_states_.find(task_name);
    if (iter != task_states_.end() && iter->second.status == TaskStatus::kRunning)
    {
        iter->second.status = TaskStatus::kFailed;
        iter->second.error = "Cancelled";
        return true;
    }
    return false;
}

auto TaskRunnerService::get_tasks() const -> std::vector<TaskDefinition>
{
    return tasks_;
}

auto TaskRunnerService::get_task_status(const std::string& task_name) const -> TaskStatus
{
    const auto iter = task_states_.find(task_name);
    if (iter != task_states_.end())
    {
        return iter->second.status;
    }
    return TaskStatus::kIdle;
}

auto TaskRunnerService::get_task_state(const std::string& task_name) const
    -> std::optional<TaskState>
{
    const auto iter = task_states_.find(task_name);
    if (iter != task_states_.end())
    {
        return iter->second;
    }
    return std::nullopt;
}

auto TaskRunnerService::load_tasks_from_json(const std::string& /*path*/) -> bool
{
    // Stub — full JSON parsing requires a JSON library
    return false;
}

void TaskRunnerService::register_built_in_tasks()
{
    tasks_.push_back({"lint", "shell", "markdownlint .", "build", "Lint Markdown"});
    tasks_.push_back({"export", "shell", "markamp export", "build", "Export Document"});
    tasks_.push_back({"reindex", "shell", "markamp reindex", "build", "Reindex Content"});
}

auto TaskRunnerService::task_count() const -> std::size_t
{
    return tasks_.size();
}

auto TaskRunnerService::is_running(const std::string& task_name) const -> bool
{
    return get_task_status(task_name) == TaskStatus::kRunning;
}

} // namespace markamp::core
