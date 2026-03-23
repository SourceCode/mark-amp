/// @file test_v26_p18_accessibility.cpp
/// @brief V26 Phase 18: Accessibility, visual contrast, focus & reduced motion tests.
#include <catch2/catch_test_macros.hpp>
#include "ui/V26AccessibilityPolishConvergence.h"

using namespace markamp::ui;

TEST_CASE("V26 P18: Focus ring dimensions", "[v26][p18]")
{
    REQUIRE(FocusRingStandards::kRingWidth > 0);
    REQUIRE(FocusRingStandards::kRingOffset > 0);
    REQUIRE(FocusRingStandards::kRingAlpha > 0.5F);
}

TEST_CASE("V26 P18: Focus ring keyboard-only policy", "[v26][p18]")
{
    REQUIRE(FocusRingStandards::kKeyboardOnly == true);
}

TEST_CASE("V26 P18: WCAG AA contrast ratios", "[v26][p18]")
{
    REQUIRE(ContrastStandards::kNormalTextRatio >= 4.5F);
    REQUIRE(ContrastStandards::kLargeTextRatio >= 3.0F);
    REQUIRE(ContrastStandards::kUIComponentRatio >= 3.0F);
}

TEST_CASE("V26 P18: Contrast AA validation", "[v26][p18]")
{
    REQUIRE(ContrastStandards::passes_aa(5.0F));
    REQUIRE(ContrastStandards::passes_aa(4.5F));
    REQUIRE_FALSE(ContrastStandards::passes_aa(4.4F));
}

TEST_CASE("V26 P18: Non-color differentiation", "[v26][p18]")
{
    REQUIRE(ContrastStandards::kMinPatternTypes >= 2);
    REQUIRE(ContrastStandards::kMinStatusIndicators >= 2);
}

TEST_CASE("V26 P18: Reduced motion rules", "[v26][p18]")
{
    REQUIRE(ReducedMotionRules::kMaxAnimationMs == 0);
    REQUIRE(ReducedMotionRules::kInstantTransitions == true);
    REQUIRE(ReducedMotionRules::kDisableParallax == true);
    REQUIRE(ReducedMotionRules::kStaticProgressBars == true);
}
