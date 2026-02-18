/// test_adaptive_throttle.cpp — Unit tests
#include "core/AdaptiveThrottle.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("ActivityMode: enum values", "[adaptive_throttle]")
{
    REQUIRE(static_cast<int>(ActivityMode::Typing) != static_cast<int>(ActivityMode::Idle));
}

TEST_CASE("AdaptiveThrottle: compiles", "[adaptive_throttle]")
{
    static_assert(sizeof(AdaptiveThrottle) > 0);
}
