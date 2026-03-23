/// @file test_v26_p09_canvas.cpp
/// @brief V26 Phase 09: Canvas shell, tools, overlays and interaction visuals tests.
#include <catch2/catch_test_macros.hpp>
#include "ui/V26NotebookCanvasConvergence.h"

using namespace markamp::ui;

TEST_CASE("V26 P09: Canvas shell dimensions", "[v26][p09]")
{
    REQUIRE(CanvasV26Metrics::kToolRailWidth == 40);
    REQUIRE(CanvasV26Metrics::kInspectorWidth == 240);
    REQUIRE(CanvasV26Metrics::kMinimapHeight == 120);
    REQUIRE(CanvasV26Metrics::kContextBarHeight == 36);
}

TEST_CASE("V26 P09: Tool palette button sizing", "[v26][p09]")
{
    REQUIRE(CanvasV26Metrics::kToolButtonSize > CanvasV26Metrics::kToolIconSize);
    REQUIRE(CanvasV26Metrics::kToolButtonGap > 0);
    REQUIRE(CanvasV26Metrics::kToolGroupGap > CanvasV26Metrics::kToolButtonGap);
}

TEST_CASE("V26 P09: Selection handles", "[v26][p09]")
{
    REQUIRE(CanvasV26Metrics::kHandleSize > 0);
    REQUIRE(CanvasV26Metrics::kHandleBorderW > 0);
    REQUIRE(CanvasV26Metrics::kRotationHandleGap > CanvasV26Metrics::kHandleSize);
    REQUIRE(CanvasV26Metrics::kHandleFillAlpha > 0.5F);
}

TEST_CASE("V26 P09: Grid and guides", "[v26][p09]")
{
    REQUIRE(CanvasV26Metrics::kGridMinSize > 0);
    REQUIRE(CanvasV26Metrics::kGridDefaultSize > CanvasV26Metrics::kGridMinSize);
    REQUIRE(CanvasV26Metrics::kGridLineAlpha < CanvasV26Metrics::kGridDotAlpha);
    REQUIRE(CanvasV26Metrics::kGuideLineAlpha > CanvasV26Metrics::kGridDotAlpha);
}

TEST_CASE("V26 P09: Zoom validation", "[v26][p09]")
{
    REQUIRE(CanvasV26Metrics::is_zoom_valid(100));
    REQUIRE(CanvasV26Metrics::is_zoom_valid(10));
    REQUIRE(CanvasV26Metrics::is_zoom_valid(400));
    REQUIRE_FALSE(CanvasV26Metrics::is_zoom_valid(9));
    REQUIRE_FALSE(CanvasV26Metrics::is_zoom_valid(401));
}

TEST_CASE("V26 P09: Zoom clamping", "[v26][p09]")
{
    REQUIRE(CanvasV26Metrics::clamp_zoom(5) == 10);
    REQUIRE(CanvasV26Metrics::clamp_zoom(200) == 200);
    REQUIRE(CanvasV26Metrics::clamp_zoom(999) == 400);
}

TEST_CASE("V26 P09: Inline editing", "[v26][p09]")
{
    REQUIRE(CanvasV26Metrics::kInlineEditPaddingH > 0);
    REQUIRE(CanvasV26Metrics::kInlineEditBorderW > 0);
    REQUIRE(CanvasV26Metrics::kInlineEditMinW > 0);
}
