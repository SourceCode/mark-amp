/// @file TaskAggregator.cpp
/// @brief V9 Phase 23 – Cross-document task aggregation implementation.

#include "core/TaskAggregator.h"

#include <algorithm>
#include <set>

namespace markamp::core
{

// ============================================================================
// Public API
// ============================================================================

auto TaskAggregator::add_tasks(const std::string& document_id, const std::vector<Task>& tasks)
    -> void
{
    document_tasks_[document_id] = tasks;
}

auto TaskAggregator::remove_document(const std::string& document_id) -> int
{
    auto iter = document_tasks_.find(document_id);
    if (iter == document_tasks_.end())
    {
        return 0;
    }
    auto count = static_cast<int>(iter->second.size());
    document_tasks_.erase(iter);
    return count;
}

auto TaskAggregator::all_tasks() const -> std::vector<Task>
{
    std::vector<Task> result;
    for (const auto& [doc_id, tasks] : document_tasks_)
    {
        result.insert(result.end(), tasks.begin(), tasks.end());
    }
    return result;
}

auto TaskAggregator::task_count() const -> int
{
    int total = 0;
    for (const auto& [doc_id, tasks] : document_tasks_)
    {
        total += static_cast<int>(tasks.size());
    }
    return total;
}

auto TaskAggregator::document_count() const -> int
{
    return static_cast<int>(document_tasks_.size());
}

auto TaskAggregator::query(const TaskQuery& query_params) const -> std::vector<Task>
{
    auto tasks = all_tasks();
    std::vector<Task> result;

    for (const auto& task : tasks)
    {
        // Status filter
        if (query_params.status.has_value() && task.status != query_params.status.value())
        {
            continue;
        }

        // Skip completed unless requested
        if (!query_params.include_completed && task.is_completed())
        {
            continue;
        }

        // Priority filter
        if (query_params.min_priority.has_value() &&
            static_cast<uint8_t>(task.priority) <
                static_cast<uint8_t>(query_params.min_priority.value()))
        {
            continue;
        }

        // Overdue filter
        if (query_params.overdue_only && !task.is_overdue())
        {
            continue;
        }

        // Project filter
        if (!query_params.project.empty() && task.project != query_params.project)
        {
            continue;
        }

        // Tag filter — task must contain at least one of the query tags
        if (!query_params.tags.empty())
        {
            bool has_tag = false;
            for (const auto& query_tag : query_params.tags)
            {
                for (const auto& task_tag : task.tags)
                {
                    if (task_tag == query_tag)
                    {
                        has_tag = true;
                        break;
                    }
                }
                if (has_tag)
                {
                    break;
                }
            }
            if (!has_tag)
            {
                continue;
            }
        }

        // Date range filters
        if (query_params.due_before.has_value() && task.due_date.has_value())
        {
            if (std::chrono::sys_days{task.due_date.value()} >=
                std::chrono::sys_days{query_params.due_before.value()})
            {
                continue;
            }
        }

        if (query_params.due_after.has_value() && task.due_date.has_value())
        {
            if (std::chrono::sys_days{task.due_date.value()} <=
                std::chrono::sys_days{query_params.due_after.value()})
            {
                continue;
            }
        }

        result.push_back(task);
    }

    // Sort by due_date by default
    if (query_params.sort_by == "due_date")
    {
        std::sort(result.begin(),
                  result.end(),
                  [](const Task& lhs, const Task& rhs)
                  {
                      if (!lhs.due_date.has_value() && !rhs.due_date.has_value())
                      {
                          return false;
                      }
                      if (!lhs.due_date.has_value())
                      {
                          return false;
                      }
                      if (!rhs.due_date.has_value())
                      {
                          return true;
                      }
                      return std::chrono::sys_days{lhs.due_date.value()} <
                             std::chrono::sys_days{rhs.due_date.value()};
                  });
    }
    else if (query_params.sort_by == "priority")
    {
        std::sort(
            result.begin(),
            result.end(),
            [](const Task& lhs, const Task& rhs)
            { return static_cast<uint8_t>(lhs.priority) > static_cast<uint8_t>(rhs.priority); });
    }

    return result;
}

auto TaskAggregator::summary() const -> TaskSummary
{
    TaskSummary stats;
    auto current_date = today();
    auto current_sys = std::chrono::sys_days{current_date};
    auto week_end = current_sys + std::chrono::days{7};

    for (const auto& [doc_id, tasks] : document_tasks_)
    {
        for (const auto& task : tasks)
        {
            ++stats.total;

            switch (task.status)
            {
                case TaskStatus::kDone:
                    ++stats.done;
                    break;
                case TaskStatus::kInProgress:
                    ++stats.in_progress;
                    break;
                case TaskStatus::kCancelled:
                    ++stats.cancelled;
                    break;
                default:
                    break;
            }

            if (task.is_overdue())
            {
                ++stats.overdue;
            }

            if (task.due_date.has_value())
            {
                auto due_sys = std::chrono::sys_days{task.due_date.value()};
                if (due_sys == current_sys)
                {
                    ++stats.due_today;
                }
                if (due_sys >= current_sys && due_sys <= week_end)
                {
                    ++stats.due_this_week;
                }
            }

            if (!task.project.empty())
            {
                ++stats.by_project[task.project];
            }

            ++stats.by_priority[static_cast<int>(task.priority)];
        }
    }

    return stats;
}

auto TaskAggregator::tasks_due_on(std::chrono::year_month_day date) const -> std::vector<Task>
{
    std::vector<Task> result;
    auto target_sys = std::chrono::sys_days{date};

    for (const auto& [doc_id, tasks] : document_tasks_)
    {
        for (const auto& task : tasks)
        {
            if (task.due_date.has_value() &&
                std::chrono::sys_days{task.due_date.value()} == target_sys)
            {
                result.push_back(task);
            }
        }
    }

    return result;
}

auto TaskAggregator::tasks_due_between(std::chrono::year_month_day start,
                                       std::chrono::year_month_day end) const -> std::vector<Task>
{
    std::vector<Task> result;
    auto start_sys = std::chrono::sys_days{start};
    auto end_sys = std::chrono::sys_days{end};

    for (const auto& [doc_id, tasks] : document_tasks_)
    {
        for (const auto& task : tasks)
        {
            if (task.due_date.has_value())
            {
                auto due_sys = std::chrono::sys_days{task.due_date.value()};
                if (due_sys >= start_sys && due_sys <= end_sys)
                {
                    result.push_back(task);
                }
            }
        }
    }

    return result;
}

auto TaskAggregator::overdue_tasks() const -> std::vector<Task>
{
    std::vector<Task> result;

    for (const auto& [doc_id, tasks] : document_tasks_)
    {
        for (const auto& task : tasks)
        {
            if (task.is_overdue())
            {
                result.push_back(task);
            }
        }
    }

    return result;
}

auto TaskAggregator::projects() const -> std::vector<std::string>
{
    std::set<std::string> unique_projects;

    for (const auto& [doc_id, tasks] : document_tasks_)
    {
        for (const auto& task : tasks)
        {
            if (!task.project.empty())
            {
                unique_projects.insert(task.project);
            }
        }
    }

    return {unique_projects.begin(), unique_projects.end()};
}

auto TaskAggregator::tags() const -> std::vector<std::string>
{
    std::set<std::string> unique_tags;

    for (const auto& [doc_id, tasks] : document_tasks_)
    {
        for (const auto& task : tasks)
        {
            for (const auto& tag : task.tags)
            {
                unique_tags.insert(tag);
            }
        }
    }

    return {unique_tags.begin(), unique_tags.end()};
}

auto TaskAggregator::due_date_counts() const -> std::map<std::chrono::year_month_day, int>
{
    std::map<std::chrono::year_month_day, int> counts;

    for (const auto& [doc_id, tasks] : document_tasks_)
    {
        for (const auto& task : tasks)
        {
            if (task.due_date.has_value() && !task.is_completed())
            {
                ++counts[task.due_date.value()];
            }
        }
    }

    return counts;
}

auto TaskAggregator::clear() -> void
{
    document_tasks_.clear();
}

auto TaskAggregator::today() -> std::chrono::year_month_day
{
    auto now = std::chrono::system_clock::now();
    auto days = std::chrono::floor<std::chrono::days>(now);
    return std::chrono::year_month_day{days};
}

} // namespace markamp::core
