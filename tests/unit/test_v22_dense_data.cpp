#include "core/DenseDataCoordinator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("DenseDataCoordinator row metrics", "[v22][dense_data]")
{
    DenseDataCoordinator coord;
    auto metrics = coord.dense_row_metrics();

    REQUIRE(metrics.tree_row_height > 0);
    REQUIRE(metrics.list_row_height > 0);
    REQUIRE(metrics.table_row_height > 0);
    // Tree rows are tightest, tables are largest
    REQUIRE(metrics.tree_row_height <= metrics.list_row_height);
    REQUIRE(metrics.list_row_height <= metrics.table_row_height);
    REQUIRE(metrics.indent_per_level > 0);
    REQUIRE(metrics.hover_bg_opacity > 0.0F);
    REQUIRE(metrics.selection_bg_opacity > metrics.hover_bg_opacity);
}

TEST_CASE("DenseDataCoordinator table surface metrics", "[v22][dense_data]")
{
    DenseDataCoordinator coord;
    auto metrics = coord.table_surface_metrics();

    REQUIRE(metrics.header_height > 0);
    REQUIRE(metrics.cell_padding_h > 0);
    REQUIRE(metrics.cell_padding_v > 0);
    REQUIRE(metrics.resize_handle_width > 0);
    REQUIRE(metrics.column_min_width > 0);
    REQUIRE(metrics.header_border == BorderWeightToken::kThin);
    REQUIRE(metrics.header_font == TypeScaleToken::kCaption);
    REQUIRE(metrics.cell_font == TypeScaleToken::kBody);
}

TEST_CASE("DenseDataCoordinator gallery card metrics", "[v22][dense_data]")
{
    DenseDataCoordinator coord;
    auto metrics = coord.gallery_card_metrics();

    REQUIRE(metrics.card_min_width > 0);
    REQUIRE(metrics.card_max_width > metrics.card_min_width);
    REQUIRE(metrics.card_gap > 0);
    REQUIRE(metrics.cover_height > 0);
    REQUIRE(metrics.card_corner == CornerRadiusToken::kMd);
    REQUIRE(metrics.card_elevation == ElevationToken::kLow);
}

TEST_CASE("DenseDataCoordinator kanban lane metrics", "[v22][dense_data]")
{
    DenseDataCoordinator coord;
    auto metrics = coord.kanban_lane_metrics();

    REQUIRE(metrics.lane_min_width > 0);
    REQUIRE(metrics.lane_max_width > metrics.lane_min_width);
    REQUIRE(metrics.lane_header_height > 0);
    REQUIRE(metrics.card_gap > 0);
    REQUIRE(metrics.lane_corner == CornerRadiusToken::kMd);
    REQUIRE(metrics.card_corner == CornerRadiusToken::kSm);
    REQUIRE(metrics.header_font == TypeScaleToken::kSubtitle);
}

TEST_CASE("DenseDataCoordinator table overlay metrics", "[v22][dense_data]")
{
    DenseDataCoordinator coord;
    auto metrics = coord.table_overlay_metrics();

    REQUIRE(metrics.filter_chip_height > 0);
    REQUIRE(metrics.filter_chip_padding_h > 0);
    REQUIRE(metrics.sort_menu_width > 0);
    REQUIRE(metrics.column_picker_width > 0);
    REQUIRE(metrics.chip_corner == CornerRadiusToken::kSm);
    REQUIRE(metrics.chip_font == TypeScaleToken::kCaption);
}
