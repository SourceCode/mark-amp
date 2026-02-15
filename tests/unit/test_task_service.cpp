/// @file test_task_service.cpp
/// @brief V4 Phase 24 – TaskService unit tests.

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/TaskService.h"
#include "core/VaultService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

namespace
{

struct TestFixture
{
    EventBus event_bus;
    Config config;
    VaultService vault_service{event_bus, config};
    TaskService service{event_bus, vault_service};
};

} // anonymous namespace

// ---------------------------------------------------------------------------
// Test 1: Parse todo
// ---------------------------------------------------------------------------
TEST_CASE("TaskService: parse todo", "[task_service]")
{
    auto task = TaskService::parse_task_line("- [ ] Buy groceries");
    REQUIRE(task.has_value());
    CHECK(task->status == TaskStatus::kTodo);
    CHECK_FALSE(task->text.empty());
}

// ---------------------------------------------------------------------------
// Test 2: Parse done
// ---------------------------------------------------------------------------
TEST_CASE("TaskService: parse done", "[task_service]")
{
    auto task = TaskService::parse_task_line("- [x] Completed task");
    REQUIRE(task.has_value());
    CHECK(task->status == TaskStatus::kDone);
    CHECK(task->is_completed());
}

// ---------------------------------------------------------------------------
// Test 3: Parse with due date
// ---------------------------------------------------------------------------
TEST_CASE("TaskService: parse with due date", "[task_service]")
{
    auto task = TaskService::parse_task_line("- [ ] Task due:2026-03-01");
    REQUIRE(task.has_value());
    REQUIRE(task->due_date.has_value());
    CHECK(task->due_date->year() == std::chrono::year{2026});
    CHECK(task->due_date->month() == std::chrono::March);
    CHECK(task->due_date->day() == std::chrono::day{1});
}

// ---------------------------------------------------------------------------
// Test 4: Parse with priority
// ---------------------------------------------------------------------------
TEST_CASE("TaskService: parse with priority", "[task_service]")
{
    auto task = TaskService::parse_task_line("- [ ] !!! Urgent task");
    REQUIRE(task.has_value());
    CHECK(task->priority == TaskPriority::kHigh);

    auto task2 = TaskService::parse_task_line("- [ ] ! Low priority");
    REQUIRE(task2.has_value());
    CHECK(task2->priority == TaskPriority::kLow);

    auto task3 = TaskService::parse_task_line("- [ ] !! Medium priority");
    REQUIRE(task3.has_value());
    CHECK(task3->priority == TaskPriority::kMedium);

    auto task4 = TaskService::parse_task_line("- [ ] !!!! Critical task");
    REQUIRE(task4.has_value());
    CHECK(task4->priority == TaskPriority::kCritical);
}

// ---------------------------------------------------------------------------
// Test 5: Parse with tags
// ---------------------------------------------------------------------------
TEST_CASE("TaskService: parse with tags", "[task_service]")
{
    auto task = TaskService::parse_task_line("- [ ] Task #work #urgent");
    REQUIRE(task.has_value());
    REQUIRE(task->tags.size() == 2);
    CHECK(task->tags[0] == "work");
    CHECK(task->tags[1] == "urgent");
}

// ---------------------------------------------------------------------------
// Test 6: Query by status
// ---------------------------------------------------------------------------
TEST_CASE("TaskService: query by status", "[task_service]")
{
    TestFixture fixture;

    std::vector<Task> tasks;
    auto t1 = TaskService::parse_task_line("- [ ] Task A", 1);
    auto t2 = TaskService::parse_task_line("- [x] Task B", 2);
    auto t3 = TaskService::parse_task_line("- [ ] Task C", 3);
    auto t4 = TaskService::parse_task_line("- [x] Task D", 4);
    auto t5 = TaskService::parse_task_line("- [ ] Task E", 5);

    tasks.push_back(*t1);
    tasks.push_back(*t2);
    tasks.push_back(*t3);
    tasks.push_back(*t4);
    tasks.push_back(*t5);

    TaskQuery query;
    query.status = TaskStatus::kTodo;

    auto results = fixture.service.query(tasks, query);
    CHECK(results.size() == 3);
}

// ---------------------------------------------------------------------------
// Test 7: Stats
// ---------------------------------------------------------------------------
TEST_CASE("TaskService: stats", "[task_service]")
{
    std::vector<Task> tasks;

    for (int idx = 0; idx < 6; ++idx)
    {
        tasks.push_back(*TaskService::parse_task_line("- [ ] Todo " + std::to_string(idx), idx));
    }
    for (int idx = 6; idx < 10; ++idx)
    {
        tasks.push_back(*TaskService::parse_task_line("- [x] Done " + std::to_string(idx), idx));
    }

    auto [total, done, overdue] = TaskService::stats(tasks);
    CHECK(total == 10);
    CHECK(done == 4);
    CHECK(overdue == 0);
}

// ---------------------------------------------------------------------------
// Test 8: Scan document
// ---------------------------------------------------------------------------
TEST_CASE("TaskService: scan document", "[task_service]")
{
    std::string content = R"(# My Document

Some text here.

- [ ] First task
- [x] Second task (done)
- [ ] Third task #important

More text.
)";

    auto tasks = TaskService::scan_document(content, "doc1");
    REQUIRE(tasks.size() == 3);
    CHECK(tasks[0].document_id == "doc1");
    CHECK(tasks[1].status == TaskStatus::kDone);
    CHECK(tasks[2].tags.size() == 1);
}

// ---------------------------------------------------------------------------
// Test 9: Project extraction
// ---------------------------------------------------------------------------
TEST_CASE("TaskService: project extraction", "[task_service]")
{
    auto task = TaskService::parse_task_line("- [ ] Task @backend");
    REQUIRE(task.has_value());
    CHECK(task->project == "backend");

    // @due should not be treated as project
    auto task2 = TaskService::parse_task_line("- [ ] Task @due(2026-01-01)");
    REQUIRE(task2.has_value());
    CHECK(task2->project.empty());
}

// ---------------------------------------------------------------------------
// Test 10: In-progress/cancelled/deferred statuses
// ---------------------------------------------------------------------------
TEST_CASE("TaskService: alternative statuses", "[task_service]")
{
    auto in_progress = TaskService::parse_task_line("- [/] In progress task");
    REQUIRE(in_progress.has_value());
    CHECK(in_progress->status == TaskStatus::kInProgress);

    auto cancelled = TaskService::parse_task_line("- [-] Cancelled task");
    REQUIRE(cancelled.has_value());
    CHECK(cancelled->status == TaskStatus::kCancelled);
    CHECK(cancelled->is_completed());

    auto deferred = TaskService::parse_task_line("- [>] Deferred task");
    REQUIRE(deferred.has_value());
    CHECK(deferred->status == TaskStatus::kDeferred);
}

// ---------------------------------------------------------------------------
// Test 11: Non-task lines return nullopt
// ---------------------------------------------------------------------------
TEST_CASE("TaskService: non-task lines", "[task_service]")
{
    CHECK_FALSE(TaskService::parse_task_line("Regular text").has_value());
    CHECK_FALSE(TaskService::parse_task_line("# Heading").has_value());
    CHECK_FALSE(TaskService::parse_task_line("- Regular list item").has_value());
    CHECK_FALSE(TaskService::parse_task_line("").has_value());
}

// ---------------------------------------------------------------------------
// Test 12: Priority label
// ---------------------------------------------------------------------------
TEST_CASE("TaskService: priority label", "[task_service]")
{
    Task task;
    task.priority = TaskPriority::kNone;
    CHECK(task.priority_label() == "none");

    task.priority = TaskPriority::kLow;
    CHECK(task.priority_label() == "low");

    task.priority = TaskPriority::kMedium;
    CHECK(task.priority_label() == "medium");

    task.priority = TaskPriority::kHigh;
    CHECK(task.priority_label() == "high");

    task.priority = TaskPriority::kCritical;
    CHECK(task.priority_label() == "critical");
}
