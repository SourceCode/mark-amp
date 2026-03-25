/// @file test_v27_p18_accessibility.cpp
/// @brief V27 Phase 18: Focus ring, contrast, reduced motion, high-contrast, screen reader tokens, coordinator.
#include <catch2/catch_test_macros.hpp>
#include "ui/V27AccessibilityTokens.h"
#include "core/V27AccessibilityDesignCoordinator.h"
using namespace markamp::ui;
using namespace markamp::core;
TEST_CASE("V27 P18: Focus ring tokens", "[v27][p18]") {
    REQUIRE(V27FocusRingTokens::kThickness > 0);
    REQUIRE(V27FocusRingTokens::kHighContrastThickness >= V27FocusRingTokens::kThickness);
}
TEST_CASE("V27 P18: Contrast ratio tokens (WCAG)", "[v27][p18]") {
    REQUIRE(V27ContrastTokens::kMinContrastNormal >= 45);
    REQUIRE(V27ContrastTokens::kMinContrastLarge >= 30);
    REQUIRE(V27ContrastTokens::kEnhancedContrastNormal > V27ContrastTokens::kMinContrastNormal);
}
TEST_CASE("V27 P18: Reduced motion tokens", "[v27][p18]") {
    REQUIRE(V27ReducedMotionTokens::kTransitionMs == 0);
    REQUIRE(V27ReducedMotionTokens::kFadeMs == 0);
    REQUIRE(V27ReducedMotionTokens::kDisableSpring);
    REQUIRE(V27ReducedMotionTokens::kStaticFocusRing);
}
TEST_CASE("V27 P18: High-contrast tokens", "[v27][p18]") {
    REQUIRE(V27HighContrastTokens::kOverlayOpacityPct > 0);
    REQUIRE(V27HighContrastTokens::kBorderEmphasisWidth > 0);
    REQUIRE(V27HighContrastTokens::kDisabledOpacityPct < 100);
}
TEST_CASE("V27 P18: Screen reader tokens", "[v27][p18]") {
    REQUIRE(V27ScreenReaderTokens::kAriaLabelMaxLen > 0);
    REQUIRE(V27ScreenReaderTokens::kDescriptionMaxLen > V27ScreenReaderTokens::kAriaLabelMaxLen);
    REQUIRE(V27ScreenReaderTokens::kLiveRegionDelayMs > 0);
}
TEST_CASE("V27 P18: Accessibility coordinator registration", "[v27][p18]") {
    V27AccessibilityDesignCoordinator coord;
    coord.register_surface({"Shell", true, true, true, true});
    coord.register_surface({"Editor", true, true, false, true});
    coord.register_surface({"Canvas", false, true, true, false});
    REQUIRE(coord.surface_count() == 3);
    REQUIRE(coord.focus_ring_count() == 2);
    REQUIRE(coord.contrast_pass_count() == 3);
    REQUIRE(coord.reduced_motion_count() == 2);
    REQUIRE(coord.non_color_cue_count() == 2);
    REQUIRE_FALSE(coord.all_meet_criteria());
}
TEST_CASE("V27 P18: Accessibility coordinator all criteria met", "[v27][p18]") {
    V27AccessibilityDesignCoordinator coord;
    coord.register_surface({"Shell", true, true, true, true});
    coord.register_surface({"Editor", true, true, true, true});
    REQUIRE(coord.all_meet_criteria());
}
