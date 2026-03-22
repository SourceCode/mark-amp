#include "core/ThemeParityCoordinator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ThemeParityCoordinator token coverage audit", "[v22][theme_parity]")
{
    ThemeParityCoordinator coord;
    auto result = coord.audit_token_coverage();

    REQUIRE(result.total_tokens > 0);
    REQUIRE(result.themed_tokens == result.total_tokens);
    REQUIRE(result.hardcoded_tokens == 0);
    REQUIRE(result.coverage_percent == 100.0F);
}

TEST_CASE("ThemeParityCoordinator dark theme parity", "[v22][theme_parity]")
{
    ThemeParityCoordinator coord;
    auto summary = coord.check_parity(ThemeParityCoordinator::ThemeVariant::kDark);

    REQUIRE(summary.surfaces_checked > 0);
    REQUIRE(summary.surfaces_passing == summary.surfaces_checked);
    REQUIRE(summary.contrast_failures == 0);
    REQUIRE(summary.parity_score == 100.0F);
}

TEST_CASE("ThemeParityCoordinator light theme parity", "[v22][theme_parity]")
{
    ThemeParityCoordinator coord;
    auto summary = coord.check_parity(ThemeParityCoordinator::ThemeVariant::kLight);

    REQUIRE(summary.surfaces_passing == summary.surfaces_checked);
    REQUIRE(summary.parity_score == 100.0F);
}

TEST_CASE("ThemeParityCoordinator high contrast parity", "[v22][theme_parity]")
{
    ThemeParityCoordinator coord;

    auto hc_dark = coord.check_parity(ThemeParityCoordinator::ThemeVariant::kHighContrastDark);
    auto hc_light = coord.check_parity(ThemeParityCoordinator::ThemeVariant::kHighContrastLight);

    REQUIRE(hc_dark.surfaces_passing == hc_dark.surfaces_checked);
    REQUIRE(hc_light.surfaces_passing == hc_light.surfaces_checked);
}

TEST_CASE("ThemeParityCoordinator theme preview config", "[v22][theme_parity]")
{
    ThemeParityCoordinator coord;
    auto config = coord.theme_preview_config();

    REQUIRE(config.preview_width > 0);
    REQUIRE(config.preview_height > 0);
    REQUIRE(config.color_swatch_size > 0);
    REQUIRE(config.sample_code_lines > 0);
    REQUIRE(config.preview_corner == CornerRadiusToken::kMd);
}
