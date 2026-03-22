#include "core/CommandSurfaceCoordinator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("CommandSurfaceCoordinator menu item metrics", "[v22][command_surface]")
{
    CommandSurfaceCoordinator coord;
    auto metrics = coord.menu_item_metrics();

    REQUIRE(metrics.height > 0);
    REQUIRE(metrics.icon_size > 0);
    REQUIRE(metrics.icon_gap > 0);
    REQUIRE(metrics.padding_h > 0);
    REQUIRE(metrics.separator_height > 0);
    REQUIRE(metrics.shortcut_gap > 0);
    REQUIRE(metrics.hover_radius == CornerRadiusToken::kSm);
}

TEST_CASE("CommandSurfaceCoordinator palette result metrics", "[v22][command_surface]")
{
    CommandSurfaceCoordinator coord;
    auto metrics = coord.palette_result_metrics();

    REQUIRE(metrics.height > 0);
    REQUIRE(metrics.icon_size > 0);
    REQUIRE(metrics.label_font == TypeScaleToken::kBody);
    REQUIRE(metrics.detail_font == TypeScaleToken::kCaption);
    REQUIRE(metrics.container_radius == CornerRadiusToken::kLg);
    REQUIRE(metrics.container_elevation == ElevationToken::kHigh);
}

TEST_CASE("CommandSurfaceCoordinator context menu metrics", "[v22][command_surface]")
{
    CommandSurfaceCoordinator coord;
    auto metrics = coord.context_menu_metrics();

    REQUIRE(metrics.min_width > 0);
    REQUIRE(metrics.max_width > metrics.min_width);
    REQUIRE(metrics.item_height > 0);
    REQUIRE(metrics.corner == CornerRadiusToken::kMd);
    REQUIRE(metrics.elevation == ElevationToken::kMedium);
    REQUIRE(metrics.border == BorderWeightToken::kThin);
}

TEST_CASE("CommandSurfaceCoordinator tooltip metrics", "[v22][command_surface]")
{
    CommandSurfaceCoordinator coord;
    auto metrics = coord.tooltip_metrics();

    REQUIRE(metrics.max_width > 0);
    REQUIRE(metrics.delay_ms > 0);
    REQUIRE(metrics.fade_duration_ms > 0);
    REQUIRE(metrics.corner == CornerRadiusToken::kSm);
    REQUIRE(metrics.elevation == ElevationToken::kMedium);
    REQUIRE(metrics.font == TypeScaleToken::kCaption);
}

TEST_CASE("CommandSurfaceCoordinator shortcut chip style", "[v22][command_surface]")
{
    CommandSurfaceCoordinator coord;
    auto style = coord.shortcut_chip_style();

    REQUIRE(style.padding_h > 0);
    REQUIRE(style.padding_v > 0);
    REQUIRE(style.corner == CornerRadiusToken::kSm);
    REQUIRE(style.border == BorderWeightToken::kThin);
    REQUIRE(style.font == TypeScaleToken::kCaption);
}
