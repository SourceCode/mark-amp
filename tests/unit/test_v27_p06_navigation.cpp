/// @file test_v27_p06_navigation.cpp
/// @brief V27 Phase 06: Tab, breadcrumb, history tokens.
#include <catch2/catch_test_macros.hpp>
#include "ui/V27NavigationTokens.h"
using namespace markamp::ui;
TEST_CASE("V27 P06: Tab bar tokens", "[v27][p06]") {
    REQUIRE(V27TabBarTokens::kTabMinWidth < V27TabBarTokens::kTabMaxWidth);
    REQUIRE(V27TabBarTokens::kActiveIndicatorHeight > 0);
    REQUIRE(V27TabBarTokens::kTabRadius > 0);
}
TEST_CASE("V27 P06: Breadcrumb tokens", "[v27][p06]") {
    REQUIRE(V27BreadcrumbTokens::kHeight > 0);
    REQUIRE(V27BreadcrumbTokens::kIconSize > 0);
}
TEST_CASE("V27 P06: History tokens", "[v27][p06]") {
    REQUIRE(V27HistoryTokens::kButtonSize > 0);
    REQUIRE(V27HistoryTokens::kHistoryListRowHeight > 0);
}
