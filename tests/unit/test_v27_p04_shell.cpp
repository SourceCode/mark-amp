/// @file test_v27_p04_shell.cpp
/// @brief V27 Phase 04: Shell, workspace chrome, region tokens.
#include <catch2/catch_test_macros.hpp>
#include "ui/V27ShellTokens.h"
using namespace markamp::ui;
TEST_CASE("V27 P04: Shell region dimensions", "[v27][p04]") {
    REQUIRE(V27ShellRegionTokens::kActivityBarWidth > 0);
    REQUIRE(V27ShellRegionTokens::kSidebarMinWidth < V27ShellRegionTokens::kSidebarMaxWidth);
    REQUIRE(V27ShellRegionTokens::kSidebarDefaultWidth >= V27ShellRegionTokens::kSidebarMinWidth);
    REQUIRE(V27ShellRegionTokens::kSidebarDefaultWidth <= V27ShellRegionTokens::kSidebarMaxWidth);
}
TEST_CASE("V27 P04: Splitter tokens", "[v27][p04]") {
    REQUIRE(V27SplitterTokens::kHitTarget >= V27SplitterTokens::kWidth);
    REQUIRE(V27SplitterTokens::kHandleLength > 0);
}
TEST_CASE("V27 P04: Activity bar tokens", "[v27][p04]") {
    REQUIRE(V27ActivityBarTokens::kItemHeight >= V27ActivityBarTokens::kIconSize);
    REQUIRE(V27ActivityBarTokens::kBadgeSize > 0);
    REQUIRE(V27ActivityBarTokens::kIndicatorWidth > 0);
}
TEST_CASE("V27 P04: Workspace atmosphere tokens", "[v27][p04]") {
    REQUIRE(V27WorkspaceAtmosphereTokens::kHeroIconSize >= 32);
    REQUIRE(V27WorkspaceAtmosphereTokens::kMaxContentWidth > 0);
}
