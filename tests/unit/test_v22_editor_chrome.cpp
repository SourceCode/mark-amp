#include "core/EditorChromeCoordinator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("EditorChromeCoordinator editor frame metrics", "[v22][editor_chrome]")
{
    EditorChromeCoordinator coord;
    auto metrics = coord.editor_frame_metrics();

    REQUIRE(metrics.gutter_width > 0);
    REQUIRE(metrics.line_number_width > 0);
    REQUIRE(metrics.fold_marker_width > 0);
    REQUIRE(metrics.decoration_gutter_width > 0);
    REQUIRE(metrics.content_left_margin > 0);
    REQUIRE(metrics.content_right_margin > 0);
    REQUIRE(metrics.line_number_font == TypeScaleToken::kCaption);

    // Gutter components should sum to gutter_width
    REQUIRE(metrics.gutter_width >= metrics.fold_marker_width + metrics.decoration_gutter_width);
}

TEST_CASE("EditorChromeCoordinator caret selection style", "[v22][editor_chrome]")
{
    EditorChromeCoordinator coord;
    auto style = coord.caret_selection_style();

    REQUIRE(style.caret_width > 0);
    REQUIRE(style.caret_blink_rate_ms > 0);
    REQUIRE(style.selection_opacity > 0.0F);
    REQUIRE(style.selection_opacity <= 1.0F);
    REQUIRE(style.line_highlight_opacity > 0.0F);
    REQUIRE(style.line_highlight_opacity < style.selection_opacity);
}

TEST_CASE("EditorChromeCoordinator minimap metrics", "[v22][editor_chrome]")
{
    EditorChromeCoordinator coord;
    auto metrics = coord.minimap_metrics();

    REQUIRE(metrics.width > 0);
    REQUIRE(metrics.slider_min_height > 0);
    REQUIRE(metrics.char_width > 0.0F);
    REQUIRE(metrics.char_height > 0.0F);
    REQUIRE(metrics.slider_opacity > 0.0F);
    REQUIRE(metrics.slider_hover_opacity > metrics.slider_opacity);
}

TEST_CASE("EditorChromeCoordinator overview ruler metrics", "[v22][editor_chrome]")
{
    EditorChromeCoordinator coord;
    auto metrics = coord.overview_ruler_metrics();

    REQUIRE(metrics.width > 0);
    REQUIRE(metrics.marker_height > 0);
    REQUIRE(metrics.marker_corner == CornerRadiusToken::kSm);
}

TEST_CASE("EditorChromeCoordinator inline furniture metrics", "[v22][editor_chrome]")
{
    EditorChromeCoordinator coord;
    auto metrics = coord.inline_furniture_metrics();

    REQUIRE(metrics.indent_guide_width > 0);
    REQUIRE(metrics.bracket_pair_width > 0);
    REQUIRE(metrics.ruler_width > 0);
    REQUIRE(metrics.indent_guide_opacity > 0.0F);
    REQUIRE(metrics.indent_guide_opacity < 1.0F);
    REQUIRE(metrics.bracket_highlight_opacity > 0.0F);
    REQUIRE(metrics.bracket_corner == CornerRadiusToken::kSm);
}
