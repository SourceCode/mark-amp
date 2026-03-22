#include "core/NotebookChromeCoordinator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("NotebookChromeCoordinator shell metrics", "[v22][notebook]")
{
    NotebookChromeCoordinator coord;
    auto metrics = coord.shell_metrics();

    REQUIRE(metrics.toolbar_height > 0);
    REQUIRE(metrics.cell_gap > 0);
    REQUIRE(metrics.content_max_width > 0);
    REQUIRE(metrics.margin_h > 0);
    REQUIRE(metrics.toolbar_font == TypeScaleToken::kBody);
}

TEST_CASE("NotebookChromeCoordinator cell container metrics", "[v22][notebook]")
{
    NotebookChromeCoordinator coord;
    auto metrics = coord.cell_container_metrics();

    REQUIRE(metrics.prompt_gutter_width > 0);
    REQUIRE(metrics.cell_padding_h > 0);
    REQUIRE(metrics.cell_padding_v > 0);
    REQUIRE(metrics.add_cell_button_height > 0);
    REQUIRE(metrics.corner == CornerRadiusToken::kMd);
    REQUIRE(metrics.border == BorderWeightToken::kThin);
    REQUIRE(metrics.selected_border == BorderWeightToken::kMedium);
}

TEST_CASE("NotebookChromeCoordinator execution chrome", "[v22][notebook]")
{
    NotebookChromeCoordinator coord;
    auto metrics = coord.execution_chrome_metrics();

    REQUIRE(metrics.run_button_size > 0);
    REQUIRE(metrics.status_indicator_size > 0);
    REQUIRE(metrics.progress_bar_height > 0);
    REQUIRE(metrics.elapsed_time_width > 0);
}

TEST_CASE("NotebookChromeCoordinator output block metrics", "[v22][notebook]")
{
    NotebookChromeCoordinator coord;
    auto metrics = coord.output_block_metrics();

    REQUIRE(metrics.max_height > 0);
    REQUIRE(metrics.padding_h > 0);
    REQUIRE(metrics.padding_v > 0);
    REQUIRE(metrics.collapse_button_size > 0);
    REQUIRE(metrics.corner == CornerRadiusToken::kSm);
}

TEST_CASE("NotebookChromeCoordinator cell border per state", "[v22][notebook]")
{
    NotebookChromeCoordinator coord;

    SECTION("Idle state uses default thin border")
    {
        auto border = coord.cell_border_for_state(NotebookChromeCoordinator::CellState::kIdle);
        REQUIRE(border == BorderWeightToken::kThin);
    }

    SECTION("Selected state uses medium border")
    {
        auto border = coord.cell_border_for_state(NotebookChromeCoordinator::CellState::kSelected);
        REQUIRE(border == BorderWeightToken::kMedium);
    }

    SECTION("Running state uses medium border")
    {
        auto border = coord.cell_border_for_state(NotebookChromeCoordinator::CellState::kRunning);
        REQUIRE(border == BorderWeightToken::kMedium);
    }

    SECTION("Error state uses medium border for emphasis")
    {
        auto border = coord.cell_border_for_state(NotebookChromeCoordinator::CellState::kError);
        REQUIRE(border == BorderWeightToken::kMedium);
    }

    SECTION("Success and queued use thin border")
    {
        auto success = coord.cell_border_for_state(NotebookChromeCoordinator::CellState::kSuccess);
        auto queued = coord.cell_border_for_state(NotebookChromeCoordinator::CellState::kQueued);
        REQUIRE(success == BorderWeightToken::kThin);
        REQUIRE(queued == BorderWeightToken::kThin);
    }
}

TEST_CASE("NotebookChromeCoordinator cell corner per state", "[v22][notebook]")
{
    NotebookChromeCoordinator coord;

    // All states should return the same corner radius
    for (auto state : {NotebookChromeCoordinator::CellState::kIdle,
                       NotebookChromeCoordinator::CellState::kSelected,
                       NotebookChromeCoordinator::CellState::kRunning,
                       NotebookChromeCoordinator::CellState::kSuccess,
                       NotebookChromeCoordinator::CellState::kError,
                       NotebookChromeCoordinator::CellState::kQueued})
    {
        auto corner = coord.cell_corner_for_state(state);
        REQUIRE(corner == CornerRadiusToken::kMd);
    }
}
