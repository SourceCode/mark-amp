#include "core/EditorOverlayCoordinator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("EditorOverlayCoordinator find/replace metrics", "[v22][editor_overlay]")
{
    EditorOverlayCoordinator coord;
    auto metrics = coord.find_replace_metrics();

    REQUIRE(metrics.bar_height > 0);
    REQUIRE(metrics.input_width > 0);
    REQUIRE(metrics.button_size > 0);
    REQUIRE(metrics.match_count_width > 0);
    REQUIRE(metrics.input_corner == CornerRadiusToken::kSm);
    REQUIRE(metrics.bar_corner == CornerRadiusToken::kMd);
    REQUIRE(metrics.elevation == ElevationToken::kMedium);
    REQUIRE(metrics.input_font == TypeScaleToken::kBody);
}

TEST_CASE("EditorOverlayCoordinator search result metrics", "[v22][editor_overlay]")
{
    EditorOverlayCoordinator coord;
    auto metrics = coord.search_result_metrics();

    REQUIRE(metrics.file_row_height > 0);
    REQUIRE(metrics.match_row_height > 0);
    REQUIRE(metrics.match_indent > 0);
    REQUIRE(metrics.context_lines > 0);
    REQUIRE(metrics.file_font == TypeScaleToken::kBody);
    REQUIRE(metrics.match_font == TypeScaleToken::kBody);
    REQUIRE(metrics.line_number_font == TypeScaleToken::kCaption);
}

TEST_CASE("EditorOverlayCoordinator diagnostics metrics", "[v22][editor_overlay]")
{
    EditorOverlayCoordinator coord;
    auto metrics = coord.diagnostics_metrics();

    REQUIRE(metrics.squiggle_thickness > 0);
    REQUIRE(metrics.severity_icon_size > 0);
    // Severity opacities should decrease from error to hint
    REQUIRE(metrics.error_squiggle_opacity >= metrics.warning_squiggle_opacity);
    REQUIRE(metrics.warning_squiggle_opacity >= metrics.info_squiggle_opacity);
    REQUIRE(metrics.info_squiggle_opacity >= metrics.hint_squiggle_opacity);
}

TEST_CASE("EditorOverlayCoordinator peek view metrics", "[v22][editor_overlay]")
{
    EditorOverlayCoordinator coord;
    auto metrics = coord.peek_view_metrics();

    REQUIRE(metrics.header_height > 0);
    REQUIRE(metrics.min_height > 0);
    REQUIRE(metrics.max_height > metrics.min_height);
    REQUIRE(metrics.result_list_width > 0);
    REQUIRE(metrics.corner == CornerRadiusToken::kMd);
    REQUIRE(metrics.border == BorderWeightToken::kMedium);
    REQUIRE(metrics.elevation == ElevationToken::kMedium);
}

TEST_CASE("EditorOverlayCoordinator hover metrics", "[v22][editor_overlay]")
{
    EditorOverlayCoordinator coord;
    auto metrics = coord.editor_hover_metrics();

    REQUIRE(metrics.max_width > 0);
    REQUIRE(metrics.max_height > 0);
    REQUIRE(metrics.item_height > 0);
    REQUIRE(metrics.icon_size > 0);
    REQUIRE(metrics.corner == CornerRadiusToken::kMd);
    REQUIRE(metrics.elevation == ElevationToken::kHigh);
    REQUIRE(metrics.body_font == TypeScaleToken::kBody);
    REQUIRE(metrics.detail_font == TypeScaleToken::kCaption);
}
