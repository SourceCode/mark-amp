/// @file test_v27_p16_onboarding.cpp
/// @brief V27 Phase 16: Startup, empty states, onboarding tokens.
#include <catch2/catch_test_macros.hpp>
#include "ui/V27OnboardingTokens.h"
using namespace markamp::ui;
TEST_CASE("V27 P16: Startup tokens", "[v27][p16]") {
    REQUIRE(V27StartupTokens::kMaxWidth > 0);
    REQUIRE(V27StartupTokens::kLogoSize > 0);
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
