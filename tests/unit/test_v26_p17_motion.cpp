/// @file test_v26_p17_motion.cpp
/// @brief V26 Phase 17: Motion, transitions & responsiveness feel tests.
#include <catch2/catch_test_macros.hpp>
#include "ui/V26IconMotionConvergence.h"

using namespace markamp::ui;

TEST_CASE("V26 P17: Motion duration tier count", "[v26][p17]")
{
    REQUIRE(MotionDurationMetrics::tier_count() == 6);
}

TEST_CASE("V26 P17: Duration tiers are monotonically increasing", "[v26][p17]")
{
    REQUIRE(MotionDurationMetrics::kInstantMs < MotionDurationMetrics::kMicroMs);
    REQUIRE(MotionDurationMetrics::kMicroMs < MotionDurationMetrics::kFastMs);
    REQUIRE(MotionDurationMetrics::kFastMs < MotionDurationMetrics::kNormalMs);
    REQUIRE(MotionDurationMetrics::kNormalMs < MotionDurationMetrics::kSlowMs);
    REQUIRE(MotionDurationMetrics::kSlowMs < MotionDurationMetrics::kDramaticMs);
}

TEST_CASE("V26 P17: Transition timing consistency", "[v26][p17]")
{
    REQUIRE(TransitionRules::is_timing_consistent());
}

TEST_CASE("V26 P17: Hover enter is faster than leave", "[v26][p17]")
{
    REQUIRE(TransitionRules::kHoverEnterMs <= TransitionRules::kHoverLeaveMs);
    REQUIRE(TransitionRules::kPressMs < TransitionRules::kHoverEnterMs);
}

TEST_CASE("V26 P17: Menu open/close symmetry", "[v26][p17]")
{
    REQUIRE(TransitionRules::kMenuOpenMs > TransitionRules::kMenuCloseMs);
    REQUIRE(TransitionRules::kPaletteOpenMs > TransitionRules::kPaletteCloseMs);
}

TEST_CASE("V26 P17: Dialog transitions", "[v26][p17]")
{
    REQUIRE(TransitionRules::kDialogOpenMs > 0);
    REQUIRE(TransitionRules::kDialogCloseMs > 0);
    REQUIRE(TransitionRules::kDialogOpenMs >= TransitionRules::kDialogCloseMs);
}

TEST_CASE("V26 P17: Easing curve enum", "[v26][p17]")
{
    REQUIRE(static_cast<int>(EasingCurve::kLinear) == 0);
    REQUIRE(static_cast<int>(EasingCurve::kSpring) == 4);
}
