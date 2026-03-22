#include "core/PanelChromeCoordinator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("PanelChromeCoordinator panel header config", "[v22][panel_chrome]")
{
    PanelChromeCoordinator coord;
    auto config = coord.panel_header_config("explorer");

    REQUIRE(config.height == 30);
    REQUIRE(config.title_font == TypeScaleToken::kSubtitle);
    REQUIRE(config.border == BorderWeightToken::kThin);
    REQUIRE(config.action_button_size > 0);
    REQUIRE(config.action_button_gap > 0);
    REQUIRE(config.title_padding_left > 0);
}

TEST_CASE("PanelChromeCoordinator sidebar rhythm rules", "[v22][panel_chrome]")
{
    PanelChromeCoordinator coord;
    auto rhythm = coord.sidebar_section_rhythm();

    REQUIRE(rhythm.section_gap > 0);
    REQUIRE(rhythm.section_header_height > 0);
    REQUIRE(rhythm.section_indent > 0);
    REQUIRE(rhythm.section_font == TypeScaleToken::kCaption);
    REQUIRE(rhythm.show_section_separator);
}

TEST_CASE("PanelChromeCoordinator set sidebar rhythm", "[v22][panel_chrome]")
{
    PanelChromeCoordinator coord;
    PanelChromeCoordinator::SidebarRhythmConfig custom;
    custom.section_gap = 16;
    custom.section_header_height = 28;
    coord.set_sidebar_rhythm(custom);

    auto rhythm = coord.sidebar_section_rhythm();
    REQUIRE(rhythm.section_gap == 16);
    REQUIRE(rhythm.section_header_height == 28);
}

TEST_CASE("PanelChromeCoordinator loading state config", "[v22][panel_chrome]")
{
    PanelChromeCoordinator coord;
    auto config = coord.loading_state_config("explorer");

    REQUIRE(config.skeleton_style == "shimmer");
    REQUIRE(config.skeleton_rows > 0);
    REQUIRE(config.row_height > 0);
    REQUIRE(config.animation_duration_ms > 0);
}

TEST_CASE("PanelChromeCoordinator empty state config", "[v22][panel_chrome]")
{
    PanelChromeCoordinator coord;

    SECTION("Explorer panel")
    {
        auto config = coord.empty_state_config("explorer");
        REQUIRE_FALSE(config.title.empty());
        REQUIRE_FALSE(config.icon_name.empty());
        REQUIRE_FALSE(config.action_label.empty());
    }

    SECTION("Search panel")
    {
        auto config = coord.empty_state_config("search");
        REQUIRE_FALSE(config.title.empty());
    }

    SECTION("Graph panel")
    {
        auto config = coord.empty_state_config("graph");
        REQUIRE_FALSE(config.title.empty());
    }

    SECTION("Unknown panel has fallback")
    {
        auto config = coord.empty_state_config("unknown_panel");
        REQUIRE_FALSE(config.title.empty());
    }
}
