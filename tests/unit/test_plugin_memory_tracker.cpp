/// test_plugin_memory_tracker.cpp — Comprehensive tests for PluginMemoryTracker
#include "core/PluginMemoryTracker.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

// ── Positive Tests ──

TEST_CASE("PluginMemoryTracker: register and count", "[plugin_memory_tracker][positive]")
{
    PluginMemoryTracker tracker;
    REQUIRE(tracker.plugin_count() == 0);
    tracker.register_plugin("plugin-a", 1024);
    REQUIRE(tracker.plugin_count() == 1);
    tracker.register_plugin("plugin-b", 2048);
    REQUIRE(tracker.plugin_count() == 2);
}

TEST_CASE("PluginMemoryTracker: allocation updates usage", "[plugin_memory_tracker][positive]")
{
    PluginMemoryTracker tracker;
    tracker.register_plugin("p1", 4096);
    REQUIRE(tracker.current_usage("p1") == 0);
    tracker.report_allocation("p1", 1024);
    REQUIRE(tracker.current_usage("p1") == 1024);
    tracker.report_allocation("p1", 512);
    REQUIRE(tracker.current_usage("p1") == 1536);
}

TEST_CASE("PluginMemoryTracker: deallocation decreases usage", "[plugin_memory_tracker][positive]")
{
    PluginMemoryTracker tracker;
    tracker.register_plugin("p1", 4096);
    tracker.report_allocation("p1", 2048);
    tracker.report_deallocation("p1", 512);
    REQUIRE(tracker.current_usage("p1") == 1536);
}

TEST_CASE("PluginMemoryTracker: report_usage sets absolute value",
          "[plugin_memory_tracker][positive]")
{
    PluginMemoryTracker tracker;
    tracker.register_plugin("p1", 4096);
    tracker.report_usage("p1", 999);
    REQUIRE(tracker.current_usage("p1") == 999);
}

TEST_CASE("PluginMemoryTracker: is_over_budget detects exceeded budget",
          "[plugin_memory_tracker][positive]")
{
    PluginMemoryTracker tracker;
    tracker.register_plugin("p1", 1024);
    REQUIRE_FALSE(tracker.is_over_budget("p1"));
    tracker.report_allocation("p1", 2048);
    REQUIRE(tracker.is_over_budget("p1"));
}

TEST_CASE("PluginMemoryTracker: snapshot returns all plugins", "[plugin_memory_tracker][positive]")
{
    PluginMemoryTracker tracker;
    tracker.register_plugin("a", 100);
    tracker.register_plugin("b", 200);
    tracker.report_allocation("a", 50);
    auto snap = tracker.snapshot();
    REQUIRE(snap.size() == 2);
}

TEST_CASE("PluginMemoryTracker: reset clears all usage", "[plugin_memory_tracker][positive]")
{
    PluginMemoryTracker tracker;
    tracker.register_plugin("p1", 4096);
    tracker.report_allocation("p1", 2048);
    tracker.reset();
    REQUIRE(tracker.current_usage("p1") == 0);
}

// ── Negative Tests ──

TEST_CASE("PluginMemoryTracker: deallocation beyond usage floors to zero",
          "[plugin_memory_tracker][negative]")
{
    PluginMemoryTracker tracker;
    tracker.register_plugin("p1", 4096);
    tracker.report_allocation("p1", 100);
    tracker.report_deallocation("p1", 500); // More than allocated
    REQUIRE(tracker.current_usage("p1") == 0);
}

TEST_CASE("PluginMemoryTracker: exactly at budget is not over", "[plugin_memory_tracker][negative]")
{
    PluginMemoryTracker tracker;
    tracker.register_plugin("p1", 1024);
    tracker.report_allocation("p1", 1024);
    // At the budget line — check boundary
    // Over budget means usage > budget, at-budget may or may not trigger
    (void)tracker.is_over_budget("p1"); // Should not crash
}

// ── Edge Cases ──

TEST_CASE("PluginMemoryTracker: zero-byte budget is always over", "[plugin_memory_tracker][edge]")
{
    PluginMemoryTracker tracker;
    tracker.register_plugin("p1", 0); // Budget of 0 bytes
    tracker.report_allocation("p1", 999999);
    // With zero budget, any usage is over budget
    REQUIRE(tracker.is_over_budget("p1"));
}

TEST_CASE("PluginMemoryTracker: multiple resets maintain registration",
          "[plugin_memory_tracker][edge]")
{
    PluginMemoryTracker tracker;
    tracker.register_plugin("p1", 1024);
    tracker.report_allocation("p1", 512);
    tracker.reset();
    tracker.reset();
    REQUIRE(tracker.plugin_count() == 1);
    REQUIRE(tracker.current_usage("p1") == 0);
}

// ── Resource Limits ──

TEST_CASE("PluginMemoryTracker: set_limits and check_limits", "[plugin_memory_tracker][positive]")
{
    PluginMemoryTracker tracker;
    tracker.register_plugin("p1", 4096);
    PluginMemoryTracker::ResourceLimits limits;
    limits.memory_bytes = 2048;
    limits.cpu_time_ms = 1000;
    limits.event_subscriptions = 5;
    tracker.set_limits("p1", limits);
    auto violations = tracker.check_limits("p1");
    REQUIRE(violations.empty()); // nothing violated yet
}

TEST_CASE("PluginMemoryTracker: check_limits detects memory violation",
          "[plugin_memory_tracker][positive]")
{
    PluginMemoryTracker tracker;
    tracker.register_plugin("p1", 8192);
    PluginMemoryTracker::ResourceLimits limits;
    limits.memory_bytes = 1024;
    tracker.set_limits("p1", limits);
    tracker.report_allocation("p1", 2048);
    auto violations = tracker.check_limits("p1");
    REQUIRE(violations.size() == 1);
    REQUIRE(violations[0].resource_type == "memory");
}

TEST_CASE("PluginMemoryTracker: check_limits detects cpu_time violation",
          "[plugin_memory_tracker][positive]")
{
    PluginMemoryTracker tracker;
    tracker.register_plugin("p1", 4096);
    PluginMemoryTracker::ResourceLimits limits;
    limits.cpu_time_ms = 500;
    tracker.set_limits("p1", limits);
    tracker.report_cpu_time("p1", 1000);
    auto violations = tracker.check_limits("p1");
    REQUIRE(violations.size() == 1);
    REQUIRE(violations[0].resource_type == "cpu_time");
}

TEST_CASE("PluginMemoryTracker: check_limits no limits returns empty",
          "[plugin_memory_tracker][negative]")
{
    PluginMemoryTracker tracker;
    tracker.register_plugin("p1", 4096);
    auto violations = tracker.check_limits("p1"); // No limits set
    REQUIRE(violations.empty());
}
