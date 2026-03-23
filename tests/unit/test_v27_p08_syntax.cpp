/// @file test_v27_p08_syntax.cpp
/// @brief V27 Phase 08: Syntax palette tokens.
#include <catch2/catch_test_macros.hpp>
#include "core/V27SyntaxPalette.h"
using namespace markamp::core;
TEST_CASE("V27 P08: Syntax saturation count", "[v27][p08]") { REQUIRE(v27_syntax_saturation_count() == 3); }
TEST_CASE("V27 P08: Code context count", "[v27][p08]") { REQUIRE(v27_code_context_count() == 5); }
TEST_CASE("V27 P08: Syntax palette values", "[v27][p08]") {
    REQUIRE(V27SyntaxPaletteTokens::kEditorFontSize >= V27SyntaxPaletteTokens::kNotebookFontSize);
    REQUIRE(V27SyntaxPaletteTokens::kLineNumberOpacityPct > 0);
    REQUIRE(V27SyntaxPaletteTokens::kCommentOpacityPct > V27SyntaxPaletteTokens::kLineNumberOpacityPct);
}
