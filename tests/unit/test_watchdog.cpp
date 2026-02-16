/// test_watchdog.cpp — Phase 31: Watchdog tests

#include "core/Watchdog.h"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <thread>

using namespace markamp::core;

TEST_CASE("Watchdog: starts not running", "[watchdog]")
{
    Watchdog watchdog;
    REQUIRE_FALSE(watchdog.is_running());
    REQUIRE(watchdog.heartbeat_count() == 0);
    REQUIRE(watchdog.stall_count() == 0);
}

TEST_CASE("Watchdog: heartbeat increments count", "[watchdog]")
{
    Watchdog watchdog;
    watchdog.heartbeat();
    watchdog.heartbeat();
    watchdog.heartbeat();

    REQUIRE(watchdog.heartbeat_count() == 3);
}

TEST_CASE("Watchdog: start and stop", "[watchdog]")
{
    Watchdog watchdog;
    watchdog.set_threshold(std::chrono::milliseconds(500));
    watchdog.start();

    REQUIRE(watchdog.is_running());

    watchdog.stop();
    REQUIRE_FALSE(watchdog.is_running());
}

TEST_CASE("Watchdog: double start is safe", "[watchdog]")
{
    Watchdog watchdog;
    watchdog.set_threshold(std::chrono::milliseconds(500));
    watchdog.start();
    watchdog.start(); // should be no-op

    REQUIRE(watchdog.is_running());
    watchdog.stop();
}

TEST_CASE("Watchdog: time_since_heartbeat is positive", "[watchdog]")
{
    Watchdog watchdog;
    watchdog.heartbeat();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    auto elapsed = watchdog.time_since_heartbeat();
    REQUIRE(elapsed.count() >= 4); // at least ~4ms
}

TEST_CASE("Watchdog: stall detection fires callback", "[watchdog]")
{
    Watchdog watchdog;
    watchdog.set_threshold(std::chrono::milliseconds(30));

    bool stall_detected = false;
    watchdog.on_stall([&stall_detected](const StallEvent& /*event*/) { stall_detected = true; });

    watchdog.heartbeat();
    watchdog.start();

    // Don't heartbeat for a while to trigger stall
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    watchdog.stop();

    REQUIRE(stall_detected);
    REQUIRE(watchdog.stall_count() > 0);
}
