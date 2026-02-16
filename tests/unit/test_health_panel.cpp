/// test_health_panel.cpp — Phase 32: HealthPanel tests

#include "core/HealthPanel.h"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <thread>

using namespace markamp::core;

TEST_CASE("HealthPanel: initial snapshot has zero values", "[health_panel]")
{
    HealthPanel panel;
    auto snap = panel.snapshot();

    REQUIRE(snap.total_frames == 0);
    REQUIRE(snap.dropped_frames == 0);
    REQUIRE(snap.memory_used_bytes == 0);
    REQUIRE(snap.active_plugins == 0);
}

TEST_CASE("HealthPanel: record_frame tracks count", "[health_panel]")
{
    HealthPanel panel;
    panel.record_frame(std::chrono::microseconds(8000)); // 8ms
    panel.record_frame(std::chrono::microseconds(8000));

    auto snap = panel.snapshot();
    REQUIRE(snap.total_frames == 2);
    REQUIRE(snap.dropped_frames == 0);
}

TEST_CASE("HealthPanel: dropped frames when over budget", "[health_panel]")
{
    HealthPanel panel;
    panel.record_frame(std::chrono::microseconds(20000)); // 20ms > 16.67ms

    auto snap = panel.snapshot();
    REQUIRE(snap.total_frames == 1);
    REQUIRE(snap.dropped_frames == 1);
}

TEST_CASE("HealthPanel: set_memory updates snapshot", "[health_panel]")
{
    HealthPanel panel;
    panel.set_memory(1024 * 1024, 10 * 1024 * 1024); // 1MB / 10MB

    auto snap = panel.snapshot();
    REQUIRE(snap.memory_used_bytes == 1024 * 1024);
    REQUIRE(snap.memory_budget_bytes == 10 * 1024 * 1024);
    REQUIRE(snap.memory_usage_percent > 9.0);
    REQUIRE(snap.memory_usage_percent < 11.0);
}

TEST_CASE("HealthPanel: set_plugins updates snapshot", "[health_panel]")
{
    HealthPanel panel;
    panel.set_plugins(5, 1);

    auto snap = panel.snapshot();
    REQUIRE(snap.active_plugins == 5);
    REQUIRE(snap.plugins_over_budget == 1);
}

TEST_CASE("HealthPanel: set_events updates snapshot", "[health_panel]")
{
    HealthPanel panel;
    panel.set_events(100, 5);

    auto snap = panel.snapshot();
    REQUIRE(snap.events_dispatched == 100);
    REQUIRE(snap.queue_depth == 5);
}

TEST_CASE("HealthPanel: to_metrics produces entries", "[health_panel]")
{
    HealthPanel panel;
    panel.record_frame(std::chrono::microseconds(16000));
    panel.set_memory(5 * 1024 * 1024, 10 * 1024 * 1024);

    auto snap = panel.snapshot();
    auto metrics = snap.to_metrics();

    REQUIRE(metrics.size() >= 8);
    REQUIRE(metrics[0].name == "Frame Time");
    REQUIRE(metrics[1].name == "FPS");
}

TEST_CASE("HealthPanel: uptime increases", "[health_panel]")
{
    HealthPanel panel;
    auto snap1 = panel.snapshot();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto snap2 = panel.snapshot();

    REQUIRE(snap2.uptime_ms >= snap1.uptime_ms);
}
