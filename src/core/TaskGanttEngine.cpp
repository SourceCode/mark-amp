/// @file TaskGanttEngine.cpp
/// @brief V9 Phase 23 – Gantt timeline engine implementation.

#include "core/TaskGanttEngine.h"

#include <algorithm>
#include <map>
#include <set>

namespace markamp::core
{

// ============================================================================
// Public API
// ============================================================================

auto TaskGanttEngine::set_tasks(const std::vector<Task>& tasks) -> void
{
    bars_.clear();
    bars_.reserve(tasks.size());

    for (const auto& task : tasks)
    {
        bars_.push_back(task_to_bar(task));
    }
}

auto TaskGanttEngine::set_time_range(std::chrono::year_month_day start,
                                     std::chrono::year_month_day end) -> void
{
    range_start_ = start;
    range_end_ = end;
}

auto TaskGanttEngine::get_bars() const -> std::vector<GanttBar>
{
    std::vector<GanttBar> result;

    for (const auto& bar : bars_)
    {
        if (!bar.is_milestone && is_in_range(bar))
        {
            result.push_back(bar);
        }
    }

    // Sort by start date
    std::sort(result.begin(),
              result.end(),
              [](const GanttBar& lhs, const GanttBar& rhs)
              {
                  if (!lhs.start_date.has_value() && !rhs.start_date.has_value())
                  {
                      return false;
                  }
                  if (!lhs.start_date.has_value())
                  {
                      return false;
                  }
                  if (!rhs.start_date.has_value())
                  {
                      return true;
                  }
                  return std::chrono::sys_days{lhs.start_date.value()} <
                         std::chrono::sys_days{rhs.start_date.value()};
              });

    return result;
}

auto TaskGanttEngine::get_milestones() const -> std::vector<GanttBar>
{
    std::vector<GanttBar> result;

    for (const auto& bar : bars_)
    {
        if (bar.is_milestone && is_in_range(bar))
        {
            result.push_back(bar);
        }
    }

    return result;
}

auto TaskGanttEngine::set_time_scale(GanttTimeScale scale) -> void
{
    time_scale_ = scale;
}

auto TaskGanttEngine::time_scale() const -> GanttTimeScale
{
    return time_scale_;
}

auto TaskGanttEngine::get_critical_path() const -> std::vector<std::string>
{
    // Build adjacency list from dependencies
    std::map<std::string, std::vector<std::string>> adj;
    std::set<std::string> all_ids;

    for (const auto& bar : bars_)
    {
        all_ids.insert(bar.task_id);
        for (const auto& dep : bar.dependencies)
        {
            adj[dep].push_back(bar.task_id); // dep -> task_id
        }
    }

    if (adj.empty())
    {
        return {}; // No dependencies, no critical path
    }

    // Find longest path using DFS with memoization
    std::map<std::string, int> longest;
    std::map<std::string, std::string> next_on_path;

    // Compute in-degree for topological ordering
    std::map<std::string, int> in_degree;
    for (const auto& task_id : all_ids)
    {
        in_degree[task_id] = 0;
    }
    for (const auto& bar : bars_)
    {
        for (const auto& dep : bar.dependencies)
        {
            if (all_ids.contains(dep))
            {
                ++in_degree[bar.task_id];
            }
        }
    }

    // Topological sort (Kahn's algorithm)
    std::vector<std::string> topo_order;
    std::vector<std::string> queue;
    for (const auto& [task_id, deg] : in_degree)
    {
        if (deg == 0)
        {
            queue.push_back(task_id);
        }
    }

    auto topo_in_degree = in_degree;
    while (!queue.empty())
    {
        auto current = queue.back();
        queue.pop_back();
        topo_order.push_back(current);

        if (adj.contains(current))
        {
            for (const auto& neighbor : adj[current])
            {
                --topo_in_degree[neighbor];
                if (topo_in_degree[neighbor] == 0)
                {
                    queue.push_back(neighbor);
                }
            }
        }
    }

    // Compute longest path from topological order
    for (const auto& task_id : all_ids)
    {
        longest[task_id] = 1;
    }

    for (const auto& current : topo_order)
    {
        if (adj.contains(current))
        {
            for (const auto& neighbor : adj[current])
            {
                if (longest[current] + 1 > longest[neighbor])
                {
                    longest[neighbor] = longest[current] + 1;
                    next_on_path[neighbor] = current;
                }
            }
        }
    }

    // Find the task with the longest path
    std::string max_task;
    int max_len = 0;
    for (const auto& [task_id, path_len] : longest)
    {
        if (path_len > max_len)
        {
            max_len = path_len;
            max_task = task_id;
        }
    }

    // Reconstruct the path
    std::vector<std::string> path;
    auto current = max_task;
    while (!current.empty())
    {
        path.push_back(current);
        if (next_on_path.contains(current))
        {
            current = next_on_path[current];
        }
        else
        {
            break;
        }
    }

    // Reverse to get start -> end order
    std::reverse(path.begin(), path.end());

    return path;
}

auto TaskGanttEngine::bar_count() const -> int
{
    return static_cast<int>(bars_.size());
}

auto TaskGanttEngine::start_date() const -> std::chrono::year_month_day
{
    return range_start_;
}

auto TaskGanttEngine::end_date() const -> std::chrono::year_month_day
{
    return range_end_;
}

// ============================================================================
// Private helpers
// ============================================================================

auto TaskGanttEngine::task_to_bar(const Task& task) -> GanttBar
{
    GanttBar bar;
    bar.task_id = task.id;
    bar.label = task.text;
    bar.priority = task.priority;
    bar.project = task.project;
    bar.progress = status_to_progress(task.status);

    // Determine dates
    if (task.scheduled_date.has_value() && task.due_date.has_value())
    {
        // Full bar: scheduled_date → due_date
        bar.start_date = task.scheduled_date;
        bar.end_date = task.due_date;
        bar.is_milestone = false;
    }
    else if (task.due_date.has_value())
    {
        // Milestone: only a due date
        bar.start_date = task.due_date;
        bar.end_date = task.due_date;
        bar.is_milestone = true;
    }
    else if (task.scheduled_date.has_value())
    {
        // Only start, no end: single-day bar
        bar.start_date = task.scheduled_date;
        bar.end_date = task.scheduled_date;
        bar.is_milestone = false;
    }
    // else: no dates, bar will not appear in timeline

    return bar;
}

auto TaskGanttEngine::status_to_progress(TaskStatus status) -> double
{
    switch (status)
    {
        case TaskStatus::kTodo:
            return 0.0;
        case TaskStatus::kInProgress:
            return 0.5;
        case TaskStatus::kDone:
        case TaskStatus::kCancelled:
            return 1.0;
        case TaskStatus::kDeferred:
            return 0.0;
    }
    return 0.0;
}

auto TaskGanttEngine::is_in_range(const GanttBar& bar) const -> bool
{
    // If no range is set (both at epoch), include everything
    auto range_start_sys = std::chrono::sys_days{range_start_};
    auto range_end_sys = std::chrono::sys_days{range_end_};

    if (range_start_sys == range_end_sys && range_start_sys == std::chrono::sys_days{})
    {
        return true; // No range filter
    }

    if (bar.start_date.has_value())
    {
        auto bar_start = std::chrono::sys_days{bar.start_date.value()};
        if (bar_start > range_end_sys)
        {
            return false;
        }
    }

    if (bar.end_date.has_value())
    {
        auto bar_end = std::chrono::sys_days{bar.end_date.value()};
        if (bar_end < range_start_sys)
        {
            return false;
        }
    }

    // Bars without dates are not in range
    return bar.start_date.has_value() || bar.end_date.has_value();
}

} // namespace markamp::core
