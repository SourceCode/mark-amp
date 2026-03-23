/// @file test_v27_p18_accessibility.cpp
/// @brief V27 Phase 18: Focus ring, contrast, reduced motion tokens.
#include <catch2/catch_test_macros.hpp>
#include "ui/V27AccessibilityTokens.h"
using namespace markamp::ui;
TEST_CASE("V27 P18: Focus ring tokens", "[v27][p18]") {
    REQUIRE(V27FocusRingTokens::kThickness > 0);
    REQUIRE(V27FocusRingTokens::kHighContrastThickness >= V27FocusRingTokens::kThickness);
}
TEST_CASE("V27 P18: Contrast ratio tokens (WCAG)", "[v27][p18]") {
    REQUIRE(V27ContrastTokens::kMinContrastNormal >= 45); // 4.5:1
    REQUIRE(V27ContrastTokens::kMinContrastLarge >= 30);  // 3.0:1
    REQUIRE(V27ContrastTokens::kEnhancedContrastNormal > V27ContrastTokens::kMinContrastNormal);
}
TEST_CASE("V27 P18: Reduced motion tokens", "[v27][p18]") {
    REQUIRE(V27ReducedMotionTokens::kTransitionMs == 0);
    REQUIRE(V27ReducedMotionTokens::kFadeMs == 0);
    REQUIRE(V27ReducedMotionTokens::kDisableSpring);
    REQUIRE(V27ReducedMotionTokens::kStaticFocusRing);
}
