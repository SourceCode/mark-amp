/// @file test_v27_p11_panels.cpp
/// @brief V27 Phase 11: Panel shell, dense row tokens.
#include <catch2/catch_test_macros.hpp>
#include "ui/V27PanelTokens.h"
using namespace markamp::ui;
TEST_CASE("V27 P11: Panel shell tokens", "[v27][p11]") {
    REQUIRE(V27PanelShellTokens::kHeaderHeight > 0);
    REQUIRE(V27PanelShellTokens::kHeaderActionSize < V27PanelShellTokens::kHeaderHeight);
    REQUIRE(V27PanelShellTokens::kSectionGap > V27PanelShellTokens::kGroupGap);
}
TEST_CASE("V27 P11: Dense row tokens", "[v27][p11]") {
    REQUIRE(V27DenseRowTokens::kRowHeight > V27DenseRowTokens::kRowHeightCompact);
    REQUIRE(V27DenseRowTokens::kIndentWidth > 0);
}
