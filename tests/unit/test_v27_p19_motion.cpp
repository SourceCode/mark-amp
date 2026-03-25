/// @file test_v27_p19_motion.cpp
/// @brief V27 Phase 19: Motion tiers, easing, micro-interactions, drag feedback, coordinator.
#include <catch2/catch_test_macros.hpp>
#include "ui/V27MotionSystem.h"
#include "core/V27MotionDesignCoordinator.h"
using namespace markamp::ui;
using namespace markamp::core;
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
TEST_CASE("V27 P19: Drag feedback tokens", "[v27][p19]") {
    REQUIRE(V27DragFeedbackTokens::kDragShadowOffsetY > V27DragFeedbackTokens::kDragShadowOffsetX);
    REQUIRE(V27DragFeedbackTokens::kDragGhostOpacityPct > 0);
    REQUIRE(V27DragFeedbackTokens::kDragGhostOpacityPct < 100);
    REQUIRE(V27DragFeedbackTokens::kDragThresholdPx > 0);
    REQUIRE(V27DragFeedbackTokens::kDropZoneRadius > 0);
}
TEST_CASE("V27 P19: Motion coordinator registration", "[v27][p19]") {
    V27MotionDesignCoordinator coord;
    coord.register_surface({"Tab Switch", V27MotionTier::kFast, V27EasingCurve::kEaseOut, true});
    coord.register_surface({"Dialog Open", V27MotionTier::kStandard, V27EasingCurve::kEaseInOut, true});
    coord.register_surface({"Menu Close", V27MotionTier::kMicro, V27EasingCurve::kEaseOut, false});
    REQUIRE(coord.surface_count() == 3);
    REQUIRE(coord.reduced_motion_count() == 2);
    REQUIRE_FALSE(coord.all_have_fallback());
    REQUIRE(coord.count_by_tier(V27MotionTier::kFast) == 1);
    REQUIRE(coord.count_by_tier(V27MotionTier::kStandard) == 1);
    REQUIRE(coord.count_by_tier(V27MotionTier::kMicro) == 1);
}
TEST_CASE("V27 P19: Motion coordinator all fallbacks", "[v27][p19]") {
    V27MotionDesignCoordinator coord;
    coord.register_surface({"Tab", V27MotionTier::kFast, V27EasingCurve::kEaseOut, true});
    coord.register_surface({"Dialog", V27MotionTier::kStandard, V27EasingCurve::kEaseInOut, true});
    REQUIRE(coord.all_have_fallback());
}
