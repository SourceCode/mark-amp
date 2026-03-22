#include "core/SettingsSurfaceCoordinator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SettingsSurfaceCoordinator category nav metrics", "[v22][settings]")
{
    SettingsSurfaceCoordinator coord;
    auto metrics = coord.category_nav_metrics();

    REQUIRE(metrics.nav_width > 0);
    REQUIRE(metrics.category_row_height > 0);
    REQUIRE(metrics.category_indent > 0);
    REQUIRE(metrics.icon_size > 0);
    REQUIRE(metrics.category_font == TypeScaleToken::kBody);
    REQUIRE(metrics.group_font == TypeScaleToken::kSubtitle);
}

TEST_CASE("SettingsSurfaceCoordinator settings row metrics", "[v22][settings]")
{
    SettingsSurfaceCoordinator coord;
    auto metrics = coord.settings_row_metrics();

    REQUIRE(metrics.row_min_height > 0);
    REQUIRE(metrics.label_width > 0);
    REQUIRE(metrics.description_max_width > 0);
    REQUIRE(metrics.toggle_width > 0);
    REQUIRE(metrics.toggle_height > 0);
    REQUIRE(metrics.input_width > 0);
    REQUIRE(metrics.input_corner == CornerRadiusToken::kSm);
    REQUIRE(metrics.toggle_corner == CornerRadiusToken::kPill);
}

TEST_CASE("SettingsSurfaceCoordinator keybinding editor metrics", "[v22][settings]")
{
    SettingsSurfaceCoordinator coord;
    auto metrics = coord.keybinding_editor_metrics();

    REQUIRE(metrics.row_height > 0);
    REQUIRE(metrics.key_chip_height > 0);
    REQUIRE(metrics.key_chip_padding_h > 0);
    REQUIRE(metrics.search_input_height > 0);
    REQUIRE(metrics.key_chip_corner == CornerRadiusToken::kSm);
    REQUIRE(metrics.key_chip_border == BorderWeightToken::kThin);
}

TEST_CASE("SettingsSurfaceCoordinator theme gallery metrics", "[v22][settings]")
{
    SettingsSurfaceCoordinator coord;
    auto metrics = coord.theme_gallery_metrics();

    REQUIRE(metrics.card_width > 0);
    REQUIRE(metrics.card_height > 0);
    REQUIRE(metrics.card_height > metrics.preview_height);
    REQUIRE(metrics.card_gap > 0);
    REQUIRE(metrics.card_corner == CornerRadiusToken::kMd);
    REQUIRE(metrics.card_elevation == ElevationToken::kLow);
    REQUIRE(metrics.card_border == BorderWeightToken::kThin);
}
