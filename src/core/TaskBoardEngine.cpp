/// @file TaskBoardEngine.cpp
/// @brief V9 Phase 23 – Kanban board engine implementation.

#include "core/TaskBoardEngine.h"

#include <algorithm>

namespace markamp::core
{

// ============================================================================
// Constructor
// ============================================================================

TaskBoardEngine::TaskBoardEngine()
{
    use_default_layout();
}

// ============================================================================
// Public API
// ============================================================================

auto TaskBoardEngine::configure(const BoardConfig& board_config) -> void
{
    config_ = board_config;
}

auto TaskBoardEngine::use_default_layout() -> void
{
    config_.columns.clear();
    config_.group_by = "none";

    BoardColumn todo;
    todo.col_id = "todo";
    todo.title = "To Do";
    todo.status = TaskStatus::kTodo;
    config_.columns.push_back(std::move(todo));

    BoardColumn in_progress;
    in_progress.col_id = "in_progress";
    in_progress.title = "In Progress";
    in_progress.status = TaskStatus::kInProgress;
    config_.columns.push_back(std::move(in_progress));

    BoardColumn done;
    done.col_id = "done";
    done.title = "Done";
    done.status = TaskStatus::kDone;
    config_.columns.push_back(std::move(done));

    BoardColumn cancelled;
    cancelled.col_id = "cancelled";
    cancelled.title = "Cancelled";
    cancelled.status = TaskStatus::kCancelled;
    config_.columns.push_back(std::move(cancelled));
}

auto TaskBoardEngine::populate(const std::vector<Task>& tasks) -> void
{
    // Clear existing task_ids in all columns
    for (auto& col : config_.columns)
    {
        col.task_ids.clear();
    }

    // Assign each task to its corresponding column
    for (const auto& task : tasks)
    {
        auto target_col = status_to_column_id(task.status);
        auto col_idx = find_column_index(target_col);

        if (col_idx.has_value())
        {
            config_.columns[col_idx.value()].task_ids.push_back(task.id);
        }
        else if (!config_.columns.empty())
        {
            // Default to first column if no match
            config_.columns[0].task_ids.push_back(task.id);
        }
    }
}

auto TaskBoardEngine::move_task(const std::string& task_id,
                                const std::string& from_column,
                                const std::string& to_column) -> std::expected<void, std::string>
{
    auto from_idx = find_column_index(from_column);
    if (!from_idx.has_value())
    {
        return std::unexpected("Source column not found: " + from_column);
    }

    auto to_idx = find_column_index(to_column);
    if (!to_idx.has_value())
    {
        return std::unexpected("Target column not found: " + to_column);
    }

    // Find and remove from source column
    auto& from_tasks = config_.columns[from_idx.value()].task_ids;
    auto iter = std::find(from_tasks.begin(), from_tasks.end(), task_id);
    if (iter == from_tasks.end())
    {
        return std::unexpected("Task not found in source column: " + task_id);
    }

    from_tasks.erase(iter);

    // Add to target column
    config_.columns[to_idx.value()].task_ids.push_back(task_id);

    return {};
}

auto TaskBoardEngine::get_column(const std::string& col_id) const -> std::optional<BoardColumn>
{
    auto idx = find_column_index(col_id);
    if (idx.has_value())
    {
        return config_.columns[idx.value()];
    }
    return std::nullopt;
}

auto TaskBoardEngine::get_columns() const -> const std::vector<BoardColumn>&
{
    return config_.columns;
}

auto TaskBoardEngine::is_wip_exceeded(const std::string& col_id) const -> bool
{
    auto idx = find_column_index(col_id);
    if (!idx.has_value())
    {
        return false;
    }

    const auto& col = config_.columns[idx.value()];
    if (col.wip_limit <= 0)
    {
        return false; // No limit set
    }

    return static_cast<int>(col.task_ids.size()) > col.wip_limit;
}

auto TaskBoardEngine::task_count() const -> int
{
    int total = 0;
    for (const auto& col : config_.columns)
    {
        total += static_cast<int>(col.task_ids.size());
    }
    return total;
}

auto TaskBoardEngine::config() const -> const BoardConfig&
{
    return config_;
}

// ============================================================================
// Private helpers
// ============================================================================

auto TaskBoardEngine::find_column_index(const std::string& col_id) const -> std::optional<size_t>
{
    for (size_t idx = 0; idx < config_.columns.size(); ++idx)
    {
        if (config_.columns[idx].col_id == col_id)
        {
            return idx;
        }
    }
    return std::nullopt;
}

auto TaskBoardEngine::status_to_column_id(TaskStatus status) -> std::string
{
    switch (status)
    {
        case TaskStatus::kTodo:
            return "todo";
        case TaskStatus::kInProgress:
            return "in_progress";
        case TaskStatus::kDone:
            return "done";
        case TaskStatus::kCancelled:
            return "cancelled";
        case TaskStatus::kDeferred:
            return "todo"; // Deferred goes to todo column
    }
    return "todo";
}

} // namespace markamp::core
