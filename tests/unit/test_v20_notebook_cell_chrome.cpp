/// @file test_v20_notebook_cell_chrome.cpp
/// @brief V20 Phase 08 – NotebookCellChrome unit tests.

#include "core/NotebookCellChrome.h"
#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("CellChrome: construction", "[v20][cell-chrome]")
{
    EventBus bus;
    NotebookCellChrome chrome(bus);
    REQUIRE(chrome.cell_count() == 0);
    REQUIRE(chrome.update_count() == 0);
}

TEST_CASE("CellChrome: set and get cell state", "[v20][cell-chrome]")
{
    EventBus bus;
    NotebookCellChrome chrome(bus);

    CellChromeState state;
    state.cell_id = "cell-1";
    state.cell_type = CellChromeType::kCode;
    state.execution_state = CellExecutionState::kIdle;
    chrome.set_cell_state("cell-1", state);

    REQUIRE(chrome.cell_count() == 1);
    auto* s = chrome.cell_state("cell-1");
    REQUIRE(s != nullptr);
    REQUIRE(s->cell_type == CellChromeType::kCode);
    REQUIRE(s->needs_prompt());
}

TEST_CASE("CellChrome: active cell management", "[v20][cell-chrome]")
{
    EventBus bus;
    NotebookCellChrome chrome(bus);

    CellChromeState s1, s2;
    s1.cell_id = "cell-1";
    s2.cell_id = "cell-2";
    chrome.set_cell_state("cell-1", s1);
    chrome.set_cell_state("cell-2", s2);

    chrome.set_active_cell("cell-1");
    REQUIRE(chrome.active_cell_id() == "cell-1");
    REQUIRE(chrome.cell_state("cell-1")->is_active);

    chrome.set_active_cell("cell-2");
    REQUIRE(chrome.active_cell_id() == "cell-2");
    REQUIRE(chrome.cell_state("cell-2")->is_active);
    REQUIRE_FALSE(chrome.cell_state("cell-1")->is_active);
}

TEST_CASE("CellChrome: execution state changes", "[v20][cell-chrome]")
{
    EventBus bus;
    NotebookCellChrome chrome(bus);

    CellChromeState state;
    state.cell_id = "cell-1";
    chrome.set_cell_state("cell-1", state);

    chrome.set_execution_state("cell-1", CellExecutionState::kRunning);
    REQUIRE(chrome.cell_state("cell-1")->is_running());

    chrome.set_execution_state("cell-1", CellExecutionState::kSuccess);
    REQUIRE_FALSE(chrome.cell_state("cell-1")->is_running());
}

TEST_CASE("CellChrome: collapse and remove", "[v20][cell-chrome]")
{
    EventBus bus;
    NotebookCellChrome chrome(bus);

    CellChromeState state;
    state.cell_id = "cell-1";
    chrome.set_cell_state("cell-1", state);

    chrome.set_collapsed("cell-1", true);
    REQUIRE(chrome.cell_state("cell-1")->is_collapsed);

    chrome.remove_cell("cell-1");
    REQUIRE(chrome.cell_count() == 0);
}
