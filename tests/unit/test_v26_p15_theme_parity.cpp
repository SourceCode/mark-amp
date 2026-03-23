/// @file test_v26_p15_theme_parity.cpp
/// @brief V26 Phase 15: Theme parity, token adoption & cross-surface correctness tests.
#include <catch2/catch_test_macros.hpp>
#include "ui/V26StartupThemeConvergence.h"

using namespace markamp::ui;

TEST_CASE("V26 P15: WCAG AA contrast ratios", "[v26][p15]")
{
    REQUIRE(ThemeParityRules::kTextOnBgMinContrast >= 4.5F);
    REQUIRE(ThemeParityRules::kLargeTextMinContrast >= 3.0F);
    REQUIRE(ThemeParityRules::kUIComponentMinContrast >= 3.0F);
}

TEST_CASE("V26 P15: Text contrast validation", "[v26][p15]")
{
    REQUIRE(ThemeParityRules::is_text_contrast_valid(5.0F));
    REQUIRE(ThemeParityRules::is_text_contrast_valid(4.5F));
    REQUIRE_FALSE(ThemeParityRules::is_text_contrast_valid(4.4F));
}

TEST_CASE("V26 P15: Large text contrast validation", "[v26][p15]")
{
    REQUIRE(ThemeParityRules::is_large_text_contrast_valid(3.5F));
    REQUIRE(ThemeParityRules::is_large_text_contrast_valid(3.0F));
    REQUIRE_FALSE(ThemeParityRules::is_large_text_contrast_valid(2.9F));
}

TEST_CASE("V26 P15: Token adoption targets are zero", "[v26][p15]")
{
    REQUIRE(ThemeParityRules::kMaxHardcodedColors == 0);
    REQUIRE(ThemeParityRules::kMaxLocalDerivations == 0);
}

TEST_CASE("V26 P15: Required state coverage", "[v26][p15]")
{
    REQUIRE(ThemeParityRules::kRequiredStates == 5);
}

TEST_CASE("V26 P15: Contrast check count", "[v26][p15]")
{
    REQUIRE(ThemeParityRules::contrast_check_count() == 3);
}
