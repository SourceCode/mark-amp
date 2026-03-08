// test_adaptive_throttle.cpp — 10 tests for AdaptiveThrottle
#include "core/AdaptiveThrottle.h"

#include <catch2/catch_test_macros.hpp>

#include <thread>

using namespace markamp::core;

TEST_CASE("AdaptiveThrottle starts in Idle mode", "[throttle]")
{
    AdaptiveThrottle throttle;
    // Constructed with last_activity in the past, so should be idle
    CHECK(throttle.is_idle());
    CHECK(throttle.current_mode() == ActivityMode::Idle);
}

TEST_CASE("AdaptiveThrottle switches to Typing after update_activity", "[throttle]")
{
    AdaptiveThrottle throttle;
    throttle.update_activity();
    CHECK(throttle.is_typing());
    CHECK(throttle.current_mode() == ActivityMode::Typing);
}

TEST_CASE("AdaptiveThrottle returns to Idle after threshold", "[throttle]")
{
    AdaptiveThrottle throttle(std::chrono::milliseconds(10));
    throttle.update_activity();
    CHECK(throttle.is_typing());
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    CHECK(throttle.is_idle());
}

TEST_CASE("AdaptiveThrottle typing budget is smaller than idle", "[throttle]")
{
    CHECK(AdaptiveThrottle::kDefaultTypingBudget < AdaptiveThrottle::kDefaultIdleBudget);
}

TEST_CASE("AdaptiveThrottle current_budget reflects mode", "[throttle]")
{
    AdaptiveThrottle throttle;
    // Starts idle
    CHECK(throttle.current_budget() == AdaptiveThrottle::kDefaultIdleBudget);
    throttle.update_activity();
    CHECK(throttle.current_budget() == AdaptiveThrottle::kDefaultTypingBudget);
}

TEST_CASE("AdaptiveThrottle set_idle_threshold changes threshold", "[throttle]")
{
    AdaptiveThrottle throttle;
    throttle.set_idle_threshold(std::chrono::milliseconds(500));
    CHECK(throttle.idle_threshold() == std::chrono::milliseconds(500));
}

TEST_CASE("AdaptiveThrottle time_since_activity returns positive", "[throttle]")
{
    AdaptiveThrottle throttle;
    throttle.update_activity();
    auto elapsed = throttle.time_since_activity();
    CHECK(elapsed.count() >= 0);
}

TEST_CASE("AdaptiveThrottle default idle threshold is 300ms", "[throttle]")
{
    CHECK(AdaptiveThrottle::kDefaultIdleThreshold == std::chrono::milliseconds(300));
}

TEST_CASE("AdaptiveThrottle is_typing and is_idle are mutually exclusive", "[throttle]")
{
    AdaptiveThrottle throttle;
    throttle.update_activity();
    CHECK(throttle.is_typing() != throttle.is_idle());
}

TEST_CASE("AdaptiveThrottle rapid updates keep typing mode", "[throttle]")
{
    AdaptiveThrottle throttle(std::chrono::milliseconds(50));
    for (int i = 0; i < 5; ++i)
    {
        throttle.update_activity();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        CHECK(throttle.is_typing());
    }
}
