/// @file test_v27_p09_notebook.cpp
/// @brief V27 Phase 09: Notebook cell, output, edge tokens.
#include <catch2/catch_test_macros.hpp>
#include "ui/V27NotebookTokens.h"
using namespace markamp::ui;
TEST_CASE("V27 P09: Cell tokens", "[v27][p09]") {
    REQUIRE(V27NotebookCellTokens::kCellRadius > 0);
    REQUIRE(V27NotebookCellTokens::kToolbarHeight > 0);
    REQUIRE(V27NotebookCellTokens::kGutterWidth > 0);
}
TEST_CASE("V27 P09: Output tokens", "[v27][p09]") {
    REQUIRE(V27NotebookOutputTokens::kOutputMaxHeight > 0);
    REQUIRE(V27NotebookOutputTokens::kOutputRadius > 0);
}
TEST_CASE("V27 P09: Edge tokens", "[v27][p09]") {
    REQUIRE(V27NotebookEdgeTokens::kEmptyCellIconSize >= 24);
    REQUIRE(V27NotebookEdgeTokens::kAddCellButtonHeight > 0);
}
