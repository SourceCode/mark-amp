// test_command_execution_log.cpp — 10 tests for CommandExecutionLog
#include "core/CommandExecutionLog.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("CommandExecutionLog starts empty", "[execution_log]")
{
    CommandExecutionLog log;
    CHECK(log.entry_count() == 0);
}

TEST_CASE("CommandExecutionLog record increases count", "[execution_log]")
{
    CommandExecutionLog log;
    log.record(
        {.command_id = "file.save", .source = "shortcut", .success = true, .duration_ms = 5});
    CHECK(log.entry_count() == 1);
}

TEST_CASE("CommandExecutionLog recent returns latest entries", "[execution_log]")
{
    CommandExecutionLog log;
    log.record({.command_id = "cmd1", .source = "palette"});
    log.record({.command_id = "cmd2", .source = "menu"});
    log.record({.command_id = "cmd3", .source = "shortcut"});
    auto recent = log.recent(2);
    CHECK(recent.size() == 2);
}

TEST_CASE("CommandExecutionLog filter_by_command works", "[execution_log]")
{
    CommandExecutionLog log;
    log.record({.command_id = "file.save", .source = "shortcut"});
    log.record({.command_id = "edit.undo", .source = "shortcut"});
    log.record({.command_id = "file.save", .source = "menu"});
    auto saves = log.filter_by_command("file.save");
    CHECK(saves.size() == 2);
}

TEST_CASE("CommandExecutionLog filter_by_source works", "[execution_log]")
{
    CommandExecutionLog log;
    log.record({.command_id = "a", .source = "shortcut"});
    log.record({.command_id = "b", .source = "palette"});
    log.record({.command_id = "c", .source = "shortcut"});
    auto shortcuts = log.filter_by_source("shortcut");
    CHECK(shortcuts.size() == 2);
}

TEST_CASE("CommandExecutionLog failures returns only failures", "[execution_log]")
{
    CommandExecutionLog log;
    log.record({.command_id = "ok", .success = true});
    log.record({.command_id = "fail", .success = false, .error_message = "timeout"});
    auto fails = log.failures();
    CHECK(fails.size() == 1);
    CHECK(fails[0].command_id == "fail");
}

TEST_CASE("CommandExecutionLog stats_for computes statistics", "[execution_log]")
{
    CommandExecutionLog log;
    log.record({.command_id = "cmd", .success = true, .duration_ms = 10});
    log.record({.command_id = "cmd", .success = true, .duration_ms = 20});
    log.record({.command_id = "cmd", .success = false, .duration_ms = 5});
    auto stats = log.stats_for("cmd");
    CHECK(stats.total_runs == 3);
    CHECK(stats.success_count == 2);
    CHECK(stats.failure_count == 1);
}

TEST_CASE("CommandExecutionLog export_json returns valid JSON", "[execution_log]")
{
    CommandExecutionLog log;
    log.record({.command_id = "test", .source = "unit", .success = true});
    auto json = log.export_json();
    CHECK_FALSE(json.empty());
    CHECK(json.find("test") != std::string::npos);
}

TEST_CASE("CommandExecutionLog clear removes all", "[execution_log]")
{
    CommandExecutionLog log;
    log.record({.command_id = "a"});
    log.record({.command_id = "b"});
    log.clear();
    CHECK(log.entry_count() == 0);
}

TEST_CASE("CommandExecutionLog all_stats returns per-command", "[execution_log]")
{
    CommandExecutionLog log;
    log.record({.command_id = "a", .success = true});
    log.record({.command_id = "b", .success = true});
    auto all = log.all_stats();
    CHECK(all.size() == 2);
}
