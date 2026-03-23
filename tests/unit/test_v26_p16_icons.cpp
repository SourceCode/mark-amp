/// @file test_v26_p16_icons.cpp
/// @brief V26 Phase 16: Iconography, optical alignment & semantic completion tests.
#include <catch2/catch_test_macros.hpp>
#include "ui/V26IconMotionConvergence.h"

using namespace markamp::ui;

TEST_CASE("V26 P16: Icon size role count", "[v26][p16]")
{
    REQUIRE(IconConvergenceMetrics::role_count() == 6);
}

TEST_CASE("V26 P16: Icon sizes are monotonically increasing", "[v26][p16]")
{
    REQUIRE(IconConvergenceMetrics::kTinySize < IconConvergenceMetrics::kSmallSize);
    REQUIRE(IconConvergenceMetrics::kSmallSize < IconConvergenceMetrics::kMediumSize);
    REQUIRE(IconConvergenceMetrics::kMediumSize < IconConvergenceMetrics::kLargeSize);
    REQUIRE(IconConvergenceMetrics::kLargeSize < IconConvergenceMetrics::kHeroSize);
    REQUIRE(IconConvergenceMetrics::kHeroSize < IconConvergenceMetrics::kDisplaySize);
}

TEST_CASE("V26 P16: Icon size role lookup", "[v26][p16]")
{
    REQUIRE(IconConvergenceMetrics::size_for_role(IconSizeRole::kTiny) == 12);
    REQUIRE(IconConvergenceMetrics::size_for_role(IconSizeRole::kMedium) == 16);
    REQUIRE(IconConvergenceMetrics::size_for_role(IconSizeRole::kDisplay) == 32);
}

TEST_CASE("V26 P16: Optical alignment nudge", "[v26][p16]")
{
    REQUIRE(IconConvergenceMetrics::kOpticalNudge == 1);
    REQUIRE(IconConvergenceMetrics::kTextGapH > IconConvergenceMetrics::kTextGapHSmall);
}

TEST_CASE("V26 P16: Hit target meets minimum", "[v26][p16]")
{
    REQUIRE(IconConvergenceMetrics::kHitTargetMin >= 24);
    REQUIRE(IconConvergenceMetrics::kHitTargetIdeal >= IconConvergenceMetrics::kHitTargetMin);
}

TEST_CASE("V26 P16: Icon category enum", "[v26][p16]")
{
    REQUIRE(static_cast<int>(IconCategory::kNavigation) == 0);
    REQUIRE(static_cast<int>(IconCategory::kUI) == 4);
}
