/// @file test_v27_p14_structured.cpp
/// @brief V27 Phase 14: Tree row, table, structured edge tokens.
#include <catch2/catch_test_macros.hpp>
#include "ui/V27StructuredDataTokens.h"
using namespace markamp::ui;
TEST_CASE("V27 P14: Tree row tokens", "[v27][p14]") {
    REQUIRE(V27TreeRowTokens::kRowHeight > 0);
    REQUIRE(V27TreeRowTokens::kGuideWidth > 0);
    REQUIRE(V27TreeRowTokens::kBadgeRadius > 0);
}
TEST_CASE("V27 P14: Table tokens", "[v27][p14]") {
    REQUIRE(V27TableTokens::kColumnMinWidth > 0);
    REQUIRE(V27TableTokens::kHeaderHeight > 0);
}
TEST_CASE("V27 P14: Structured edge tokens", "[v27][p14]") {
    REQUIRE(V27StructuredEdgeTokens::kEmptyIconSize >= 24);
    REQUIRE(V27StructuredEdgeTokens::kLoadingShimmerHeight > 0);
}
