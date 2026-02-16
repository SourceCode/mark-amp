/// test_load_harness.cpp — Phase 39: LoadTestHarness tests

#include "core/LoadTestHarness.h"

#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <chrono>

using namespace markamp::core;

TEST_CASE("LoadTestHarness: single thread basic run", "[load_test]")
{
    LoadTestHarness harness("basic test");
    harness.set_threads(1);
    harness.set_duration(std::chrono::milliseconds(50));

    auto result = harness.run([]() { return true; });

    REQUIRE(result.scenario_name == "basic test");
    REQUIRE(result.total_operations > 0);
    REQUIRE(result.errors == 0);
    REQUIRE(result.thread_count == 1);
    REQUIRE(result.ops_per_second > 0.0);
}

TEST_CASE("LoadTestHarness: multi-threaded run", "[load_test]")
{
    LoadTestHarness harness("multi-thread test");
    harness.set_threads(4);
    harness.set_duration(std::chrono::milliseconds(50));

    std::atomic<int> counter{0};

    auto result = harness.run(
        [&counter]()
        {
            counter.fetch_add(1, std::memory_order_relaxed);
            return true;
        });

    REQUIRE(result.total_operations > 0);
    REQUIRE(result.thread_count == 4);
    REQUIRE(counter.load() > 0);
}

TEST_CASE("LoadTestHarness: error counting", "[load_test]")
{
    LoadTestHarness harness("error test");
    harness.set_threads(1);
    harness.set_duration(std::chrono::milliseconds(50));

    auto result = harness.run([]() { return false; }); // all fail

    REQUIRE(result.errors > 0);
    REQUIRE(result.total_operations == 0);
}

TEST_CASE("LoadTestHarness: duration is respected", "[load_test]")
{
    LoadTestHarness harness("duration test");
    harness.set_threads(1);
    harness.set_duration(std::chrono::milliseconds(100));

    auto start = std::chrono::steady_clock::now();
    auto result = harness.run([]() { return true; });
    auto elapsed = std::chrono::steady_clock::now() - start;

    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    REQUIRE(elapsed_ms >= 90); // at least ~90ms
    REQUIRE(result.duration_ms >= 90);
}

TEST_CASE("LoadTestHarness: latency is measured", "[load_test]")
{
    LoadTestHarness harness("latency test");
    harness.set_threads(1);
    harness.set_duration(std::chrono::milliseconds(50));

    auto result = harness.run([]() { return true; });

    REQUIRE(result.avg_latency_us >= 0.0);
}
