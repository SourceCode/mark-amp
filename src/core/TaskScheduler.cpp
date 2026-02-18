/// @file TaskScheduler.cpp
/// @brief V9 Phase 40 — TaskScheduler implementation.

#include "TaskScheduler.h"

#include <algorithm>

namespace markamp::core
{

void TaskScheduler::schedule(ScheduledTask task)
{
    if (task.task_id.empty())
    {
        task.task_id = "task_" + std::to_string(tasks_.size());
    }
    task.created_at = std::chrono::system_clock::now();
    if (task.next_run == std::chrono::system_clock::time_point{})
    {
        task.next_run = task.created_at;
    }
    task.state = ScheduledTaskState::kPending;
    tasks_.push_back(std::move(task));
}

auto TaskScheduler::cancel(const std::string& task_id) -> bool
{
    auto* task = find_mut(task_id);
    if (task == nullptr)
    {
        return false;
    }
    task->state = ScheduledTaskState::kCancelled;
    return true;
}

auto TaskScheduler::pause(const std::string& task_id) -> bool
{
    auto* task = find_mut(task_id);
    if (task == nullptr || task->state != ScheduledTaskState::kPending)
    {
        return false;
    }
    task->state = ScheduledTaskState::kPaused;
    return true;
}

auto TaskScheduler::resume(const std::string& task_id) -> bool
{
    auto* task = find_mut(task_id);
    if (task == nullptr || task->state != ScheduledTaskState::kPaused)
    {
        return false;
    }
    task->state = ScheduledTaskState::kPending;
    return true;
}

auto TaskScheduler::find_task(const std::string& task_id) const -> const ScheduledTask*
{
    for (const auto& task : tasks_)
    {
        if (task.task_id == task_id)
        {
            return &task;
        }
    }
    return nullptr;
}

auto TaskScheduler::all_tasks() const -> const std::vector<ScheduledTask>&
{
    return tasks_;
}

auto TaskScheduler::task_count() const -> int
{
    return static_cast<int>(tasks_.size());
}

auto TaskScheduler::tick() -> int
{
    auto now = std::chrono::system_clock::now();
    int executed = 0;

    for (auto& task : tasks_)
    {
        if (task.state != ScheduledTaskState::kPending)
        {
            continue;
        }
        if (now >= task.next_run)
        {
            execute_task(task);
            ++executed;
        }
    }
    return executed;
}

auto TaskScheduler::execute_now(const std::string& task_id) -> bool
{
    auto* task = find_mut(task_id);
    if (task == nullptr)
    {
        return false;
    }
    execute_task(*task);
    return true;
}

auto TaskScheduler::pending_tasks() const -> std::vector<const ScheduledTask*>
{
    std::vector<const ScheduledTask*> result;
    for (const auto& task : tasks_)
    {
        if (task.state == ScheduledTaskState::kPending)
        {
            result.push_back(&task);
        }
    }
    return result;
}

auto TaskScheduler::running_tasks() const -> std::vector<const ScheduledTask*>
{
    std::vector<const ScheduledTask*> result;
    for (const auto& task : tasks_)
    {
        if (task.state == ScheduledTaskState::kRunning)
        {
            result.push_back(&task);
        }
    }
    return result;
}

auto TaskScheduler::completed_tasks() const -> std::vector<const ScheduledTask*>
{
    std::vector<const ScheduledTask*> result;
    for (const auto& task : tasks_)
    {
        if (task.state == ScheduledTaskState::kCompleted)
        {
            result.push_back(&task);
        }
    }
    return result;
}

void TaskScheduler::clear_completed()
{
    auto iter = std::remove_if(tasks_.begin(),
                               tasks_.end(),
                               [](const ScheduledTask& task)
                               {
                                   return task.state == ScheduledTaskState::kCompleted ||
                                          task.state == ScheduledTaskState::kCancelled ||
                                          task.state == ScheduledTaskState::kFailed;
                               });
    tasks_.erase(iter, tasks_.end());
}

void TaskScheduler::clear_all()
{
    tasks_.clear();
}

auto TaskScheduler::find_mut(const std::string& task_id) -> ScheduledTask*
{
    for (auto& task : tasks_)
    {
        if (task.task_id == task_id)
        {
            return &task;
        }
    }
    return nullptr;
}

void TaskScheduler::execute_task(ScheduledTask& task)
{
    task.state = ScheduledTaskState::kRunning;
    task.last_run = std::chrono::system_clock::now();
    ++task.execution_count;

    bool success = false;
    if (task.execute_fn)
    {
        success = task.execute_fn();
    }

    if (success)
    {
        // Check if we should repeat
        if (task.repeat && (task.max_executions == 0 || task.execution_count < task.max_executions))
        {
            task.state = ScheduledTaskState::kPending;
            task.next_run = task.last_run + std::chrono::seconds(task.interval_seconds);
        }
        else
        {
            task.state = ScheduledTaskState::kCompleted;
        }
    }
    else
    {
        task.state = ScheduledTaskState::kFailed;
    }
}

} // namespace markamp::core
