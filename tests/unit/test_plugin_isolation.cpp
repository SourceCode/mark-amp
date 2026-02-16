/// test_plugin_isolation.cpp — Phase 24: Plugin isolation tests
///
/// Validates PluginSafeCall (exception boundary, timeout detection)
/// and PluginMemoryTracker (per-plugin budget tracking).

#include "core/PluginMemoryTracker.h"
#include "core/PluginSafeCall.h"

#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <string>

using namespace markamp::core;

// ══════════════════════════════════════════
// PluginSafeCall
// ══════════════════════════════════════════

TEST_CASE("plugin_safe_call: returns value on success", "[plugin_isolation]")
{
    auto result = plugin_safe_call<int>(
        "test-plugin", []() { return 42; }, 1000, -1);

    REQUIRE(result.ok());
    REQUIRE(result.value == 42);
    REQUIRE(result.plugin_id == "test-plugin");
    REQUIRE(result.duration_us >= 0);
}

TEST_CASE("plugin_safe_call: returns fallback on exception", "[plugin_isolation]")
{
    auto result = plugin_safe_call<std::string>(
        "bad-plugin",
        []() -> std::string { throw std::runtime_error("plugin crash"); },
        1000,
        "error");

    REQUIRE_FALSE(result.ok());
    REQUIRE(result.value == "error");
    REQUIRE(result.status == PluginCallStatus::Exception);
    REQUIRE(result.error_message.find("plugin crash") != std::string::npos);
    REQUIRE(result.plugin_id == "bad-plugin");
}

TEST_CASE("plugin_safe_call: captures logic_error", "[plugin_isolation]")
{
    auto result = plugin_safe_call<int>(
        "buggy-plugin", []() -> int { throw std::logic_error("bad logic"); }, 1000, -1);

    REQUIRE(result.status == PluginCallStatus::Exception);
    REQUIRE(result.error_message.find("bad logic") != std::string::npos);
}

TEST_CASE("plugin_safe_call_void: success", "[plugin_isolation]")
{
    int side_effect = 0;
    auto result = plugin_safe_call_void(
        "void-plugin", [&]() { side_effect = 99; }, 1000);

    REQUIRE(result.ok());
    REQUIRE(side_effect == 99);
}

TEST_CASE("plugin_safe_call_void: exception captured", "[plugin_isolation]")
{
    auto result = plugin_safe_call_void(
        "crash-plugin", []() { throw std::runtime_error("void crash"); }, 1000);

    REQUIRE_FALSE(result.ok());
    REQUIRE(result.error_message.find("void crash") != std::string::npos);
}

TEST_CASE("plugin_safe_call: zero timeout disables timeout check", "[plugin_isolation]")
{
    auto result = plugin_safe_call<int>(
        "fast-plugin", []() { return 1; }, 0, -1);
    REQUIRE(result.ok());
}

// ══════════════════════════════════════════
// PluginMemoryTracker
// ══════════════════════════════════════════

TEST_CASE("PluginMemoryTracker: register and report allocation", "[plugin_isolation]")
{
    PluginMemoryTracker tracker;
    tracker.register_plugin("my-plugin", 1024);

    tracker.report_allocation("my-plugin", 512);

    REQUIRE(tracker.current_usage("my-plugin") == 512);
    REQUIRE_FALSE(tracker.is_over_budget("my-plugin"));
}

TEST_CASE("PluginMemoryTracker: over-budget detection", "[plugin_isolation]")
{
    PluginMemoryTracker tracker;
    tracker.register_plugin("big-plugin", 100);

    tracker.report_allocation("big-plugin", 150);

    REQUIRE(tracker.is_over_budget("big-plugin"));
}

TEST_CASE("PluginMemoryTracker: deallocation reduces usage", "[plugin_isolation]")
{
    PluginMemoryTracker tracker;
    tracker.register_plugin("clean-plugin", 1024);

    tracker.report_allocation("clean-plugin", 800);
    tracker.report_deallocation("clean-plugin", 300);

    REQUIRE(tracker.current_usage("clean-plugin") == 500);
}

TEST_CASE("PluginMemoryTracker: deallocation doesn't go below zero", "[plugin_isolation]")
{
    PluginMemoryTracker tracker;
    tracker.register_plugin("edge-plugin", 1024);

    tracker.report_allocation("edge-plugin", 100);
    tracker.report_deallocation("edge-plugin", 200); // more than allocated

    REQUIRE(tracker.current_usage("edge-plugin") == 0);
}

TEST_CASE("PluginMemoryTracker: multiple plugins independent", "[plugin_isolation]")
{
    PluginMemoryTracker tracker;
    tracker.register_plugin("alpha", 1000);
    tracker.register_plugin("beta", 500);

    tracker.report_allocation("alpha", 900);
    tracker.report_allocation("beta", 100);

    REQUIRE_FALSE(tracker.is_over_budget("alpha"));
    REQUIRE_FALSE(tracker.is_over_budget("beta"));
    REQUIRE(tracker.plugin_count() == 2);
}

TEST_CASE("PluginMemoryTracker: snapshot returns all plugins", "[plugin_isolation]")
{
    PluginMemoryTracker tracker;
    tracker.register_plugin("one", 1000);
    tracker.register_plugin("two", 2000);
    tracker.report_allocation("one", 500);

    auto snap = tracker.snapshot();
    REQUIRE(snap.size() == 2);
}

TEST_CASE("PluginMemoryTracker: reset clears usage", "[plugin_isolation]")
{
    PluginMemoryTracker tracker;
    tracker.register_plugin("resetme", 1024);
    tracker.report_allocation("resetme", 512);

    tracker.reset();

    REQUIRE(tracker.current_usage("resetme") == 0);
}
