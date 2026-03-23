/// @file test_v27_p07_editor.cpp
/// @brief V27 Phase 07: Editor chrome, overlay, readability tokens.
#include <catch2/catch_test_macros.hpp>
#include "ui/V27EditorTokens.h"
using namespace markamp::ui;
TEST_CASE("V27 P07: Editor chrome tokens", "[v27][p07]") {
    REQUIRE(V27EditorChromeTokens::kGutterWidth > 0);
    REQUIRE(V27EditorChromeTokens::kMinimapWidth > 0);
    REQUIRE(V27EditorChromeTokens::kScrollbarMinThumb > 0);
}
TEST_CASE("V27 P07: Editor overlay tokens", "[v27][p07]") {
    REQUIRE(V27EditorOverlayTokens::kSuggestionWidth > 0);
    REQUIRE(V27EditorOverlayTokens::kHoverMaxWidth > V27EditorOverlayTokens::kSuggestionWidth);
}
TEST_CASE("V27 P07: Editor readability tokens", "[v27][p07]") {
    REQUIRE(V27EditorReadabilityTokens::kCaretWidth > 0);
    REQUIRE(V27EditorReadabilityTokens::kMaxLineWidth > 0);
}
