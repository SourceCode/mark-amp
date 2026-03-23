/// @file test_v27_p19_motion.cpp
/// @brief V27 Phase 19: Motion tiers, easing, micro-interactions, icon controls.
#include <catch2/catch_test_macros.hpp>
#include "ui/V27MotionSystem.h"
using namespace markamp::ui;
TEST_CASE("V27 P19: Motion tier count", "[v27][p19]") { REQUIRE(v27_motion_tier_count() == 5); }
TEST_CASE("V27 P19: Motion tiers increase", "[v27][p19]") {
    REQUIRE(v27_motion_tier_ms(V27MotionTier::kInstant) == 0);
    REQUIRE(v27_motion_tier_ms(V27MotionTier::kMicro) < v27_motion_tier_ms(V27MotionTier::kFast));
    REQUIRE(v27_motion_tier_ms(V27MotionTier::kFast) < v27_motion_tier_ms(V27MotionTier::kStandard));
    REQUIRE(v27_motion_tier_ms(V27MotionTier::kStandard) < v27_motion_tier_ms(V27MotionTier::kGentle));
}
TEST_CASE("V27 P19: Easing curve count", "[v27][p19]") { REQUIRE(v27_easing_curve_count() == 5); }
TEST_CASE("V27 P19: Micro-interaction timing", "[v27][p19]") {
    REQUIRE(V27MicroInteractionTokens::kPressScaleMs < V27MicroInteractionTokens::kHoverScaleMs);
    REQUIRE(V27MicroInteractionTokens::kMenuCloseMs < V27MicroInteractionTokens::kMenuOpenMs);
}
TEST_CASE("V27 P19: Icon control tokens", "[v27][p19]") {
    REQUIRE(V27IconControlTokens::kIconOnlySize >= 24);
    REQUIRE(V27IconControlTokens::kIconLabelGap > 0);
}
