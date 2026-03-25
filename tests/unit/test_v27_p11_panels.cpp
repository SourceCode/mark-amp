/// @file test_v27_p11_panels.cpp
/// @brief V27 Phase 11: Panel shell, dense row tokens, panel coordinator.
#include <catch2/catch_test_macros.hpp>
#include "ui/V27PanelTokens.h"
#include "core/V27PanelDesignCoordinator.h"
using namespace markamp::ui;
using namespace markamp::core;
TEST_CASE("V27 P11: Panel shell tokens", "[v27][p11]") {
    REQUIRE(V27PanelShellTokens::kHeaderHeight > 0);
    REQUIRE(V27PanelShellTokens::kHeaderActionSize < V27PanelShellTokens::kHeaderHeight);
    REQUIRE(V27PanelShellTokens::kSectionGap > V27PanelShellTokens::kGroupGap);
}
TEST_CASE("V27 P11: Dense row tokens", "[v27][p11]") {
    REQUIRE(V27DenseRowTokens::kRowHeight > V27DenseRowTokens::kRowHeightCompact);
    REQUIRE(V27DenseRowTokens::kIndentWidth > 0);
}
TEST_CASE("V27 P11: Secondary panel tokens", "[v27][p11]") {
    REQUIRE(V27PanelSecondaryTokens::kMinShellHeight > 0);
    REQUIRE(V27PanelSecondaryTokens::kEmptyIconSize >= 24);
    REQUIRE(V27PanelSecondaryTokens::kLoadingBarHeight > 0);
}
TEST_CASE("V27 P11: Panel coordinator registration", "[v27][p11]") {
    V27PanelDesignCoordinator coord;
    coord.register_panel({"Explorer", PanelRedesignTier::kPrimary, 32, true, true});
    coord.register_panel({"Search", PanelRedesignTier::kPrimary, 32, true, true});
    coord.register_panel({"History", PanelRedesignTier::kSecondary, 32, false, true});
    REQUIRE(coord.panel_count() == 3);
    REQUIRE(coord.primary_count() == 2);
    REQUIRE(coord.panels_with_canonical_icons() == 2);
}
TEST_CASE("V27 P11: Panel coordinator divider consistency", "[v27][p11]") {
    V27PanelDesignCoordinator coord;
    coord.register_panel({"Problems", PanelRedesignTier::kPrimary, 32, true, true});
    coord.register_panel({"Output", PanelRedesignTier::kPrimary, 32, true, true});
    REQUIRE(coord.all_dividers_tokenized());
}
