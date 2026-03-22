#include "core/UtilityPanelCoordinator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("UtilityPanelCoordinator terminal metrics", "[v22][utility]")
{
    UtilityPanelCoordinator coord;
    auto metrics = coord.terminal_metrics();

    REQUIRE(metrics.tab_height > 0);
    REQUIRE(metrics.shell_padding_h > 0);
    REQUIRE(metrics.cursor_width > 0);
    REQUIRE(metrics.scrollbar_width > 0);
    REQUIRE(metrics.terminal_font == TypeScaleToken::kBody);
    REQUIRE(metrics.tab_font == TypeScaleToken::kCaption);
}

TEST_CASE("UtilityPanelCoordinator output panel metrics", "[v22][utility]")
{
    UtilityPanelCoordinator coord;
    auto metrics = coord.output_panel_metrics();

    REQUIRE(metrics.row_height > 0);
    REQUIRE(metrics.timestamp_width > 0);
    REQUIRE(metrics.padding_h > 0);
    REQUIRE(metrics.log_font == TypeScaleToken::kBody);
    REQUIRE(metrics.timestamp_font == TypeScaleToken::kCaption);
}

TEST_CASE("UtilityPanelCoordinator problems panel metrics", "[v22][utility]")
{
    UtilityPanelCoordinator coord;
    auto metrics = coord.problems_panel_metrics();

    REQUIRE(metrics.row_height > 0);
    REQUIRE(metrics.severity_icon_size > 0);
    REQUIRE(metrics.file_path_width > 0);
    REQUIRE(metrics.message_font == TypeScaleToken::kBody);
    REQUIRE(metrics.path_font == TypeScaleToken::kCaption);
}

TEST_CASE("UtilityPanelCoordinator bottom tab strip metrics", "[v22][utility]")
{
    UtilityPanelCoordinator coord;
    auto metrics = coord.bottom_tab_strip_metrics();

    REQUIRE(metrics.tab_height > 0);
    REQUIRE(metrics.tab_min_width > 0);
    REQUIRE(metrics.badge_size > 0);
    REQUIRE(metrics.badge_corner == CornerRadiusToken::kPill);
}

TEST_CASE("UtilityPanelCoordinator severity icon sizing", "[v22][utility]")
{
    UtilityPanelCoordinator coord;

    // Error and Warning get larger icons
    REQUIRE(coord.severity_icon_size(UtilityPanelCoordinator::SeverityLevel::kError) == 16);
    REQUIRE(coord.severity_icon_size(UtilityPanelCoordinator::SeverityLevel::kWarning) == 16);
    // Info and Hint are slightly smaller
    REQUIRE(coord.severity_icon_size(UtilityPanelCoordinator::SeverityLevel::kInfo) == 14);
    REQUIRE(coord.severity_icon_size(UtilityPanelCoordinator::SeverityLevel::kHint) == 14);
}
