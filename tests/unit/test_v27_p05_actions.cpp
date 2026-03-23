/// @file test_v27_p05_actions.cpp
/// @brief V27 Phase 05: Toolbar, menu, command palette tokens.
#include <catch2/catch_test_macros.hpp>
#include "ui/V27ActionSurfaceTokens.h"
using namespace markamp::ui;
TEST_CASE("V27 P05: Toolbar tokens", "[v27][p05]") {
    REQUIRE(V27ToolbarTokens::kHeight > 0);
    REQUIRE(V27ToolbarTokens::kButtonSize >= V27ToolbarTokens::kButtonIconSize);
}
TEST_CASE("V27 P05: Menu tokens", "[v27][p05]") {
    REQUIRE(V27MenuTokens::kMinWidth < V27MenuTokens::kMaxWidth);
    REQUIRE(V27MenuTokens::kRadius > 0);
    REQUIRE(V27MenuTokens::kShadowBlur > 0);
}
TEST_CASE("V27 P05: Command palette tokens", "[v27][p05]") {
    REQUIRE(V27CommandPaletteTokens::kWidth > 0);
    REQUIRE(V27CommandPaletteTokens::kResultRowHeight > 0);
    REQUIRE(V27CommandPaletteTokens::kRadius > 0);
}
