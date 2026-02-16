#include "core/Profiler.h"
#include "core/StartupTimer.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

#include <chrono>
#include <thread>

using namespace markamp::core;

// ══════════════════════════════════════════════════════════
// StartupTimer tests
// ══════════════════════════════════════════════════════════

TEST_CASE("StartupTimer: initial state", "[startup_timer]")
{
    StartupTimer timer;

    REQUIRE(timer.checkpoint_count() == 0);
    REQUIRE(timer.total_ms() == 0.0);
    REQUIRE(timer.elapsed_ms() > 0.0);
}

TEST_CASE("StartupTimer: single checkpoint", "[startup_timer]")
{
    StartupTimer timer;

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    timer.checkpoint("config_loaded");

    REQUIRE(timer.checkpoint_count() == 1);
    REQUIRE(timer.total_ms() >= 4.0); // at least 4ms (allowing timer jitter)
}

TEST_CASE("StartupTimer: multiple checkpoints in order", "[startup_timer]")
{
    StartupTimer timer;

    timer.checkpoint("phase_1");
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    timer.checkpoint("phase_2");
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    timer.checkpoint("phase_3");

    REQUIRE(timer.checkpoint_count() == 3);
    REQUIRE(timer.total_ms() >= 3.0);
}

TEST_CASE("StartupTimer: JSON report structure", "[startup_timer]")
{
    StartupTimer timer;

    timer.checkpoint("init");
    timer.checkpoint("services_created");
    timer.checkpoint("first_frame");

    auto json_str = timer.to_json();
    auto report = nlohmann::json::parse(json_str);

    REQUIRE(report.contains("total_ms"));
    REQUIRE(report.contains("checkpoint_count"));
    REQUIRE(report.contains("phases"));
    REQUIRE(report["checkpoint_count"].get<std::size_t>() == 3);

    const auto& phases = report["phases"];
    REQUIRE(phases.is_array());
    REQUIRE(phases.size() == 3);

    // Each phase has name, start_offset_ms, duration_ms
    for (const auto& phase : phases)
    {
        REQUIRE(phase.contains("name"));
        REQUIRE(phase.contains("start_offset_ms"));
        REQUIRE(phase.contains("duration_ms"));
    }

    // Phase names match
    REQUIRE(phases[0]["name"].get<std::string>() == "init");
    REQUIRE(phases[1]["name"].get<std::string>() == "services_created");
    REQUIRE(phases[2]["name"].get<std::string>() == "first_frame");

    // start_offset_ms is non-decreasing
    double prev_offset = 0.0;
    for (const auto& phase : phases)
    {
        double offset = phase["start_offset_ms"].get<double>();
        REQUIRE(offset >= prev_offset);
        prev_offset = offset;
    }
}

TEST_CASE("StartupTimer: elapsed_ms increases over time", "[startup_timer]")
{
    StartupTimer timer;

    double first_ms = timer.elapsed_ms();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    double second_ms = timer.elapsed_ms();

    REQUIRE(second_ms > first_ms);
}

// ══════════════════════════════════════════════════════════
// Profiler non-singleton tests (Phase 03: injectable Profiler)
// ══════════════════════════════════════════════════════════

TEST_CASE("Profiler: can be constructed as standalone instance", "[profiler_v2]")
{
    Profiler profiler;

    profiler.record("test_metric", 1.5);
    profiler.record("test_metric", 2.5);
    profiler.record("test_metric", 3.5);

    auto timing_results = profiler.results();
    REQUIRE(timing_results.size() == 1);
    REQUIRE(timing_results[0].name == "test_metric");
    REQUIRE(timing_results[0].call_count == 3);
    REQUIRE(timing_results[0].avg_ms == Catch::Approx(2.5).margin(0.1));
}

TEST_CASE("Profiler: standalone instances are independent", "[profiler_v2]")
{
    Profiler profiler_a;
    Profiler profiler_b;

    profiler_a.record("metric_a", 1.0);
    profiler_b.record("metric_b", 2.0);

    auto results_a = profiler_a.results();
    auto results_b = profiler_b.results();

    REQUIRE(results_a.size() == 1);
    REQUIRE(results_a[0].name == "metric_a");

    REQUIRE(results_b.size() == 1);
    REQUIRE(results_b[0].name == "metric_b");
}

TEST_CASE("Profiler: singleton still works", "[profiler_v2]")
{
    auto& global = Profiler::instance();
    global.reset();

    global.record("global_metric", 5.0);
    auto timing_results = global.results();

    REQUIRE(timing_results.size() == 1);
    REQUIRE(timing_results[0].name == "global_metric");

    global.reset();
}

TEST_CASE("Profiler: begin/end timing", "[profiler_v2]")
{
    Profiler profiler;

    profiler.begin("operation");
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    profiler.end("operation");

    auto timing_results = profiler.results();
    REQUIRE(timing_results.size() == 1);
    REQUIRE(timing_results[0].call_count == 1);
    REQUIRE(timing_results[0].avg_ms >= 4.0);
}

TEST_CASE("Profiler: reset clears all data", "[profiler_v2]")
{
    Profiler profiler;

    profiler.record("metric", 1.0);
    REQUIRE(profiler.results().size() == 1);

    profiler.reset();
    REQUIRE(profiler.results().empty());
}
