/// @file test_phase40_automation_hooks.cpp
/// @brief Comprehensive tests for Phase 40 — Workspace Automation & Hooks.

#include "core/AutomationCommandProvider.h"
#include "core/AutomationRule.h"
#include "core/CommandRegistry.h"
#include "core/Events.h"
#include "core/FileWatcherService.h"
#include "core/HooksCommandProvider.h"
#include "core/TaskScheduler.h"
#include "core/WorkspaceHookManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::core::events;

// ═══════════════════════════════════════════════════════════════════
// WorkspaceHookManager Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("WorkspaceHookManager: register and find", "[phase40][hooks]")
{
    WorkspaceHookManager mgr;
    HookRegistration reg;
    reg.hook_id = "h1";
    reg.description = "Test hook";
    reg.hook_type = WorkspaceHookType::kOnSave;
    reg.callback = [](const HookContext&) {};
    mgr.register_hook(std::move(reg));

    REQUIRE(mgr.hook_count() == 1);
    const auto* found = mgr.find_hook("h1");
    REQUIRE(found != nullptr);
    REQUIRE(found->description == "Test hook");
}

TEST_CASE("WorkspaceHookManager: unregister", "[phase40][hooks]")
{
    WorkspaceHookManager mgr;
    HookRegistration reg;
    reg.hook_id = "h1";
    mgr.register_hook(std::move(reg));

    REQUIRE(mgr.unregister_hook("h1") == true);
    REQUIRE(mgr.hook_count() == 0);
    REQUIRE(mgr.unregister_hook("nonexistent") == false);
}

TEST_CASE("WorkspaceHookManager: enable/disable", "[phase40][hooks]")
{
    WorkspaceHookManager mgr;
    HookRegistration reg;
    reg.hook_id = "h1";
    mgr.register_hook(std::move(reg));

    REQUIRE(mgr.disable_hook("h1") == true);
    REQUIRE(mgr.find_hook("h1")->enabled == false);
    REQUIRE(mgr.enable_hook("h1") == true);
    REQUIRE(mgr.find_hook("h1")->enabled == true);
}

TEST_CASE("WorkspaceHookManager: fire hooks in priority order", "[phase40][hooks]")
{
    WorkspaceHookManager mgr;
    std::vector<std::string> order;

    HookRegistration low;
    low.hook_id = "low";
    low.hook_type = WorkspaceHookType::kOnSave;
    low.priority = 1;
    low.callback = [&](const HookContext&) { order.push_back("low"); };
    mgr.register_hook(std::move(low));

    HookRegistration high;
    high.hook_id = "high";
    high.hook_type = WorkspaceHookType::kOnSave;
    high.priority = 10;
    high.callback = [&](const HookContext&) { order.push_back("high"); };
    mgr.register_hook(std::move(high));

    HookContext ctx;
    ctx.hook_type = WorkspaceHookType::kOnSave;
    mgr.fire_hooks(WorkspaceHookType::kOnSave, ctx);

    REQUIRE(order.size() == 2);
    REQUIRE(order[0] == "high");
    REQUIRE(order[1] == "low");
}

TEST_CASE("WorkspaceHookManager: disabled hooks not fired", "[phase40][hooks]")
{
    WorkspaceHookManager mgr;
    int count = 0;

    HookRegistration reg;
    reg.hook_id = "h1";
    reg.hook_type = WorkspaceHookType::kOnOpen;
    reg.callback = [&](const HookContext&) { ++count; };
    mgr.register_hook(std::move(reg));
    mgr.disable_hook("h1");

    HookContext ctx;
    mgr.fire_hooks(WorkspaceHookType::kOnOpen, ctx);
    REQUIRE(count == 0);
}

TEST_CASE("WorkspaceHookManager: hooks_for_type", "[phase40][hooks]")
{
    WorkspaceHookManager mgr;
    HookRegistration reg1;
    reg1.hook_id = "h1";
    reg1.hook_type = WorkspaceHookType::kOnSave;
    mgr.register_hook(std::move(reg1));

    HookRegistration reg2;
    reg2.hook_id = "h2";
    reg2.hook_type = WorkspaceHookType::kOnOpen;
    mgr.register_hook(std::move(reg2));

    auto saves = mgr.hooks_for_type(WorkspaceHookType::kOnSave);
    REQUIRE(saves.size() == 1);
    REQUIRE(saves[0]->hook_id == "h1");
}

TEST_CASE("WorkspaceHookManager: registered_types", "[phase40][hooks]")
{
    WorkspaceHookManager mgr;
    HookRegistration reg1;
    reg1.hook_id = "h1";
    reg1.hook_type = WorkspaceHookType::kOnSave;
    mgr.register_hook(std::move(reg1));

    HookRegistration reg2;
    reg2.hook_id = "h2";
    reg2.hook_type = WorkspaceHookType::kOnOpen;
    mgr.register_hook(std::move(reg2));

    auto types = mgr.registered_types();
    REQUIRE(types.size() == 2);
}

TEST_CASE("WorkspaceHookManager: hook_type_name", "[phase40][hooks]")
{
    REQUIRE(hook_type_name(WorkspaceHookType::kOnOpen) == "on_open");
    REQUIRE(hook_type_name(WorkspaceHookType::kOnSave) == "on_save");
    REQUIRE(hook_type_name(WorkspaceHookType::kOnDelete) == "on_delete");
    REQUIRE(hook_type_name(WorkspaceHookType::kOnShutdown) == "on_shutdown");
}

TEST_CASE("WorkspaceHookManager: clear_all", "[phase40][hooks]")
{
    WorkspaceHookManager mgr;
    HookRegistration reg;
    reg.hook_id = "h1";
    mgr.register_hook(std::move(reg));
    mgr.clear_all();
    REQUIRE(mgr.hook_count() == 0);
}

// ═══════════════════════════════════════════════════════════════════
// AutomationRuleManager Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("AutomationRuleManager: add and find", "[phase40][automation]")
{
    AutomationRuleManager mgr;
    AutomationRuleEntry rule;
    rule.rule_id = "r1";
    rule.name = "Refresh on Save";
    rule.trigger = AutomationTrigger::kOnFileSave;
    rule.action = AutomationAction::kRefreshIndex;
    mgr.add_rule(std::move(rule));

    REQUIRE(mgr.rule_count() == 1);
    const auto* found = mgr.find_rule("r1");
    REQUIRE(found != nullptr);
    REQUIRE(found->name == "Refresh on Save");
}

TEST_CASE("AutomationRuleManager: remove rule", "[phase40][automation]")
{
    AutomationRuleManager mgr;
    AutomationRuleEntry rule;
    rule.rule_id = "r1";
    mgr.add_rule(std::move(rule));

    REQUIRE(mgr.remove_rule("r1") == true);
    REQUIRE(mgr.rule_count() == 0);
    REQUIRE(mgr.remove_rule("nonexistent") == false);
}

TEST_CASE("AutomationRuleManager: enable/disable", "[phase40][automation]")
{
    AutomationRuleManager mgr;
    AutomationRuleEntry rule;
    rule.rule_id = "r1";
    mgr.add_rule(std::move(rule));

    REQUIRE(mgr.disable_rule("r1") == true);
    REQUIRE(mgr.find_rule("r1")->enabled == false);
    REQUIRE(mgr.enable_rule("r1") == true);
    REQUIRE(mgr.find_rule("r1")->enabled == true);
}

TEST_CASE("AutomationRuleManager: rules_for_trigger", "[phase40][automation]")
{
    AutomationRuleManager mgr;
    AutomationRuleEntry rule1;
    rule1.rule_id = "r1";
    rule1.trigger = AutomationTrigger::kOnFileSave;
    mgr.add_rule(std::move(rule1));

    AutomationRuleEntry rule2;
    rule2.rule_id = "r2";
    rule2.trigger = AutomationTrigger::kOnWorkspaceOpen;
    mgr.add_rule(std::move(rule2));

    auto saves = mgr.rules_for_trigger(AutomationTrigger::kOnFileSave);
    REQUIRE(saves.size() == 1);
    REQUIRE(saves[0]->rule_id == "r1");
}

TEST_CASE("AutomationRuleManager: matching rules with pattern", "[phase40][automation]")
{
    AutomationRuleManager mgr;
    AutomationRuleEntry rule;
    rule.rule_id = "r1";
    rule.trigger = AutomationTrigger::kOnFileSave;
    rule.file_pattern = "*.md";
    mgr.add_rule(std::move(rule));

    auto md_matches = mgr.matching_rules(AutomationTrigger::kOnFileSave, "/notes/todo.md");
    REQUIRE(md_matches.size() == 1);

    auto js_matches = mgr.matching_rules(AutomationTrigger::kOnFileSave, "/src/app.js");
    REQUIRE(js_matches.empty());
}

TEST_CASE("AutomationRuleManager: mark_executed", "[phase40][automation]")
{
    AutomationRuleManager mgr;
    AutomationRuleEntry rule;
    rule.rule_id = "r1";
    mgr.add_rule(std::move(rule));

    REQUIRE(mgr.mark_executed("r1") == true);
    REQUIRE(mgr.find_rule("r1")->execution_count == 1);
    REQUIRE(mgr.mark_executed("r1") == true);
    REQUIRE(mgr.find_rule("r1")->execution_count == 2);
}

TEST_CASE("AutomationRuleManager: export_json", "[phase40][automation]")
{
    AutomationRuleManager mgr;
    AutomationRuleEntry rule;
    rule.rule_id = "r1";
    rule.name = "Test Rule";
    rule.trigger = AutomationTrigger::kOnManual;
    rule.action = AutomationAction::kRunCommand;
    mgr.add_rule(std::move(rule));

    auto json = mgr.export_json();
    REQUIRE(json.find("automation_rules") != std::string::npos);
    REQUIRE(json.find("Test Rule") != std::string::npos);
}

TEST_CASE("AutomationRuleManager: load_defaults", "[phase40][automation]")
{
    AutomationRuleManager mgr;
    mgr.load_defaults();
    REQUIRE(mgr.rule_count() == 2);
}

TEST_CASE("AutomationRuleManager: trigger names", "[phase40][automation]")
{
    REQUIRE(automation_trigger_name(AutomationTrigger::kOnFileSave) == "on_file_save");
    REQUIRE(automation_trigger_name(AutomationTrigger::kOnManual) == "on_manual");
    REQUIRE(automation_trigger_name(AutomationTrigger::kOnTimer) == "on_timer");
}

TEST_CASE("AutomationRuleManager: action names", "[phase40][automation]")
{
    REQUIRE(automation_action_name(AutomationAction::kRunCommand) == "run_command");
    REQUIRE(automation_action_name(AutomationAction::kShowNotification) == "show_notification");
    REQUIRE(automation_action_name(AutomationAction::kRefreshIndex) == "refresh_index");
}

// ═══════════════════════════════════════════════════════════════════
// TaskScheduler Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("TaskScheduler: schedule and find", "[phase40][scheduler]")
{
    TaskScheduler sched;
    ScheduledTask task;
    task.task_id = "t1";
    task.name = "Index rebuild";
    task.execute_fn = []() { return true; };
    sched.schedule(std::move(task));

    REQUIRE(sched.task_count() == 1);
    const auto* found = sched.find_task("t1");
    REQUIRE(found != nullptr);
    REQUIRE(found->name == "Index rebuild");
}

TEST_CASE("TaskScheduler: cancel", "[phase40][scheduler]")
{
    TaskScheduler sched;
    ScheduledTask task;
    task.task_id = "t1";
    sched.schedule(std::move(task));

    REQUIRE(sched.cancel("t1") == true);
    REQUIRE(sched.find_task("t1")->state == ScheduledTaskState::kCancelled);
}

TEST_CASE("TaskScheduler: pause/resume", "[phase40][scheduler]")
{
    TaskScheduler sched;
    ScheduledTask task;
    task.task_id = "t1";
    sched.schedule(std::move(task));

    REQUIRE(sched.pause("t1") == true);
    REQUIRE(sched.find_task("t1")->state == ScheduledTaskState::kPaused);
    REQUIRE(sched.resume("t1") == true);
    REQUIRE(sched.find_task("t1")->state == ScheduledTaskState::kPending);
}

TEST_CASE("TaskScheduler: tick executes pending tasks", "[phase40][scheduler]")
{
    TaskScheduler sched;
    bool executed = false;
    ScheduledTask task;
    task.task_id = "t1";
    task.execute_fn = [&]()
    {
        executed = true;
        return true;
    };
    sched.schedule(std::move(task));

    int count = sched.tick();
    REQUIRE(count == 1);
    REQUIRE(executed == true);
}

TEST_CASE("TaskScheduler: execute_now", "[phase40][scheduler]")
{
    TaskScheduler sched;
    int count = 0;
    ScheduledTask task;
    task.task_id = "t1";
    task.execute_fn = [&]()
    {
        ++count;
        return true;
    };
    sched.schedule(std::move(task));

    REQUIRE(sched.execute_now("t1") == true);
    REQUIRE(count == 1);
}

TEST_CASE("TaskScheduler: repeating tasks", "[phase40][scheduler]")
{
    TaskScheduler sched;
    int count = 0;
    ScheduledTask task;
    task.task_id = "t1";
    task.repeat = true;
    task.interval_seconds = 0; // Immediately repeatable
    task.execute_fn = [&]()
    {
        ++count;
        return true;
    };
    sched.schedule(std::move(task));

    sched.tick();
    REQUIRE(count == 1);
    REQUIRE(sched.find_task("t1")->state == ScheduledTaskState::kPending);
}

TEST_CASE("TaskScheduler: max executions limit", "[phase40][scheduler]")
{
    TaskScheduler sched;
    int count = 0;
    ScheduledTask task;
    task.task_id = "t1";
    task.repeat = true;
    task.interval_seconds = 0;
    task.max_executions = 2;
    task.execute_fn = [&]()
    {
        ++count;
        return true;
    };
    sched.schedule(std::move(task));

    sched.tick();
    sched.tick();
    REQUIRE(count == 2);
    REQUIRE(sched.find_task("t1")->state == ScheduledTaskState::kCompleted);
}

TEST_CASE("TaskScheduler: failed task", "[phase40][scheduler]")
{
    TaskScheduler sched;
    ScheduledTask task;
    task.task_id = "t1";
    task.execute_fn = []() { return false; };
    sched.schedule(std::move(task));

    sched.tick();
    REQUIRE(sched.find_task("t1")->state == ScheduledTaskState::kFailed);
}

TEST_CASE("TaskScheduler: clear_completed", "[phase40][scheduler]")
{
    TaskScheduler sched;
    ScheduledTask task;
    task.task_id = "t1";
    task.execute_fn = []() { return true; };
    sched.schedule(std::move(task));
    sched.tick();

    sched.clear_completed();
    REQUIRE(sched.task_count() == 0);
}

// ═══════════════════════════════════════════════════════════════════
// FileWatcherService Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("FileWatcherService: add and find watch", "[phase40][file-watcher]")
{
    FileWatcherService svc;
    FileWatch watch;
    watch.watch_id = "w1";
    watch.directory = "/notes";
    watch.glob_pattern = "*.md";
    svc.add_watch(std::move(watch));

    REQUIRE(svc.watch_count() == 1);
    REQUIRE(svc.find_watch("w1") != nullptr);
    REQUIRE(svc.find_watch("w1")->directory == "/notes");
}

TEST_CASE("FileWatcherService: remove watch", "[phase40][file-watcher]")
{
    FileWatcherService svc;
    FileWatch watch;
    watch.watch_id = "w1";
    svc.add_watch(std::move(watch));

    REQUIRE(svc.remove_watch("w1") == true);
    REQUIRE(svc.watch_count() == 0);
}

TEST_CASE("FileWatcherService: notify_change dispatches to matching watchers",
          "[phase40][file-watcher]")
{
    FileWatcherService svc;
    int callback_count = 0;

    FileWatch watch;
    watch.watch_id = "w1";
    watch.directory = "/notes";
    watch.glob_pattern = "*.md";
    watch.callback = [&](const FileChangeEvent&) { ++callback_count; };
    svc.add_watch(std::move(watch));

    FileChangeEvent evt;
    evt.file_path = "/notes/todo.md";
    evt.change_type = FileChangeType::kModified;
    svc.notify_change(evt);

    REQUIRE(callback_count == 1);
    REQUIRE(svc.event_count() == 1);
}

TEST_CASE("FileWatcherService: glob mismatch does not fire", "[phase40][file-watcher]")
{
    FileWatcherService svc;
    int callback_count = 0;

    FileWatch watch;
    watch.watch_id = "w1";
    watch.directory = "/notes";
    watch.glob_pattern = "*.md";
    watch.callback = [&](const FileChangeEvent&) { ++callback_count; };
    svc.add_watch(std::move(watch));

    FileChangeEvent evt;
    evt.file_path = "/notes/image.png";
    svc.notify_change(evt);

    REQUIRE(callback_count == 0);
}

TEST_CASE("FileWatcherService: disabled watch not fired", "[phase40][file-watcher]")
{
    FileWatcherService svc;
    int callback_count = 0;

    FileWatch watch;
    watch.watch_id = "w1";
    watch.directory = "/notes";
    watch.callback = [&](const FileChangeEvent&) { ++callback_count; };
    svc.add_watch(std::move(watch));
    svc.disable_watch("w1");

    FileChangeEvent evt;
    evt.file_path = "/notes/todo.md";
    svc.notify_change(evt);
    REQUIRE(callback_count == 0);
}

TEST_CASE("FileWatcherService: enable/disable all", "[phase40][file-watcher]")
{
    FileWatcherService svc;
    FileWatch w1;
    w1.watch_id = "w1";
    svc.add_watch(std::move(w1));

    FileWatch w2;
    w2.watch_id = "w2";
    svc.add_watch(std::move(w2));

    svc.disable_all();
    REQUIRE(svc.find_watch("w1")->enabled == false);
    REQUIRE(svc.find_watch("w2")->enabled == false);

    svc.enable_all();
    REQUIRE(svc.find_watch("w1")->enabled == true);
    REQUIRE(svc.find_watch("w2")->enabled == true);
}

TEST_CASE("FileWatcherService: watches_for_directory", "[phase40][file-watcher]")
{
    FileWatcherService svc;
    FileWatch w1;
    w1.watch_id = "w1";
    w1.directory = "/notes";
    svc.add_watch(std::move(w1));

    FileWatch w2;
    w2.watch_id = "w2";
    w2.directory = "/src";
    svc.add_watch(std::move(w2));

    auto notes = svc.watches_for_directory("/notes");
    REQUIRE(notes.size() == 1);
    REQUIRE(notes[0]->watch_id == "w1");
}

// ═══════════════════════════════════════════════════════════════════
// AutomationCommandProvider Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("AutomationCommandProvider: command count", "[phase40][automation-commands]")
{
    REQUIRE(AutomationCommandProvider::command_count() == 8);
}

TEST_CASE("AutomationCommandProvider: command IDs", "[phase40][automation-commands]")
{
    auto ids = AutomationCommandProvider::command_ids();
    REQUIRE(ids.size() == 8);
    REQUIRE(ids[0] == "automation.showRules");
    REQUIRE(ids[7] == "automation.loadDefaults");
}

TEST_CASE("AutomationCommandProvider: register to registry", "[phase40][automation-commands]")
{
    CommandRegistry registry;
    AutomationCommandProvider provider;
    provider.register_commands(registry);

    const auto* cmd = registry.get_command("automation.showRules");
    REQUIRE(cmd != nullptr);
    REQUIRE(cmd->category == "Automation");
}

// ═══════════════════════════════════════════════════════════════════
// HooksCommandProvider Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("HooksCommandProvider: command count", "[phase40][hooks-commands]")
{
    REQUIRE(HooksCommandProvider::command_count() == 8);
}

TEST_CASE("HooksCommandProvider: command IDs", "[phase40][hooks-commands]")
{
    auto ids = HooksCommandProvider::command_ids();
    REQUIRE(ids.size() == 8);
    REQUIRE(ids[0] == "hooks.showAll");
    REQUIRE(ids[7] == "hooks.showScheduledTasks");
}

TEST_CASE("HooksCommandProvider: register to registry", "[phase40][hooks-commands]")
{
    CommandRegistry registry;
    HooksCommandProvider provider;
    provider.register_commands(registry);

    const auto* cmd = registry.get_command("hooks.showAll");
    REQUIRE(cmd != nullptr);
    REQUIRE(cmd->category == "Hooks");
}

// ═══════════════════════════════════════════════════════════════════
// Phase 40 Events Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("Phase 40 Events: WorkspaceHookFiredEvent", "[phase40][events]")
{
    WorkspaceHookFiredEvent event;
    event.hook_id = "h1";
    event.hook_type = "on_save";
    event.workspace_path = "/notes";
    REQUIRE(event.hook_type == "on_save");
}

TEST_CASE("Phase 40 Events: AutomationRuleExecutedEvent", "[phase40][events]")
{
    AutomationRuleExecutedEvent event;
    event.rule_id = "r1";
    event.rule_name = "Refresh Index";
    event.trigger = "on_file_save";
    REQUIRE(event.trigger == "on_file_save");
}

TEST_CASE("Phase 40 Events: ScheduledTaskCompletedEvent", "[phase40][events]")
{
    ScheduledTaskCompletedEvent event;
    event.task_id = "t1";
    event.task_name = "Index rebuild";
    event.success = true;
    REQUIRE(event.success == true);
}

TEST_CASE("Phase 40 Events: FileWatcherNotificationEvent", "[phase40][events]")
{
    FileWatcherNotificationEvent event;
    event.watch_id = "w1";
    event.file_path = "/notes/todo.md";
    event.change_type = "modified";
    REQUIRE(event.change_type == "modified");
}

TEST_CASE("Phase 40 Events: HookRegisteredEvent", "[phase40][events]")
{
    HookRegisteredEvent event;
    event.hook_id = "h1";
    event.hook_type = "on_open";
    event.description = "Welcome hook";
    REQUIRE(event.description == "Welcome hook");
}

TEST_CASE("Phase 40 Events: AutomationRuleChangedEvent", "[phase40][events]")
{
    AutomationRuleChangedEvent event;
    event.rule_id = "r1";
    event.action = "added";
    REQUIRE(event.action == "added");
}
