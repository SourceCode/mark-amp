/// @file test_v26_p08_notebook.cpp
/// @brief V26 Phase 08: Notebook shell, cell chrome, output and mixed content tests.
#include <catch2/catch_test_macros.hpp>
#include "ui/V26NotebookCanvasConvergence.h"

using namespace markamp::ui;

TEST_CASE("V26 P08: Notebook shell dimensions", "[v26][p08]")
{
    REQUIRE(NotebookV26Metrics::kToolbarHeight == 36);
    REQUIRE(NotebookV26Metrics::kCellGap > 0);
    REQUIRE(NotebookV26Metrics::kContentMaxWidth > 0);
    REQUIRE(NotebookV26Metrics::kMarginH > 0);
}

TEST_CASE("V26 P08: Cell container sizing", "[v26][p08]")
{
    REQUIRE(NotebookV26Metrics::kPromptGutterWidth > 0);
    REQUIRE(NotebookV26Metrics::kCellPaddingH > 0);
    REQUIRE(NotebookV26Metrics::kExecCountWidth < NotebookV26Metrics::kPromptGutterWidth);
}

TEST_CASE("V26 P08: Execution chrome", "[v26][p08]")
{
    REQUIRE(NotebookV26Metrics::kRunButtonSize > 0);
    REQUIRE(NotebookV26Metrics::kStatusIndicatorSize < NotebookV26Metrics::kRunButtonSize);
    REQUIRE(NotebookV26Metrics::kProgressBarH > 0);
}

TEST_CASE("V26 P08: Output block constraints", "[v26][p08]")
{
    REQUIRE(NotebookV26Metrics::kOutputMaxHeight > 0);
    REQUIRE(NotebookV26Metrics::kCollapseButtonSize > 0);
}

TEST_CASE("V26 P08: Cell state border hierarchy", "[v26][p08]")
{
    REQUIRE(NotebookV26Metrics::kIdleBorderW < NotebookV26Metrics::kSelectedBorderW);
    REQUIRE(NotebookV26Metrics::kSelectedBorderW == NotebookV26Metrics::kRunningBorderW);
}

TEST_CASE("V26 P08: Cell opacity hierarchy", "[v26][p08]")
{
    REQUIRE(NotebookV26Metrics::kIdleCellOpacity < NotebookV26Metrics::kSelectedCellOpacity);
}
