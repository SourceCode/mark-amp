#include "core/CanvasChromeCoordinator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("CanvasChromeCoordinator workspace frame metrics", "[v22][canvas]")
{
    CanvasChromeCoordinator coord;
    auto metrics = coord.workspace_frame_metrics();

    REQUIRE(metrics.toolbar_height > 0);
    REQUIRE(metrics.tool_palette_width > 0);
    REQUIRE(metrics.mini_map_size > 0);
    REQUIRE(metrics.zoom_control_width > 0);
    REQUIRE(metrics.zoom_button_size > 0);
}

TEST_CASE("CanvasChromeCoordinator tool palette metrics", "[v22][canvas]")
{
    CanvasChromeCoordinator coord;
    auto metrics = coord.tool_palette_metrics();

    REQUIRE(metrics.tool_button_size > 0);
    REQUIRE(metrics.tool_button_gap > 0);
    REQUIRE(metrics.separator_height > 0);
    REQUIRE(metrics.group_gap > 0);
    REQUIRE(metrics.button_corner == CornerRadiusToken::kSm);
    REQUIRE(metrics.icon_size == IconMetricToken::kMedium);
}

TEST_CASE("CanvasChromeCoordinator selection visual metrics", "[v22][canvas]")
{
    CanvasChromeCoordinator coord;
    auto metrics = coord.selection_visual_metrics();

    REQUIRE(metrics.handle_size > 0);
    REQUIRE(metrics.rotation_handle_offset > 0);
    REQUIRE(metrics.snap_guide_thickness > 0);
    REQUIRE(metrics.selection_border_width > 0);
    REQUIRE(metrics.snap_guide_opacity > 0.0F);
    REQUIRE(metrics.multi_select_fill_opacity > 0.0F);
    REQUIRE(metrics.handle_corner == CornerRadiusToken::kSm);
}

TEST_CASE("CanvasChromeCoordinator object chrome metrics", "[v22][canvas]")
{
    CanvasChromeCoordinator coord;
    auto metrics = coord.object_chrome_metrics();

    REQUIRE(metrics.inspector_width > 0);
    REQUIRE(metrics.inspector_section_gap > 0);
    REQUIRE(metrics.property_row_height > 0);
    REQUIRE(metrics.property_label_width > 0);
    REQUIRE(metrics.color_swatch_size > 0);
    REQUIRE(metrics.section_font == TypeScaleToken::kSubtitle);
    REQUIRE(metrics.property_font == TypeScaleToken::kBody);
}

TEST_CASE("CanvasChromeCoordinator collab overlay metrics", "[v22][canvas]")
{
    CanvasChromeCoordinator coord;
    auto metrics = coord.collab_overlay_metrics();

    REQUIRE(metrics.cursor_label_height > 0);
    REQUIRE(metrics.cursor_label_max_width > 0);
    REQUIRE(metrics.cursor_caret_width > 0);
    REQUIRE(metrics.cursor_caret_height > 0);
    REQUIRE(metrics.selection_fill_opacity > 0.0F);
    REQUIRE(metrics.label_corner == CornerRadiusToken::kSm);
    REQUIRE(metrics.label_font == TypeScaleToken::kCaption);
}

TEST_CASE("CanvasChromeCoordinator empty board config", "[v22][canvas]")
{
    CanvasChromeCoordinator coord;
    auto config = coord.empty_board_config();

    REQUIRE_FALSE(config.title.empty());
    REQUIRE_FALSE(config.subtitle.empty());
    REQUIRE_FALSE(config.icon_name.empty());
    REQUIRE(config.icon_size == IconMetricToken::kHero);
    REQUIRE(config.title_font == TypeScaleToken::kTitle);
}

TEST_CASE("CanvasChromeCoordinator tool corner for state", "[v22][canvas]")
{
    CanvasChromeCoordinator coord;

    SECTION("Idle uses default button corner")
    {
        auto corner = coord.tool_corner_for_state(CanvasChromeCoordinator::ToolState::kIdle);
        REQUIRE(corner == CornerRadiusToken::kSm);
    }

    SECTION("Active uses medium corner")
    {
        auto corner = coord.tool_corner_for_state(CanvasChromeCoordinator::ToolState::kActive);
        REQUIRE(corner == CornerRadiusToken::kMd);
    }

    SECTION("All states resolve without crash")
    {
        for (auto state : {CanvasChromeCoordinator::ToolState::kIdle,
                           CanvasChromeCoordinator::ToolState::kActive,
                           CanvasChromeCoordinator::ToolState::kHover,
                           CanvasChromeCoordinator::ToolState::kDisabled})
        {
            auto corner = coord.tool_corner_for_state(state);
            (void)corner; // Just ensure no crash
        }
    }
}
