/// @file test_performance_dashboard.cpp
/// @brief Phase 49 — Unit tests for Performance Dashboard and Monitoring.

#include "core/PerformanceCounters.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// MetricsRingBuffer
// ============================================================================

TEST_CASE("RingBuffer: wraps at capacity", "[performance_dashboard]")
{
    MetricsRingBuffer<int, 5> buf;
    for (int i = 0; i < 10; ++i)
    {
        buf.push(i);
    }
    REQUIRE(buf.size() == 5);
    REQUIRE(buf.at(0) == 5); // Oldest should be 5
    REQUIRE(buf.latest() == 9);
}

// ============================================================================
// Frame Timing
// ============================================================================

TEST_CASE("PerformanceCounters: frame time averages", "[performance_dashboard]")
{
    PerformanceCounters counters;
    counters.record_frame_time(10.0);
    counters.record_frame_time(20.0);
    counters.record_frame_time(30.0);

    REQUIRE(counters.frame_time_average() == Catch::Approx(20.0));
}

// ============================================================================
// EventBus Throughput
// ============================================================================

TEST_CASE("PerformanceCounters: event count", "[performance_dashboard]")
{
    PerformanceCounters counters;
    counters.increment_event_count();
    counters.increment_event_count();
    counters.increment_event_count();

    REQUIRE(counters.event_count() == 3);
}

// ============================================================================
// Memory
// ============================================================================

TEST_CASE("PerformanceCounters: memory delta detection", "[performance_dashboard]")
{
    PerformanceCounters counters;
    counters.record_memory_usage(100 * 1024 * 1024); // 100 MB
    counters.record_memory_usage(120 * 1024 * 1024); // 120 MB

    REQUIRE(counters.memory_delta_mb() == Catch::Approx(20.0));
}

// ============================================================================
// File Cache
// ============================================================================

TEST_CASE("PerformanceCounters: cache hit rate", "[performance_dashboard]")
{
    PerformanceCounters counters;
    for (int i = 0; i < 80; ++i)
    {
        counters.record_cache_hit();
    }
    for (int i = 0; i < 20; ++i)
    {
        counters.record_cache_miss();
    }

    REQUIRE(counters.cache_hit_rate() == Catch::Approx(80.0));
}

// ============================================================================
// Startup Timeline
// ============================================================================

TEST_CASE("PerformanceCounters: startup phase durations", "[performance_dashboard]")
{
    PerformanceCounters counters;
    counters.record_startup_phase("init", std::chrono::microseconds(500));
    counters.record_startup_phase("load_plugins", std::chrono::microseconds(300));
    counters.record_startup_phase("render", std::chrono::microseconds(200));

    REQUIRE(counters.startup_phases().size() == 3);
    REQUIRE(counters.total_startup_time().count() == 1000);
}

// ============================================================================
// Slow Events
// ============================================================================

TEST_CASE("PerformanceCounters: slow event detection at 5ms", "[performance_dashboard]")
{
    PerformanceCounters counters;
    counters.record_event_duration(3.0);  // Below threshold
    counters.record_event_duration(6.0);  // Above threshold
    counters.record_event_duration(10.0); // Above threshold

    REQUIRE(counters.slow_event_count() == 2);
}

// ============================================================================
// JSON Export
// ============================================================================

TEST_CASE("PerformanceCounters: JSON export contains expected fields", "[performance_dashboard]")
{
    PerformanceCounters counters;
    counters.record_frame_time(16.0);
    counters.increment_event_count();

    auto json = counters.export_json();
    REQUIRE(json.find("frame_time_avg_ms") != std::string::npos);
    REQUIRE(json.find("event_count") != std::string::npos);
    REQUIRE(json.find("cache_hit_rate") != std::string::npos);
}

// ============================================================================
// Regression Detection
// ============================================================================

TEST_CASE("PerformanceCounters: regression detection >20%", "[performance_dashboard]")
{
    PerformanceCounters counters;
    // Baseline: 10ms, Current avg: 15ms → 50% regression
    counters.record_frame_time(15.0);
    REQUIRE(counters.detect_frame_regression(10.0));

    // Baseline: 10ms, Current avg: 11ms → 10% → no regression
    PerformanceCounters counters2;
    counters2.record_frame_time(11.0);
    REQUIRE_FALSE(counters2.detect_frame_regression(10.0));
}

// ============================================================================
// Layout Reflow
// ============================================================================

TEST_CASE("PerformanceCounters: layout reflow counter", "[performance_dashboard]")
{
    PerformanceCounters counters;
    counters.increment_reflow_count();
    counters.increment_reflow_count();
    counters.increment_reflow_count();

    REQUIRE(counters.reflow_count() == 3);
}
