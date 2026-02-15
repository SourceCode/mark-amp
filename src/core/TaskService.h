/// @file TaskService.h
/// @brief V4 Phase 24 – Task Management System.

#pragma once

#include <chrono>
#include <expected>
#include <mutex>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

namespace markamp::core
{

class EventBus;
class VaultService;

// ============================================================================
// Task enums
// ============================================================================

enum class TaskPriority : uint8_t
{
    kNone,
    kLow,
    kMedium,
    kHigh,
    kCritical
};

enum class TaskStatus : uint8_t
{
    kTodo,       // - [ ]
    kInProgress, // - [/]
    kDone,       // - [x]
    kCancelled,  // - [-]
    kDeferred    // - [>]
};

// ============================================================================
// Task: a single task item extracted from Markdown
// ============================================================================

struct Task
{
    std::string id;
    std::string text;
    TaskStatus status{TaskStatus::kTodo};
    TaskPriority priority{TaskPriority::kNone};
    std::string document_id;
    std::string document_title;
    int line_number{0};
    std::vector<std::string> tags;
    std::optional<std::chrono::year_month_day> due_date;
    std::optional<std::chrono::year_month_day> scheduled_date;
    std::optional<std::chrono::year_month_day> completed_date;
    std::string project;
    std::string context;

    [[nodiscard]] auto is_overdue() const -> bool;
    [[nodiscard]] auto is_completed() const -> bool;
    [[nodiscard]] auto priority_label() const -> std::string;
};

// ============================================================================
// TaskQuery: filter/sort parameters for querying tasks
// ============================================================================

struct TaskQuery
{
    std::optional<TaskStatus> status;
    std::optional<TaskPriority> min_priority;
    std::vector<std::string> tags;
    std::string project;
    bool include_completed{false};
    bool overdue_only{false};
    std::optional<std::chrono::year_month_day> due_before;
    std::optional<std::chrono::year_month_day> due_after;
    std::string sort_by{"due_date"};
};

// ============================================================================
// TaskService
// ============================================================================

class TaskService
{
public:
    TaskService(EventBus& event_bus, VaultService& vault_service);

    /// Parse a single line for task syntax.
    [[nodiscard]] static auto parse_task_line(const std::string& line, int line_number = 0)
        -> std::optional<Task>;

    /// Extract due date from task text.
    [[nodiscard]] static auto extract_due_date(const std::string& text)
        -> std::optional<std::chrono::year_month_day>;

    /// Extract priority from task text.
    [[nodiscard]] static auto extract_priority(const std::string& text) -> TaskPriority;

    /// Extract tags from task text.
    [[nodiscard]] static auto extract_tags(const std::string& text) -> std::vector<std::string>;

    /// Extract project from task text.
    [[nodiscard]] static auto extract_project(const std::string& text) -> std::string;

    /// Query tasks with filters.
    [[nodiscard]] auto query(const std::vector<Task>& tasks, const TaskQuery& query_params) const
        -> std::vector<Task>;

    /// Get task statistics: (total, done, overdue).
    [[nodiscard]] static auto stats(const std::vector<Task>& tasks) -> std::tuple<int, int, int>;

    /// Get all unique project names.
    [[nodiscard]] static auto projects(const std::vector<Task>& tasks) -> std::vector<std::string>;

    /// Scan document content for tasks.
    [[nodiscard]] static auto scan_document(const std::string& content,
                                            const std::string& document_id) -> std::vector<Task>;

private:
    EventBus& event_bus_;
    VaultService& vault_service_;
};

} // namespace markamp::core
