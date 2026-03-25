/// @file test_v27_p16_onboarding.cpp
/// @brief V27 Phase 16: Startup, empty states, onboarding tokens, coordinator.
#include <catch2/catch_test_macros.hpp>
#include "ui/V27OnboardingTokens.h"
#include "core/V27OnboardingDesignCoordinator.h"
using namespace markamp::ui;
using namespace markamp::core;
TEST_CASE("V27 P16: Startup tokens", "[v27][p16]") {
    REQUIRE(V27StartupTokens::kMaxWidth > 0);
    REQUIRE(V27StartupTokens::kLogoSize > 0);
    REQUIRE(V27StartupTokens::kActionButtonHeight > 0);
}
TEST_CASE("V27 P16: Empty state variant count", "[v27][p16]") { REQUIRE(v27_empty_state_variant_count() == 3); }
TEST_CASE("V27 P16: Empty state template tokens", "[v27][p16]") {
    REQUIRE(V27EmptyStateTemplateTokens::kFullPageIconSize > V27EmptyStateTemplateTokens::kPanelIconSize);
    REQUIRE(V27EmptyStateTemplateTokens::kPanelIconSize > V27EmptyStateTemplateTokens::kCompactIconSize);
}
TEST_CASE("V27 P16: Onboarding hint tokens", "[v27][p16]") {
    REQUIRE(V27OnboardingHintTokens::kHintMaxWidth > 0);
    REQUIRE(V27OnboardingHintTokens::kHintRadius > 0);
    REQUIRE(V27OnboardingHintTokens::kStepIndicatorSize > 0);
}
TEST_CASE("V27 P16: Welcome animation tokens", "[v27][p16]") {
    REQUIRE(V27WelcomeAnimationTokens::kFadeInMs > 0);
    REQUIRE(V27WelcomeAnimationTokens::kStaggerDelayMs > 0);
    REQUIRE(V27WelcomeAnimationTokens::kLogoPulseMs > V27WelcomeAnimationTokens::kFadeInMs);
    REQUIRE(V27WelcomeAnimationTokens::kSkeletonShimmerMs > 0);
}
TEST_CASE("V27 P16: Onboarding coordinator registration", "[v27][p16]") {
    V27OnboardingDesignCoordinator coord;
    coord.register_surface({"Welcome Screen", OnboardingSurfaceType::kStartup, true, true});
    coord.register_surface({"Explorer Empty", OnboardingSurfaceType::kEmptyState, true, true});
    coord.register_surface({"Canvas Hint", OnboardingSurfaceType::kHint, true, false});
    REQUIRE(coord.surface_count() == 3);
    REQUIRE(coord.redesigned_count() == 2);
    REQUIRE(coord.canonical_icon_count() == 3);
    REQUIRE(coord.coverage_pct() == 66);
}
TEST_CASE("V27 P16: Onboarding coordinator type filter", "[v27][p16]") {
    V27OnboardingDesignCoordinator coord;
    coord.register_surface({"Startup", OnboardingSurfaceType::kStartup, true, true});
    coord.register_surface({"Panel Empty", OnboardingSurfaceType::kEmptyState, true, true});
    coord.register_surface({"Table Empty", OnboardingSurfaceType::kEmptyState, true, true});
    REQUIRE(coord.count_by_type(OnboardingSurfaceType::kStartup) == 1);
    REQUIRE(coord.count_by_type(OnboardingSurfaceType::kEmptyState) == 2);
    REQUIRE(coord.count_by_type(OnboardingSurfaceType::kHint) == 0);
}
