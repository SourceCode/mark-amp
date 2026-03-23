/// @file test_v26_p14_startup.cpp
/// @brief V26 Phase 14: Startup, welcome, onboarding & empty-state elegance tests.
#include <catch2/catch_test_macros.hpp>
#include "ui/V26StartupThemeConvergence.h"

using namespace markamp::ui;

TEST_CASE("V26 P14: Startup shell dimensions", "[v26][p14]")
{
    REQUIRE(StartupV26Metrics::kMaxContentWidth > 0);
    REQUIRE(StartupV26Metrics::kPaddingH > 0);
    REQUIRE(StartupV26Metrics::kPaddingV > 0);
}

TEST_CASE("V26 P14: Hero typography", "[v26][p14]")
{
    REQUIRE(StartupV26Metrics::kHeroTitlePt > StartupV26Metrics::kHeroSubtitlePt);
    REQUIRE(StartupV26Metrics::kLogoSize > 0);
    REQUIRE(StartupV26Metrics::kHeroGap > 0);
}

TEST_CASE("V26 P14: Action cards", "[v26][p14]")
{
    REQUIRE(StartupV26Metrics::kActionCardW > 0);
    REQUIRE(StartupV26Metrics::kActionCardH > 0);
    REQUIRE(StartupV26Metrics::kActionCardRadius > 0);
    REQUIRE(StartupV26Metrics::kActionTitlePt > StartupV26Metrics::kActionDescPt);
}

TEST_CASE("V26 P14: Recent files list", "[v26][p14]")
{
    REQUIRE(StartupV26Metrics::kRecentRowH > 0);
    REQUIRE(StartupV26Metrics::kRecentMaxItems > 0);
}

TEST_CASE("V26 P14: Empty state illustration", "[v26][p14]")
{
    REQUIRE(EmptyStateV26Metrics::kIllustrationSize > 0);
    REQUIRE(EmptyStateV26Metrics::kIllustrationAlpha > 0.0F);
    REQUIRE(EmptyStateV26Metrics::kIllustrationAlpha < 1.0F);
}

TEST_CASE("V26 P14: Onboarding coachmark", "[v26][p14]")
{
    REQUIRE(OnboardingV26Metrics::kCoachmarkMaxW > 0);
    REQUIRE(OnboardingV26Metrics::kCoachmarkRadius > 0);
    REQUIRE(OnboardingV26Metrics::kCoachmarkTitlePt > OnboardingV26Metrics::kCoachmarkBodyPt);
}

TEST_CASE("V26 P14: Onboarding dots opacity", "[v26][p14]")
{
    REQUIRE(OnboardingV26Metrics::kDotInactiveAlpha < OnboardingV26Metrics::kDotActiveAlpha);
}
