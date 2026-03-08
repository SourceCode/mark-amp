/// test_extension_resource_tracker.cpp — Comprehensive tests for ExtensionResourceTracker
#include "core/ExtensionResourceTracker.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

// ── Positive Tests ──

TEST_CASE("ExtensionResourceTracker: register and count", "[extension_resource_tracker][positive]")
{
    ExtensionResourceTracker tracker;
    REQUIRE(tracker.extension_count() == 0);
    tracker.register_extension("ext.a");
    REQUIRE(tracker.extension_count() == 1);
    tracker.register_extension("ext.b");
    REQUIRE(tracker.extension_count() == 2);
}

TEST_CASE("ExtensionResourceTracker: record_cpu_time accumulates",
          "[extension_resource_tracker][positive]")
{
    ExtensionResourceTracker tracker;
    tracker.register_extension("ext.a");
    tracker.record_cpu_time("ext.a", std::chrono::microseconds(100));
    tracker.record_cpu_time("ext.a", std::chrono::microseconds(200));
    auto snap = tracker.snapshot();
    REQUIRE(snap.size() == 1);
    REQUIRE(snap[0].cpu_time_us == 300);
}

TEST_CASE("ExtensionResourceTracker: record_memory sets value",
          "[extension_resource_tracker][positive]")
{
    ExtensionResourceTracker tracker;
    tracker.register_extension("ext.a");
    tracker.record_memory("ext.a", 4096);
    auto snap = tracker.snapshot();
    REQUIRE(snap[0].memory_bytes == 4096);
}

TEST_CASE("ExtensionResourceTracker: record_event_latency computes average",
          "[extension_resource_tracker][positive]")
{
    ExtensionResourceTracker tracker;
    tracker.register_extension("ext.a");
    tracker.record_event_latency("ext.a", std::chrono::microseconds(100));
    tracker.record_event_latency("ext.a", std::chrono::microseconds(200));
    auto snap = tracker.snapshot();
    REQUIRE(snap[0].avg_event_latency_us == 150);
    REQUIRE(snap[0].event_count == 2);
}

TEST_CASE("ExtensionResourceTracker: record_error increments",
          "[extension_resource_tracker][positive]")
{
    ExtensionResourceTracker tracker;
    tracker.register_extension("ext.a");
    tracker.record_error("ext.a");
    tracker.record_error("ext.a");
    auto snap = tracker.snapshot();
    REQUIRE(snap[0].error_count == 2);
}

TEST_CASE("ExtensionResourceTracker: ranked_by_cpu orders correctly",
          "[extension_resource_tracker][positive]")
{
    ExtensionResourceTracker tracker;
    tracker.register_extension("slow");
    tracker.register_extension("fast");
    tracker.record_cpu_time("slow", std::chrono::microseconds(1000));
    tracker.record_cpu_time("fast", std::chrono::microseconds(100));
    auto ranked = tracker.ranked_by_cpu();
    REQUIRE(ranked.size() == 2);
    REQUIRE(ranked[0].extension_id == "slow");
    REQUIRE(ranked[1].extension_id == "fast");
}

TEST_CASE("ExtensionResourceTracker: ranked_by_memory orders correctly",
          "[extension_resource_tracker][positive]")
{
    ExtensionResourceTracker tracker;
    tracker.register_extension("heavy");
    tracker.register_extension("light");
    tracker.record_memory("heavy", 8192);
    tracker.record_memory("light", 256);
    auto ranked = tracker.ranked_by_memory();
    REQUIRE(ranked[0].extension_id == "heavy");
    REQUIRE(ranked[1].extension_id == "light");
}

TEST_CASE("ExtensionResourceTracker: reset clears counters",
          "[extension_resource_tracker][positive]")
{
    ExtensionResourceTracker tracker;
    tracker.register_extension("ext.a");
    tracker.record_cpu_time("ext.a", std::chrono::microseconds(500));
    tracker.record_error("ext.a");
    tracker.reset();
    auto snap = tracker.snapshot();
    REQUIRE(snap[0].cpu_time_us == 0);
    REQUIRE(snap[0].error_count == 0);
}

// ── Negative Tests ──

TEST_CASE("ExtensionResourceTracker: record on unregistered is ignored",
          "[extension_resource_tracker][negative]")
{
    ExtensionResourceTracker tracker;
    tracker.record_cpu_time("unknown", std::chrono::microseconds(100));
    tracker.record_memory("unknown", 4096);
    tracker.record_error("unknown");
    REQUIRE(tracker.extension_count() == 0);
}

TEST_CASE("ExtensionResourceTracker: empty snapshot", "[extension_resource_tracker][negative]")
{
    ExtensionResourceTracker tracker;
    auto snap = tracker.snapshot();
    REQUIRE(snap.empty());
}

// ── Edge Cases ──

TEST_CASE("ExtensionResourceTracker: zero event latency avg is zero",
          "[extension_resource_tracker][edge]")
{
    ExtensionResourceTracker tracker;
    tracker.register_extension("ext.a");
    auto snap = tracker.snapshot();
    REQUIRE(snap[0].avg_event_latency_us == 0);
    REQUIRE(snap[0].event_count == 0);
}

TEST_CASE("ExtensionResourceTracker: re-register resets entry",
          "[extension_resource_tracker][edge]")
{
    ExtensionResourceTracker tracker;
    tracker.register_extension("ext.a");
    tracker.record_cpu_time("ext.a", std::chrono::microseconds(500));
    tracker.register_extension("ext.a"); // Re-register
    auto snap = tracker.snapshot();
    REQUIRE(snap[0].cpu_time_us == 0); // Reset after re-register
}
