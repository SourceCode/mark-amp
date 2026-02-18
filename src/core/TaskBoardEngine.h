/// @file TaskBoardEngine.h
/// @brief V9 Phase 23 – Kanban-style task board engine (pure logic, no UI).
/// Organizes tasks into configurable columns by status, supports WIP limits
/// and task movement between columns.

#pragma once

#include "core/TaskService.h"

#include <cstdint>
#include <expected>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Board types
// ============================================================================

/// A single column in the Kanban board.
struct BoardColumn
{
    std::string col_id;
    std::string title;
    TaskStatus status{TaskStatus::kTodo};
    std::vector<std::string> task_ids; ///< Tasks in this column
    int wip_limit{0};                  ///< 0 = unlimited
};

/// Board configuration.
struct BoardConfig
{
    std::vector<BoardColumn> columns;
    std::string group_by; ///< "none", "project", "priority"
};

// ============================================================================
// TaskBoardEngine
// ============================================================================

class TaskBoardEngine
{
public:
    TaskBoardEngine();

    /// Configure the board with custom columns.
    auto configure(const BoardConfig& config) -> void;

    /// Use the default column layout (Todo, In Progress, Done, Cancelled).
    auto use_default_layout() -> void;

    /// Populate the board by assigning tasks to columns based on status.
    auto populate(const std::vector<Task>& tasks) -> void;

    /// Move a task from one column to another.
    auto move_task(const std::string& task_id,
                   const std::string& from_column,
                   const std::string& to_column) -> std::expected<void, std::string>;

    /// Get a specific column by ID.
    [[nodiscard]] auto get_column(const std::string& col_id) const -> std::optional<BoardColumn>;

    /// Get all columns.
    [[nodiscard]] auto get_columns() const -> const std::vector<BoardColumn>&;

    /// Check if a column's WIP limit is exceeded.
    [[nodiscard]] auto is_wip_exceeded(const std::string& col_id) const -> bool;

    /// Get total task count across all columns.
    [[nodiscard]] auto task_count() const -> int;

    /// Get the board configuration.
    [[nodiscard]] auto config() const -> const BoardConfig&;

private:
    BoardConfig config_;

    /// Find a column index by ID.
    [[nodiscard]] auto find_column_index(const std::string& col_id) const -> std::optional<size_t>;

    /// Map TaskStatus to the default column ID.
    [[nodiscard]] static auto status_to_column_id(TaskStatus status) -> std::string;
};

} // namespace markamp::core
