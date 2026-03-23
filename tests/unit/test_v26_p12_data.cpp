/// @file test_v26_p12_data.cpp
/// @brief V26 Phase 12: Trees, lists, tables, AV & structured data presentation tests.
#include <catch2/catch_test_macros.hpp>
#include "ui/V26DataFeedbackConvergence.h"

using namespace markamp::ui;

TEST_CASE("V26 P12: Tree/list row dimensions", "[v26][p12]")
{
    REQUIRE(TreeListV26Metrics::kRowHeight == 24);
    REQUIRE(TreeListV26Metrics::kIndentStep == 16);
    REQUIRE(TreeListV26Metrics::kIconSize == 16);
}

TEST_CASE("V26 P12: Tree guide opacity hierarchy", "[v26][p12]")
{
    REQUIRE(TreeListV26Metrics::kGuideLineAlpha < TreeListV26Metrics::kActiveGuideAlpha);
    REQUIRE(TreeListV26Metrics::kActiveGuideAlpha < 1.0F);
}

TEST_CASE("V26 P12: Inline action opacity", "[v26][p12]")
{
    REQUIRE(TreeListV26Metrics::kInlineActionAlpha == 0.0F); // hidden until hover
    REQUIRE(TreeListV26Metrics::kInlineActionHover == 1.0F); // fully visible
}

TEST_CASE("V26 P12: Table cell dimensions", "[v26][p12]")
{
    REQUIRE(TableCellV26Metrics::kCellMinHeight > 0);
    REQUIRE(TableCellV26Metrics::kHeaderHeight >= TableCellV26Metrics::kCellMinHeight);
    REQUIRE(TableCellV26Metrics::kColumnMinW < TableCellV26Metrics::kColumnDefaultW);
    REQUIRE(TableCellV26Metrics::kColumnDefaultW < TableCellV26Metrics::kColumnMaxW);
}

TEST_CASE("V26 P12: Table selection alpha hierarchy", "[v26][p12]")
{
    REQUIRE(TableCellV26Metrics::kHoverRowAlpha < TableCellV26Metrics::kSelectedRowAlpha);
}
