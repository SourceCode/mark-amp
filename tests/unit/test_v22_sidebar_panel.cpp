#include "core/SidebarPanelCoordinator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SidebarPanelCoordinator explorer metrics", "[v22][sidebar]")
{
    SidebarPanelCoordinator coord;
    auto metrics = coord.explorer_section_metrics();

    REQUIRE(metrics.section_header_height > 0);
    REQUIRE(metrics.file_row_height > 0);
    REQUIRE(metrics.indent_per_level > 0);
    REQUIRE(metrics.icon_size > 0);
    REQUIRE(metrics.icon_gap > 0);
    REQUIRE(metrics.badge_size > 0);
    REQUIRE(metrics.file_font == TypeScaleToken::kBody);
    REQUIRE(metrics.section_font == TypeScaleToken::kCaption);
}

TEST_CASE("SidebarPanelCoordinator search sidebar metrics", "[v22][sidebar]")
{
    SidebarPanelCoordinator coord;
    auto metrics = coord.search_sidebar_metrics();

    REQUIRE(metrics.input_height > 0);
    REQUIRE(metrics.result_file_height > 0);
    REQUIRE(metrics.result_match_height > 0);
    REQUIRE(metrics.match_indent > 0);
    REQUIRE(metrics.input_corner == CornerRadiusToken::kSm);
    REQUIRE(metrics.input_font == TypeScaleToken::kBody);
    REQUIRE(metrics.match_font == TypeScaleToken::kCaption);
}

TEST_CASE("SidebarPanelCoordinator metadata card metrics", "[v22][sidebar]")
{
    SidebarPanelCoordinator coord;
    auto metrics = coord.metadata_card_metrics();

    REQUIRE(metrics.card_padding > 0);
    REQUIRE(metrics.chip_height > 0);
    REQUIRE(metrics.chip_padding_h > 0);
    REQUIRE(metrics.chip_corner == CornerRadiusToken::kSm);
    REQUIRE(metrics.card_corner == CornerRadiusToken::kMd);
    REQUIRE(metrics.label_font == TypeScaleToken::kCaption);
    REQUIRE(metrics.value_font == TypeScaleToken::kBody);
}

TEST_CASE("SidebarPanelCoordinator panel action metrics", "[v22][sidebar]")
{
    SidebarPanelCoordinator coord;
    auto metrics = coord.panel_action_metrics();

    REQUIRE(metrics.action_button_size > 0);
    REQUIRE(metrics.badge_min_width > 0);
    REQUIRE(metrics.badge_dot_size > 0);
    REQUIRE(metrics.badge_corner == CornerRadiusToken::kPill);
}
