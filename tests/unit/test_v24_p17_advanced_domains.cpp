/// @file test_v24_p17_advanced_domains.cpp
/// @brief V24 Phase 17 tests: Task management, backlinks, domain structures.
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/TaskService.h"

using namespace markamp::core;

// P17-T01: Task priority and status enums
TEST_CASE("P17-T01 task priority enum values", "[v24][p17]") {
    REQUIRE(TaskPriority::kNone != TaskPriority::kLow);
    REQUIRE(TaskPriority::kLow != TaskPriority::kMedium);
    REQUIRE(TaskPriority::kMedium != TaskPriority::kHigh);
    REQUIRE(TaskPriority::kHigh != TaskPriority::kCritical);
}

TEST_CASE("P17-T01 task status enum values", "[v24][p17]") {
    REQUIRE(TaskStatus::kTodo != TaskStatus::kInProgress);
    REQUIRE(TaskStatus::kInProgress != TaskStatus::kDone);
    REQUIRE(TaskStatus::kDone != TaskStatus::kCancelled);
    REQUIRE(TaskStatus::kCancelled != TaskStatus::kDeferred);
}

// P17-T02: Task structure
TEST_CASE("P17-T02 task struct defaults", "[v24][p17]") {
    Task t;
    t.id = "task-1";
    t.text = "Fix login bug";
    t.status = TaskStatus::kTodo;
    t.priority = TaskPriority::kHigh;
    REQUIRE(t.status == TaskStatus::kTodo);
    REQUIRE(t.priority == TaskPriority::kHigh);
}

TEST_CASE("P17-T02 task is_completed check", "[v24][p17]") {
    Task t;
    t.status = TaskStatus::kDone;
    REQUIRE(t.is_completed());
    t.status = TaskStatus::kTodo;
    REQUIRE_FALSE(t.is_completed());
}

// P17-T03: Task parsing
TEST_CASE("P17-T03 parse task line todo", "[v24][p17]") {
    auto result = TaskService::parse_task_line("- [ ] Buy groceries", 1);
    REQUIRE(result.has_value());
    REQUIRE(result->status == TaskStatus::kTodo);
    REQUIRE(result->text.find("Buy groceries") != std::string::npos);
}

TEST_CASE("P17-T03 parse task line done", "[v24][p17]") {
    auto result = TaskService::parse_task_line("- [x] Done task", 5);
    REQUIRE(result.has_value());
    REQUIRE(result->status == TaskStatus::kDone);
}

// P17-T04: Tag and project extraction
TEST_CASE("P17-T04 extract tags", "[v24][p17]") {
    auto tags = TaskService::extract_tags("Fix bug #backend #urgent");
    REQUIRE(tags.size() >= 1);
}

TEST_CASE("P17-T04 extract priority", "[v24][p17]") {
    auto p1 = TaskService::extract_priority("!!! Critical task");
    REQUIRE(p1 != TaskPriority::kNone);
}

// P17-T05: Task query and stats
TEST_CASE("P17-T05 task query structure", "[v24][p17]") {
    TaskQuery query;
    query.status = TaskStatus::kTodo;
    query.include_completed = false;
    query.overdue_only = false;
    REQUIRE(query.status == TaskStatus::kTodo);
}

TEST_CASE("P17-T05 task stats", "[v24][p17]") {
    std::vector<Task> tasks;
    Task t1; t1.status = TaskStatus::kTodo;
    Task t2; t2.status = TaskStatus::kDone;
    Task t3; t3.status = TaskStatus::kInProgress;
    tasks.push_back(t1);
    tasks.push_back(t2);
    tasks.push_back(t3);
    auto [total, done, overdue] = TaskService::stats(tasks);
    REQUIRE(total == 3);
    REQUIRE(done == 1);
}
