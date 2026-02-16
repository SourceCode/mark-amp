/// test_extension_resource_tracking.cpp — Phase 33: ExtensionResourceTracker tests

#include "core/ExtensionResourceTracker.h"

#include <catch2/catch_test_macros.hpp>

#include <chrono>

using namespace markamp::core;

TEST_CASE("ExtensionResourceTracker: register and count", "[ext_resource_tracker]")
{
    ExtensionResourceTracker tracker;
    tracker.register_extension("ext.a");
    tracker.register_extension("ext.b");

    REQUIRE(tracker.extension_count() == 2);
}

TEST_CASE("ExtensionResourceTracker: record CPU time", "[ext_resource_tracker]")
{
    ExtensionResourceTracker tracker;
    tracker.register_extension("ext.fast");

    tracker.record_cpu_time("ext.fast", std::chrono::microseconds(100));
    tracker.record_cpu_time("ext.fast", std::chrono::microseconds(200));

    auto snaps = tracker.snapshot();
    REQUIRE(snaps.size() == 1);
    REQUIRE(snaps[0].cpu_time_us == 300);
}

TEST_CASE("ExtensionResourceTracker: record memory", "[ext_resource_tracker]")
{
    ExtensionResourceTracker tracker;
    tracker.register_extension("ext.mem");

    tracker.record_memory("ext.mem", 1024);

    auto snaps = tracker.snapshot();
    REQUIRE(snaps[0].memory_bytes == 1024);
}

TEST_CASE("ExtensionResourceTracker: event latency average", "[ext_resource_tracker]")
{
    ExtensionResourceTracker tracker;
    tracker.register_extension("ext.events");

    tracker.record_event_latency("ext.events", std::chrono::microseconds(100));
    tracker.record_event_latency("ext.events", std::chrono::microseconds(300));

    auto snaps = tracker.snapshot();
    REQUIRE(snaps[0].avg_event_latency_us == 200); // avg of 100 + 300
    REQUIRE(snaps[0].event_count == 2);
}

TEST_CASE("ExtensionResourceTracker: error counting", "[ext_resource_tracker]")
{
    ExtensionResourceTracker tracker;
    tracker.register_extension("ext.buggy");

    tracker.record_error("ext.buggy");
    tracker.record_error("ext.buggy");

    auto snaps = tracker.snapshot();
    REQUIRE(snaps[0].error_count == 2);
}

TEST_CASE("ExtensionResourceTracker: ranked_by_cpu", "[ext_resource_tracker]")
{
    ExtensionResourceTracker tracker;
    tracker.register_extension("ext.slow");
    tracker.register_extension("ext.fast");

    tracker.record_cpu_time("ext.slow", std::chrono::microseconds(1000));
    tracker.record_cpu_time("ext.fast", std::chrono::microseconds(100));

    auto ranked = tracker.ranked_by_cpu();
    REQUIRE(ranked.size() == 2);
    REQUIRE(ranked[0].extension_id == "ext.slow");
    REQUIRE(ranked[1].extension_id == "ext.fast");
}

TEST_CASE("ExtensionResourceTracker: ranked_by_memory", "[ext_resource_tracker]")
{
    ExtensionResourceTracker tracker;
    tracker.register_extension("ext.small");
    tracker.register_extension("ext.large");

    tracker.record_memory("ext.small", 100);
    tracker.record_memory("ext.large", 10000);

    auto ranked = tracker.ranked_by_memory();
    REQUIRE(ranked[0].extension_id == "ext.large");
}

TEST_CASE("ExtensionResourceTracker: reset clears data", "[ext_resource_tracker]")
{
    ExtensionResourceTracker tracker;
    tracker.register_extension("ext.x");
    tracker.record_cpu_time("ext.x", std::chrono::microseconds(500));

    tracker.reset();

    auto snaps = tracker.snapshot();
    REQUIRE(snaps[0].cpu_time_us == 0);
    REQUIRE(tracker.extension_count() == 1); // extension still registered
}

TEST_CASE("ExtensionResourceTracker: unregistered extension ignored", "[ext_resource_tracker]")
{
    ExtensionResourceTracker tracker;
    // Should not crash
    tracker.record_cpu_time("ext.unknown", std::chrono::microseconds(100));
    tracker.record_memory("ext.unknown", 1024);
    tracker.record_error("ext.unknown");

    REQUIRE(tracker.extension_count() == 0);
}
