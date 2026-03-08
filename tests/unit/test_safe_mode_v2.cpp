// test_safe_mode_v2.cpp — 10 tests for SafeModeController and CrashCounter
#include "core/SafeMode.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SafeModeController default tier is Normal", "[core][safemode]")
{
    SafeModeController controller;
    CHECK(controller.current_tier() == SafeModeTier::kNormal);
}

TEST_CASE("SafeModeController set_tier changes tier", "[core][safemode]")
{
    SafeModeController controller;
    controller.set_tier(SafeModeTier::kNoExtensions);
    CHECK(controller.current_tier() == SafeModeTier::kNoExtensions);
}

TEST_CASE("SafeModeController extensions_allowed in Normal", "[core][safemode]")
{
    SafeModeController controller;
    CHECK(controller.extensions_allowed());
}

TEST_CASE("SafeModeController extensions not allowed in NoExtensions", "[core][safemode]")
{
    SafeModeController controller;
    controller.set_tier(SafeModeTier::kNoExtensions);
    CHECK_FALSE(controller.extensions_allowed());
}

TEST_CASE("SafeModeController preview_allowed in Normal", "[core][safemode]")
{
    SafeModeController controller;
    CHECK(controller.preview_allowed());
}

TEST_CASE("SafeModeController preview not allowed in NoPreview", "[core][safemode]")
{
    SafeModeController controller;
    controller.set_tier(SafeModeTier::kNoPreview);
    CHECK_FALSE(controller.preview_allowed());
}

TEST_CASE("SafeModeController reset returns to Normal", "[core][safemode]")
{
    SafeModeController controller;
    controller.set_tier(SafeModeTier::kFull);
    controller.reset();
    CHECK(controller.current_tier() == SafeModeTier::kNormal);
}

TEST_CASE("CrashCounter starts at zero", "[core][safemode]")
{
    CrashCounter counter;
    CHECK(counter.recent_crash_count() == 0);
}

TEST_CASE("CrashCounter record_crash increments count", "[core][safemode]")
{
    CrashCounter counter;
    auto tier = counter.record_crash();
    CHECK(counter.recent_crash_count() >= 1);
    (void)tier;
}

TEST_CASE("tier_name returns display names", "[core][safemode]")
{
    auto normal = tier_name(SafeModeTier::kNormal);
    auto full = tier_name(SafeModeTier::kFull);
    CHECK_FALSE(normal.empty());
    CHECK_FALSE(full.empty());
    CHECK(normal != full);
}
