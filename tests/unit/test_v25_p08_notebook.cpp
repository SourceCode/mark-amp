/// @file test_v25_p08_notebook.cpp
/// @brief V25 Phase 08: Notebook shell adapter tests.
#include <catch2/catch_test_macros.hpp>
#include "core/V25ReleaseServices.h"

using namespace markamp::core;

TEST_CASE("V25 P08: Notebook create/open/save/close lifecycle", "[v25][p08]")
{
    NotebookShellAdapter adapter;
    REQUIRE(adapter.state() == NotebookShellState::kIdle);
    REQUIRE(adapter.create_notebook("test.nb"));
    REQUIRE(adapter.state() == NotebookShellState::kOpened);
    REQUIRE(adapter.save_notebook());
    REQUIRE(adapter.close_notebook());
    REQUIRE(adapter.state() == NotebookShellState::kClosed);
}

TEST_CASE("V25 P08: Notebook is shell-owned", "[v25][p08]")
{
    NotebookShellAdapter adapter;
    REQUIRE(adapter.is_shell_owned());
}

TEST_CASE("V25 P08: Notebook open from path", "[v25][p08]")
{
    NotebookShellAdapter adapter;
    REQUIRE(adapter.open_notebook("/path/test.nb"));
    REQUIRE(adapter.state() == NotebookShellState::kOpened);
}
