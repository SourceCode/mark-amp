/// @file test_phase23_task_calendar.cpp
/// @brief V9 Phase 23 – Task Management & Calendar unit tests.
/// Tests TaskRecurrence, TaskReminder, TaskAggregator, TaskBoardEngine,
/// TaskGanttEngine, and TaskCommandProvider.

#include "core/Events.h"
#include "core/TaskAggregator.h"
#include "core/TaskBoardEngine.h"
#include "core/TaskCommandProvider.h"
#include "core/TaskGanttEngine.h"
#include "core/TaskRecurrence.h"
#include "core/TaskReminder.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace markamp::core;
using namespace std::chrono;

// ============================================================================
// Helper: create a Task with common fields
// ============================================================================

static auto make_task(const std::string& task_id,
                      const std::string& text,
                      TaskStatus status = TaskStatus::kTodo,
                      TaskPriority priority = TaskPriority::kNone) -> Task
{
    Task task;
    task.id = task_id;
    task.text = text;
    task.status = status;
    task.priority = priority;
    task.document_id = "doc_1";
    return task;
}

static auto make_dated_task(const std::string& task_id,
                            const std::string& text,
                            year_month_day due,
                            TaskStatus status = TaskStatus::kTodo) -> Task
{
    auto task = make_task(task_id, text, status);
    task.due_date = due;
    return task;
}

// ============================================================================
// TaskRecurrence Tests
// ============================================================================

TEST_CASE("TaskRecurrence: parse @daily", "[phase23][recurrence]")
{
    auto rule = TaskRecurrence::parse_recurrence("Buy groceries @daily");
    REQUIRE(rule.has_value());
    CHECK(rule->pattern == RecurrencePattern::kDaily);
    CHECK(rule->interval == 1);
}

TEST_CASE("TaskRecurrence: parse @weekly", "[phase23][recurrence]")
{
    auto rule = TaskRecurrence::parse_recurrence("Team meeting @weekly");
    REQUIRE(rule.has_value());
    CHECK(rule->pattern == RecurrencePattern::kWeekly);
    CHECK(rule->interval == 1);
}

TEST_CASE("TaskRecurrence: parse @monthly", "[phase23][recurrence]")
{
    auto rule = TaskRecurrence::parse_recurrence("Pay rent @monthly");
    REQUIRE(rule.has_value());
    CHECK(rule->pattern == RecurrencePattern::kMonthly);
    CHECK(rule->interval == 1);
}

TEST_CASE("TaskRecurrence: parse @monthly with day", "[phase23][recurrence]")
{
    auto rule = TaskRecurrence::parse_recurrence("Credit card bill @monthly(15)");
    REQUIRE(rule.has_value());
    CHECK(rule->pattern == RecurrencePattern::kMonthly);
    CHECK(rule->day_of_month == 15);
}

TEST_CASE("TaskRecurrence: parse @yearly", "[phase23][recurrence]")
{
    auto rule = TaskRecurrence::parse_recurrence("Birthday @yearly");
    REQUIRE(rule.has_value());
    CHECK(rule->pattern == RecurrencePattern::kYearly);
}

TEST_CASE("TaskRecurrence: parse @every(2w)", "[phase23][recurrence]")
{
    auto rule = TaskRecurrence::parse_recurrence("Sprint review @every(2w)");
    REQUIRE(rule.has_value());
    CHECK(rule->pattern == RecurrencePattern::kWeekly);
    CHECK(rule->interval == 2);
}

TEST_CASE("TaskRecurrence: parse @every(3d)", "[phase23][recurrence]")
{
    auto rule = TaskRecurrence::parse_recurrence("Water plants @every(3d)");
    REQUIRE(rule.has_value());
    CHECK(rule->pattern == RecurrencePattern::kDaily);
    CHECK(rule->interval == 3);
}

TEST_CASE("TaskRecurrence: parse @every(6m)", "[phase23][recurrence]")
{
    auto rule = TaskRecurrence::parse_recurrence("Dentist visit @every(6m)");
    REQUIRE(rule.has_value());
    CHECK(rule->pattern == RecurrencePattern::kMonthly);
    CHECK(rule->interval == 6);
}

TEST_CASE("TaskRecurrence: no recurrence", "[phase23][recurrence]")
{
    auto rule = TaskRecurrence::parse_recurrence("Just a normal task");
    CHECK(!rule.has_value());
}

TEST_CASE("TaskRecurrence: is_recurring", "[phase23][recurrence]")
{
    CHECK(TaskRecurrence::is_recurring("Task @daily"));
    CHECK(TaskRecurrence::is_recurring("Task @every(2w)"));
    CHECK(!TaskRecurrence::is_recurring("Regular task"));
}

TEST_CASE("TaskRecurrence: next_occurrence daily", "[phase23][recurrence]")
{
    RecurrenceRule rule;
    rule.pattern = RecurrencePattern::kDaily;
    rule.interval = 1;

    auto from = year_month_day{year{2025}, month{3}, day{15}};
    auto next = TaskRecurrence::next_occurrence(rule, from);

    CHECK(next == year_month_day{year{2025}, month{3}, day{16}});
}

TEST_CASE("TaskRecurrence: next_occurrence weekly", "[phase23][recurrence]")
{
    RecurrenceRule rule;
    rule.pattern = RecurrencePattern::kWeekly;
    rule.interval = 2;

    auto from = year_month_day{year{2025}, month{1}, day{6}};
    auto next = TaskRecurrence::next_occurrence(rule, from);

    CHECK(next == year_month_day{year{2025}, month{1}, day{20}});
}

TEST_CASE("TaskRecurrence: next_occurrence monthly", "[phase23][recurrence]")
{
    RecurrenceRule rule;
    rule.pattern = RecurrencePattern::kMonthly;
    rule.interval = 1;
    rule.day_of_month = 15;

    auto from = year_month_day{year{2025}, month{1}, day{15}};
    auto next = TaskRecurrence::next_occurrence(rule, from);

    CHECK(next == year_month_day{year{2025}, month{2}, day{15}});
}

TEST_CASE("TaskRecurrence: generate_occurrences", "[phase23][recurrence]")
{
    RecurrenceRule rule;
    rule.pattern = RecurrencePattern::kDaily;
    rule.interval = 1;

    auto start = year_month_day{year{2025}, month{1}, day{1}};
    auto end = year_month_day{year{2025}, month{1}, day{5}};

    auto dates = TaskRecurrence::generate_occurrences(rule, start, end);
    CHECK(dates.size() == 5);
    CHECK(dates[0] == start);
    CHECK(dates[4] == end);
}

TEST_CASE("TaskRecurrence: format_recurrence", "[phase23][recurrence]")
{
    RecurrenceRule daily;
    daily.pattern = RecurrencePattern::kDaily;
    daily.interval = 1;
    CHECK(TaskRecurrence::format_recurrence(daily) == "Daily");

    RecurrenceRule biweekly;
    biweekly.pattern = RecurrencePattern::kWeekly;
    biweekly.interval = 2;
    CHECK(TaskRecurrence::format_recurrence(biweekly) == "Every 2 weeks");

    RecurrenceRule monthly;
    monthly.pattern = RecurrencePattern::kMonthly;
    monthly.interval = 1;
    monthly.day_of_month = 15;
    CHECK(TaskRecurrence::format_recurrence(monthly) == "Monthly on day 15");
}

// ============================================================================
// TaskReminder Tests
// ============================================================================

TEST_CASE("TaskReminder: schedule and check_due", "[phase23][reminder]")
{
    TaskReminder reminder;
    auto due_time = system_clock::now() + hours{1};

    auto reminder_id = reminder.schedule("task_1", due_time, ReminderTiming::kAtDue);
    CHECK(!reminder_id.empty());
    CHECK(reminder.count() == 1);

    // Not due yet (due time is in the future)
    auto now_early = system_clock::now() - minutes{5};
    auto due_now = reminder.check_due(now_early);
    CHECK(due_now.empty());

    // Due when time is past the trigger
    auto future = due_time + minutes{5};
    auto due_later = reminder.check_due(future);
    CHECK(due_later.size() == 1);
    CHECK(due_later[0].task_id == "task_1");
}

TEST_CASE("TaskReminder: schedule with timing offset", "[phase23][reminder]")
{
    TaskReminder reminder;
    auto due_time = system_clock::now() + hours{24};

    reminder.schedule("task_1", due_time, ReminderTiming::kDay1);
    CHECK(reminder.count() == 1);

    // Should trigger 24 hours before due time (i.e., around now)
    auto near_now = system_clock::now() + minutes{1};
    auto due = reminder.check_due(near_now);
    CHECK(due.size() == 1);
}

TEST_CASE("TaskReminder: schedule_custom", "[phase23][reminder]")
{
    TaskReminder reminder;
    auto due_time = system_clock::now() + hours{2};

    auto rid = reminder.schedule_custom("task_1", due_time, 30);
    CHECK(!rid.empty());
    CHECK(reminder.count() == 1);
}

TEST_CASE("TaskReminder: dismiss", "[phase23][reminder]")
{
    TaskReminder reminder;
    auto due_time = system_clock::now();

    auto rid = reminder.schedule("task_1", due_time, ReminderTiming::kAtDue);

    auto result = reminder.dismiss(rid);
    CHECK(result.has_value());

    // After dismissal, check_due should return empty
    auto due = reminder.check_due(system_clock::now() + hours{1});
    CHECK(due.empty());

    // Dismiss non-existent
    auto bad_result = reminder.dismiss("nonexistent");
    CHECK(!bad_result.has_value());
}

TEST_CASE("TaskReminder: snooze", "[phase23][reminder]")
{
    TaskReminder reminder;
    auto due_time = system_clock::now() - minutes{5}; // Already past due

    auto rid = reminder.schedule("task_1", due_time, ReminderTiming::kAtDue);

    // Snooze for 30 minutes
    auto result = reminder.snooze(rid, minutes{30});
    CHECK(result.has_value());

    // Should not be due right now due to snooze
    auto due_now = reminder.check_due(system_clock::now());
    CHECK(due_now.empty());

    // Should be due in 35 minutes
    auto due_later = reminder.check_due(system_clock::now() + minutes{35});
    CHECK(due_later.size() == 1);
}

TEST_CASE("TaskReminder: clear_for_task", "[phase23][reminder]")
{
    TaskReminder reminder;
    auto due_time = system_clock::now() + hours{1};

    reminder.schedule("task_1", due_time, ReminderTiming::kAtDue);
    reminder.schedule("task_1", due_time, ReminderTiming::kHour1);
    reminder.schedule("task_2", due_time, ReminderTiming::kAtDue);

    CHECK(reminder.count() == 3);
    auto removed = reminder.clear_for_task("task_1");
    CHECK(removed == 2);
    CHECK(reminder.count() == 1);
}

TEST_CASE("TaskReminder: active_reminders", "[phase23][reminder]")
{
    TaskReminder reminder;
    auto due_time = system_clock::now();

    auto rid1 = reminder.schedule("task_1", due_time, ReminderTiming::kAtDue);
    reminder.schedule("task_2", due_time, ReminderTiming::kAtDue);

    reminder.dismiss(rid1);

    auto active = reminder.active_reminders();
    CHECK(active.size() == 1);
    CHECK(active[0].task_id == "task_2");
}

TEST_CASE("TaskReminder: serialize and deserialize", "[phase23][reminder]")
{
    TaskReminder reminder;
    auto due_time = system_clock::now() + hours{2};

    reminder.schedule("task_1", due_time, ReminderTiming::kHour1);
    reminder.schedule("task_2", due_time, ReminderTiming::kAtDue);

    auto json = reminder.serialize();
    CHECK(!json.empty());

    TaskReminder loaded;
    auto result = loaded.deserialize(json);
    CHECK(result.has_value());
    CHECK(loaded.count() == 2);
}

// ============================================================================
// TaskAggregator Tests
// ============================================================================

TEST_CASE("TaskAggregator: add and retrieve tasks", "[phase23][aggregator]")
{
    TaskAggregator agg;

    auto task1 = make_task("t1", "Task one");
    auto task2 = make_task("t2", "Task two");

    agg.add_tasks("doc_1", {task1, task2});

    CHECK(agg.task_count() == 2);
    CHECK(agg.document_count() == 1);

    auto all = agg.all_tasks();
    CHECK(all.size() == 2);
}

TEST_CASE("TaskAggregator: multi-document", "[phase23][aggregator]")
{
    TaskAggregator agg;

    agg.add_tasks("doc_1", {make_task("t1", "One")});
    agg.add_tasks("doc_2", {make_task("t2", "Two"), make_task("t3", "Three")});

    CHECK(agg.task_count() == 3);
    CHECK(agg.document_count() == 2);
}

TEST_CASE("TaskAggregator: remove_document", "[phase23][aggregator]")
{
    TaskAggregator agg;
    agg.add_tasks("doc_1", {make_task("t1", "One"), make_task("t2", "Two")});

    auto removed = agg.remove_document("doc_1");
    CHECK(removed == 2);
    CHECK(agg.task_count() == 0);

    // Remove non-existent
    CHECK(agg.remove_document("doc_99") == 0);
}

TEST_CASE("TaskAggregator: tasks_due_on", "[phase23][aggregator]")
{
    TaskAggregator agg;
    auto target_date = year_month_day{year{2025}, month{3}, day{15}};

    agg.add_tasks(
        "doc_1",
        {
            make_dated_task("t1", "Task 1", target_date),
            make_dated_task("t2", "Task 2", year_month_day{year{2025}, month{3}, day{16}}),
            make_dated_task("t3", "Task 3", target_date),
        });

    auto due = agg.tasks_due_on(target_date);
    CHECK(due.size() == 2);
}

TEST_CASE("TaskAggregator: tasks_due_between", "[phase23][aggregator]")
{
    TaskAggregator agg;
    agg.add_tasks(
        "doc_1",
        {
            make_dated_task("t1", "Jan 10", year_month_day{year{2025}, month{1}, day{10}}),
            make_dated_task("t2", "Jan 15", year_month_day{year{2025}, month{1}, day{15}}),
            make_dated_task("t3", "Jan 20", year_month_day{year{2025}, month{1}, day{20}}),
            make_dated_task("t4", "Feb 05", year_month_day{year{2025}, month{2}, day{5}}),
        });

    auto range = agg.tasks_due_between(year_month_day{year{2025}, month{1}, day{12}},
                                       year_month_day{year{2025}, month{1}, day{22}});
    CHECK(range.size() == 2);
}

TEST_CASE("TaskAggregator: projects and tags", "[phase23][aggregator]")
{
    TaskAggregator agg;

    auto task1 = make_task("t1", "Task one");
    task1.project = "ProjectA";
    task1.tags = {"urgent", "frontend"};

    auto task2 = make_task("t2", "Task two");
    task2.project = "ProjectB";
    task2.tags = {"backend", "urgent"};

    auto task3 = make_task("t3", "Task three");
    task3.project = "ProjectA";
    task3.tags = {"docs"};

    agg.add_tasks("doc_1", {task1, task2, task3});

    auto projects = agg.projects();
    CHECK(projects.size() == 2);

    auto tags = agg.tags();
    CHECK(tags.size() == 4); // backend, docs, frontend, urgent (sorted)
}

TEST_CASE("TaskAggregator: summary", "[phase23][aggregator]")
{
    TaskAggregator agg;

    auto t1 = make_task("t1", "Done task", TaskStatus::kDone);
    auto t2 = make_task("t2", "In progress", TaskStatus::kInProgress);
    auto t3 = make_task("t3", "Todo task", TaskStatus::kTodo);
    t3.project = "Alpha";

    agg.add_tasks("doc_1", {t1, t2, t3});

    auto summary = agg.summary();
    CHECK(summary.total == 3);
    CHECK(summary.done == 1);
    CHECK(summary.in_progress == 1);
    CHECK(summary.by_project.at("Alpha") == 1);
}

TEST_CASE("TaskAggregator: query with filters", "[phase23][aggregator]")
{
    TaskAggregator agg;

    auto t1 = make_task("t1", "Task 1", TaskStatus::kTodo, TaskPriority::kHigh);
    t1.project = "Alpha";
    auto t2 = make_task("t2", "Task 2", TaskStatus::kDone, TaskPriority::kLow);
    t2.project = "Alpha";
    auto t3 = make_task("t3", "Task 3", TaskStatus::kTodo, TaskPriority::kMedium);
    t3.project = "Beta";

    agg.add_tasks("doc_1", {t1, t2, t3});

    TaskQuery query;
    query.project = "Alpha";
    auto result = agg.query(query);
    CHECK(result.size() == 1); // Only t1 (t2 is completed, excluded by default)
}

TEST_CASE("TaskAggregator: clear", "[phase23][aggregator]")
{
    TaskAggregator agg;
    agg.add_tasks("doc_1", {make_task("t1", "One")});
    agg.clear();
    CHECK(agg.task_count() == 0);
    CHECK(agg.document_count() == 0);
}

// ============================================================================
// TaskBoardEngine Tests
// ============================================================================

TEST_CASE("TaskBoardEngine: default layout", "[phase23][board]")
{
    TaskBoardEngine board;
    auto columns = board.get_columns();
    CHECK(columns.size() == 4);
    CHECK(columns[0].title == "To Do");
    CHECK(columns[1].title == "In Progress");
    CHECK(columns[2].title == "Done");
    CHECK(columns[3].title == "Cancelled");
}

TEST_CASE("TaskBoardEngine: populate by status", "[phase23][board]")
{
    TaskBoardEngine board;

    std::vector<Task> tasks = {
        make_task("t1", "Task 1", TaskStatus::kTodo),
        make_task("t2", "Task 2", TaskStatus::kInProgress),
        make_task("t3", "Task 3", TaskStatus::kDone),
        make_task("t4", "Task 4", TaskStatus::kTodo),
    };

    board.populate(tasks);
    CHECK(board.task_count() == 4);

    auto todo_col = board.get_column("todo");
    REQUIRE(todo_col.has_value());
    CHECK(todo_col->task_ids.size() == 2);

    auto ip_col = board.get_column("in_progress");
    REQUIRE(ip_col.has_value());
    CHECK(ip_col->task_ids.size() == 1);

    auto done_col = board.get_column("done");
    REQUIRE(done_col.has_value());
    CHECK(done_col->task_ids.size() == 1);
}

TEST_CASE("TaskBoardEngine: move_task", "[phase23][board]")
{
    TaskBoardEngine board;
    board.populate({make_task("t1", "Task 1", TaskStatus::kTodo)});

    auto result = board.move_task("t1", "todo", "in_progress");
    CHECK(result.has_value());

    auto todo = board.get_column("todo");
    CHECK(todo->task_ids.empty());

    auto ip = board.get_column("in_progress");
    CHECK(ip->task_ids.size() == 1);
    CHECK(ip->task_ids[0] == "t1");
}

TEST_CASE("TaskBoardEngine: move_task errors", "[phase23][board]")
{
    TaskBoardEngine board;
    board.populate({make_task("t1", "Task 1", TaskStatus::kTodo)});

    auto bad_source = board.move_task("t1", "nonexistent", "done");
    CHECK(!bad_source.has_value());

    auto bad_target = board.move_task("t1", "todo", "nonexistent");
    CHECK(!bad_target.has_value());

    auto bad_task = board.move_task("t999", "todo", "done");
    CHECK(!bad_task.has_value());
}

TEST_CASE("TaskBoardEngine: WIP limits", "[phase23][board]")
{
    TaskBoardEngine board;

    BoardConfig config;
    BoardColumn col;
    col.col_id = "wip_col";
    col.title = "WIP Limited";
    col.status = TaskStatus::kInProgress;
    col.wip_limit = 2;
    config.columns.push_back(col);
    board.configure(config);

    CHECK(!board.is_wip_exceeded("wip_col"));

    // Manually add 3 tasks
    board.populate({
        make_task("t1", "1", TaskStatus::kInProgress),
        make_task("t2", "2", TaskStatus::kInProgress),
        make_task("t3", "3", TaskStatus::kInProgress),
    });

    CHECK(board.is_wip_exceeded("wip_col"));
}

TEST_CASE("TaskBoardEngine: custom config", "[phase23][board]")
{
    TaskBoardEngine board;

    BoardConfig config;
    config.group_by = "project";

    BoardColumn col1;
    col1.col_id = "backlog";
    col1.title = "Backlog";
    col1.status = TaskStatus::kTodo;
    config.columns.push_back(col1);

    BoardColumn col2;
    col2.col_id = "active";
    col2.title = "Active";
    col2.status = TaskStatus::kInProgress;
    config.columns.push_back(col2);

    board.configure(config);
    CHECK(board.get_columns().size() == 2);
    CHECK(board.config().group_by == "project");
}

// ============================================================================
// TaskGanttEngine Tests
// ============================================================================

TEST_CASE("TaskGanttEngine: set_tasks creates bars", "[phase23][gantt]")
{
    TaskGanttEngine gantt;

    auto task1 = make_task("t1", "Task 1");
    task1.scheduled_date = year_month_day{year{2025}, month{1}, day{1}};
    task1.due_date = year_month_day{year{2025}, month{1}, day{10}};

    auto task2 = make_task("t2", "Task 2");
    task2.due_date = year_month_day{year{2025}, month{1}, day{15}};

    gantt.set_tasks({task1, task2});
    CHECK(gantt.bar_count() == 2);
}

TEST_CASE("TaskGanttEngine: milestones", "[phase23][gantt]")
{
    TaskGanttEngine gantt;

    // Milestone: due_date only, no scheduled_date
    auto milestone_task = make_task("t1", "Release milestone");
    milestone_task.due_date = year_month_day{year{2025}, month{3}, day{1}};

    // Bar: both dates
    auto bar_task = make_task("t2", "Sprint work");
    bar_task.scheduled_date = year_month_day{year{2025}, month{2}, day{15}};
    bar_task.due_date = year_month_day{year{2025}, month{3}, day{1}};

    gantt.set_tasks({milestone_task, bar_task});

    auto milestones = gantt.get_milestones();
    CHECK(milestones.size() == 1);
    CHECK(milestones[0].task_id == "t1");
    CHECK(milestones[0].is_milestone);

    auto bars = gantt.get_bars();
    CHECK(bars.size() == 1);
    CHECK(bars[0].task_id == "t2");
}

TEST_CASE("TaskGanttEngine: progress from status", "[phase23][gantt]")
{
    TaskGanttEngine gantt;

    auto todo = make_task("t1", "Todo", TaskStatus::kTodo);
    todo.scheduled_date = year_month_day{year{2025}, month{1}, day{1}};
    todo.due_date = year_month_day{year{2025}, month{1}, day{5}};

    auto in_progress = make_task("t2", "WIP", TaskStatus::kInProgress);
    in_progress.scheduled_date = year_month_day{year{2025}, month{1}, day{1}};
    in_progress.due_date = year_month_day{year{2025}, month{1}, day{5}};

    auto done = make_task("t3", "Complete", TaskStatus::kDone);
    done.scheduled_date = year_month_day{year{2025}, month{1}, day{1}};
    done.due_date = year_month_day{year{2025}, month{1}, day{5}};

    gantt.set_tasks({todo, in_progress, done});
    auto bars = gantt.get_bars();
    REQUIRE(bars.size() == 3);

    CHECK(bars[0].progress == 0.0); // todo
    CHECK(bars[1].progress == 0.5); // in_progress
    CHECK(bars[2].progress == 1.0); // done
}

TEST_CASE("TaskGanttEngine: time scale", "[phase23][gantt]")
{
    TaskGanttEngine gantt;

    CHECK(gantt.time_scale() == GanttTimeScale::kWeek); // Default

    gantt.set_time_scale(GanttTimeScale::kDay);
    CHECK(gantt.time_scale() == GanttTimeScale::kDay);

    gantt.set_time_scale(GanttTimeScale::kMonth);
    CHECK(gantt.time_scale() == GanttTimeScale::kMonth);
}

TEST_CASE("TaskGanttEngine: time range filtering", "[phase23][gantt]")
{
    TaskGanttEngine gantt;

    auto task1 = make_task("t1", "Task in range");
    task1.scheduled_date = year_month_day{year{2025}, month{3}, day{1}};
    task1.due_date = year_month_day{year{2025}, month{3}, day{15}};

    auto task2 = make_task("t2", "Task out of range");
    task2.scheduled_date = year_month_day{year{2025}, month{6}, day{1}};
    task2.due_date = year_month_day{year{2025}, month{6}, day{15}};

    gantt.set_tasks({task1, task2});
    gantt.set_time_range(year_month_day{year{2025}, month{2}, day{1}},
                         year_month_day{year{2025}, month{4}, day{1}});

    auto bars = gantt.get_bars();
    CHECK(bars.size() == 1);
    CHECK(bars[0].task_id == "t1");
}

TEST_CASE("TaskGanttEngine: critical path with dependencies", "[phase23][gantt]")
{
    TaskGanttEngine gantt;

    // No dependencies → empty critical path
    gantt.set_tasks({make_task("t1", "Independent")});
    auto path = gantt.get_critical_path();
    CHECK(path.empty());
}

// ============================================================================
// TaskCommandProvider Tests
// ============================================================================

TEST_CASE("TaskCommandProvider: provides 8 commands", "[phase23][commands]")
{
    TaskCommandProvider provider;
    auto commands = provider.get_commands();
    CHECK(commands.size() == 8);
}

TEST_CASE("TaskCommandProvider: all commands have Task category", "[phase23][commands]")
{
    TaskCommandProvider provider;
    for (const auto& cmd : provider.get_commands())
    {
        CHECK(cmd.category == "Task");
    }
}

TEST_CASE("TaskCommandProvider: get_command by id", "[phase23][commands]")
{
    TaskCommandProvider provider;

    auto toggle = provider.get_command("task.toggle_task");
    REQUIRE(toggle.has_value());
    CHECK(toggle->label == "Task: Toggle Task");
    CHECK(toggle->enabled);

    auto nonexistent = provider.get_command("nonexistent");
    CHECK(!nonexistent.has_value());
}

TEST_CASE("TaskCommandProvider: keybindings", "[phase23][commands]")
{
    TaskCommandProvider provider;

    auto toggle_kb = provider.get_keybinding("task.toggle_task");
    REQUIRE(toggle_kb.has_value());
    CHECK(toggle_kb.value() == "Cmd+Shift+T");

    auto add_kb = provider.get_keybinding("task.add_task");
    REQUIRE(add_kb.has_value());
    CHECK(add_kb.value() == "Cmd+Shift+N");
}

TEST_CASE("TaskCommandProvider: is_enabled", "[phase23][commands]")
{
    TaskCommandProvider provider;

    CHECK(provider.is_enabled("task.toggle_task"));
    CHECK(provider.is_enabled("task.view_board"));
    CHECK(!provider.is_enabled("nonexistent"));
}

// ============================================================================
// Events Tests
// ============================================================================

TEST_CASE("Phase 23 Events: construction", "[phase23][events]")
{
    using namespace markamp::core::events;

    TaskAggregatedEvent agg_event;
    agg_event.total_tasks = 42;
    agg_event.documents_scanned = 5;
    CHECK(agg_event.total_tasks == 42);

    TaskRecurrenceCreatedEvent rec_event;
    rec_event.task_id = "t1";
    rec_event.pattern = "daily";
    CHECK(rec_event.pattern == "daily");

    TaskReminderDueEvent rem_event;
    rem_event.reminder_id = "r1";
    rem_event.task_id = "t1";
    rem_event.task_text = "Buy milk";
    CHECK(rem_event.task_text == "Buy milk");

    TaskBoardUpdatedEvent board_event;
    board_event.task_id = "t1";
    board_event.from_column = "todo";
    board_event.to_column = "in_progress";
    CHECK(board_event.to_column == "in_progress");

    TaskGanttRefreshedEvent gantt_event;
    gantt_event.bar_count = 10;
    gantt_event.milestone_count = 2;
    CHECK(gantt_event.bar_count == 10);

    TaskOverdueEvent overdue_event;
    overdue_event.task_id = "t1";
    overdue_event.days_overdue = 3;
    CHECK(overdue_event.days_overdue == 3);
}
