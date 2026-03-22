/// @file test_v25_p09_canvas.cpp
/// @brief V25 Phase 09: Canvas shell adapter tests.
#include <catch2/catch_test_macros.hpp>
#include "core/V25ReleaseServices.h"

using namespace markamp::core;

TEST_CASE("V25 P09: Canvas create/save/close lifecycle", "[v25][p09]")
{
    CanvasShellAdapter adapter;
    REQUIRE(adapter.state() == CanvasShellState::kIdle);
    REQUIRE(adapter.create_canvas("board.canvas"));
    REQUIRE(adapter.state() == CanvasShellState::kOpened);
    REQUIRE(adapter.save_canvas());
    REQUIRE(adapter.close_canvas());
    REQUIRE(adapter.state() == CanvasShellState::kClosed);
}

TEST_CASE("V25 P09: Canvas dirty tracking", "[v25][p09]")
{
    CanvasShellAdapter adapter;
    adapter.create_canvas("test");
    REQUIRE_FALSE(adapter.is_dirty());
    adapter.mark_dirty();
    REQUIRE(adapter.is_dirty());
    adapter.save_canvas();
    REQUIRE_FALSE(adapter.is_dirty());
}

TEST_CASE("V25 P09: Canvas is shell-owned", "[v25][p09]")
{
    CanvasShellAdapter adapter;
    REQUIRE(adapter.is_shell_owned());
}
