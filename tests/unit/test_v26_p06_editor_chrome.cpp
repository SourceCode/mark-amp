/// @file test_v26_p06_editor_chrome.cpp
/// @brief V26 Phase 06: Editor chrome, gutter, selection, caret and minimap tests.
#include <catch2/catch_test_macros.hpp>
#include "ui/V26EditorConvergence.h"

using namespace markamp::ui;

TEST_CASE("V26 P06: Gutter min width", "[v26][p06]")
{
    const int min_w = GutterConvergenceMetrics::min_gutter_width();
    REQUIRE(min_w > 0);
    REQUIRE(min_w == (3 * 8) + 14 + 16 + 4 + 8); // 66
}

TEST_CASE("V26 P06: Gutter opacity hierarchy", "[v26][p06]")
{
    REQUIRE(GutterConvergenceMetrics::kLineNumberOpacity < GutterConvergenceMetrics::kLineNumberActiveOpacity);
    REQUIRE(GutterConvergenceMetrics::kFoldMarkerOpacity < GutterConvergenceMetrics::kFoldMarkerHoverOpacity);
}

TEST_CASE("V26 P06: Editor highlight intensity hierarchy", "[v26][p06]")
{
    REQUIRE(EditorHighlightMetrics::is_hierarchy_valid());
}

TEST_CASE("V26 P06: Caret dimensions", "[v26][p06]")
{
    REQUIRE(EditorHighlightMetrics::kCaretWidth == 2);
    REQUIRE(EditorHighlightMetrics::kCaretBlinkMs > 0);
    REQUIRE(EditorHighlightMetrics::kCaretLineAlpha > 0.0F);
    REQUIRE(EditorHighlightMetrics::kCaretLineAlpha < 0.2F);
}

TEST_CASE("V26 P06: Diagnostic squiggle thickness", "[v26][p06]")
{
    REQUIRE(EditorHighlightMetrics::kErrorSquiggleThickness > EditorHighlightMetrics::kWarnSquiggleThickness);
}

TEST_CASE("V26 P06: Minimap metrics", "[v26][p06]")
{
    REQUIRE(MinimapConvergenceMetrics::kMinimapWidth > 0);
    REQUIRE(MinimapConvergenceMetrics::kViewportAlpha < MinimapConvergenceMetrics::kSelectionAlpha);
    REQUIRE(MinimapConvergenceMetrics::kSelectionAlpha < MinimapConvergenceMetrics::kSearchMatchAlpha);
    REQUIRE(MinimapConvergenceMetrics::kSliderMinHeight > 0);
}
