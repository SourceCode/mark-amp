/// @file TaskAggregator.h
/// @brief V9 Phase 23 – Cross-document task collection and indexing.
/// Aggregates tasks from all scanned documents, provides unified queries,
/// dashboard statistics, and date-range lookups.

#pragma once

#include "core/TaskService.h"

#include <chrono>
#include <map>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// TaskSummary: aggregated statistics
// ============================================================================

struct TaskSummary
{
    int total{0};
    int done{0};
    int in_progress{0};
    int overdue{0};
    int due_today{0};
    int due_this_week{0};
    int cancelled{0};
    std::map<std::string, int> by_project; ///< Count per project
    std::map<int, int> by_priority;        ///< Count per priority level
};

// ============================================================================
// TaskAggregator
// ============================================================================

class TaskAggregator
{
public:
    TaskAggregator() = default;

    /// Add/update tasks from a document scan.
    auto add_tasks(const std::string& document_id, const std::vector<Task>& tasks) -> void;

    /// Remove all tasks from a specific document.
    auto remove_document(const std::string& document_id) -> int;

    /// Get all indexed tasks across all documents.
    [[nodiscard]] auto all_tasks() const -> std::vector<Task>;

    /// Total task count.
    [[nodiscard]] auto task_count() const -> int;

    /// Number of indexed documents.
    [[nodiscard]] auto document_count() const -> int;

    /// Query tasks with filters (delegates to TaskService::query logic).
    [[nodiscard]] auto query(const TaskQuery& query_params) const -> std::vector<Task>;

    /// Get dashboard summary statistics.
    [[nodiscard]] auto summary() const -> TaskSummary;

    /// Get tasks due on a specific date.
    [[nodiscard]] auto tasks_due_on(std::chrono::year_month_day date) const -> std::vector<Task>;

    /// Get tasks due within a date range (inclusive).
    [[nodiscard]] auto tasks_due_between(std::chrono::year_month_day start,
                                         std::chrono::year_month_day end) const
        -> std::vector<Task>;

    /// Get all overdue tasks.
    [[nodiscard]] auto overdue_tasks() const -> std::vector<Task>;

    /// Get all unique project names.
    [[nodiscard]] auto projects() const -> std::vector<std::string>;

    /// Get all unique tags.
    [[nodiscard]] auto tags() const -> std::vector<std::string>;

    /// Get task-due-date counts for calendar integration.
    [[nodiscard]] auto due_date_counts() const -> std::map<std::chrono::year_month_day, int>;

    /// Clear all indexed tasks.
    auto clear() -> void;

private:
    std::map<std::string, std::vector<Task>> document_tasks_;

    /// Get today's date for comparisons.
    [[nodiscard]] static auto today() -> std::chrono::year_month_day;
};

} // namespace markamp::core
