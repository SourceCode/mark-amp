/// @file test_v27_p10_canvas.cpp
/// @brief V27 Phase 10: Canvas tool palette, selection, overlay tokens.
#include <catch2/catch_test_macros.hpp>
#include "canvas/V27CanvasTokens.h"
using namespace markamp::canvas;
TEST_CASE("V27 P10: Tool palette tokens", "[v27][p10]") {
    REQUIRE(V27CanvasToolPaletteTokens::kToolButtonSize >= V27CanvasToolPaletteTokens::kToolIconSize);
    REQUIRE(V27CanvasToolPaletteTokens::kPaletteRadius > 0);
}
TEST_CASE("V27 P10: Selection tokens", "[v27][p10]") {
    REQUIRE(V27CanvasSelectionTokens::kHandleSize > 0);
    REQUIRE(V27CanvasSelectionTokens::kRotationHandleOffset > 0);
}
TEST_CASE("V27 P10: Overlay tokens", "[v27][p10]") {
    REQUIRE(V27CanvasOverlayTokens::kPropertyPanelWidth > 0);
    REQUIRE(V27CanvasOverlayTokens::kMinimapSize > 0);
}
