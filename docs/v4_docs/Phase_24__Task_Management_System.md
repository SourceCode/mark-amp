# Phase 24 -- Task Management System

## Objective

Implement a task management system that extracts `- [ ] task` and `- [x] completed` items from all vault documents, aggregates them into a queryable task index, and provides a dedicated Tasks panel with filtering, sorting, due dates, priorities, and project grouping. Tasks live in Markdown but are surfaced as structured data.

## Prerequisites

- Phase 02 (VaultService -- document scanning)
- Phase 04 (TagService -- tag-based filtering)
- Phase 01 (WikiLinkParser -- inline parsing)

## Feature References (PRD)

- PRD #31: Task Management

## Data Structures to Implement

### File: `src/core/TaskService.h`

```cpp
#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;
class VaultService;

enum class TaskPriority : uint8_t { None, Low, Medium, High, Critical };

enum class TaskStatus : uint8_t
{
    Todo,           // - [ ]
    InProgress,     // - [/]
    Done,           // - [x]
    Cancelled,      // - [-]
    Deferred        // - [>]
};

struct Task
{
    std::string id;                 // Generated from doc_id + line_number
    std::string text;               // Task text (markdown stripped)
    TaskStatus status{TaskStatus::Todo};
    TaskPriority priority{TaskPriority::None};
    std::string document_id;        // Source document
    std::string document_title;
    int line_number{0};
    std::vector<std::string> tags;
    std::optional<std::chrono::year_month_day> due_date;
    std::optional<std::chrono::year_month_day> scheduled_date;
    std::optional<std::chrono::year_month_day> completed_date;
    std::string project;            // Extracted from parent heading or tag
    std::string context;            // Surrounding text

    [[nodiscard]] auto is_overdue() const -> bool;
    [[nodiscard]] auto is_completed() const -> bool;
    [[nodiscard]] auto priority_label() const -> std::string;
};

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
    std::string sort_by{"due_date"};  // due_date, priority, document, created
};

class TaskService
{
public:
    TaskService(EventBus& event_bus, VaultService& vault_service);

    /// Rebuild the task index from all vault documents.
    auto rebuild() -> void;

    /// Update tasks for a single document.
    auto update_document(const std::string& document_id) -> void;

    /// Query tasks with filters.
    [[nodiscard]] auto query(const TaskQuery& q) const -> std::vector<Task>;

    /// Get all tasks for a specific document.
    [[nodiscard]] auto tasks_in_document(const std::string& document_id) const
        -> std::vector<Task>;

    /// Toggle a task's completion status (in the source document).
    [[nodiscard]] auto toggle_task(const std::string& task_id)
        -> std::expected<void, std::string>;

    /// Set a task's priority.
    auto set_priority(const std::string& task_id, TaskPriority priority) -> void;

    /// Set a task's due date.
    auto set_due_date(const std::string& task_id,
                       std::chrono::year_month_day date) -> void;

    /// Get task statistics.
    [[nodiscard]] auto stats() const -> std::tuple<int, int, int>;  // total, done, overdue

    /// Get all unique project names.
    [[nodiscard]] auto projects() const -> std::vector<std::string>;

    /// Parse a single line for task syntax.
    [[nodiscard]] static auto parse_task_line(const std::string& line) -> std::optional<Task>;

private:
    EventBus& event_bus_;
    VaultService& vault_service_;

    mutable std::mutex mutex_;
    std::vector<Task> tasks_;

    auto scan_document_for_tasks(const std::string& document_id) -> std::vector<Task>;
    auto extract_due_date(const std::string& text) const
        -> std::optional<std::chrono::year_month_day>;
    auto extract_priority(const std::string& text) const -> TaskPriority;
    auto extract_tags(const std::string& text) const -> std::vector<std::string>;

    Subscription doc_saved_sub_;
    Subscription vault_opened_sub_;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`rebuild()`** -- Scan all vault documents for task lines. Parse each into Task struct. Store in tasks_ vector.

2. **`parse_task_line(line)`** -- Detect `- [ ]`, `- [x]`, `- [/]`, `- [-]`, `- [>]` prefixes. Extract text after checkbox. Parse inline metadata: `due:2026-03-01`, `priority:high`, `#tag`, `@project`.

3. **`query(q)`** -- Filter tasks_ by status, priority, tags, project, due date range. Sort by specified field. Return matching tasks.

4. **`toggle_task(task_id)`** -- Find task. Toggle `[ ]` to `[x]` (or vice versa) in the source document. Save document. Update index.

5. **`extract_due_date(text)`** -- Parse dates in formats: `due:YYYY-MM-DD`, `@due(YYYY-MM-DD)`, natural dates like `due:tomorrow`.

6. **`extract_priority(text)`** -- Parse priority markers: `!` = low, `!!` = medium, `!!!` = high, `!!!!` = critical. Or explicit: `priority:high`.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TaskIndexRebuiltEvent)
int total_tasks{0};
int completed{0};
int overdue{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TaskToggledEvent)
std::string task_id;
TaskStatus new_status;
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_task_service.cpp`

1. **Parse todo** -- `- [ ] Buy groceries` -> status=Todo, text="Buy groceries".
2. **Parse done** -- `- [x] Done task` -> status=Done.
3. **Parse with due date** -- `- [ ] Task due:2026-03-01` -> due_date set.
4. **Parse with priority** -- `- [ ] !!! Urgent task` -> priority=High.
5. **Parse with tags** -- `- [ ] Task #work #urgent` -> tags=["work","urgent"].
6. **Query by status** -- 5 tasks, 2 done. query(status=Todo) returns 3.
7. **Query overdue** -- Task with past due date. overdue_only=true returns it.
8. **Toggle task** -- Toggle todo to done. Verify document updated.
9. **Stats** -- 10 tasks, 4 done, 2 overdue. stats() returns (10, 4, 2).
10. **Tasks in document** -- Document has 3 tasks. tasks_in_document() returns 3.
11. **Project extraction** -- Task under heading "## Work". project="Work".
12. **Incremental update** -- Save document with new task. update_document() adds it.

## Acceptance Criteria

- [ ] Task extraction supports `[ ]`, `[x]`, `[/]`, `[-]`, `[>]` checkboxes
- [ ] Due dates, priorities, and tags parsed from task lines
- [ ] Query with filters returns correct task subsets
- [ ] Toggle modifies source document and saves it
- [ ] Incremental updates on document save
- [ ] All 12 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/TaskService.h` | TaskService, Task, TaskQuery |
| CREATE | `src/core/TaskService.cpp` | Full implementation |
| MODIFY | `src/core/Events.h` | Add 2 task events |
| MODIFY | `src/core/PluginContext.h` | Add `TaskService* task_service{nullptr};` |
| MODIFY | `src/CMakeLists.txt` | Add TaskService.cpp |
| CREATE | `tests/unit/test_task_service.cpp` | 12 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_task_service target |

## Estimated Complexity

**L** -- Task parsing with metadata extraction, query engine, document modification, 12 tests.
