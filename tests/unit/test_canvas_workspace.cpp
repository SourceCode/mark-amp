/// @file test_canvas_workspace.cpp
/// @brief V8 Phase 6: Canvas Workbench Shell — compile-only guardrail tests.
///
/// Verifies that Phase 6 event types, CanvasWorkspacePanel constants,
/// and menu IDs compile correctly without requiring wxWidgets GUI linkage.

#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <type_traits>

// ── Event type tests ──────────────────────────────────────────────

TEST_CASE("BoardOpenRequestEvent compiles and defaults to empty", "[canvas_workspace]")
{
    markamp::core::events::BoardOpenRequestEvent evt;
    REQUIRE(evt.board_id.empty());
    REQUIRE(evt.board_name.empty());

    evt.board_id = "test_board_001";
    evt.board_name = "Test Board";
    REQUIRE(evt.board_id == "test_board_001");
    REQUIRE(evt.board_name == "Test Board");
}

TEST_CASE("CanvasModeActivatedEvent is default constructible", "[canvas_workspace]")
{
    static_assert(std::is_default_constructible_v<markamp::core::events::CanvasModeActivatedEvent>,
                  "CanvasModeActivatedEvent must be default constructible");

    markamp::core::events::CanvasModeActivatedEvent evt;
    (void)evt;
    SUCCEED();
}

TEST_CASE("CanvasModeDeactivatedEvent is default constructible", "[canvas_workspace]")
{
    static_assert(
        std::is_default_constructible_v<markamp::core::events::CanvasModeDeactivatedEvent>,
        "CanvasModeDeactivatedEvent must be default constructible");

    markamp::core::events::CanvasModeDeactivatedEvent evt;
    (void)evt;
    SUCCEED();
}

// ── CanvasWorkspacePanel constants (header-only) ──────────────────

#include "ui/CanvasWorkspacePanel.h"

TEST_CASE("CanvasWorkspacePanel constants are reasonable", "[canvas_workspace]")
{
    using CWP = markamp::ui::CanvasWorkspacePanel;
    REQUIRE(CWP::kToolRailWidth == 40);
    REQUIRE(CWP::kInspectorWidth == 240);
    REQUIRE(CWP::kMinimapHeight == 120);
    REQUIRE(CWP::kContextBarHeight == 36);
}

TEST_CASE("CanvasWorkspacePanel layout dimensions are positive", "[canvas_workspace]")
{
    using CWP = markamp::ui::CanvasWorkspacePanel;
    REQUIRE(CWP::kToolRailWidth > 0);
    REQUIRE(CWP::kInspectorWidth > 0);
    REQUIRE(CWP::kMinimapHeight > 0);
    REQUIRE(CWP::kContextBarHeight > 0);
}
