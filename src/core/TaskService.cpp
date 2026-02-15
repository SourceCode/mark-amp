/// @file TaskService.cpp
/// @brief V4 Phase 24 – Task Management System implementation.

#include "core/TaskService.h"

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/VaultService.h"

#include <algorithm>
#include <regex>
#include <set>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// Constructor
// ============================================================================

TaskService::TaskService(EventBus& event_bus, VaultService& vault_service)
    : event_bus_(event_bus)
    , vault_service_(vault_service)
{
}

// ============================================================================
// Task helper methods
// ============================================================================

auto Task::is_overdue() const -> bool
{
    if (!due_date.has_value() || is_completed())
    {
        return false;
    }

    const auto today = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
    const auto due_sys = std::chrono::sys_days{*due_date};

    return due_sys < today;
}

auto Task::is_completed() const -> bool
{
    return status == TaskStatus::kDone || status == TaskStatus::kCancelled;
}

auto Task::priority_label() const -> std::string
{
    switch (priority)
    {
        case TaskPriority::kNone:
            return "none";
        case TaskPriority::kLow:
            return "low";
        case TaskPriority::kMedium:
            return "medium";
        case TaskPriority::kHigh:
            return "high";
        case TaskPriority::kCritical:
            return "critical";
    }
    return "none";
}

// ============================================================================
// Parse task line
// ============================================================================

auto TaskService::parse_task_line(const std::string& line, int line_number) -> std::optional<Task>
{
    // Match: optional whitespace + "- [" + status_char + "]" + space + text
    const std::regex task_re(R"(^\s*- \[(.)\]\s+(.*))");
    std::smatch match;

    if (!std::regex_match(line, match, task_re))
    {
        return std::nullopt;
    }

    Task task;
    task.line_number = line_number;

    // Parse status character
    const std::string status_char = match[1].str();
    if (status_char == " ")
    {
        task.status = TaskStatus::kTodo;
    }
    else if (status_char == "x" || status_char == "X")
    {
        task.status = TaskStatus::kDone;
    }
    else if (status_char == "/")
    {
        task.status = TaskStatus::kInProgress;
    }
    else if (status_char == "-")
    {
        task.status = TaskStatus::kCancelled;
    }
    else if (status_char == ">")
    {
        task.status = TaskStatus::kDeferred;
    }
    else
    {
        return std::nullopt; // Unknown status character
    }

    std::string text = match[2].str();
    task.text = text;

    // Extract metadata from text
    task.priority = extract_priority(text);
    task.due_date = extract_due_date(text);
    task.tags = extract_tags(text);
    task.project = extract_project(text);

    // Strip metadata markers from display text
    // Remove priority markers !+
    {
        const std::regex priority_re(R"(\s*!{1,4}\s*)");
        task.text = std::regex_replace(task.text, priority_re, " ");
    }

    // Trim leading/trailing whitespace
    while (!task.text.empty() && task.text.front() == ' ')
    {
        task.text.erase(task.text.begin());
    }
    while (!task.text.empty() && task.text.back() == ' ')
    {
        task.text.pop_back();
    }

    // Generate ID from line_number
    task.id = "task_" + std::to_string(line_number);

    return task;
}

// ============================================================================
// Extract due date
// ============================================================================

auto TaskService::extract_due_date(const std::string& text)
    -> std::optional<std::chrono::year_month_day>
{
    // Match due:YYYY-MM-DD or @due(YYYY-MM-DD)
    const std::regex due_re(R"((?:due:|@due\()(\d{4})-(\d{2})-(\d{2})\)?)");
    std::smatch match;

    if (!std::regex_search(text, match, due_re))
    {
        return std::nullopt;
    }

    int year_val = std::stoi(match[1].str());
    unsigned month_val = static_cast<unsigned>(std::stoi(match[2].str()));
    unsigned day_val = static_cast<unsigned>(std::stoi(match[3].str()));

    auto ymd =
        std::chrono::year{year_val} / std::chrono::month{month_val} / std::chrono::day{day_val};

    if (!ymd.ok())
    {
        return std::nullopt;
    }

    return ymd;
}

// ============================================================================
// Extract priority
// ============================================================================

auto TaskService::extract_priority(const std::string& text) -> TaskPriority
{
    // Check for explicit priority:xxx
    const std::regex explicit_re(R"(priority:(low|medium|high|critical))", std::regex::icase);
    std::smatch match;
    if (std::regex_search(text, match, explicit_re))
    {
        std::string level = match[1].str();
        // Convert to lowercase
        std::transform(level.begin(), level.end(), level.begin(), ::tolower);
        if (level == "critical")
        {
            return TaskPriority::kCritical;
        }
        if (level == "high")
        {
            return TaskPriority::kHigh;
        }
        if (level == "medium")
        {
            return TaskPriority::kMedium;
        }
        if (level == "low")
        {
            return TaskPriority::kLow;
        }
    }

    // Check for ! markers (count consecutive !)
    const std::regex bang_re(R"((!{1,4})\s)");
    if (std::regex_search(text, match, bang_re))
    {
        auto bang_count = match[1].str().size();
        if (bang_count >= 4)
        {
            return TaskPriority::kCritical;
        }
        if (bang_count == 3)
        {
            return TaskPriority::kHigh;
        }
        if (bang_count == 2)
        {
            return TaskPriority::kMedium;
        }
        return TaskPriority::kLow;
    }

    return TaskPriority::kNone;
}

// ============================================================================
// Extract tags
// ============================================================================

auto TaskService::extract_tags(const std::string& text) -> std::vector<std::string>
{
    std::vector<std::string> tags;

    const std::regex tag_re(R"(#([\w-]+))");
    auto begin = std::sregex_iterator(text.begin(), text.end(), tag_re);
    const auto end = std::sregex_iterator();

    for (auto iter = begin; iter != end; ++iter)
    {
        tags.push_back((*iter)[1].str());
    }

    return tags;
}

// ============================================================================
// Extract project
// ============================================================================

auto TaskService::extract_project(const std::string& text) -> std::string
{
    const std::regex project_re(R"(@([\w-]+))");
    std::smatch match;

    if (std::regex_search(text, match, project_re))
    {
        // Exclude @due which is a date marker
        std::string project = match[1].str();
        if (project != "due")
        {
            return project;
        }
    }

    return {};
}

// ============================================================================
// Query tasks
// ============================================================================

auto TaskService::query(const std::vector<Task>& tasks, const TaskQuery& query_params) const
    -> std::vector<Task>
{
    std::vector<Task> results;

    for (const auto& task : tasks)
    {
        // Filter by status
        if (query_params.status.has_value() && task.status != *query_params.status)
        {
            continue;
        }

        // Filter completed
        if (!query_params.include_completed && task.is_completed())
        {
            continue;
        }

        // Filter by priority
        if (query_params.min_priority.has_value() &&
            static_cast<int>(task.priority) < static_cast<int>(*query_params.min_priority))
        {
            continue;
        }

        // Filter by tags
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

        // Filter by project
        if (!query_params.project.empty() && task.project != query_params.project)
        {
            continue;
        }

        // Filter overdue only
        if (query_params.overdue_only && !task.is_overdue())
        {
            continue;
        }

        // Filter by due date range
        if (query_params.due_before.has_value() && task.due_date.has_value())
        {
            if (std::chrono::sys_days{*task.due_date} >=
                std::chrono::sys_days{*query_params.due_before})
            {
                continue;
            }
        }

        if (query_params.due_after.has_value() && task.due_date.has_value())
        {
            if (std::chrono::sys_days{*task.due_date} <=
                std::chrono::sys_days{*query_params.due_after})
            {
                continue;
            }
        }

        results.push_back(task);
    }

    // Sort
    if (query_params.sort_by == "priority")
    {
        std::sort(results.begin(),
                  results.end(),
                  [](const Task& lhs, const Task& rhs)
                  { return static_cast<int>(lhs.priority) > static_cast<int>(rhs.priority); });
    }
    else if (query_params.sort_by == "due_date")
    {
        std::sort(results.begin(),
                  results.end(),
                  [](const Task& lhs, const Task& rhs)
                  {
                      if (!lhs.due_date.has_value())
                      {
                          return false;
                      }
                      if (!rhs.due_date.has_value())
                      {
                          return true;
                      }
                      return std::chrono::sys_days{*lhs.due_date} <
                             std::chrono::sys_days{*rhs.due_date};
                  });
    }

    return results;
}

// ============================================================================
// Stats
// ============================================================================

auto TaskService::stats(const std::vector<Task>& tasks) -> std::tuple<int, int, int>
{
    int total = static_cast<int>(tasks.size());
    int done = 0;
    int overdue = 0;

    for (const auto& task : tasks)
    {
        if (task.is_completed())
        {
            ++done;
        }
        if (task.is_overdue())
        {
            ++overdue;
        }
    }

    return {total, done, overdue};
}

// ============================================================================
// Projects
// ============================================================================

auto TaskService::projects(const std::vector<Task>& tasks) -> std::vector<std::string>
{
    std::set<std::string> unique_projects;
    for (const auto& task : tasks)
    {
        if (!task.project.empty())
        {
            unique_projects.insert(task.project);
        }
    }
    return {unique_projects.begin(), unique_projects.end()};
}

// ============================================================================
// Scan document
// ============================================================================

auto TaskService::scan_document(const std::string& content, const std::string& document_id)
    -> std::vector<Task>
{
    std::vector<Task> tasks;
    std::istringstream stream(content);
    std::string line;
    int line_number = 1;

    while (std::getline(stream, line))
    {
        auto task = parse_task_line(line, line_number);
        if (task.has_value())
        {
            task->document_id = document_id;
            task->id = document_id + ":" + std::to_string(line_number);
            tasks.push_back(std::move(*task));
        }
        ++line_number;
    }

    return tasks;
}

} // namespace markamp::core
