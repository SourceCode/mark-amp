/// @file test_canvas_workspace.cpp
/// @brief V17 Phase 01 W01: Canvas Workspace Shell — behavioral + guardrail tests.
///
/// Verifies workspace shell event types, CanvasWorkbench event emission,
/// CanvasWorkspacePanel constants, and tool state machine transitions.

#include "core/Events.h"
#include "core/EventBus.h"
#include "canvas/CanvasWorkbench.h"
#include "canvas/CameraModel.h"
#include "canvas/CanvasInputManager.h"
#include "canvas/HitTestModel.h"
#include "canvas/SelectionManager.h"
#include "canvas/SmartSpacingModel.h"
#include "canvas/PrecisionNudgeModel.h"
#include "canvas/DragAutoscrollModel.h"
#include "canvas/PenEngineModel.h"
#include "canvas/PenPresetModel.h"
#include "ui/CanvasWorkspacePanel.h"

#include <catch2/catch_test_macros.hpp>

#include <cmath>
#include <memory>
#include <string>
#include <type_traits>

// ── New event type compile + default tests ────────────────────────

TEST_CASE("BoardOpenedEvent compiles with correct fields", "[canvas_workspace]")
{
    markamp::core::events::BoardOpenedEvent evt;
    REQUIRE(evt.board_id.empty());
    REQUIRE(evt.board_name.empty());
    REQUIRE(evt.object_count == 0);

    evt.board_id = "board_42";
    evt.board_name = "Design Brainstorm";
    evt.object_count = 15;
    REQUIRE(evt.board_id == "board_42");
    REQUIRE(evt.board_name == "Design Brainstorm");
    REQUIRE(evt.object_count == 15);
}

TEST_CASE("BoardClosedEvent compiles with correct fields", "[canvas_workspace]")
{
    markamp::core::events::BoardClosedEvent evt;
    REQUIRE(evt.board_id.empty());

    evt.board_id = "board_closed_99";
    REQUIRE(evt.board_id == "board_closed_99");
}

TEST_CASE("WorkspaceShellReadyEvent is default constructible", "[canvas_workspace]")
{
    static_assert(std::is_default_constructible_v<markamp::core::events::WorkspaceShellReadyEvent>,
                  "WorkspaceShellReadyEvent must be default constructible");

    markamp::core::events::WorkspaceShellReadyEvent evt;
    (void)evt;
    SUCCEED();
}

TEST_CASE("CanvasInspectorToggledEvent compiles with visible field", "[canvas_workspace]")
{
    markamp::core::events::CanvasInspectorToggledEvent evt;
    REQUIRE(evt.visible == false);

    evt.visible = true;
    REQUIRE(evt.visible == true);
}

TEST_CASE("CanvasMinimapToggledEvent compiles with visible field", "[canvas_workspace]")
{
    markamp::core::events::CanvasMinimapToggledEvent evt;
    REQUIRE(evt.visible == false);

    evt.visible = true;
    REQUIRE(evt.visible == true);
}

// ── Pre-existing event type tests ─────────────────────────────────

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

// ── CanvasWorkspacePanel constants ────────────────────────────────

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

// ── CanvasWorkbench event emission via EventBus ───────────────────

TEST_CASE("CanvasWorkbench emits BoardOpenedEvent on board open", "[canvas_workspace]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench workbench(bus);

    bool received = false;
    std::string received_board_id;
    auto sub = bus->subscribe<markamp::core::events::BoardOpenedEvent>(
        [&](const markamp::core::events::BoardOpenedEvent& evt)
        {
            received = true;
            received_board_id = evt.board_id;
        });

    const auto board_id = workbench.create_board("Test Board");
    // Board constructor does not generate UUID — ID defaults to empty
    REQUIRE(received);
    REQUIRE(received_board_id == board_id);
}

TEST_CASE("CanvasWorkbench emits BoardClosedEvent on board close", "[canvas_workspace]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench workbench(bus);

    const auto board_id = workbench.create_board("Closable Board");

    bool closed = false;
    std::string closed_board_id;
    auto sub = bus->subscribe<markamp::core::events::BoardClosedEvent>(
        [&](const markamp::core::events::BoardClosedEvent& evt)
        {
            closed = true;
            closed_board_id = evt.board_id;
        });

    REQUIRE(workbench.close_board(board_id));
    REQUIRE(closed);
    REQUIRE(closed_board_id == board_id);
}

TEST_CASE("CanvasWorkbench emits CanvasToolChangedEvent on tool switch", "[canvas_workspace]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench workbench(bus);

    bool received = false;
    std::string received_tool_name;
    auto sub = bus->subscribe<markamp::core::events::CanvasToolChangedEvent>(
        [&](const markamp::core::events::CanvasToolChangedEvent& evt)
        {
            received = true;
            received_tool_name = evt.tool_name;
        });

    workbench.set_tool(markamp::canvas::ToolMode::Pan);
    REQUIRE(received);
    REQUIRE(received_tool_name == "Pan");
}

// ── CanvasWorkbench board lifecycle ───────────────────────────────

TEST_CASE("CanvasWorkbench create board adds to board map", "[canvas_workspace]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench workbench(bus);

    REQUIRE(workbench.board_count() == 0);

    workbench.create_board("Active Board");
    REQUIRE(workbench.board_count() == 1);
}

TEST_CASE("CanvasWorkbench close last board leaves empty state", "[canvas_workspace]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench workbench(bus);

    const auto board_id = workbench.create_board("Only Board");
    REQUIRE(workbench.board_count() == 1);

    workbench.close_board(board_id);
    REQUIRE(workbench.board_count() == 0);
    REQUIRE(workbench.has_active_board() == false);
}

TEST_CASE("CanvasWorkbench close active fallback clears state", "[canvas_workspace]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench workbench(bus);

    // Note: Board constructor doesn't generate unique IDs,
    // so multiple create_board calls share the same empty-string key
    const auto board_id = workbench.create_board("Board");
    REQUIRE(workbench.board_count() == 1);

    workbench.close_board(board_id);
    REQUIRE(workbench.board_count() == 0);
}

TEST_CASE("CanvasWorkbench recent boards are tracked", "[canvas_workspace]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench workbench(bus);

    workbench.create_board("Board A");

    const auto& recent = workbench.recent_boards();
    REQUIRE(recent.size() >= 1);
}

// ── CanvasWorkbench surface name ──────────────────────────────────

TEST_CASE("CanvasWorkbench surface name is not empty", "[canvas_workspace]")
{
    REQUIRE_FALSE(markamp::canvas::CanvasWorkbench::surface_name().empty());
}

TEST_CASE("CanvasWorkbench surface kind constant is 3", "[canvas_workspace]")
{
    REQUIRE(markamp::canvas::CanvasWorkbench::kSurfaceKindCanvas == 3);
}

// ── Event roundtrip via EventBus ──────────────────────────────────

TEST_CASE("BoardOpenedEvent roundtrips through EventBus", "[canvas_workspace]")
{
    markamp::core::EventBus bus;

    bool received = false;
    size_t received_count = 0;
    auto sub = bus.subscribe<markamp::core::events::BoardOpenedEvent>(
        [&](const markamp::core::events::BoardOpenedEvent& evt)
        {
            received = true;
            received_count = evt.object_count;
        });

    markamp::core::events::BoardOpenedEvent evt;
    evt.board_id = "roundtrip_board";
    evt.board_name = "Roundtrip";
    evt.object_count = 42;
    bus.publish(evt);

    REQUIRE(received);
    REQUIRE(received_count == 42);
}

TEST_CASE("CanvasInspectorToggledEvent roundtrips through EventBus", "[canvas_workspace]")
{
    markamp::core::EventBus bus;

    bool received_visible = false;
    auto sub = bus.subscribe<markamp::core::events::CanvasInspectorToggledEvent>(
        [&](const markamp::core::events::CanvasInspectorToggledEvent& evt)
        { received_visible = evt.visible; });

    markamp::core::events::CanvasInspectorToggledEvent evt;
    evt.visible = true;
    bus.publish(evt);

    REQUIRE(received_visible == true);
}

TEST_CASE("CanvasMinimapToggledEvent roundtrips through EventBus", "[canvas_workspace]")
{
    markamp::core::EventBus bus;

    bool received_visible = true; // default to true, expecting false
    auto sub = bus.subscribe<markamp::core::events::CanvasMinimapToggledEvent>(
        [&](const markamp::core::events::CanvasMinimapToggledEvent& evt)
        { received_visible = evt.visible; });

    markamp::core::events::CanvasMinimapToggledEvent evt;
    evt.visible = false;
    bus.publish(evt);

    REQUIRE(received_visible == false);
}

// ============================================================================
// W02: Board Lifecycle Tests
// ============================================================================

// ── Board UUID Generation ─────────────────────────────────────────

TEST_CASE("Board constructor generates non-empty ID", "[canvas_workspace][w02]")
{
    markamp::canvas::Board board("Test Board");
    REQUIRE_FALSE(board.metadata().id.empty());
    REQUIRE(board.metadata().id.substr(0, 2) == "b-");
}

TEST_CASE("Two boards get distinct IDs", "[canvas_workspace][w02]")
{
    markamp::canvas::Board board_a("Board A");
    markamp::canvas::Board board_b("Board B");
    REQUIRE(board_a.metadata().id != board_b.metadata().id);
}

// ── New Event Types ───────────────────────────────────────────────

TEST_CASE("BoardCreatedEvent fields default correctly", "[canvas_workspace][w02]")
{
    markamp::core::events::BoardCreatedEvent evt;
    REQUIRE(evt.board_id.empty());
    REQUIRE(evt.board_name.empty());
}

TEST_CASE("BoardRenamedEvent fields default correctly", "[canvas_workspace][w02]")
{
    markamp::core::events::BoardRenamedEvent evt;
    REQUIRE(evt.board_id.empty());
    REQUIRE(evt.old_name.empty());
    REQUIRE(evt.new_name.empty());
}

TEST_CASE("BoardDuplicatedEvent fields default correctly", "[canvas_workspace][w02]")
{
    markamp::core::events::BoardDuplicatedEvent evt;
    REQUIRE(evt.source_board_id.empty());
    REQUIRE(evt.new_board_id.empty());
    REQUIRE(evt.new_board_name.empty());
}

// ── Workbench rename_board ────────────────────────────────────────

TEST_CASE("rename_board updates name and emits BoardRenamedEvent", "[canvas_workspace][w02]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench workbench(bus);

    const auto board_id = workbench.create_board("Original Name");

    bool received = false;
    std::string received_old_name;
    std::string received_new_name;
    auto sub = bus->subscribe<markamp::core::events::BoardRenamedEvent>(
        [&](const markamp::core::events::BoardRenamedEvent& evt)
        {
            received = true;
            received_old_name = evt.old_name;
            received_new_name = evt.new_name;
        });

    REQUIRE(workbench.rename_board(board_id, "New Name"));
    REQUIRE(received);
    REQUIRE(received_old_name == "Original Name");
    REQUIRE(received_new_name == "New Name");

    // Verify the board name actually changed
    const auto* board = workbench.get_board(board_id);
    REQUIRE(board != nullptr);
    REQUIRE(board->metadata().name == "New Name");
}

TEST_CASE("rename_board fails on non-existent board", "[canvas_workspace][w02]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench workbench(bus);

    REQUIRE_FALSE(workbench.rename_board("non-existent-id", "Foo"));
}

// ── Workbench duplicate_board ─────────────────────────────────────

TEST_CASE("duplicate_board creates new board with distinct ID", "[canvas_workspace][w02]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench workbench(bus);

    const auto source_id = workbench.create_board("Source Board");
    const auto new_id = workbench.duplicate_board(source_id);

    REQUIRE_FALSE(new_id.empty());
    REQUIRE(new_id != source_id);
    REQUIRE(workbench.board_count() == 2);
}

TEST_CASE("duplicate_board emits BoardDuplicatedEvent", "[canvas_workspace][w02]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench workbench(bus);

    const auto source_id = workbench.create_board("My Board");

    bool received = false;
    std::string evt_source_id;
    std::string evt_new_name;
    auto sub = bus->subscribe<markamp::core::events::BoardDuplicatedEvent>(
        [&](const markamp::core::events::BoardDuplicatedEvent& evt)
        {
            received = true;
            evt_source_id = evt.source_board_id;
            evt_new_name = evt.new_board_name;
        });

    workbench.duplicate_board(source_id);
    REQUIRE(received);
    REQUIRE(evt_source_id == source_id);
    REQUIRE(evt_new_name == "My Board (copy)");
}

TEST_CASE("duplicate_board fails on non-existent board", "[canvas_workspace][w02]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench workbench(bus);

    const auto result = workbench.duplicate_board("non-existent-id");
    REQUIRE(result.empty());
}

// ── create_board now emits BoardCreatedEvent ──────────────────────

TEST_CASE("create_board emits BoardCreatedEvent", "[canvas_workspace][w02]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench workbench(bus);

    bool received = false;
    std::string received_name;
    auto sub = bus->subscribe<markamp::core::events::BoardCreatedEvent>(
        [&](const markamp::core::events::BoardCreatedEvent& evt)
        {
            received = true;
            received_name = evt.board_name;
        });

    workbench.create_board("Test Board");
    REQUIRE(received);
    REQUIRE(received_name == "Test Board");
}

// ── Multi-board management (now works with UUID) ──────────────────

TEST_CASE("Multiple boards have distinct IDs and correct count", "[canvas_workspace][w02]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench workbench(bus);

    const auto id_a = workbench.create_board("Board A");
    const auto id_b = workbench.create_board("Board B");
    const auto id_c = workbench.create_board("Board C");

    REQUIRE(id_a != id_b);
    REQUIRE(id_b != id_c);
    REQUIRE(id_a != id_c);
    REQUIRE(workbench.board_count() == 3);
    REQUIRE(workbench.has_active_board());
}

// ============================================================================
// W03: Viewport Camera Tests
// ============================================================================

TEST_CASE("ViewportFitRequestEvent fields default correctly", "[canvas_workspace][w03]")
{
    markamp::core::events::ViewportFitRequestEvent evt;
    REQUIRE(evt.fit_mode.empty());
}

TEST_CASE("ViewportZoomChangedEvent fields default correctly", "[canvas_workspace][w03]")
{
    markamp::core::events::ViewportZoomChangedEvent evt;
    REQUIRE(evt.old_zoom == 1.0);
    REQUIRE(evt.new_zoom == 1.0);
    REQUIRE(evt.anchor_x == 0.0);
    REQUIRE(evt.anchor_y == 0.0);
}

TEST_CASE("ViewportResetRequestEvent roundtrips through EventBus", "[canvas_workspace][w03]")
{
    markamp::core::EventBus bus;
    bool received = false;
    auto sub = bus.subscribe<markamp::core::events::ViewportResetRequestEvent>(
        [&](const markamp::core::events::ViewportResetRequestEvent& /*evt*/)
        { received = true; });

    markamp::core::events::ViewportResetRequestEvent evt;
    bus.publish(evt);
    REQUIRE(received);
}

TEST_CASE("ViewportZoomChangedEvent roundtrips with values", "[canvas_workspace][w03]")
{
    markamp::core::EventBus bus;
    double received_old = 0.0;
    double received_new = 0.0;
    auto sub = bus.subscribe<markamp::core::events::ViewportZoomChangedEvent>(
        [&](const markamp::core::events::ViewportZoomChangedEvent& evt)
        {
            received_old = evt.old_zoom;
            received_new = evt.new_zoom;
        });

    markamp::core::events::ViewportZoomChangedEvent evt;
    evt.old_zoom = 1.0;
    evt.new_zoom = 2.5;
    bus.publish(evt);
    REQUIRE(received_old == 1.0);
    REQUIRE(received_new == 2.5);
}

TEST_CASE("reset_viewport sets zoom to 100% and pan to origin", "[canvas_workspace][w03]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench workbench(bus);

    // Change zoom and pan away from defaults
    workbench.viewport().set_zoom(2.5);
    workbench.viewport().set_pan({100.0, -50.0});

    workbench.reset_viewport();

    REQUIRE(workbench.viewport().zoom() == 1.0);
    REQUIRE(workbench.viewport().pan().x == 0.0);
    REQUIRE(workbench.viewport().pan().y == 0.0);
}

TEST_CASE("reset_viewport emits ViewportZoomChangedEvent", "[canvas_workspace][w03]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench workbench(bus);

    workbench.viewport().set_zoom(3.0);

    bool received = false;
    double received_old = 0.0;
    double received_new = 0.0;
    auto sub = bus->subscribe<markamp::core::events::ViewportZoomChangedEvent>(
        [&](const markamp::core::events::ViewportZoomChangedEvent& evt)
        {
            received = true;
            received_old = evt.old_zoom;
            received_new = evt.new_zoom;
        });

    workbench.reset_viewport();
    REQUIRE(received);
    REQUIRE(received_old == 3.0);
    REQUIRE(received_new == 1.0);
}

TEST_CASE("zoom_to_fit with no active board does not crash", "[canvas_workspace][w03]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench workbench(bus);
    // No board created — should just log a warning and return
    workbench.zoom_to_fit(); // must not crash
    REQUIRE(workbench.viewport().zoom() == 1.0);
}

TEST_CASE("zoom_to_selection changes zoom and emits event", "[canvas_workspace][w03]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench workbench(bus);

    bool received = false;
    auto sub = bus->subscribe<markamp::core::events::ViewportZoomChangedEvent>(
        [&](const markamp::core::events::ViewportZoomChangedEvent& /*evt*/)
        { received = true; });

    // Zoom to a large selection should change zoom
    markamp::canvas::AABB selection{0.0, 0.0, 10000.0, 10000.0};
    workbench.zoom_to_selection(selection);
    REQUIRE(received);
}

// ============================================================================
// W04: Tool Rail Tests
// ============================================================================

TEST_CASE("ToolRailSelectionChangedEvent fields default correctly", "[canvas_workspace][w04]")
{
    markamp::core::events::ToolRailSelectionChangedEvent evt;
    REQUIRE(evt.tool_id.empty());
    REQUIRE(evt.tool_name.empty());
    REQUIRE(evt.group.empty());
}

TEST_CASE("ToolGroupExpandedEvent fields default correctly", "[canvas_workspace][w04]")
{
    markamp::core::events::ToolGroupExpandedEvent evt;
    REQUIRE(evt.group_name.empty());
    REQUIRE(evt.expanded == false);
}

TEST_CASE("ToolQuickSwitchEvent fields default correctly", "[canvas_workspace][w04]")
{
    markamp::core::events::ToolQuickSwitchEvent evt;
    REQUIRE(evt.from_tool_id.empty());
    REQUIRE(evt.to_tool_id.empty());
}

TEST_CASE("ToolRailSelectionChangedEvent roundtrips", "[canvas_workspace][w04]")
{
    markamp::core::EventBus bus;
    std::string received_id;
    std::string received_name;
    auto sub = bus.subscribe<markamp::core::events::ToolRailSelectionChangedEvent>(
        [&](const markamp::core::events::ToolRailSelectionChangedEvent& evt)
        {
            received_id = evt.tool_id;
            received_name = evt.tool_name;
        });

    markamp::core::events::ToolRailSelectionChangedEvent evt;
    evt.tool_id = "pen";
    evt.tool_name = "Pen Tool";
    bus.publish(evt);
    REQUIRE(received_id == "pen");
    REQUIRE(received_name == "Pen Tool");
}

TEST_CASE("ToolRailModel select_tool sets active", "[canvas_workspace][w04]")
{
    markamp::canvas::ToolRailModel rail;
    std::vector<markamp::canvas::ToolEntry> tools;
    tools.push_back({"select", "Select", markamp::canvas::ToolGroup::kSelect, true});
    tools.push_back({"pen", "Pen", markamp::canvas::ToolGroup::kDraw, true});
    rail.set_tools(std::move(tools));

    rail.select_tool("pen");
    REQUIRE(rail.active_tool() == "pen");
}

TEST_CASE("ToolRailModel quick_switch returns to previous tool", "[canvas_workspace][w04]")
{
    markamp::canvas::ToolRailModel rail;
    std::vector<markamp::canvas::ToolEntry> tools;
    tools.push_back({"select", "Select", markamp::canvas::ToolGroup::kSelect, true});
    tools.push_back({"pen", "Pen", markamp::canvas::ToolGroup::kDraw, true});
    rail.set_tools(std::move(tools));

    rail.select_tool("select");
    rail.select_tool("pen");
    rail.quick_switch();
    REQUIRE(rail.active_tool() == "select");
}

TEST_CASE("select_tool_by_id fails for unknown tool", "[canvas_workspace][w04]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench workbench(bus);

    REQUIRE_FALSE(workbench.select_tool_by_id("nonexistent"));
}

TEST_CASE("select_tool_by_id emits ToolRailSelectionChangedEvent", "[canvas_workspace][w04]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench workbench(bus);

    std::vector<markamp::canvas::ToolEntry> tools;
    tools.push_back({"select", "Select", markamp::canvas::ToolGroup::kSelect, true});
    tools.push_back({"pen", "Pen", markamp::canvas::ToolGroup::kDraw, true});
    workbench.tool_rail().set_tools(std::move(tools));

    bool received = false;
    std::string received_id;
    auto sub = bus->subscribe<markamp::core::events::ToolRailSelectionChangedEvent>(
        [&](const markamp::core::events::ToolRailSelectionChangedEvent& evt)
        {
            received = true;
            received_id = evt.tool_id;
        });

    REQUIRE(workbench.select_tool_by_id("pen"));
    REQUIRE(received);
    REQUIRE(received_id == "pen");
}

TEST_CASE("quick_switch_tool emits ToolQuickSwitchEvent", "[canvas_workspace][w04]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench workbench(bus);

    std::vector<markamp::canvas::ToolEntry> tools;
    tools.push_back({"select", "Select", markamp::canvas::ToolGroup::kSelect, true});
    tools.push_back({"pen", "Pen", markamp::canvas::ToolGroup::kDraw, true});
    workbench.tool_rail().set_tools(std::move(tools));
    workbench.tool_rail().select_tool("select");
    workbench.tool_rail().select_tool("pen");

    bool received = false;
    std::string from_id;
    std::string to_id;
    auto sub = bus->subscribe<markamp::core::events::ToolQuickSwitchEvent>(
        [&](const markamp::core::events::ToolQuickSwitchEvent& evt)
        {
            received = true;
            from_id = evt.from_tool_id;
            to_id = evt.to_tool_id;
        });

    workbench.quick_switch_tool();
    REQUIRE(received);
    REQUIRE(from_id == "pen");
    REQUIRE(to_id == "select");
}

// ============================================================================
// W05: Pointer Routing Tests
// ============================================================================

TEST_CASE("PointerCaptureChangedEvent fields default correctly", "[canvas_workspace][w05]")
{
    markamp::core::events::PointerCaptureChangedEvent evt;
    REQUIRE(evt.captured == false);
    REQUIRE(evt.tool_mode == 0);
}

TEST_CASE("PointerDeviceChangedEvent fields default correctly", "[canvas_workspace][w05]")
{
    markamp::core::events::PointerDeviceChangedEvent evt;
    REQUIRE(evt.device_type.empty());
}

TEST_CASE("PointerDeviceType enum has four values", "[canvas_workspace][w05]")
{
    REQUIRE(static_cast<uint8_t>(markamp::canvas::PointerDeviceType::kMouse) == 0);
    REQUIRE(static_cast<uint8_t>(markamp::canvas::PointerDeviceType::kTrackpad) == 1);
    REQUIRE(static_cast<uint8_t>(markamp::canvas::PointerDeviceType::kStylus) == 2);
    REQUIRE(static_cast<uint8_t>(markamp::canvas::PointerDeviceType::kTouch) == 3);
}

TEST_CASE("PointerCaptureChangedEvent roundtrips with values", "[canvas_workspace][w05]")
{
    markamp::core::EventBus bus;
    bool received_captured = false;
    auto sub = bus.subscribe<markamp::core::events::PointerCaptureChangedEvent>(
        [&](const markamp::core::events::PointerCaptureChangedEvent& evt)
        { received_captured = evt.captured; });

    markamp::core::events::PointerCaptureChangedEvent evt;
    evt.captured = true;
    evt.tool_mode = 2;
    bus.publish(evt);
    REQUIRE(received_captured == true);
}

TEST_CASE("PointerDeviceChangedEvent roundtrips with values", "[canvas_workspace][w05]")
{
    markamp::core::EventBus bus;
    std::string received_device;
    auto sub = bus.subscribe<markamp::core::events::PointerDeviceChangedEvent>(
        [&](const markamp::core::events::PointerDeviceChangedEvent& evt)
        { received_device = evt.device_type; });

    markamp::core::events::PointerDeviceChangedEvent evt;
    evt.device_type = "stylus";
    bus.publish(evt);
    REQUIRE(received_device == "stylus");
}

TEST_CASE("ViewportTransform zoom_in and zoom_out work correctly", "[canvas_workspace][w03]")
{
    markamp::canvas::ViewportTransform vp;
    REQUIRE(vp.zoom() == 1.0);

    vp.zoom_in();
    REQUIRE(vp.zoom() > 1.0);

    vp.zoom_out();
    // Should be back near 1.0
    REQUIRE(std::abs(vp.zoom() - 1.0) < 0.02);
}

TEST_CASE("ViewportTransform zoom_to_100 resets to exactly 1.0", "[canvas_workspace][w03]")
{
    markamp::canvas::ViewportTransform vp;
    vp.set_zoom(2.5);
    vp.zoom_to_100();
    REQUIRE(vp.zoom() == 1.0);
}

TEST_CASE("CameraModel zoom presets work correctly", "[canvas_workspace][w03]")
{
    markamp::canvas::CameraModel cam;
    cam.apply_preset(markamp::canvas::ZoomPreset::kTwoHundredPercent);
    REQUIRE(cam.zoom() == 2.0);

    cam.apply_preset(markamp::canvas::ZoomPreset::kFiftyPercent);
    REQUIRE(cam.zoom() == 0.5);

    cam.apply_preset(markamp::canvas::ZoomPreset::kHundredPercent);
    REQUIRE(cam.zoom() == 1.0);
}

TEST_CASE("ToolRailModel visible_tools respects visibility", "[canvas_workspace][w04]")
{
    markamp::canvas::ToolRailModel rail;
    std::vector<markamp::canvas::ToolEntry> tools;
    tools.push_back({"select", "Select", markamp::canvas::ToolGroup::kSelect, true});
    tools.push_back({"hidden", "Hidden", markamp::canvas::ToolGroup::kDraw, false});
    rail.set_tools(std::move(tools));

    auto visible = rail.visible_tools();
    REQUIRE(visible.size() == 1);
    REQUIRE(visible[0].tool_id == "select");
}

// ============================================================================
// V17 Phase 01 W06: Input State Machine
// ============================================================================

TEST_CASE("W06 ToolStateTransitionEvent defaults", "[w06][events]")
{
    markamp::core::events::ToolStateTransitionEvent evt;
    REQUIRE(evt.from_state.empty());
    REQUIRE(evt.to_state.empty());
    REQUIRE(evt.tool_mode == 0);
}

TEST_CASE("W06 ToolGestureCancelledEvent defaults", "[w06][events]")
{
    markamp::core::events::ToolGestureCancelledEvent evt;
    REQUIRE(evt.tool_mode == 0);
    REQUIRE(evt.reason.empty());
}

TEST_CASE("W06 escape_cancel from idle returns false", "[w06][state_machine]")
{
    markamp::canvas::CanvasToolStateMachine sm;
    REQUIRE_FALSE(sm.escape_cancel());
    REQUIRE(sm.current_state() == markamp::canvas::ToolState::kIdle);
}

TEST_CASE("W06 escape_cancel from drag returns true", "[w06][state_machine]")
{
    markamp::canvas::CanvasToolStateMachine sm;
    sm.transition_to(markamp::canvas::ToolState::kPressed);
    sm.transition_to(markamp::canvas::ToolState::kDrag);
    REQUIRE(sm.is_mid_gesture());
    REQUIRE(sm.escape_cancel());
    REQUIRE(sm.current_state() == markamp::canvas::ToolState::kIdle);
}

TEST_CASE("W06 is_mid_gesture correct", "[w06][state_machine]")
{
    markamp::canvas::CanvasToolStateMachine sm;
    REQUIRE_FALSE(sm.is_mid_gesture());
    sm.transition_to(markamp::canvas::ToolState::kHover);
    REQUIRE_FALSE(sm.is_mid_gesture());
    sm.transition_to(markamp::canvas::ToolState::kPressed);
    REQUIRE(sm.is_mid_gesture());
}

TEST_CASE("W06 state_history tracks transitions", "[w06][state_machine]")
{
    markamp::canvas::CanvasToolStateMachine sm;
    sm.transition_to(markamp::canvas::ToolState::kHover);
    sm.transition_to(markamp::canvas::ToolState::kPressed);
    REQUIRE(sm.state_history().size() == 2);
    REQUIRE(sm.state_history()[0] == "idle -> hover");
}

TEST_CASE("W06 resume_previous_state works", "[w06][state_machine]")
{
    markamp::canvas::CanvasToolStateMachine sm;
    sm.transition_to(markamp::canvas::ToolState::kHover);
    sm.transition_to(markamp::canvas::ToolState::kIdle);
    REQUIRE(sm.resume_previous_state()); // resume to hover
    REQUIRE(sm.current_state() == markamp::canvas::ToolState::kHover);
}

TEST_CASE("W06 cancel_active_gesture via workbench", "[w06][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    // From idle, nothing to cancel
    REQUIRE_FALSE(wb.cancel_active_gesture());
}

// ============================================================================
// V17 Phase 01 W07: Keyboard Commanding
// ============================================================================

TEST_CASE("W07 KeyCommandExecutedEvent defaults", "[w07][events]")
{
    markamp::core::events::KeyCommandExecutedEvent evt;
    REQUIRE(evt.command_id.empty());
    REQUIRE(evt.shortcut.empty());
    REQUIRE(evt.category.empty());
}

TEST_CASE("W07 KeyConflictDetectedEvent defaults", "[w07][events]")
{
    markamp::core::events::KeyConflictDetectedEvent evt;
    REQUIRE(evt.shortcut.empty());
    REQUIRE(evt.conflicting_commands.empty());
}

TEST_CASE("W07 execute_key_command not found", "[w07][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    REQUIRE_FALSE(wb.execute_key_command("nonexistent"));
}

TEST_CASE("W07 execute_key_command found", "[w07][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);

    std::vector<markamp::canvas::KeyBinding> bindings;
    bindings.push_back({"select_all", "Select All", "Ctrl+A", "edit", true});
    wb.keyboard_commands().set_bindings(std::move(bindings));

    REQUIRE(wb.execute_key_command("select_all"));
}

TEST_CASE("W07 keyboard_commands accessor works", "[w07][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.keyboard_commands().set_nudge_step(5.0);
    REQUIRE(wb.keyboard_commands().nudge_step() == 5.0);
}

TEST_CASE("W07 KeyboardCommandModel binding lookup", "[w07][model]")
{
    markamp::canvas::KeyboardCommandModel model;
    std::vector<markamp::canvas::KeyBinding> bindings;
    bindings.push_back({"delete", "Delete", "Del", "edit", true});
    model.set_bindings(std::move(bindings));

    auto found = model.find_binding("delete");
    REQUIRE(found.command_id == "delete");
    REQUIRE(found.shortcut == "Del");
}

// ============================================================================
// V17 Phase 01 W08: Cursor & Mode Feedback
// ============================================================================

TEST_CASE("W08 CursorStyleChangedEvent defaults", "[w08][events]")
{
    markamp::core::events::CursorStyleChangedEvent evt;
    REQUIRE(evt.cursor_style.empty());
    REQUIRE(evt.tool_mode == 0);
}

TEST_CASE("W08 ModeFeedbackChangedEvent defaults", "[w08][events]")
{
    markamp::core::events::ModeFeedbackChangedEvent evt;
    REQUIRE(evt.label.empty());
    REQUIRE(evt.icon_hint.empty());
}

TEST_CASE("W08 CursorFeedbackModel defaults", "[w08][model]")
{
    markamp::canvas::CursorFeedbackModel model;
    REQUIRE(model.cursor() == markamp::canvas::CanvasCursorStyle::kDefault);
    REQUIRE(model.mode_label().empty());
    REQUIRE(model.show_feedback());
}

TEST_CASE("W08 CursorFeedbackModel set/get", "[w08][model]")
{
    markamp::canvas::CursorFeedbackModel model;
    model.set_cursor(markamp::canvas::CanvasCursorStyle::kCrosshair);
    REQUIRE(model.cursor() == markamp::canvas::CanvasCursorStyle::kCrosshair);

    model.set_mode_label("Drawing");
    REQUIRE(model.mode_label() == "Drawing");

    model.set_show_feedback(false);
    REQUIRE_FALSE(model.show_feedback());
}

TEST_CASE("W08 cursor_name helper", "[w08][model]")
{
    using S = markamp::canvas::CanvasCursorStyle;
    using M = markamp::canvas::CursorFeedbackModel;
    REQUIRE(M::cursor_name(S::kDefault) == "default");
    REQUIRE(M::cursor_name(S::kCrosshair) == "crosshair");
    REQUIRE(M::cursor_name(S::kGrab) == "grab");
    REQUIRE(M::cursor_name(S::kForbidden) == "forbidden");
}

TEST_CASE("W08 set_cursor_for_tool updates model", "[w08][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.set_cursor_for_tool(markamp::canvas::ToolMode::Pan);
    REQUIRE(wb.cursor_feedback().cursor() == markamp::canvas::CanvasCursorStyle::kGrab);
    REQUIRE(wb.cursor_feedback().mode_label() == "Pan");
}

// ============================================================================
// V17 Phase 01 W09: Grid & Rulers
// ============================================================================

TEST_CASE("W09 GridVisibilityChangedEvent defaults", "[w09][events]")
{
    markamp::core::events::GridVisibilityChangedEvent evt;
    REQUIRE_FALSE(evt.visible);
    REQUIRE(evt.spacing == 20.0);
}

TEST_CASE("W09 RulerVisibilityChangedEvent defaults", "[w09][events]")
{
    markamp::core::events::RulerVisibilityChangedEvent evt;
    REQUIRE_FALSE(evt.visible);
    REQUIRE(evt.units.empty());
}

TEST_CASE("W09 GridRulerModel defaults", "[w09][model]")
{
    markamp::canvas::GridRulerModel model;
    REQUIRE_FALSE(model.is_grid_visible());
    REQUIRE_FALSE(model.is_ruler_visible());
    REQUIRE(model.grid_spacing() == 20.0);
    REQUIRE(model.ruler_units() == markamp::canvas::RulerUnits::kPixels);
}

TEST_CASE("W09 GridRulerModel set/get", "[w09][model]")
{
    markamp::canvas::GridRulerModel model;
    model.set_grid_visible(true);
    REQUIRE(model.is_grid_visible());

    model.set_ruler_visible(true);
    REQUIRE(model.is_ruler_visible());

    model.set_grid_spacing(40.0);
    REQUIRE(model.grid_spacing() == 40.0);

    model.set_ruler_units(markamp::canvas::RulerUnits::kMillimeters);
    REQUIRE(model.ruler_units() == markamp::canvas::RulerUnits::kMillimeters);
}

TEST_CASE("W09 units_string helper", "[w09][model]")
{
    using U = markamp::canvas::RulerUnits;
    using M = markamp::canvas::GridRulerModel;
    REQUIRE(M::units_string(U::kPixels) == "px");
    REQUIRE(M::units_string(U::kMillimeters) == "mm");
    REQUIRE(M::units_string(U::kInches) == "in");
    REQUIRE(M::units_string(U::kPoints) == "pt");
}

TEST_CASE("W09 toggle_grid via workbench", "[w09][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    REQUIRE_FALSE(wb.grid_ruler().is_grid_visible());
    wb.toggle_grid();
    REQUIRE(wb.grid_ruler().is_grid_visible());
    wb.toggle_grid();
    REQUIRE_FALSE(wb.grid_ruler().is_grid_visible());
}

TEST_CASE("W09 toggle_rulers via workbench", "[w09][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    REQUIRE_FALSE(wb.grid_ruler().is_ruler_visible());
    wb.toggle_rulers();
    REQUIRE(wb.grid_ruler().is_ruler_visible());
}

// ============================================================================
// V17 Phase 01 W10: Snap Preferences
// ============================================================================

TEST_CASE("W10 SnapModeChangedEvent defaults", "[w10][events]")
{
    markamp::core::events::SnapModeChangedEvent evt;
    REQUIRE_FALSE(evt.grid_snap);
    REQUIRE_FALSE(evt.object_snap);
    REQUIRE_FALSE(evt.angle_snap);
}

TEST_CASE("W10 SnapThresholdChangedEvent defaults", "[w10][events]")
{
    markamp::core::events::SnapThresholdChangedEvent evt;
    REQUIRE(evt.old_threshold == 8.0);
    REQUIRE(evt.new_threshold == 8.0);
}

TEST_CASE("W10 SnapPreferenceModel defaults", "[w10][model]")
{
    markamp::canvas::SnapPreferenceModel model;
    REQUIRE(model.grid_snap());
    REQUIRE(model.object_snap());
    REQUIRE_FALSE(model.angle_snap());
    REQUIRE(model.threshold() == 8.0);
    REQUIRE(model.is_any_enabled());
}

TEST_CASE("W10 SnapPreferenceModel set/get", "[w10][model]")
{
    markamp::canvas::SnapPreferenceModel model;
    model.set_grid_snap(false);
    model.set_object_snap(false);
    REQUIRE_FALSE(model.grid_snap());
    REQUIRE_FALSE(model.is_any_enabled());

    model.set_angle_snap(true);
    REQUIRE(model.is_any_enabled());

    model.set_threshold(12.0);
    REQUIRE(model.threshold() == 12.0);
}

TEST_CASE("W10 SnapPreferenceModel apply_to", "[w10][model]")
{
    markamp::canvas::SnapPreferenceModel prefs;
    prefs.set_grid_snap(false);
    prefs.set_object_snap(true);
    prefs.set_angle_snap(true);
    prefs.set_threshold(16.0);

    markamp::canvas::SnapConfig config;
    prefs.apply_to(config);
    REQUIRE_FALSE(config.grid_enabled);
    REQUIRE(config.object_snap_enabled);
    REQUIRE(config.snap_angle_enabled);
    REQUIRE(config.snap_threshold == 16.0);
}

TEST_CASE("W10 toggle_snap_mode via workbench", "[w10][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    // Grid snap starts as true (default)
    REQUIRE(wb.snap_prefs().grid_snap());
    wb.toggle_snap_mode("grid");
    REQUIRE_FALSE(wb.snap_prefs().grid_snap());
    wb.toggle_snap_mode("grid");
    REQUIRE(wb.snap_prefs().grid_snap());
}

TEST_CASE("W10 toggle_snap_mode angle", "[w10][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    REQUIRE_FALSE(wb.snap_prefs().angle_snap());
    wb.toggle_snap_mode("angle");
    REQUIRE(wb.snap_prefs().angle_snap());
}

// ============================================================================
// V17 Phase 01 W11: Context Menus
// ============================================================================

TEST_CASE("W11 ContextMenuOpenedEvent defaults", "[w11][events]")
{
    markamp::core::events::ContextMenuOpenedEvent evt;
    REQUIRE(evt.scope.empty());
    REQUIRE(evt.object_type.empty());
    REQUIRE(evt.action_count == 0);
}

TEST_CASE("W11 ContextMenuActionExecutedEvent defaults", "[w11][events]")
{
    markamp::core::events::ContextMenuActionExecutedEvent evt;
    REQUIRE(evt.action_id.empty());
    REQUIRE(evt.scope.empty());
}

TEST_CASE("W11 ContextMenuModel scope and actions", "[w11][model]")
{
    markamp::canvas::ContextMenuModel model;
    REQUIRE(model.scope() == markamp::canvas::ContextScope::kEmptySpace);

    model.set_scope(markamp::canvas::ContextScope::kSingleObject);
    REQUIRE(model.scope() == markamp::canvas::ContextScope::kSingleObject);

    model.set_object_type("sticky_note");
    REQUIRE(model.object_type() == "sticky_note");

    markamp::canvas::ContextAction action;
    action.action_id = "canvas.cut";
    action.label = "Cut";
    action.group = "edit";
    action.applicable = true;
    model.set_actions({action});
    REQUIRE(model.actions().size() == 1);
    REQUIRE(model.applicable_actions().size() == 1);
}

TEST_CASE("W11 show_context_menu workbench", "[w11][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.show_context_menu(markamp::canvas::ContextScope::kMultiSelect);
    REQUIRE(wb.context_menu_model().scope() == markamp::canvas::ContextScope::kMultiSelect);
}

TEST_CASE("W11 show_context_menu with object type", "[w11][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.show_context_menu(markamp::canvas::ContextScope::kSingleObject, "text_box");
    REQUIRE(wb.context_menu_model().scope() == markamp::canvas::ContextScope::kSingleObject);
    REQUIRE(wb.context_menu_model().object_type() == "text_box");
}

TEST_CASE("W11 ContextAction helpers", "[w11][model]")
{
    markamp::canvas::ContextAction action;
    action.action_id = "canvas.paste";
    action.label = "Paste";
    action.group = "edit";
    action.applicable = true;
    REQUIRE(action.has_label());
    REQUIRE(action.has_group());
    REQUIRE(action.is_applicable());
}

// ============================================================================
// V17 Phase 01 W12: Undo & Redo Plumbing
// ============================================================================

TEST_CASE("W12 UndoExecutedEvent defaults", "[w12][events]")
{
    markamp::core::events::UndoExecutedEvent evt;
    REQUIRE(evt.description.empty());
    REQUIRE(evt.remaining_count == 0);
}

TEST_CASE("W12 RedoExecutedEvent defaults", "[w12][events]")
{
    markamp::core::events::RedoExecutedEvent evt;
    REQUIRE(evt.description.empty());
    REQUIRE(evt.remaining_count == 0);
}

TEST_CASE("W12 perform_undo on empty stack", "[w12][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    REQUIRE_FALSE(wb.perform_undo());
}

TEST_CASE("W12 perform_redo on empty stack", "[w12][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    REQUIRE_FALSE(wb.perform_redo());
}

TEST_CASE("W12 UndoRedoStack can_undo and can_redo defaults", "[w12][model]")
{
    markamp::canvas::UndoRedoStack stack;
    REQUIRE_FALSE(stack.can_undo());
    REQUIRE_FALSE(stack.can_redo());
    REQUIRE(stack.is_empty());
    REQUIRE(stack.undo_count() == 0);
    REQUIRE(stack.redo_count() == 0);
}

TEST_CASE("W12 undo_stack accessible via workbench", "[w12][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    REQUIRE(wb.undo_stack().is_empty());
    REQUIRE(wb.undo_stack().max_history() == 100);
}

// ============================================================================
// V17 Phase 01 W13: Autosave & Recovery
// ============================================================================

TEST_CASE("W13 AutosaveTriggeredEvent defaults", "[w13][events]")
{
    markamp::core::events::AutosaveTriggeredEvent evt;
    REQUIRE(evt.board_id.empty());
    REQUIRE(evt.save_path.empty());
}

TEST_CASE("W13 AutosaveRecoveryDetectedEvent defaults", "[w13][events]")
{
    markamp::core::events::AutosaveRecoveryDetectedEvent evt;
    REQUIRE(evt.board_id.empty());
    REQUIRE(evt.recovery_path.empty());
    REQUIRE(evt.timestamp.empty());
}

TEST_CASE("W13 AutosaveModel defaults", "[w13][model]")
{
    markamp::canvas::AutosaveModel model;
    REQUIRE(model.is_enabled());
    REQUIRE_FALSE(model.is_dirty());
    REQUIRE(model.interval_ms() == 30000);
    REQUIRE(model.state() == markamp::canvas::AutosaveState::kIdle);
    REQUIRE_FALSE(model.has_recovery());
}

TEST_CASE("W13 AutosaveModel dirty tracking", "[w13][model]")
{
    markamp::canvas::AutosaveModel model;
    REQUIRE_FALSE(model.is_dirty());
    model.mark_dirty();
    REQUIRE(model.is_dirty());
    model.clear_dirty();
    REQUIRE_FALSE(model.is_dirty());
}

TEST_CASE("W13 AutosaveModel state transitions", "[w13][model]")
{
    markamp::canvas::AutosaveModel model;
    model.set_state(markamp::canvas::AutosaveState::kPending);
    REQUIRE(model.state() == markamp::canvas::AutosaveState::kPending);
    model.set_state(markamp::canvas::AutosaveState::kSaving);
    REQUIRE(model.state() == markamp::canvas::AutosaveState::kSaving);
    model.set_state(markamp::canvas::AutosaveState::kFailed);
    REQUIRE(model.state() == markamp::canvas::AutosaveState::kFailed);
}

TEST_CASE("W13 AutosaveModel state_name helper", "[w13][model]")
{
    using markamp::canvas::AutosaveModel;
    using markamp::canvas::AutosaveState;
    REQUIRE(std::string(AutosaveModel::state_name(AutosaveState::kIdle)) == "idle");
    REQUIRE(std::string(AutosaveModel::state_name(AutosaveState::kPending)) == "pending");
    REQUIRE(std::string(AutosaveModel::state_name(AutosaveState::kSaving)) == "saving");
    REQUIRE(std::string(AutosaveModel::state_name(AutosaveState::kFailed)) == "failed");
}

TEST_CASE("W13 trigger_autosave skips when not dirty", "[w13][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.autosave().set_enabled(true);
    // Not dirty, so trigger should be a no-op
    wb.trigger_autosave();
    REQUIRE(wb.autosave().state() == markamp::canvas::AutosaveState::kIdle);
}

TEST_CASE("W13 trigger_autosave clears dirty", "[w13][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.autosave().set_enabled(true);
    wb.autosave().mark_dirty();
    REQUIRE(wb.autosave().is_dirty());
    wb.trigger_autosave();
    REQUIRE_FALSE(wb.autosave().is_dirty());
    REQUIRE(wb.autosave().state() == markamp::canvas::AutosaveState::kIdle);
}

// ============================================================================
// V17 Phase 01 W14: Inspector Shell
// ============================================================================

TEST_CASE("W14 InspectorOpenedEvent defaults", "[w14][events]")
{
    markamp::core::events::InspectorOpenedEvent evt;
    REQUIRE(evt.section_count == 0);
    REQUIRE(evt.selected_count == 0);
    REQUIRE_FALSE(evt.is_multi_select);
}

TEST_CASE("W14 InspectorPropertyChangedEvent defaults", "[w14][events]")
{
    markamp::core::events::InspectorPropertyChangedEvent evt;
    REQUIRE(evt.property_key.empty());
    REQUIRE(evt.old_value.empty());
    REQUIRE(evt.new_value.empty());
}

TEST_CASE("W14 InspectorModel defaults", "[w14][model]")
{
    markamp::canvas::InspectorModel model;
    REQUIRE(model.visible_sections().empty());
    REQUIRE(model.selected_count() == 0);
    REQUIRE_FALSE(model.is_multi_select());
    REQUIRE_FALSE(model.has_copied_style());
}

TEST_CASE("W14 InspectorModel section visibility", "[w14][model]")
{
    markamp::canvas::InspectorModel model;
    model.set_visible_sections({markamp::canvas::InspectorSection::kTransform,
                                markamp::canvas::InspectorSection::kAppearance});
    REQUIRE(model.visible_sections().size() == 2);
    REQUIRE(model.is_section_visible(markamp::canvas::InspectorSection::kTransform));
    REQUIRE_FALSE(model.is_section_visible(markamp::canvas::InspectorSection::kMedia));
}

TEST_CASE("W14 InspectorModel multi-select", "[w14][model]")
{
    markamp::canvas::InspectorModel model;
    model.set_selected_count(3);
    REQUIRE(model.selected_count() == 3);
    REQUIRE(model.is_multi_select());
}

TEST_CASE("W14 toggle_inspector workbench", "[w14][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.toggle_inspector();
    // Accessor works
    REQUIRE(wb.inspector().selected_count() == 0);
}

// ============================================================================
// V17 Phase 01 W15: Minimap Shell
// ============================================================================

TEST_CASE("W15 MinimapVisibilityChangedEvent defaults", "[w15][events]")
{
    markamp::core::events::MinimapVisibilityChangedEvent evt;
    REQUIRE_FALSE(evt.visible);
    REQUIRE(evt.mode.empty());
}

TEST_CASE("W15 MinimapNavigationEvent defaults", "[w15][events]")
{
    markamp::core::events::MinimapNavigationEvent evt;
    REQUIRE(evt.target_x == 0.0);
    REQUIRE(evt.target_y == 0.0);
    REQUIRE(evt.from_minimap);
}

TEST_CASE("W15 MinimapModel defaults", "[w15][model]")
{
    markamp::canvas::MinimapModel model;
    REQUIRE(model.viewport_x() == 0.0);
    REQUIRE(model.viewport_y() == 0.0);
    REQUIRE(model.viewport_w() == 800.0);
    REQUIRE(model.viewport_h() == 600.0);
    REQUIRE(model.mode() == markamp::canvas::MinimapMode::kSimplified);
    REQUIRE(model.markers().empty());
    REQUIRE(model.history_depth() == 0);
}

TEST_CASE("W15 MinimapModel viewport setting", "[w15][model]")
{
    markamp::canvas::MinimapModel model;
    model.set_viewport(100.0, 200.0, 1024.0, 768.0);
    REQUIRE(model.viewport_x() == 100.0);
    REQUIRE(model.viewport_y() == 200.0);
    REQUIRE(model.viewport_w() == 1024.0);
    REQUIRE(model.viewport_h() == 768.0);
}

TEST_CASE("W15 MinimapModel frame markers", "[w15][model]")
{
    markamp::canvas::MinimapModel model;
    markamp::canvas::FrameMarker marker;
    marker.frame_id = "frame1";
    marker.label = "Section A";
    marker.center_x = 500.0;
    marker.center_y = 300.0;
    model.set_markers({marker});
    REQUIRE(model.markers().size() == 1);
    REQUIRE(model.markers()[0].label == "Section A");
}

TEST_CASE("W15 toggle_minimap workbench", "[w15][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.toggle_minimap();
    REQUIRE(wb.minimap().mode() == markamp::canvas::MinimapMode::kSimplified);
}

// ============================================================================
// V17 Phase 01 W16: Onboarding & Empty States
// ============================================================================

TEST_CASE("W16 OnboardingStepCompletedEvent defaults", "[w16][events]")
{
    markamp::core::events::OnboardingStepCompletedEvent evt;
    REQUIRE(evt.step_id.empty());
    REQUIRE(evt.progress_percent == 0);
}

TEST_CASE("W16 OnboardingDismissedEvent defaults", "[w16][events]")
{
    markamp::core::events::OnboardingDismissedEvent evt;
    REQUIRE(evt.hint_id.empty());
    REQUIRE(evt.dismissed_by_user);
}

TEST_CASE("W16 OnboardingModel defaults", "[w16][model]")
{
    markamp::canvas::OnboardingModel model;
    REQUIRE(model.steps().empty());
    // With zero steps, all are considered complete (0/0 = 100%)
    REQUIRE(model.progress_percent() == 100);
    REQUIRE(model.is_complete());
    REQUIRE_FALSE(model.reference_visible());
    REQUIRE(model.starter_templates().empty());
}

TEST_CASE("W16 OnboardingModel step completion", "[w16][model]")
{
    markamp::canvas::OnboardingModel model;
    markamp::canvas::WalkthroughStep step;
    step.step_id = "intro";
    step.title = "Welcome";
    step.instruction = "Click here to begin";
    model.set_steps({step});
    REQUIRE(model.steps().size() == 1);
    REQUIRE_FALSE(model.is_complete());
    model.complete_step("intro");
    REQUIRE(model.is_complete());
    REQUIRE(model.progress_percent() == 100);
}

TEST_CASE("W16 OnboardingModel hint dismissal", "[w16][model]")
{
    markamp::canvas::OnboardingModel model;
    REQUIRE_FALSE(model.is_hint_dismissed("tip_drag"));
    model.dismiss_hint("tip_drag");
    REQUIRE(model.is_hint_dismissed("tip_drag"));
}

TEST_CASE("W16 complete_onboarding_step workbench", "[w16][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    markamp::canvas::WalkthroughStep step;
    step.step_id = "first";
    step.title = "First Step";
    step.instruction = "Do the thing";
    wb.onboarding().set_steps({step});
    wb.complete_onboarding_step("first");
    REQUIRE(wb.onboarding().is_complete());
}

// ============================================================================
// V17 Phase 01 W17: Board Settings
// ============================================================================

TEST_CASE("W17 BoardSettingsChangedEvent defaults", "[w17][events]")
{
    markamp::core::events::BoardSettingsChangedEvent evt;
    REQUIRE(evt.board_id.empty());
    REQUIRE(evt.setting_key.empty());
    REQUIRE(evt.new_value.empty());
}

TEST_CASE("W17 BoardBackgroundChangedEvent defaults", "[w17][events]")
{
    markamp::core::events::BoardBackgroundChangedEvent evt;
    REQUIRE(evt.board_id.empty());
    REQUIRE(evt.background_preset.empty());
}

TEST_CASE("W17 BoardSettingsModel defaults", "[w17][model]")
{
    markamp::canvas::BoardSettingsModel model;
    REQUIRE(model.background() == "white");
    REQUIRE(model.grid_spacing() == 20.0);
    REQUIRE(model.permission() == markamp::canvas::BoardPermission::kOwner);
    REQUIRE(model.width() == 4096.0);
    REQUIRE(model.height() == 4096.0);
    REQUIRE(model.setting_count() == 0);
    REQUIRE(model.is_owner());
    REQUIRE_FALSE(model.is_read_only());
}

TEST_CASE("W17 BoardSettingsModel custom settings", "[w17][model]")
{
    markamp::canvas::BoardSettingsModel model;
    model.set_setting("snap_to_grid", "true");
    model.set_setting("show_rulers", "false");
    REQUIRE(model.setting_count() == 2);
    REQUIRE(model.setting("snap_to_grid") == "true");
    REQUIRE(model.setting("show_rulers") == "false");
    REQUIRE(model.setting("unknown").empty());
}

TEST_CASE("W17 BoardSettingsModel dimensions", "[w17][model]")
{
    markamp::canvas::BoardSettingsModel model;
    model.set_dimensions(8192.0, 4096.0);
    REQUIRE(model.width() == 8192.0);
    REQUIRE(model.height() == 4096.0);
    REQUIRE(model.area() == 8192.0 * 4096.0);
}

TEST_CASE("W17 apply_board_setting workbench", "[w17][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.apply_board_setting("theme", "dark");
    REQUIRE(wb.board_settings().setting("theme") == "dark");
}

// ============================================================================
// V17 Phase 01 W18: Theme Tokens
// ============================================================================

TEST_CASE("W18 CanvasThemeTokenResolvedEvent defaults", "[w18][events]")
{
    markamp::core::events::CanvasThemeTokenResolvedEvent evt;
    REQUIRE(evt.token_name.empty());
    REQUIRE(evt.resolved_value.empty());
}

TEST_CASE("W18 CanvasThemeContrastWarningEvent defaults", "[w18][events]")
{
    markamp::core::events::CanvasThemeContrastWarningEvent evt;
    REQUIRE(evt.token_name.empty());
    REQUIRE(evt.contrast_ratio == 0.0);
    REQUIRE(evt.min_required == 3.0);
}

TEST_CASE("W18 CanvasThemeModel defaults", "[w18][model]")
{
    markamp::canvas::CanvasThemeModel model;
    REQUIRE(model.tokens().empty());
    REQUIRE(model.background() == markamp::canvas::BoardBackground::kWhite);
    REQUIRE(model.min_contrast() == 3.0);
}

TEST_CASE("W18 CanvasThemeModel token resolution", "[w18][model]")
{
    markamp::canvas::CanvasThemeModel model;
    markamp::canvas::CanvasToken token;
    token.token_name = "canvas.bg";
    token.value = "#1e1e2e";
    token.fallback = "#ffffff";
    model.set_tokens({token});
    REQUIRE(model.tokens().size() == 1);
    REQUIRE(model.resolve("canvas.bg") == "#1e1e2e");
}

TEST_CASE("W18 CanvasThemeModel contrast check", "[w18][model]")
{
    markamp::canvas::CanvasThemeModel model;
    model.set_min_contrast(4.5);
    REQUIRE(model.min_contrast() == 4.5);
    REQUIRE(model.passes_contrast(5.0));
    REQUIRE_FALSE(model.passes_contrast(3.0));
}

TEST_CASE("W18 resolve_theme_token workbench", "[w18][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    // Resolve unknown token returns empty
    auto result = wb.resolve_theme_token("nonexistent");
    REQUIRE(result.empty());
}

// ============================================================================
// V17 Phase 01 W19: Command Palette Integration
// ============================================================================

TEST_CASE("W19 CommandPaletteOpenedEvent defaults", "[w19][events]")
{
    markamp::core::events::CommandPaletteOpenedEvent evt;
    REQUIRE(evt.command_count == 0);
    REQUIRE(evt.context.empty());
}

TEST_CASE("W19 CommandPaletteExecutedEvent defaults", "[w19][events]")
{
    markamp::core::events::CommandPaletteExecutedEvent evt;
    REQUIRE(evt.command_id.empty());
    REQUIRE(evt.source.empty());
}

TEST_CASE("W19 CommandPaletteModel defaults", "[w19][model]")
{
    markamp::ui::CommandPaletteModel model;
    REQUIRE(model.commands().empty());
    REQUIRE(model.command_count() == 0);
    REQUIRE(model.mru_history().empty());
}

TEST_CASE("W19 CommandPaletteModel add and search", "[w19][model]")
{
    markamp::ui::CommandPaletteModel model;
    markamp::ui::CommandMetadata cmd;
    cmd.command_id = "canvas.zoom_in";
    cmd.label = "Zoom In";
    cmd.category = "View";
    cmd.shortcut = "Ctrl+=";
    model.add_command(cmd);
    REQUIRE(model.command_count() == 1);
    REQUIRE(model.commands()[0].label == "Zoom In");
}

TEST_CASE("W19 CommandPaletteModel MRU", "[w19][model]")
{
    markamp::ui::CommandPaletteModel model;
    markamp::ui::CommandMetadata cmd;
    cmd.command_id = "canvas.select_all";
    cmd.label = "Select All";
    model.add_command(cmd);
    model.record_usage("canvas.select_all");
    REQUIRE(model.mru_history().size() == 1);
    REQUIRE(model.mru_history()[0] == "canvas.select_all");
}

TEST_CASE("W19 open_command_palette workbench", "[w19][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.open_command_palette();
    REQUIRE(wb.command_palette().command_count() == 0);
}

// ============================================================================
// V17 Phase 01 W20: Event & Analytics Contracts
// ============================================================================

TEST_CASE("W20 AnalyticsEventRecordedEvent defaults", "[w20][events]")
{
    markamp::core::events::AnalyticsEventRecordedEvent evt;
    REQUIRE(evt.event_name.empty());
    REQUIRE(evt.category.empty());
}

TEST_CASE("W20 AnalyticsFlushRequestedEvent defaults", "[w20][events]")
{
    markamp::core::events::AnalyticsFlushRequestedEvent evt;
    REQUIRE(evt.pending_count == 0);
}

TEST_CASE("W20 CanvasAnalyticsModel defaults", "[w20][model]")
{
    markamp::canvas::CanvasAnalyticsModel model;
    REQUIRE(model.events().empty());
    REQUIRE(model.pending_count() == 0);
    REQUIRE(model.flush_state() == markamp::canvas::AnalyticsFlushState::kIdle);
    REQUIRE_FALSE(model.is_opted_out());
    REQUIRE(model.is_idle());
}

TEST_CASE("W20 CanvasAnalyticsModel recording and category", "[w20][model]")
{
    markamp::canvas::CanvasAnalyticsModel model;
    model.record_event("board_opened", "engagement");
    model.record_event("tool_selected", "interaction");
    model.record_event("board_closed", "engagement");
    REQUIRE(model.pending_count() == 3);
    REQUIRE(model.count_by_category("engagement") == 2);
    REQUIRE(model.count_by_category("interaction") == 1);
}

TEST_CASE("W20 CanvasAnalyticsModel flush lifecycle", "[w20][model]")
{
    markamp::canvas::CanvasAnalyticsModel model;
    model.record_event("test", "cat");
    REQUIRE(model.pending_count() == 1);
    model.start_flush();
    REQUIRE(model.is_flushing());
    model.complete_flush();
    REQUIRE(model.is_idle());
    REQUIRE(model.pending_count() == 0);
}

TEST_CASE("W20 CanvasAnalyticsModel opt-out", "[w20][model]")
{
    markamp::canvas::CanvasAnalyticsModel model;
    model.set_opted_out(true);
    REQUIRE(model.is_opted_out());
    model.record_event("should_not_record", "test");
    REQUIRE(model.pending_count() == 0);
}

TEST_CASE("W20 record_analytics_event workbench", "[w20][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.record_analytics_event("board_created", "lifecycle");
    REQUIRE(wb.canvas_analytics().pending_count() == 1);
    REQUIRE(wb.canvas_analytics().count_by_category("lifecycle") == 1);
}

// ============================================================================
// V17 Phase 02 W01: Single Selection
// ============================================================================

TEST_CASE("P02-W01 SingleSelectionEvent defaults", "[p02-w01][events]")
{
    markamp::core::events::SingleSelectionEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.object_type.empty());
    REQUIRE(evt.hit_target.empty());
}

TEST_CASE("P02-W01 SelectionClearedEvent defaults", "[p02-w01][events]")
{
    markamp::core::events::SelectionClearedEvent evt;
    REQUIRE(evt.previous_count == 0);
}

TEST_CASE("P02-W01 SelectionManager single select", "[p02-w01][model]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::SelectionManager sel(bus);
    sel.select(42);
    REQUIRE(sel.is_selected(42));
    REQUIRE(sel.is_single_selection());
    REQUIRE(sel.selection_count() == 1);
}

TEST_CASE("P02-W01 SelectionManager clear", "[p02-w01][model]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::SelectionManager sel(bus);
    sel.select(1);
    sel.select(2);
    sel.clear_selection();
    REQUIRE_FALSE(sel.has_selection());
    REQUIRE(sel.selection_count() == 0);
}

TEST_CASE("P02-W01 select_object workbench", "[p02-w01][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.select_object(99);
    // Selection is managed by the internal SelectionManager
}

TEST_CASE("P02-W01 clear_selection workbench", "[p02-w01][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.select_object(1);
    wb.clear_selection();
    // Selection cleared (no crash, event emitted)
}

// ============================================================================
// V17 Phase 02 W02: Multi Selection
// ============================================================================

TEST_CASE("P02-W02 MultiSelectionEvent defaults", "[p02-w02][events]")
{
    markamp::core::events::MultiSelectionEvent evt;
    REQUIRE(evt.selected_count == 0);
    REQUIRE(evt.method.empty());
}

TEST_CASE("P02-W02 SelectAllEvent defaults", "[p02-w02][events]")
{
    markamp::core::events::SelectAllEvent evt;
    REQUIRE(evt.total_count == 0);
}

TEST_CASE("P02-W02 SelectionManager multi select", "[p02-w02][model]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::SelectionManager sel(bus);
    sel.select(1);
    sel.add_to_selection(2);
    sel.add_to_selection(3);
    REQUIRE(sel.is_multi_selection());
    REQUIRE(sel.selection_count() == 3);
}

TEST_CASE("P02-W02 SelectionManager toggle", "[p02-w02][model]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::SelectionManager sel(bus);
    sel.select(10);
    sel.add_to_selection(20);
    sel.toggle_selection(10);
    REQUIRE_FALSE(sel.is_selected(10));
    REQUIRE(sel.is_selected(20));
    REQUIRE(sel.is_single_selection());
}

TEST_CASE("P02-W02 box_select workbench no board", "[p02-w02][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    markamp::canvas::AABB region;
    region.min_x = 0.0;
    region.min_y = 0.0;
    region.max_x = 100.0;
    region.max_y = 100.0;
    // No active board, should return safely
    wb.box_select(region);
}

TEST_CASE("P02-W02 select_all_objects workbench no board", "[p02-w02][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    // No active board, should return safely
    wb.select_all_objects();
}

// ============================================================================
// V17 Phase 02 W03: Move Operations
// ============================================================================

TEST_CASE("P02-W03 MoveStartedEvent defaults", "[p02-w03][events]")
{
    markamp::core::events::MoveStartedEvent evt;
    REQUIRE(evt.object_count == 0);
    REQUIRE(evt.start_x == 0.0);
    REQUIRE(evt.start_y == 0.0);
}

TEST_CASE("P02-W03 MoveCompletedEvent defaults", "[p02-w03][events]")
{
    markamp::core::events::MoveCompletedEvent evt;
    REQUIRE(evt.object_count == 0);
    REQUIRE(evt.delta_x == 0.0);
    REQUIRE(evt.delta_y == 0.0);
}

TEST_CASE("P02-W03 SelectionManager move state", "[p02-w03][model]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::SelectionManager sel(bus);
    REQUIRE_FALSE(sel.is_moving());
    REQUIRE_FALSE(sel.is_transforming());
}

TEST_CASE("P02-W03 begin_object_move workbench no board", "[p02-w03][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    // No active board, should return safely
    wb.begin_object_move();
}

TEST_CASE("P02-W03 end_object_move workbench", "[p02-w03][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    // End move with delta (no active operation, safe no-op)
    wb.end_object_move(10.0, 20.0);
}

TEST_CASE("P02-W03 HitTestModel defaults", "[p02-w03][model]")
{
    markamp::canvas::HitTestModel hit_model;
    REQUIRE(hit_model.candidates().empty());
    REQUIRE(hit_model.latency_ms() == 0.0);
    REQUIRE(hit_model.is_within_budget(16.0));
    REQUIRE(hit_model.prefetch_candidates().empty());
}

// ============================================================================
// V17 Phase 02 W04: Resize Operations
// ============================================================================

TEST_CASE("P02-W04 ResizeStartedEvent defaults", "[p02-w04][events]")
{
    markamp::core::events::ResizeStartedEvent evt;
    REQUIRE(evt.object_count == 0);
    REQUIRE(evt.handle.empty());
}

TEST_CASE("P02-W04 ResizeCompletedEvent defaults", "[p02-w04][events]")
{
    markamp::core::events::ResizeCompletedEvent evt;
    REQUIRE(evt.object_count == 0);
    REQUIRE(evt.scale_x == 1.0);
    REQUIRE(evt.scale_y == 1.0);
}

TEST_CASE("P02-W04 SelectionManager resize state", "[p02-w04][model]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::SelectionManager sel(bus);
    REQUIRE_FALSE(sel.is_resizing());
    REQUIRE(sel.active_handle() == markamp::canvas::HandleType::None);
}

TEST_CASE("P02-W04 begin_resize workbench no board", "[p02-w04][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.begin_resize("BottomRight");
}

TEST_CASE("P02-W04 end_resize workbench", "[p02-w04][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.end_resize(1.5, 2.0);
}

TEST_CASE("P02-W04 HandleType enum values", "[p02-w04][model]")
{
    REQUIRE(static_cast<int>(markamp::canvas::HandleType::TopLeft) == 0);
    REQUIRE(static_cast<int>(markamp::canvas::HandleType::Rotation) == 8);
    REQUIRE(static_cast<int>(markamp::canvas::HandleType::None) == 9);
}

// ============================================================================
// V17 Phase 02 W05: Rotation Operations
// ============================================================================

TEST_CASE("P02-W05 RotateStartedEvent defaults", "[p02-w05][events]")
{
    markamp::core::events::RotateStartedEvent evt;
    REQUIRE(evt.object_count == 0);
}

TEST_CASE("P02-W05 RotateCompletedEvent defaults", "[p02-w05][events]")
{
    markamp::core::events::RotateCompletedEvent evt;
    REQUIRE(evt.object_count == 0);
    REQUIRE(evt.angle_degrees == 0.0);
}

TEST_CASE("P02-W05 SelectionManager rotate state", "[p02-w05][model]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::SelectionManager sel(bus);
    REQUIRE_FALSE(sel.is_rotating());
}

TEST_CASE("P02-W05 begin_rotate workbench no board", "[p02-w05][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.begin_rotate();
}

TEST_CASE("P02-W05 end_rotate workbench", "[p02-w05][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.end_rotate(45.0);
}

TEST_CASE("P02-W05 transforming check", "[p02-w05][model]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::SelectionManager sel(bus);
    REQUIRE_FALSE(sel.is_transforming());
}

// ============================================================================
// V17 Phase 02 W06: Alignment Guides
// ============================================================================

TEST_CASE("P02-W06 AlignmentGuideShownEvent defaults", "[p02-w06][events]")
{
    markamp::core::events::AlignmentGuideShownEvent evt;
    REQUIRE(evt.guide_count == 0);
    REQUIRE(evt.axis.empty());
}

TEST_CASE("P02-W06 AlignmentGuideHiddenEvent defaults", "[p02-w06][events]")
{
    markamp::core::events::AlignmentGuideHiddenEvent evt;
    (void)evt; // No fields to check
}

TEST_CASE("P02-W06 show_alignment_guides workbench", "[p02-w06][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.show_alignment_guides(3, "horizontal");
}

TEST_CASE("P02-W06 hide_alignment_guides workbench", "[p02-w06][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.hide_alignment_guides();
}

TEST_CASE("P02-W06 show then hide guides", "[p02-w06][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.show_alignment_guides(5, "both");
    wb.hide_alignment_guides();
}

TEST_CASE("P02-W06 event fields populated", "[p02-w06][events]")
{
    markamp::core::events::AlignmentGuideShownEvent evt;
    evt.guide_count = 4;
    evt.axis = "vertical";
    REQUIRE(evt.guide_count == 4);
    REQUIRE(evt.axis == "vertical");
}

// ============================================================================
// V17 Phase 02 W07: Object Snapping
// ============================================================================

TEST_CASE("P02-W07 SnapEngagedEvent defaults", "[p02-w07][events]")
{
    markamp::core::events::SnapEngagedEvent evt;
    REQUIRE(evt.snap_type.empty());
    REQUIRE(evt.target_id.empty());
}

TEST_CASE("P02-W07 SnapDisengagedEvent defaults", "[p02-w07][events]")
{
    markamp::core::events::SnapDisengagedEvent evt;
    (void)evt;
}

TEST_CASE("P02-W07 engage_snap workbench", "[p02-w07][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.engage_snap("edge", "obj_42");
}

TEST_CASE("P02-W07 disengage_snap workbench", "[p02-w07][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.disengage_snap();
}

TEST_CASE("P02-W07 engage then disengage", "[p02-w07][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.engage_snap("center", "obj_10");
    wb.disengage_snap();
}

TEST_CASE("P02-W07 snap event fields", "[p02-w07][events]")
{
    markamp::core::events::SnapEngagedEvent evt;
    evt.snap_type = "grid";
    evt.target_id = "obj_99";
    REQUIRE(evt.snap_type == "grid");
    REQUIRE(evt.target_id == "obj_99");
}

// ============================================================================
// V17 Phase 02 W08: Distribute & Align Actions
// ============================================================================

TEST_CASE("P02-W08 AlignActionEvent defaults", "[p02-w08][events]")
{
    markamp::core::events::AlignActionEvent evt;
    REQUIRE(evt.action.empty());
    REQUIRE(evt.object_count == 0);
}

TEST_CASE("P02-W08 DistributeActionEvent defaults", "[p02-w08][events]")
{
    markamp::core::events::DistributeActionEvent evt;
    REQUIRE(evt.axis.empty());
    REQUIRE(evt.object_count == 0);
}

TEST_CASE("P02-W08 align_selected workbench", "[p02-w08][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.align_selected("left");
}

TEST_CASE("P02-W08 distribute_selected workbench", "[p02-w08][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.distribute_selected("horizontal");
}

TEST_CASE("P02-W08 all align actions", "[p02-w08][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.align_selected("left");
    wb.align_selected("center_h");
    wb.align_selected("right");
    wb.align_selected("top");
    wb.align_selected("center_v");
    wb.align_selected("bottom");
}

TEST_CASE("P02-W08 distribute both axes", "[p02-w08][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.distribute_selected("horizontal");
    wb.distribute_selected("vertical");
}

// ============================================================================
// V17 Phase 02 W09: Smart Spacing
// ============================================================================

TEST_CASE("P02-W09 SmartSpacingActivatedEvent defaults", "[p02-w09][events]")
{
    markamp::core::events::SmartSpacingActivatedEvent evt;
    REQUIRE(evt.spacing_px == 0.0);
}

TEST_CASE("P02-W09 SmartSpacingDeactivatedEvent defaults", "[p02-w09][events]")
{
    markamp::core::events::SmartSpacingDeactivatedEvent evt;
    (void)evt;
}

TEST_CASE("P02-W09 SmartSpacingModel defaults", "[p02-w09][model]")
{
    markamp::canvas::SmartSpacingModel spacing_model;
    REQUIRE(spacing_model.guides().empty());
    REQUIRE(spacing_model.target_spacing() == 20.0);
    REQUIRE(spacing_model.tolerance() == 2.0);
    REQUIRE(spacing_model.state() == markamp::canvas::SpacingState::kInactive);
    REQUIRE_FALSE(spacing_model.is_active());
    REQUIRE(spacing_model.guide_count() == 0);
}

TEST_CASE("P02-W09 SmartSpacingModel lifecycle", "[p02-w09][model]")
{
    markamp::canvas::SmartSpacingModel spacing_model;
    spacing_model.activate();
    REQUIRE(spacing_model.is_active());
    REQUIRE(spacing_model.is_previewing());
    spacing_model.apply();
    REQUIRE(spacing_model.state() == markamp::canvas::SpacingState::kApplied);
    spacing_model.deactivate();
    REQUIRE_FALSE(spacing_model.is_active());
}

TEST_CASE("P02-W09 SmartSpacingModel tolerance", "[p02-w09][model]")
{
    markamp::canvas::SmartSpacingModel spacing_model;
    spacing_model.set_target_spacing(30.0);
    spacing_model.set_tolerance(5.0);
    REQUIRE(spacing_model.is_within_tolerance(28.0));
    REQUIRE(spacing_model.is_within_tolerance(35.0));
    REQUIRE_FALSE(spacing_model.is_within_tolerance(36.0));
}

TEST_CASE("P02-W09 activate_smart_spacing workbench", "[p02-w09][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.activate_smart_spacing();
    REQUIRE(wb.smart_spacing().is_active());
}

TEST_CASE("P02-W09 deactivate_smart_spacing workbench", "[p02-w09][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.activate_smart_spacing();
    wb.deactivate_smart_spacing();
    REQUIRE_FALSE(wb.smart_spacing().is_active());
}

// ============================================================================
// V17 Phase 02 W10: Grouping
// ============================================================================

TEST_CASE("P02-W10 ObjectsGroupedEvent defaults", "[p02-w10][events]")
{
    markamp::core::events::ObjectsGroupedEvent evt;
    REQUIRE(evt.group_id.empty());
    REQUIRE(evt.member_count == 0);
}

TEST_CASE("P02-W10 ObjectsUngroupedEvent defaults", "[p02-w10][events]")
{
    markamp::core::events::ObjectsUngroupedEvent evt;
    REQUIRE(evt.group_id.empty());
}

TEST_CASE("P02-W10 group_selected workbench", "[p02-w10][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.group_selected();
}

TEST_CASE("P02-W10 ungroup_selected workbench", "[p02-w10][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.ungroup_selected();
}

TEST_CASE("P02-W10 group then ungroup", "[p02-w10][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.select_object(1);
    wb.group_selected();
    wb.ungroup_selected();
}

TEST_CASE("P02-W10 event fields populated", "[p02-w10][events]")
{
    markamp::core::events::ObjectsGroupedEvent evt;
    evt.group_id = "grp_42";
    evt.member_count = 5;
    REQUIRE(evt.group_id == "grp_42");
    REQUIRE(evt.member_count == 5);
}

// ============================================================================
// V17 Phase 02 W11: Frame Membership
// ============================================================================

TEST_CASE("P02-W11 FrameMemberAddedEvent defaults", "[p02-w11][events]")
{
    markamp::core::events::FrameMemberAddedEvent evt;
    REQUIRE(evt.frame_id.empty());
    REQUIRE(evt.object_id.empty());
}

TEST_CASE("P02-W11 FrameMemberRemovedEvent defaults", "[p02-w11][events]")
{
    markamp::core::events::FrameMemberRemovedEvent evt;
    REQUIRE(evt.frame_id.empty());
    REQUIRE(evt.object_id.empty());
}

TEST_CASE("P02-W11 add_to_frame workbench", "[p02-w11][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_to_frame("frame_1", "obj_42");
}

TEST_CASE("P02-W11 remove_from_frame workbench", "[p02-w11][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.remove_from_frame("frame_1", "obj_42");
}

TEST_CASE("P02-W11 add then remove from frame", "[p02-w11][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_to_frame("frame_A", "obj_1");
    wb.add_to_frame("frame_A", "obj_2");
    wb.remove_from_frame("frame_A", "obj_1");
}

TEST_CASE("P02-W11 event fields populated", "[p02-w11][events]")
{
    markamp::core::events::FrameMemberAddedEvent evt;
    evt.frame_id = "frame_X";
    evt.object_id = "obj_99";
    REQUIRE(evt.frame_id == "frame_X");
    REQUIRE(evt.object_id == "obj_99");
}

// ============================================================================
// V17 Phase 02 W12: Layers & Z Order
// ============================================================================

TEST_CASE("P02-W12 LayerOrderChangedEvent defaults", "[p02-w12][events]")
{
    markamp::core::events::LayerOrderChangedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.action.empty());
}

TEST_CASE("P02-W12 LayerOrderResetEvent defaults", "[p02-w12][events]")
{
    markamp::core::events::LayerOrderResetEvent evt;
    (void)evt;
}

TEST_CASE("P02-W12 bring_to_front workbench", "[p02-w12][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.bring_to_front();
}

TEST_CASE("P02-W12 send_to_back workbench", "[p02-w12][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.send_to_back();
}

TEST_CASE("P02-W12 bring front then send back", "[p02-w12][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.bring_to_front();
    wb.send_to_back();
}

TEST_CASE("P02-W12 event fields populated", "[p02-w12][events]")
{
    markamp::core::events::LayerOrderChangedEvent evt;
    evt.object_id = "obj_5";
    evt.action = "bring_to_front";
    REQUIRE(evt.object_id == "obj_5");
    REQUIRE(evt.action == "bring_to_front");
}

// ============================================================================
// V17 Phase 02 W13: Locking & Pinning
// ============================================================================

TEST_CASE("P02-W13 ObjectLockedEvent defaults", "[p02-w13][events]")
{
    markamp::core::events::ObjectLockedEvent evt;
    REQUIRE(evt.object_id.empty());
}

TEST_CASE("P02-W13 ObjectUnlockedEvent defaults", "[p02-w13][events]")
{
    markamp::core::events::ObjectUnlockedEvent evt;
    REQUIRE(evt.object_id.empty());
}

TEST_CASE("P02-W13 lock_selected workbench", "[p02-w13][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.lock_selected();
}

TEST_CASE("P02-W13 unlock_selected workbench", "[p02-w13][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.unlock_selected();
}

TEST_CASE("P02-W13 lock then unlock", "[p02-w13][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.lock_selected();
    wb.unlock_selected();
}

TEST_CASE("P02-W13 event fields populated", "[p02-w13][events]")
{
    markamp::core::events::ObjectLockedEvent evt;
    evt.object_id = "obj_locked";
    REQUIRE(evt.object_id == "obj_locked");
}

// ============================================================================
// V17 Phase 02 W14: Precision Nudge
// ============================================================================

TEST_CASE("P02-W14 PrecisionNudgeEvent defaults", "[p02-w14][events]")
{
    markamp::core::events::PrecisionNudgeEvent evt;
    REQUIRE(evt.direction.empty());
    REQUIRE(evt.step_px == 0.0);
}

TEST_CASE("P02-W14 NudgeStepChangedEvent defaults", "[p02-w14][events]")
{
    markamp::core::events::NudgeStepChangedEvent evt;
    REQUIRE(evt.step_px == 0.0);
}

TEST_CASE("P02-W14 PrecisionNudgeModel defaults", "[p02-w14][model]")
{
    markamp::canvas::PrecisionNudgeModel nudge;
    REQUIRE(nudge.small_step() == 1.0);
    REQUIRE(nudge.big_step() == 10.0);
    REQUIRE_FALSE(nudge.has_custom_steps());
}

TEST_CASE("P02-W14 PrecisionNudgeModel custom steps", "[p02-w14][model]")
{
    markamp::canvas::PrecisionNudgeModel nudge;
    nudge.set_small_step(2.0);
    nudge.set_big_step(20.0);
    REQUIRE(nudge.small_step() == 2.0);
    REQUIRE(nudge.big_step() == 20.0);
    REQUIRE(nudge.has_custom_steps());
}

TEST_CASE("P02-W14 PrecisionNudgeModel direction deltas", "[p02-w14][model]")
{
    markamp::canvas::PrecisionNudgeModel nudge;
    REQUIRE(nudge.delta_x(markamp::canvas::NudgeDirection::kRight, false) == 1.0);
    REQUIRE(nudge.delta_x(markamp::canvas::NudgeDirection::kLeft, false) == -1.0);
    REQUIRE(nudge.delta_y(markamp::canvas::NudgeDirection::kDown, false) == 1.0);
    REQUIRE(nudge.delta_y(markamp::canvas::NudgeDirection::kUp, false) == -1.0);
    REQUIRE(nudge.delta_x(markamp::canvas::NudgeDirection::kRight, true) == 10.0);
}

TEST_CASE("P02-W14 precision_nudge workbench", "[p02-w14][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.precision_nudge(markamp::canvas::NudgeDirection::kRight, false);
}

TEST_CASE("P02-W14 direction_label static", "[p02-w14][model]")
{
    REQUIRE(markamp::canvas::PrecisionNudgeModel::direction_label(markamp::canvas::NudgeDirection::kUp) == "up");
    REQUIRE(markamp::canvas::PrecisionNudgeModel::direction_label(markamp::canvas::NudgeDirection::kDown) == "down");
    REQUIRE(markamp::canvas::PrecisionNudgeModel::direction_label(markamp::canvas::NudgeDirection::kLeft) == "left");
    REQUIRE(markamp::canvas::PrecisionNudgeModel::direction_label(markamp::canvas::NudgeDirection::kRight) == "right");
}

// ============================================================================
// V17 Phase 02 W15: Duplicate & Paste In Place
// ============================================================================

TEST_CASE("P02-W15 DuplicateInPlaceEvent defaults", "[p02-w15][events]")
{
    markamp::core::events::DuplicateInPlaceEvent evt;
    REQUIRE(evt.object_count == 0);
}

TEST_CASE("P02-W15 PasteInPlaceEvent defaults", "[p02-w15][events]")
{
    markamp::core::events::PasteInPlaceEvent evt;
    REQUIRE(evt.object_count == 0);
}

TEST_CASE("P02-W15 duplicate_in_place workbench", "[p02-w15][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.duplicate_in_place();
}

TEST_CASE("P02-W15 paste_in_place workbench", "[p02-w15][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.paste_in_place();
}

TEST_CASE("P02-W15 duplicate then paste", "[p02-w15][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.duplicate_in_place();
    wb.paste_in_place();
}

TEST_CASE("P02-W15 event fields populated", "[p02-w15][events]")
{
    markamp::core::events::DuplicateInPlaceEvent evt;
    evt.object_count = 3;
    REQUIRE(evt.object_count == 3);
}

// ============================================================================
// V17 Phase 02 W16: Drag Autoscroll
// ============================================================================

TEST_CASE("P02-W16 AutoscrollStartedEvent defaults", "[p02-w16][events]")
{
    markamp::core::events::AutoscrollStartedEvent evt;
    REQUIRE(evt.direction.empty());
}

TEST_CASE("P02-W16 AutoscrollStoppedEvent defaults", "[p02-w16][events]")
{
    markamp::core::events::AutoscrollStoppedEvent evt;
    (void)evt;
}

TEST_CASE("P02-W16 DragAutoscrollModel defaults", "[p02-w16][model]")
{
    markamp::canvas::DragAutoscrollModel autoscroll;
    REQUIRE(autoscroll.edge_margin() == 40.0);
    REQUIRE(autoscroll.speed() == 8.0);
    REQUIRE(autoscroll.direction() == markamp::canvas::AutoscrollDirection::kNone);
    REQUIRE_FALSE(autoscroll.is_active());
}

TEST_CASE("P02-W16 DragAutoscrollModel lifecycle", "[p02-w16][model]")
{
    markamp::canvas::DragAutoscrollModel autoscroll;
    autoscroll.start(markamp::canvas::AutoscrollDirection::kRight);
    REQUIRE(autoscroll.is_active());
    REQUIRE(autoscroll.direction() == markamp::canvas::AutoscrollDirection::kRight);
    autoscroll.stop();
    REQUIRE_FALSE(autoscroll.is_active());
}

TEST_CASE("P02-W16 start_autoscroll workbench", "[p02-w16][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.start_autoscroll(markamp::canvas::AutoscrollDirection::kDown);
    REQUIRE(wb.autoscroll_model().is_active());
}

TEST_CASE("P02-W16 stop_autoscroll workbench", "[p02-w16][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.start_autoscroll(markamp::canvas::AutoscrollDirection::kLeft);
    wb.stop_autoscroll();
    REQUIRE_FALSE(wb.autoscroll_model().is_active());
}

// ============================================================================
// V17 Phase 02 W17: Viewport-Aware Transforms
// ============================================================================

TEST_CASE("P02-W17 ViewportTransformSyncEvent defaults", "[p02-w17][events]")
{
    markamp::core::events::ViewportTransformSyncEvent evt;
    REQUIRE(evt.zoom == 1.0);
    REQUIRE(evt.pan_x == 0.0);
    REQUIRE(evt.pan_y == 0.0);
}

TEST_CASE("P02-W17 ViewportClampedEvent defaults", "[p02-w17][events]")
{
    markamp::core::events::ViewportClampedEvent evt;
    (void)evt;
}

TEST_CASE("P02-W17 sync_viewport_transform workbench", "[p02-w17][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.sync_viewport_transform();
}

TEST_CASE("P02-W17 clamp_viewport workbench", "[p02-w17][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.clamp_viewport();
}

TEST_CASE("P02-W17 sync then clamp", "[p02-w17][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.sync_viewport_transform();
    wb.clamp_viewport();
}

TEST_CASE("P02-W17 event fields populated", "[p02-w17][events]")
{
    markamp::core::events::ViewportTransformSyncEvent evt;
    evt.zoom = 2.5;
    evt.pan_x = 100.0;
    evt.pan_y = -50.0;
    REQUIRE(evt.zoom == 2.5);
    REQUIRE(evt.pan_x == 100.0);
    REQUIRE(evt.pan_y == -50.0);
}

// ============================================================================
// V17 Phase 02 W18: Multi User Selection
// ============================================================================

TEST_CASE("P02-W18 RemoteSelectionReceivedEvent defaults", "[p02-w18][events]")
{
    markamp::core::events::RemoteSelectionReceivedEvent evt;
    REQUIRE(evt.user_id.empty());
    REQUIRE(evt.object_count == 0);
}

TEST_CASE("P02-W18 RemoteLockConflictEvent defaults", "[p02-w18][events]")
{
    markamp::core::events::RemoteLockConflictEvent evt;
    REQUIRE(evt.user_id.empty());
    REQUIRE(evt.object_id.empty());
}

TEST_CASE("P02-W18 receive_remote_selection workbench", "[p02-w18][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.receive_remote_selection("user_A", 3);
}

TEST_CASE("P02-W18 report_lock_conflict workbench", "[p02-w18][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.report_lock_conflict("user_B", "obj_42");
}

TEST_CASE("P02-W18 event fields populated", "[p02-w18][events]")
{
    markamp::core::events::RemoteLockConflictEvent evt;
    evt.user_id = "user_X";
    evt.object_id = "obj_99";
    REQUIRE(evt.user_id == "user_X");
    REQUIRE(evt.object_id == "obj_99");
}

TEST_CASE("P02-W18 multiple remote selections", "[p02-w18][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.receive_remote_selection("user_A", 2);
    wb.receive_remote_selection("user_B", 5);
}

// ============================================================================
// V17 Phase 02 W19: Transform History Quality
// ============================================================================

TEST_CASE("P02-W19 TransformUndoEvent defaults", "[p02-w19][events]")
{
    markamp::core::events::TransformUndoEvent evt;
    REQUIRE(evt.action.empty());
}

TEST_CASE("P02-W19 TransformRedoEvent defaults", "[p02-w19][events]")
{
    markamp::core::events::TransformRedoEvent evt;
    REQUIRE(evt.action.empty());
}

TEST_CASE("P02-W19 undo_transform workbench", "[p02-w19][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.undo_transform();
}

TEST_CASE("P02-W19 redo_transform workbench", "[p02-w19][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.redo_transform();
}

TEST_CASE("P02-W19 undo then redo", "[p02-w19][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.undo_transform();
    wb.redo_transform();
}

TEST_CASE("P02-W19 event fields populated", "[p02-w19][events]")
{
    markamp::core::events::TransformUndoEvent evt;
    evt.action = "move";
    REQUIRE(evt.action == "move");
}

// ============================================================================
// V17 Phase 02 W20: Selection Action Surfaces
// ============================================================================

TEST_CASE("P02-W20 ActionSurfaceShownEvent defaults", "[p02-w20][events]")
{
    markamp::core::events::ActionSurfaceShownEvent evt;
    REQUIRE(evt.surface_type.empty());
}

TEST_CASE("P02-W20 ActionSurfaceHiddenEvent defaults", "[p02-w20][events]")
{
    markamp::core::events::ActionSurfaceHiddenEvent evt;
    (void)evt;
}

TEST_CASE("P02-W20 show_action_surface workbench", "[p02-w20][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.show_action_surface("toolbar");
}

TEST_CASE("P02-W20 hide_action_surface workbench", "[p02-w20][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.hide_action_surface();
}

TEST_CASE("P02-W20 show then hide surface", "[p02-w20][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.show_action_surface("contextmenu");
    wb.hide_action_surface();
}

TEST_CASE("P02-W20 event fields populated", "[p02-w20][events]")
{
    markamp::core::events::ActionSurfaceShownEvent evt;
    evt.surface_type = "quickaction";
    REQUIRE(evt.surface_type == "quickaction");
}

// ============================================================================
// V17 Phase 03 W01: Freehand Pen Engine
// ============================================================================

TEST_CASE("P03-W01 StrokeCapturedEvent defaults", "[p03-w01][events]")
{
    markamp::core::events::StrokeCapturedEvent evt;
    REQUIRE(evt.stroke_id.empty());
    REQUIRE(evt.point_count == 0);
}

TEST_CASE("P03-W01 StrokeSmoothedEvent defaults", "[p03-w01][events]")
{
    markamp::core::events::StrokeSmoothedEvent evt;
    REQUIRE(evt.stroke_id.empty());
}

TEST_CASE("P03-W01 PenEngineModel defaults", "[p03-w01][model]")
{
    markamp::canvas::PenEngineModel pen;
    REQUIRE(pen.smoothing() == markamp::canvas::SmoothingAlgorithm::kCatmullRom);
    REQUIRE(pen.smoothing_factor() == 0.5);
    REQUIRE(pen.pressure_sensitivity() == 1.0);
    REQUIRE(pen.pressure_enabled());
    REQUIRE_FALSE(pen.replay_active());
    REQUIRE(pen.stroke_count() == 0);
}

TEST_CASE("P03-W01 PenEngineModel configuration", "[p03-w01][model]")
{
    markamp::canvas::PenEngineModel pen;
    pen.set_smoothing(markamp::canvas::SmoothingAlgorithm::kBezier);
    pen.set_smoothing_factor(0.8);
    pen.set_pressure_sensitivity(0.5);
    pen.set_pressure_enabled(false);
    REQUIRE(pen.smoothing() == markamp::canvas::SmoothingAlgorithm::kBezier);
    REQUIRE(pen.smoothing_factor() == 0.8);
    REQUIRE(pen.pressure_sensitivity() == 0.5);
    REQUIRE_FALSE(pen.pressure_enabled());
}

TEST_CASE("P03-W01 capture_stroke workbench", "[p03-w01][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.capture_stroke(42);
    REQUIRE(wb.pen_engine().stroke_count() == 1);
}

TEST_CASE("P03-W01 smooth_stroke workbench", "[p03-w01][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.smooth_stroke("stroke_1");
}

// ============================================================================
// V17 Phase 03 W02: Pen Presets
// ============================================================================

TEST_CASE("P03-W02 PenPresetAppliedEvent defaults", "[p03-w02][events]")
{
    markamp::core::events::PenPresetAppliedEvent evt;
    REQUIRE(evt.preset_name.empty());
}

TEST_CASE("P03-W02 PenPresetSavedEvent defaults", "[p03-w02][events]")
{
    markamp::core::events::PenPresetSavedEvent evt;
    REQUIRE(evt.preset_name.empty());
}

TEST_CASE("P03-W02 PenPresetModel defaults", "[p03-w02][model]")
{
    markamp::canvas::PenPresetModel presets;
    REQUIRE(presets.preset_count() == 0);
    REQUIRE(presets.recent_count() == 0);
    REQUIRE(presets.max_recent() == 10);
}

TEST_CASE("P03-W02 PenPresetModel add and track", "[p03-w02][model]")
{
    markamp::canvas::PenPresetModel presets;
    markamp::canvas::PenPreset fine{"Fine", 1.0, 1.0, "#000000"};
    markamp::canvas::PenPreset bold{"Bold", 4.0, 1.0, "#FF0000"};
    presets.add_preset(fine);
    presets.add_preset(bold);
    REQUIRE(presets.preset_count() == 2);
    REQUIRE(presets.preset_at(0).name == "Fine");
    REQUIRE(presets.preset_at(1).name == "Bold");
}

TEST_CASE("P03-W02 apply_pen_preset workbench", "[p03-w02][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.apply_pen_preset("Fine");
    REQUIRE(wb.pen_presets().active_preset_name() == "Fine");
    REQUIRE(wb.pen_presets().recent_count() == 1);
}

TEST_CASE("P03-W02 save_pen_preset workbench", "[p03-w02][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.save_pen_preset("Custom");
}

// ============================================================================
// V17 Phase 03 W03: Eraser & Lasso
// ============================================================================

TEST_CASE("P03-W03 EraserStrokeEvent defaults", "[p03-w03][events]")
{
    markamp::core::events::EraserStrokeEvent evt;
    REQUIRE(evt.erased_count == 0);
}

TEST_CASE("P03-W03 LassoSelectionEvent defaults", "[p03-w03][events]")
{
    markamp::core::events::LassoSelectionEvent evt;
    REQUIRE(evt.object_count == 0);
}

TEST_CASE("P03-W03 erase_stroke workbench", "[p03-w03][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.erase_stroke(3);
}

TEST_CASE("P03-W03 lasso_select workbench", "[p03-w03][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.lasso_select(5);
}

TEST_CASE("P03-W03 erase then lasso", "[p03-w03][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.erase_stroke(2);
    wb.lasso_select(4);
}

TEST_CASE("P03-W03 event fields populated", "[p03-w03][events]")
{
    markamp::core::events::EraserStrokeEvent evt;
    evt.erased_count = 7;
    REQUIRE(evt.erased_count == 7);
}

// ============================================================================
// V17 Phase 03 W04: Shape Tool
// ============================================================================

TEST_CASE("P03-W04 ShapeCreatedEvent defaults", "[p03-w04][events]")
{
    markamp::core::events::ShapeCreatedEvent evt;
    REQUIRE(evt.shape_type.empty());
}

TEST_CASE("P03-W04 ShapeResizedEvent defaults", "[p03-w04][events]")
{
    markamp::core::events::ShapeResizedEvent evt;
    REQUIRE(evt.shape_id.empty());
}

TEST_CASE("P03-W04 create_shape workbench", "[p03-w04][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_shape("rectangle");
}

TEST_CASE("P03-W04 resize_shape workbench", "[p03-w04][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.resize_shape("shape_1");
}

TEST_CASE("P03-W04 create then resize", "[p03-w04][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_shape("ellipse");
    wb.resize_shape("shape_2");
}

TEST_CASE("P03-W04 event fields populated", "[p03-w04][events]")
{
    markamp::core::events::ShapeCreatedEvent evt;
    evt.shape_type = "diamond";
    REQUIRE(evt.shape_type == "diamond");
}

// ============================================================================
// V17 Phase 03 W05: Text Box Authoring
// ============================================================================

TEST_CASE("P03-W05 TextBoxCreatedEvent defaults", "[p03-w05][events]")
{
    markamp::core::events::TextBoxCreatedEvent evt;
    REQUIRE(evt.text_box_id.empty());
}

TEST_CASE("P03-W05 TextBoxEditedEvent defaults", "[p03-w05][events]")
{
    markamp::core::events::TextBoxEditedEvent evt;
    REQUIRE(evt.text_box_id.empty());
}

TEST_CASE("P03-W05 create_text_box workbench", "[p03-w05][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_text_box();
}

TEST_CASE("P03-W05 edit_text_box workbench", "[p03-w05][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.edit_text_box("textbox_1");
}

TEST_CASE("P03-W05 create then edit", "[p03-w05][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_text_box();
    wb.edit_text_box("textbox_1");
}

TEST_CASE("P03-W05 event fields populated", "[p03-w05][events]")
{
    markamp::core::events::TextBoxCreatedEvent evt;
    evt.text_box_id = "tb_42";
    REQUIRE(evt.text_box_id == "tb_42");
}

// ============================================================================
// V17 Phase 03 W06: Sticky Notes
// ============================================================================

TEST_CASE("P03-W06 StickyNoteCreatedEvent defaults", "[p03-w06][events]")
{
    markamp::core::events::StickyNoteCreatedEvent evt;
    REQUIRE(evt.note_id.empty());
    REQUIRE(evt.color.empty());
}

TEST_CASE("P03-W06 StickyNoteColorChangedEvent defaults", "[p03-w06][events]")
{
    markamp::core::events::StickyNoteColorChangedEvent evt;
    REQUIRE(evt.note_id.empty());
    REQUIRE(evt.color.empty());
}

TEST_CASE("P03-W06 create_sticky_note workbench", "[p03-w06][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_sticky_note("#FFFF00");
}

TEST_CASE("P03-W06 change_sticky_note_color workbench", "[p03-w06][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.change_sticky_note_color("note_1", "#FF6600");
}

TEST_CASE("P03-W06 create then recolor", "[p03-w06][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_sticky_note("#FFFF00");
    wb.change_sticky_note_color("note_1", "#00FF00");
}

TEST_CASE("P03-W06 event fields populated", "[p03-w06][events]")
{
    markamp::core::events::StickyNoteCreatedEvent evt;
    evt.note_id = "note_5";
    evt.color = "#FF0000";
    REQUIRE(evt.note_id == "note_5");
    REQUIRE(evt.color == "#FF0000");
}

// ============================================================================
// V17 Phase 03 W07: Color Authoring
// ============================================================================

TEST_CASE("P03-W07 PaletteColorSelectedEvent defaults", "[p03-w07][events]")
{
    markamp::core::events::PaletteColorSelectedEvent evt;
    REQUIRE(evt.color.empty());
}

TEST_CASE("P03-W07 PaletteColorSavedEvent defaults", "[p03-w07][events]")
{
    markamp::core::events::PaletteColorSavedEvent evt;
    REQUIRE(evt.color.empty());
}

TEST_CASE("P03-W07 select_palette_color workbench", "[p03-w07][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.select_palette_color("#3366CC");
}

TEST_CASE("P03-W07 save_palette_color workbench", "[p03-w07][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.save_palette_color("#FF9900");
}

TEST_CASE("P03-W07 select then save", "[p03-w07][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.select_palette_color("#AA00FF");
    wb.save_palette_color("#AA00FF");
}

TEST_CASE("P03-W07 event fields populated", "[p03-w07][events]")
{
    markamp::core::events::PaletteColorSelectedEvent evt;
    evt.color = "#112233";
    REQUIRE(evt.color == "#112233");
}

// ============================================================================
// V17 Phase 03 W08: Typography Controls
// ============================================================================

TEST_CASE("P03-W08 TypographyChangedEvent defaults", "[p03-w08][events]")
{
    markamp::core::events::TypographyChangedEvent evt;
    REQUIRE(evt.property.empty());
    REQUIRE(evt.value.empty());
}

TEST_CASE("P03-W08 FontAppliedEvent defaults", "[p03-w08][events]")
{
    markamp::core::events::FontAppliedEvent evt;
    REQUIRE(evt.font_name.empty());
}

TEST_CASE("P03-W08 change_typography workbench", "[p03-w08][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.change_typography("font_size", "16px");
}

TEST_CASE("P03-W08 apply_font workbench", "[p03-w08][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.apply_font("Inter");
}

TEST_CASE("P03-W08 change then apply", "[p03-w08][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.change_typography("line_height", "1.5");
    wb.apply_font("Roboto");
}

TEST_CASE("P03-W08 event fields populated", "[p03-w08][events]")
{
    markamp::core::events::TypographyChangedEvent evt;
    evt.property = "font_weight";
    evt.value = "bold";
    REQUIRE(evt.property == "font_weight");
    REQUIRE(evt.value == "bold");
}

// ============================================================================
// V17 Phase 03 W09: Fill, Stroke & Effects
// ============================================================================

TEST_CASE("P03-W09 FillStyleChangedEvent defaults", "[p03-w09][events]")
{
    markamp::core::events::FillStyleChangedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.fill.empty());
}

TEST_CASE("P03-W09 StrokeStyleChangedEvent defaults", "[p03-w09][events]")
{
    markamp::core::events::StrokeStyleChangedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.stroke.empty());
}

TEST_CASE("P03-W09 change_fill_style workbench", "[p03-w09][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.change_fill_style("shape_1", "solid:#FF0000");
}

TEST_CASE("P03-W09 change_stroke_style workbench", "[p03-w09][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.change_stroke_style("shape_1", "dashed:2px");
}

TEST_CASE("P03-W09 fill then stroke", "[p03-w09][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.change_fill_style("shape_2", "gradient");
    wb.change_stroke_style("shape_2", "solid:3px");
}

TEST_CASE("P03-W09 event fields populated", "[p03-w09][events]")
{
    markamp::core::events::FillStyleChangedEvent evt;
    evt.object_id = "obj_A";
    evt.fill = "transparent";
    REQUIRE(evt.object_id == "obj_A");
    REQUIRE(evt.fill == "transparent");
}

// ============================================================================
// V17 Phase 03 W10: Style Presets
// ============================================================================

TEST_CASE("P03-W10 StylePresetAppliedEvent defaults", "[p03-w10][events]")
{
    markamp::core::events::StylePresetAppliedEvent evt;
    REQUIRE(evt.preset_name.empty());
}

TEST_CASE("P03-W10 StylePresetSavedEvent defaults", "[p03-w10][events]")
{
    markamp::core::events::StylePresetSavedEvent evt;
    REQUIRE(evt.preset_name.empty());
}

TEST_CASE("P03-W10 apply_style_preset workbench", "[p03-w10][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.apply_style_preset("Blueprint");
}

TEST_CASE("P03-W10 save_style_preset workbench", "[p03-w10][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.save_style_preset("MyCustom");
}

TEST_CASE("P03-W10 save then apply", "[p03-w10][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.save_style_preset("Corporate");
    wb.apply_style_preset("Corporate");
}

TEST_CASE("P03-W10 event fields populated", "[p03-w10][events]")
{
    markamp::core::events::StylePresetAppliedEvent evt;
    evt.preset_name = "Minimal";
    REQUIRE(evt.preset_name == "Minimal");
}

// ============================================================================
// V17 Phase 03 W11: Inline Rich Text
// ============================================================================

TEST_CASE("P03-W11 InlineRichTextEnabledEvent defaults", "[p03-w11][events]")
{
    markamp::core::events::InlineRichTextEnabledEvent evt;
    REQUIRE(evt.object_id.empty());
}

TEST_CASE("P03-W11 InlineRichTextFormattedEvent defaults", "[p03-w11][events]")
{
    markamp::core::events::InlineRichTextFormattedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.format.empty());
}

TEST_CASE("P03-W11 enable_inline_rich_text workbench", "[p03-w11][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.enable_inline_rich_text("textbox_1");
}

TEST_CASE("P03-W11 format_inline_rich_text workbench", "[p03-w11][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.format_inline_rich_text("textbox_1", "bold");
}

TEST_CASE("P03-W11 enable then format", "[p03-w11][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.enable_inline_rich_text("textbox_2");
    wb.format_inline_rich_text("textbox_2", "italic");
}

TEST_CASE("P03-W11 event fields populated", "[p03-w11][events]")
{
    markamp::core::events::InlineRichTextFormattedEvent evt;
    evt.object_id = "obj_1";
    evt.format = "link";
    REQUIRE(evt.object_id == "obj_1");
    REQUIRE(evt.format == "link");
}

// ============================================================================
// V17 Phase 03 W12: Shape Library Quick Insert
// ============================================================================

TEST_CASE("P03-W12 ShapeLibraryInsertEvent defaults", "[p03-w12][events]")
{
    markamp::core::events::ShapeLibraryInsertEvent evt;
    REQUIRE(evt.shape_name.empty());
}

TEST_CASE("P03-W12 ShapeLibraryOpenedEvent defaults", "[p03-w12][events]")
{
    markamp::core::events::ShapeLibraryOpenedEvent evt;
    (void)evt; // no fields
}

TEST_CASE("P03-W12 insert_from_shape_library workbench", "[p03-w12][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.insert_from_shape_library("star");
}

TEST_CASE("P03-W12 open_shape_library workbench", "[p03-w12][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.open_shape_library();
}

TEST_CASE("P03-W12 open then insert", "[p03-w12][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.open_shape_library();
    wb.insert_from_shape_library("hexagon");
}

TEST_CASE("P03-W12 event fields populated", "[p03-w12][events]")
{
    markamp::core::events::ShapeLibraryInsertEvent evt;
    evt.shape_name = "pentagon";
    REQUIRE(evt.shape_name == "pentagon");
}

// ============================================================================
// V17 Phase 03 W13: Shape Recognition
// ============================================================================

TEST_CASE("P03-W13 ShapeRecognizedEvent defaults", "[p03-w13][events]")
{
    markamp::core::events::ShapeRecognizedEvent evt;
    REQUIRE(evt.recognized_type.empty());
    REQUIRE(evt.confidence == 0.0);
}

TEST_CASE("P03-W13 ShapeRecognitionRejectedEvent defaults", "[p03-w13][events]")
{
    markamp::core::events::ShapeRecognitionRejectedEvent evt;
    REQUIRE(evt.stroke_id.empty());
}

TEST_CASE("P03-W13 recognize_shape workbench", "[p03-w13][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.recognize_shape("circle", 0.92);
}

TEST_CASE("P03-W13 reject_shape_recognition workbench", "[p03-w13][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.reject_shape_recognition("stroke_5");
}

TEST_CASE("P03-W13 recognize then reject", "[p03-w13][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.recognize_shape("triangle", 0.45);
    wb.reject_shape_recognition("stroke_6");
}

TEST_CASE("P03-W13 event fields populated", "[p03-w13][events]")
{
    markamp::core::events::ShapeRecognizedEvent evt;
    evt.recognized_type = "rectangle";
    evt.confidence = 0.87;
    REQUIRE(evt.recognized_type == "rectangle");
    REQUIRE(evt.confidence == 0.87);
}

// ============================================================================
// V17 Phase 03 W14: Image Annotation
// ============================================================================

TEST_CASE("P03-W14 ImageAnnotationAddedEvent defaults", "[p03-w14][events]")
{
    markamp::core::events::ImageAnnotationAddedEvent evt;
    REQUIRE(evt.image_id.empty());
    REQUIRE(evt.annotation_type.empty());
}

TEST_CASE("P03-W14 ImageAnnotationRemovedEvent defaults", "[p03-w14][events]")
{
    markamp::core::events::ImageAnnotationRemovedEvent evt;
    REQUIRE(evt.image_id.empty());
    REQUIRE(evt.annotation_id.empty());
}

TEST_CASE("P03-W14 add_image_annotation workbench", "[p03-w14][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_image_annotation("img_1", "arrow");
}

TEST_CASE("P03-W14 remove_image_annotation workbench", "[p03-w14][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.remove_image_annotation("img_1", "annot_1");
}

TEST_CASE("P03-W14 add then remove", "[p03-w14][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_image_annotation("img_2", "highlight");
    wb.remove_image_annotation("img_2", "annot_2");
}

TEST_CASE("P03-W14 event fields populated", "[p03-w14][events]")
{
    markamp::core::events::ImageAnnotationAddedEvent evt;
    evt.image_id = "img_X";
    evt.annotation_type = "text";
    REQUIRE(evt.image_id == "img_X");
    REQUIRE(evt.annotation_type == "text");
}

// ============================================================================
// V17 Phase 03 W15: Comments & Callouts
// ============================================================================

TEST_CASE("P03-W15 CommentAddedEvent defaults", "[p03-w15][events]")
{
    markamp::core::events::CommentAddedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.comment_text.empty());
}

TEST_CASE("P03-W15 CalloutCreatedEvent defaults", "[p03-w15][events]")
{
    markamp::core::events::CalloutCreatedEvent evt;
    REQUIRE(evt.callout_type.empty());
}

TEST_CASE("P03-W15 add_comment workbench", "[p03-w15][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_comment("shape_1", "Needs revision");
}

TEST_CASE("P03-W15 create_callout workbench", "[p03-w15][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_callout("warning");
}

TEST_CASE("P03-W15 comment then callout", "[p03-w15][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_comment("obj_3", "Please review");
    wb.create_callout("critical");
}

TEST_CASE("P03-W15 event fields populated", "[p03-w15][events]")
{
    markamp::core::events::CommentAddedEvent evt;
    evt.object_id = "obj_Z";
    evt.comment_text = "Great work!";
    REQUIRE(evt.object_id == "obj_Z");
    REQUIRE(evt.comment_text == "Great work!");
}

// ============================================================================
// V17 Phase 03 W16: Tables For Authoring
// ============================================================================

TEST_CASE("P03-W16 CanvasTableCreatedEvent defaults", "[p03-w16][events]")
{
    markamp::core::events::CanvasTableCreatedEvent evt;
    REQUIRE(evt.rows == 0);
    REQUIRE(evt.columns == 0);
}

TEST_CASE("P03-W16 CanvasTableCellEditedEvent defaults", "[p03-w16][events]")
{
    markamp::core::events::CanvasTableCellEditedEvent evt;
    REQUIRE(evt.table_id.empty());
    REQUIRE(evt.row == 0);
    REQUIRE(evt.column == 0);
}

TEST_CASE("P03-W16 create_canvas_table workbench", "[p03-w16][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_canvas_table(3, 4);
}

TEST_CASE("P03-W16 edit_canvas_table_cell workbench", "[p03-w16][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.edit_canvas_table_cell("tbl_1", 1, 2);
}

TEST_CASE("P03-W16 create then edit", "[p03-w16][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_canvas_table(5, 3);
    wb.edit_canvas_table_cell("tbl_1", 0, 0);
}

TEST_CASE("P03-W16 event fields populated", "[p03-w16][events]")
{
    markamp::core::events::CanvasTableCreatedEvent evt;
    evt.rows = 10;
    evt.columns = 5;
    REQUIRE(evt.rows == 10);
    REQUIRE(evt.columns == 5);
}

// ============================================================================
// V17 Phase 03 W17: Icons, Stickers & Emoji
// ============================================================================

TEST_CASE("P03-W17 IconInsertedEvent defaults", "[p03-w17][events]")
{
    markamp::core::events::IconInsertedEvent evt;
    REQUIRE(evt.icon_name.empty());
}

TEST_CASE("P03-W17 StickerInsertedEvent defaults", "[p03-w17][events]")
{
    markamp::core::events::StickerInsertedEvent evt;
    REQUIRE(evt.sticker_id.empty());
}

TEST_CASE("P03-W17 insert_icon workbench", "[p03-w17][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.insert_icon("star");
}

TEST_CASE("P03-W17 insert_sticker workbench", "[p03-w17][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.insert_sticker("rocket_001");
}

TEST_CASE("P03-W17 icon then sticker", "[p03-w17][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.insert_icon("check");
    wb.insert_sticker("thumbsup_002");
}

TEST_CASE("P03-W17 event fields populated", "[p03-w17][events]")
{
    markamp::core::events::IconInsertedEvent evt;
    evt.icon_name = "arrow-right";
    REQUIRE(evt.icon_name == "arrow-right");
}

// ============================================================================
// V17 Phase 03 W18: Creator Shortcuts
// ============================================================================

TEST_CASE("P03-W18 CreatorShortcutInvokedEvent defaults", "[p03-w18][events]")
{
    markamp::core::events::CreatorShortcutInvokedEvent evt;
    REQUIRE(evt.shortcut_id.empty());
}

TEST_CASE("P03-W18 CreatorShortcutRegisteredEvent defaults", "[p03-w18][events]")
{
    markamp::core::events::CreatorShortcutRegisteredEvent evt;
    REQUIRE(evt.shortcut_id.empty());
    REQUIRE(evt.action.empty());
}

TEST_CASE("P03-W18 invoke_creator_shortcut workbench", "[p03-w18][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.invoke_creator_shortcut("quick_shape");
}

TEST_CASE("P03-W18 register_creator_shortcut workbench", "[p03-w18][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.register_creator_shortcut("ctrl_d", "duplicate");
}

TEST_CASE("P03-W18 register then invoke", "[p03-w18][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.register_creator_shortcut("ctrl_g", "group");
    wb.invoke_creator_shortcut("ctrl_g");
}

TEST_CASE("P03-W18 event fields populated", "[p03-w18][events]")
{
    markamp::core::events::CreatorShortcutRegisteredEvent evt;
    evt.shortcut_id = "alt_s";
    evt.action = "save_board";
    REQUIRE(evt.shortcut_id == "alt_s");
    REQUIRE(evt.action == "save_board");
}

// ============================================================================
// V17 Phase 03 W19: Bulk Style Editing
// ============================================================================

TEST_CASE("P03-W19 BulkStyleAppliedEvent defaults", "[p03-w19][events]")
{
    markamp::core::events::BulkStyleAppliedEvent evt;
    REQUIRE(evt.object_count == 0);
    REQUIRE(evt.style_property.empty());
}

TEST_CASE("P03-W19 BulkStyleResetEvent defaults", "[p03-w19][events]")
{
    markamp::core::events::BulkStyleResetEvent evt;
    REQUIRE(evt.object_count == 0);
}

TEST_CASE("P03-W19 apply_bulk_style workbench", "[p03-w19][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.apply_bulk_style(5, "fill_color");
}

TEST_CASE("P03-W19 reset_bulk_style workbench", "[p03-w19][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.reset_bulk_style(8);
}

TEST_CASE("P03-W19 apply then reset", "[p03-w19][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.apply_bulk_style(12, "stroke_width");
    wb.reset_bulk_style(12);
}

TEST_CASE("P03-W19 event fields populated", "[p03-w19][events]")
{
    markamp::core::events::BulkStyleAppliedEvent evt;
    evt.object_count = 7;
    evt.style_property = "opacity";
    REQUIRE(evt.object_count == 7);
    REQUIRE(evt.style_property == "opacity");
}

// ============================================================================
// V17 Phase 03 W20: Authoring Motion Feedback
// ============================================================================

TEST_CASE("P03-W20 MotionPreviewStartedEvent defaults", "[p03-w20][events]")
{
    markamp::core::events::MotionPreviewStartedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.motion_type.empty());
}

TEST_CASE("P03-W20 MotionPreviewEndedEvent defaults", "[p03-w20][events]")
{
    markamp::core::events::MotionPreviewEndedEvent evt;
    REQUIRE(evt.object_id.empty());
}

TEST_CASE("P03-W20 start_motion_preview workbench", "[p03-w20][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.start_motion_preview("shape_1", "fade_in");
}

TEST_CASE("P03-W20 end_motion_preview workbench", "[p03-w20][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.end_motion_preview("shape_1");
}

TEST_CASE("P03-W20 start then end", "[p03-w20][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.start_motion_preview("obj_A", "bounce");
    wb.end_motion_preview("obj_A");
}

TEST_CASE("P03-W20 event fields populated", "[p03-w20][events]")
{
    markamp::core::events::MotionPreviewStartedEvent evt;
    evt.object_id = "widget_3";
    evt.motion_type = "slide_left";
    REQUIRE(evt.object_id == "widget_3");
    REQUIRE(evt.motion_type == "slide_left");
}

// ============================================================================
// V17 Phase 04 W01: Connector Creation
// ============================================================================

TEST_CASE("P04-W01 ConnectorCreatedEvent defaults", "[p04-w01][events]")
{
    markamp::core::events::ConnectorCreatedEvent evt;
    REQUIRE(evt.from_id.empty());
    REQUIRE(evt.to_id.empty());
    REQUIRE(evt.connector_type.empty());
}

TEST_CASE("P04-W01 ConnectorDeletedEvent defaults", "[p04-w01][events]")
{
    markamp::core::events::ConnectorDeletedEvent evt;
    REQUIRE(evt.connector_id.empty());
}

TEST_CASE("P04-W01 create_connector workbench", "[p04-w01][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_connector("nodeA", "nodeB", "straight");
}

TEST_CASE("P04-W01 delete_connector workbench", "[p04-w01][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.delete_connector("conn_1");
}

TEST_CASE("P04-W01 create then delete", "[p04-w01][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_connector("n1", "n2", "curved");
    wb.delete_connector("conn_1");
}

TEST_CASE("P04-W01 event fields populated", "[p04-w01][events]")
{
    markamp::core::events::ConnectorCreatedEvent evt;
    evt.from_id = "box_A";
    evt.to_id = "box_B";
    evt.connector_type = "orthogonal";
    REQUIRE(evt.from_id == "box_A");
    REQUIRE(evt.to_id == "box_B");
    REQUIRE(evt.connector_type == "orthogonal");
}

// ============================================================================
// V17 Phase 04 W02: Connector Routing
// ============================================================================

TEST_CASE("P04-W02 ConnectorRouteChangedEvent defaults", "[p04-w02][events]")
{
    markamp::core::events::ConnectorRouteChangedEvent evt;
    REQUIRE(evt.connector_id.empty());
    REQUIRE(evt.route_type.empty());
}

TEST_CASE("P04-W02 ConnectorWaypointAddedEvent defaults", "[p04-w02][events]")
{
    markamp::core::events::ConnectorWaypointAddedEvent evt;
    REQUIRE(evt.connector_id.empty());
    REQUIRE(evt.waypoint_x == 0.0);
    REQUIRE(evt.waypoint_y == 0.0);
}

TEST_CASE("P04-W02 change_connector_route workbench", "[p04-w02][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.change_connector_route("conn_1", "bezier");
}

TEST_CASE("P04-W02 add_connector_waypoint workbench", "[p04-w02][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_connector_waypoint("conn_1", 100.5, 200.3);
}

TEST_CASE("P04-W02 route then waypoint", "[p04-w02][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.change_connector_route("conn_2", "orthogonal");
    wb.add_connector_waypoint("conn_2", 50.0, 75.0);
}

TEST_CASE("P04-W02 event fields populated", "[p04-w02][events]")
{
    markamp::core::events::ConnectorWaypointAddedEvent evt;
    evt.connector_id = "conn_X";
    evt.waypoint_x = 42.5;
    evt.waypoint_y = 99.1;
    REQUIRE(evt.connector_id == "conn_X");
    REQUIRE(evt.waypoint_x == 42.5);
    REQUIRE(evt.waypoint_y == 99.1);
}

// ============================================================================
// V17 Phase 04 W03: Connector Labels & Ports
// ============================================================================

TEST_CASE("P04-W03 ConnectorLabelSetEvent defaults", "[p04-w03][events]")
{
    markamp::core::events::ConnectorLabelSetEvent evt;
    REQUIRE(evt.connector_id.empty());
    REQUIRE(evt.label.empty());
}

TEST_CASE("P04-W03 PortAssignedEvent defaults", "[p04-w03][events]")
{
    markamp::core::events::PortAssignedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.port_id.empty());
}

TEST_CASE("P04-W03 set_connector_label workbench", "[p04-w03][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.set_connector_label("conn_1", "depends on");
}

TEST_CASE("P04-W03 assign_port workbench", "[p04-w03][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.assign_port("shape_1", "port_south");
}

TEST_CASE("P04-W03 label then port", "[p04-w03][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.set_connector_label("conn_3", "triggers");
    wb.assign_port("node_1", "port_east");
}

TEST_CASE("P04-W03 event fields populated", "[p04-w03][events]")
{
    markamp::core::events::ConnectorLabelSetEvent evt;
    evt.connector_id = "conn_Z";
    evt.label = "references";
    REQUIRE(evt.connector_id == "conn_Z");
    REQUIRE(evt.label == "references");
}

// ============================================================================
// V17 Phase 04 W04: Diagram Library
// ============================================================================

TEST_CASE("P04-W04 DiagramShapeInsertedEvent defaults", "[p04-w04][events]")
{
    markamp::core::events::DiagramShapeInsertedEvent evt;
    REQUIRE(evt.shape_type.empty());
    REQUIRE(evt.category.empty());
}

TEST_CASE("P04-W04 DiagramLibraryBrowsedEvent defaults", "[p04-w04][events]")
{
    markamp::core::events::DiagramLibraryBrowsedEvent evt;
    REQUIRE(evt.category.empty());
}

TEST_CASE("P04-W04 insert_diagram_shape workbench", "[p04-w04][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.insert_diagram_shape("decision", "flowchart");
}

TEST_CASE("P04-W04 browse_diagram_library workbench", "[p04-w04][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.browse_diagram_library("network");
}

TEST_CASE("P04-W04 browse then insert", "[p04-w04][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.browse_diagram_library("UML");
    wb.insert_diagram_shape("class", "UML");
}

TEST_CASE("P04-W04 event fields populated", "[p04-w04][events]")
{
    markamp::core::events::DiagramShapeInsertedEvent evt;
    evt.shape_type = "database";
    evt.category = "systems";
    REQUIRE(evt.shape_type == "database");
    REQUIRE(evt.category == "systems");
}

// ============================================================================
// V17 Phase 04 W05: Flowchart Grammar
// ============================================================================

TEST_CASE("P04-W05 FlowchartDefaultsAppliedEvent defaults", "[p04-w05][events]")
{
    markamp::core::events::FlowchartDefaultsAppliedEvent evt;
    REQUIRE(evt.grammar_name.empty());
}

TEST_CASE("P04-W05 FlowchartValidatedEvent defaults", "[p04-w05][events]")
{
    markamp::core::events::FlowchartValidatedEvent evt;
    REQUIRE(evt.node_count == 0);
    REQUIRE(evt.connector_count == 0);
    REQUIRE(evt.is_valid == false);
}

TEST_CASE("P04-W05 apply_flowchart_defaults workbench", "[p04-w05][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.apply_flowchart_defaults("BPMN");
}

TEST_CASE("P04-W05 validate_flowchart workbench", "[p04-w05][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.validate_flowchart(8, 10, true);
}

TEST_CASE("P04-W05 defaults then validate", "[p04-w05][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.apply_flowchart_defaults("standard");
    wb.validate_flowchart(5, 4, true);
}

TEST_CASE("P04-W05 event fields populated", "[p04-w05][events]")
{
    markamp::core::events::FlowchartValidatedEvent evt;
    evt.node_count = 12;
    evt.connector_count = 15;
    evt.is_valid = true;
    REQUIRE(evt.node_count == 12);
    REQUIRE(evt.connector_count == 15);
    REQUIRE(evt.is_valid == true);
}

// ============================================================================
// V17 Phase 04 W06: Mind Maps
// ============================================================================

TEST_CASE("P04-W06 MindMapNodeAddedEvent defaults", "[p04-w06][events]")
{
    markamp::core::events::MindMapNodeAddedEvent evt;
    REQUIRE(evt.parent_id.empty());
    REQUIRE(evt.node_label.empty());
}

TEST_CASE("P04-W06 MindMapBranchToggledEvent defaults", "[p04-w06][events]")
{
    markamp::core::events::MindMapBranchToggledEvent evt;
    REQUIRE(evt.node_id.empty());
    REQUIRE(evt.collapsed == false);
}

TEST_CASE("P04-W06 add_mind_map_node workbench", "[p04-w06][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_mind_map_node("root", "Idea A");
}

TEST_CASE("P04-W06 toggle_mind_map_branch workbench", "[p04-w06][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.toggle_mind_map_branch("node_3", true);
}

TEST_CASE("P04-W06 add then toggle", "[p04-w06][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_mind_map_node("root", "Branch 1");
    wb.toggle_mind_map_branch("branch_1", true);
}

TEST_CASE("P04-W06 event fields populated", "[p04-w06][events]")
{
    markamp::core::events::MindMapNodeAddedEvent evt;
    evt.parent_id = "center";
    evt.node_label = "Design";
    REQUIRE(evt.parent_id == "center");
    REQUIRE(evt.node_label == "Design");
}

// ============================================================================
// V17 Phase 04 W07: Kanban Boards
// ============================================================================

TEST_CASE("P04-W07 CanvasKanbanCardMovedEvent defaults", "[p04-w07][events]")
{
    markamp::core::events::CanvasKanbanCardMovedEvent evt;
    REQUIRE(evt.card_id.empty());
    REQUIRE(evt.from_lane.empty());
    REQUIRE(evt.to_lane.empty());
}

TEST_CASE("P04-W07 KanbanLaneAddedEvent defaults", "[p04-w07][events]")
{
    markamp::core::events::KanbanLaneAddedEvent evt;
    REQUIRE(evt.lane_name.empty());
}

TEST_CASE("P04-W07 move_canvas_kanban_card workbench", "[p04-w07][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.move_canvas_kanban_card("card_1", "todo", "doing");
}

TEST_CASE("P04-W07 add_kanban_lane workbench", "[p04-w07][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_kanban_lane("Review");
}

TEST_CASE("P04-W07 add lane then move card", "[p04-w07][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_kanban_lane("Done");
    wb.move_canvas_kanban_card("card_2", "doing", "Done");
}

TEST_CASE("P04-W07 event fields populated", "[p04-w07][events]")
{
    markamp::core::events::CanvasKanbanCardMovedEvent evt;
    evt.card_id = "task_X";
    evt.from_lane = "backlog";
    evt.to_lane = "in_progress";
    REQUIRE(evt.card_id == "task_X");
    REQUIRE(evt.from_lane == "backlog");
    REQUIRE(evt.to_lane == "in_progress");
}

// ============================================================================
// V17 Phase 04 W08: Frames & Sections
// ============================================================================

TEST_CASE("P04-W08 FrameCreatedEvent defaults", "[p04-w08][events]")
{
    markamp::core::events::FrameCreatedEvent evt;
    REQUIRE(evt.frame_name.empty());
    REQUIRE(evt.width == 0.0);
    REQUIRE(evt.height == 0.0);
}

TEST_CASE("P04-W08 SectionAddedEvent defaults", "[p04-w08][events]")
{
    markamp::core::events::SectionAddedEvent evt;
    REQUIRE(evt.frame_id.empty());
    REQUIRE(evt.section_name.empty());
}

TEST_CASE("P04-W08 create_frame workbench", "[p04-w08][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_frame("Workshop", 1920.0, 1080.0);
}

TEST_CASE("P04-W08 add_section workbench", "[p04-w08][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_section("frame_1", "Brainstorm");
}

TEST_CASE("P04-W08 frame then section", "[p04-w08][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_frame("Sprint", 800.0, 600.0);
    wb.add_section("frame_1", "Tasks");
}

TEST_CASE("P04-W08 event fields populated", "[p04-w08][events]")
{
    markamp::core::events::FrameCreatedEvent evt;
    evt.frame_name = "Retro";
    evt.width = 1200.5;
    evt.height = 900.0;
    REQUIRE(evt.frame_name == "Retro");
    REQUIRE(evt.width == 1200.5);
    REQUIRE(evt.height == 900.0);
}

// ============================================================================
// V17 Phase 04 W09: Swimlanes & Grids
// ============================================================================

TEST_CASE("P04-W09 SwimlaneCreatedEvent defaults", "[p04-w09][events]")
{
    markamp::core::events::SwimlaneCreatedEvent evt;
    REQUIRE(evt.orientation.empty());
    REQUIRE(evt.lane_count == 0);
}

TEST_CASE("P04-W09 GridLayoutAppliedEvent defaults", "[p04-w09][events]")
{
    markamp::core::events::GridLayoutAppliedEvent evt;
    REQUIRE(evt.grid_rows == 0);
    REQUIRE(evt.grid_columns == 0);
}

TEST_CASE("P04-W09 create_swimlane workbench", "[p04-w09][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_swimlane("horizontal", 3);
}

TEST_CASE("P04-W09 apply_grid_layout workbench", "[p04-w09][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.apply_grid_layout(4, 6);
}

TEST_CASE("P04-W09 swimlane then grid", "[p04-w09][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_swimlane("vertical", 5);
    wb.apply_grid_layout(3, 3);
}

TEST_CASE("P04-W09 event fields populated", "[p04-w09][events]")
{
    markamp::core::events::SwimlaneCreatedEvent evt;
    evt.orientation = "vertical";
    evt.lane_count = 4;
    REQUIRE(evt.orientation == "vertical");
    REQUIRE(evt.lane_count == 4);
}

// ============================================================================
// V17 Phase 04 W10: Container Auto Layout
// ============================================================================

TEST_CASE("P04-W10 AutoLayoutTriggeredEvent defaults", "[p04-w10][events]")
{
    markamp::core::events::AutoLayoutTriggeredEvent evt;
    REQUIRE(evt.container_id.empty());
    REQUIRE(evt.layout_type.empty());
}

TEST_CASE("P04-W10 AutoLayoutSpacingChangedEvent defaults", "[p04-w10][events]")
{
    markamp::core::events::AutoLayoutSpacingChangedEvent evt;
    REQUIRE(evt.container_id.empty());
    REQUIRE(evt.spacing == 0.0);
}

TEST_CASE("P04-W10 trigger_auto_layout workbench", "[p04-w10][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.trigger_auto_layout("frame_1", "horizontal");
}

TEST_CASE("P04-W10 change_auto_layout_spacing workbench", "[p04-w10][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.change_auto_layout_spacing("frame_1", 16.0);
}

TEST_CASE("P04-W10 layout then spacing", "[p04-w10][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.trigger_auto_layout("container_A", "grid");
    wb.change_auto_layout_spacing("container_A", 24.0);
}

TEST_CASE("P04-W10 event fields populated", "[p04-w10][events]")
{
    markamp::core::events::AutoLayoutTriggeredEvent evt;
    evt.container_id = "panel_1";
    evt.layout_type = "wrap";
    REQUIRE(evt.container_id == "panel_1");
    REQUIRE(evt.layout_type == "wrap");
}

// ============================================================================
// V17 Phase 04 W11: Advanced Tables
// ============================================================================

TEST_CASE("P04-W11 AdvancedTableColumnAddedEvent defaults", "[p04-w11][events]")
{
    markamp::core::events::AdvancedTableColumnAddedEvent evt;
    REQUIRE(evt.table_id.empty());
    REQUIRE(evt.column_name.empty());
}

TEST_CASE("P04-W11 AdvancedTableSortedEvent defaults", "[p04-w11][events]")
{
    markamp::core::events::AdvancedTableSortedEvent evt;
    REQUIRE(evt.table_id.empty());
    REQUIRE(evt.sort_column.empty());
    REQUIRE(evt.ascending == true);
}

TEST_CASE("P04-W11 add_advanced_table_column workbench", "[p04-w11][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_advanced_table_column("tbl_1", "Priority");
}

TEST_CASE("P04-W11 sort_advanced_table workbench", "[p04-w11][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.sort_advanced_table("tbl_1", "Priority", false);
}

TEST_CASE("P04-W11 add column then sort", "[p04-w11][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_advanced_table_column("tbl_2", "Status");
    wb.sort_advanced_table("tbl_2", "Status", true);
}

TEST_CASE("P04-W11 event fields populated", "[p04-w11][events]")
{
    markamp::core::events::AdvancedTableSortedEvent evt;
    evt.table_id = "tbl_X";
    evt.sort_column = "Name";
    evt.ascending = false;
    REQUIRE(evt.table_id == "tbl_X");
    REQUIRE(evt.sort_column == "Name");
    REQUIRE(evt.ascending == false);
}

// ============================================================================
// V17 Phase 04 W12: Cross Board Links
// ============================================================================

TEST_CASE("P04-W12 CrossBoardLinkCreatedEvent defaults", "[p04-w12][events]")
{
    markamp::core::events::CrossBoardLinkCreatedEvent evt;
    REQUIRE(evt.source_board_id.empty());
    REQUIRE(evt.target_board_id.empty());
}

TEST_CASE("P04-W12 CrossBoardLinkNavigatedEvent defaults", "[p04-w12][events]")
{
    markamp::core::events::CrossBoardLinkNavigatedEvent evt;
    REQUIRE(evt.link_id.empty());
}

TEST_CASE("P04-W12 create_cross_board_link workbench", "[p04-w12][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_cross_board_link("board_A", "board_B");
}

TEST_CASE("P04-W12 navigate_cross_board_link workbench", "[p04-w12][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.navigate_cross_board_link("link_1");
}

TEST_CASE("P04-W12 create then navigate", "[p04-w12][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_cross_board_link("arch", "deploy");
    wb.navigate_cross_board_link("link_1");
}

TEST_CASE("P04-W12 event fields populated", "[p04-w12][events]")
{
    markamp::core::events::CrossBoardLinkCreatedEvent evt;
    evt.source_board_id = "design";
    evt.target_board_id = "impl";
    REQUIRE(evt.source_board_id == "design");
    REQUIRE(evt.target_board_id == "impl");
}

// ============================================================================
// V17 Phase 04 W13: Diagram From Selection
// ============================================================================

TEST_CASE("P04-W13 DiagramFromSelectionEvent defaults", "[p04-w13][events]")
{
    markamp::core::events::DiagramFromSelectionEvent evt;
    REQUIRE(evt.selected_count == 0);
    REQUIRE(evt.diagram_type.empty());
}

TEST_CASE("P04-W13 SelectionStructuredEvent defaults", "[p04-w13][events]")
{
    markamp::core::events::SelectionStructuredEvent evt;
    REQUIRE(evt.structure_type.empty());
}

TEST_CASE("P04-W13 generate_diagram_from_selection workbench", "[p04-w13][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.generate_diagram_from_selection(5, "flowchart");
}

TEST_CASE("P04-W13 structure_selection workbench", "[p04-w13][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.structure_selection("hierarchy");
}

TEST_CASE("P04-W13 generate then structure", "[p04-w13][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.generate_diagram_from_selection(8, "sequence");
    wb.structure_selection("sequence");
}

TEST_CASE("P04-W13 event fields populated", "[p04-w13][events]")
{
    markamp::core::events::DiagramFromSelectionEvent evt;
    evt.selected_count = 12;
    evt.diagram_type = "mind_map";
    REQUIRE(evt.selected_count == 12);
    REQUIRE(evt.diagram_type == "mind_map");
}

// ============================================================================
// V17 Phase 04 W14: Connected Layout Cleanup
// ============================================================================

TEST_CASE("P04-W14 LayoutCleanupTriggeredEvent defaults", "[p04-w14][events]")
{
    markamp::core::events::LayoutCleanupTriggeredEvent evt;
    REQUIRE(evt.affected_connectors == 0);
}

TEST_CASE("P04-W14 ConnectorReflowEvent defaults", "[p04-w14][events]")
{
    markamp::core::events::ConnectorReflowEvent evt;
    REQUIRE(evt.connector_id.empty());
    REQUIRE(evt.trigger_action.empty());
}

TEST_CASE("P04-W14 trigger_layout_cleanup workbench", "[p04-w14][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.trigger_layout_cleanup(7);
}

TEST_CASE("P04-W14 reflow_connector workbench", "[p04-w14][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.reflow_connector("conn_5", "align");
}

TEST_CASE("P04-W14 cleanup then reflow", "[p04-w14][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.trigger_layout_cleanup(3);
    wb.reflow_connector("conn_2", "distribute");
}

TEST_CASE("P04-W14 event fields populated", "[p04-w14][events]")
{
    markamp::core::events::ConnectorReflowEvent evt;
    evt.connector_id = "conn_Z";
    evt.trigger_action = "move";
    REQUIRE(evt.connector_id == "conn_Z");
    REQUIRE(evt.trigger_action == "move");
}

// ============================================================================
// V17 Phase 04 W15: Relationship Metadata
// ============================================================================

TEST_CASE("P04-W15 RelationshipMetadataSetEvent defaults", "[p04-w15][events]")
{
    markamp::core::events::RelationshipMetadataSetEvent evt;
    REQUIRE(evt.connector_id.empty());
    REQUIRE(evt.metadata_key.empty());
    REQUIRE(evt.metadata_value.empty());
}

TEST_CASE("P04-W15 RelationshipTypeAssignedEvent defaults", "[p04-w15][events]")
{
    markamp::core::events::RelationshipTypeAssignedEvent evt;
    REQUIRE(evt.connector_id.empty());
    REQUIRE(evt.relationship_type.empty());
}

TEST_CASE("P04-W15 set_relationship_metadata workbench", "[p04-w15][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.set_relationship_metadata("conn_1", "weight", "5");
}

TEST_CASE("P04-W15 assign_relationship_type workbench", "[p04-w15][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.assign_relationship_type("conn_1", "dependency");
}

TEST_CASE("P04-W15 metadata then type", "[p04-w15][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.set_relationship_metadata("conn_3", "protocol", "TCP");
    wb.assign_relationship_type("conn_3", "association");
}

TEST_CASE("P04-W15 event fields populated", "[p04-w15][events]")
{
    markamp::core::events::RelationshipMetadataSetEvent evt;
    evt.connector_id = "conn_Q";
    evt.metadata_key = "latency";
    evt.metadata_value = "50ms";
    REQUIRE(evt.connector_id == "conn_Q");
    REQUIRE(evt.metadata_key == "latency");
    REQUIRE(evt.metadata_value == "50ms");
}

// ============================================================================
// V17 Phase 04 W16: Diagram Templates
// ============================================================================

TEST_CASE("P04-W16 DiagramTemplateAppliedEvent defaults", "[p04-w16][events]")
{
    markamp::core::events::DiagramTemplateAppliedEvent evt;
    REQUIRE(evt.template_name.empty());
    REQUIRE(evt.template_category.empty());
}

TEST_CASE("P04-W16 DiagramTemplateSavedEvent defaults", "[p04-w16][events]")
{
    markamp::core::events::DiagramTemplateSavedEvent evt;
    REQUIRE(evt.template_name.empty());
    REQUIRE(evt.node_count == 0);
}

TEST_CASE("P04-W16 apply_diagram_template workbench", "[p04-w16][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.apply_diagram_template("ERD Basic", "database");
}

TEST_CASE("P04-W16 save_diagram_template workbench", "[p04-w16][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.save_diagram_template("My Flow", 12);
}

TEST_CASE("P04-W16 apply then save", "[p04-w16][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.apply_diagram_template("Sequence", "UML");
    wb.save_diagram_template("Custom Sequence", 8);
}

TEST_CASE("P04-W16 event fields populated", "[p04-w16][events]")
{
    markamp::core::events::DiagramTemplateAppliedEvent evt;
    evt.template_name = "Network Diagram";
    evt.template_category = "infrastructure";
    REQUIRE(evt.template_name == "Network Diagram");
    REQUIRE(evt.template_category == "infrastructure");
}

// ============================================================================
// V17 Phase 04 W17: Dependency Validation
// ============================================================================

TEST_CASE("P04-W17 DependencyCheckRunEvent defaults", "[p04-w17][events]")
{
    markamp::core::events::DependencyCheckRunEvent evt;
    REQUIRE(evt.total_links == 0);
    REQUIRE(evt.broken_links == 0);
}

TEST_CASE("P04-W17 BrokenDependencyFixedEvent defaults", "[p04-w17][events]")
{
    markamp::core::events::BrokenDependencyFixedEvent evt;
    REQUIRE(evt.connector_id.empty());
    REQUIRE(evt.fix_action.empty());
}

TEST_CASE("P04-W17 run_dependency_check workbench", "[p04-w17][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.run_dependency_check(15, 2);
}

TEST_CASE("P04-W17 fix_broken_dependency workbench", "[p04-w17][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.fix_broken_dependency("conn_7", "reconnect");
}

TEST_CASE("P04-W17 check then fix", "[p04-w17][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.run_dependency_check(20, 3);
    wb.fix_broken_dependency("conn_5", "remove");
}

TEST_CASE("P04-W17 event fields populated", "[p04-w17][events]")
{
    markamp::core::events::DependencyCheckRunEvent evt;
    evt.total_links = 50;
    evt.broken_links = 5;
    REQUIRE(evt.total_links == 50);
    REQUIRE(evt.broken_links == 5);
}

// ============================================================================
// V17 Phase 04 W18: Diagram Export Readiness
// ============================================================================

TEST_CASE("P04-W18 DiagramExportPreviewEvent defaults", "[p04-w18][events]")
{
    markamp::core::events::DiagramExportPreviewEvent evt;
    REQUIRE(evt.export_format.empty());
}

TEST_CASE("P04-W18 DiagramExportCompletedEvent defaults", "[p04-w18][events]")
{
    markamp::core::events::DiagramExportCompletedEvent evt;
    REQUIRE(evt.export_format.empty());
    REQUIRE(evt.output_path.empty());
}

TEST_CASE("P04-W18 preview_diagram_export workbench", "[p04-w18][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.preview_diagram_export("svg");
}

TEST_CASE("P04-W18 complete_diagram_export workbench", "[p04-w18][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.complete_diagram_export("png", "/tmp/diagram.png");
}

TEST_CASE("P04-W18 preview then export", "[p04-w18][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.preview_diagram_export("pdf");
    wb.complete_diagram_export("pdf", "/tmp/flow.pdf");
}

TEST_CASE("P04-W18 event fields populated", "[p04-w18][events]")
{
    markamp::core::events::DiagramExportCompletedEvent evt;
    evt.export_format = "svg";
    evt.output_path = "/docs/arch.svg";
    REQUIRE(evt.export_format == "svg");
    REQUIRE(evt.output_path == "/docs/arch.svg");
}

// ============================================================================
// V17 Phase 04 W19: Collaborative Diagram Editing
// ============================================================================

TEST_CASE("P04-W19 DiagramLockAcquiredEvent defaults", "[p04-w19][events]")
{
    markamp::core::events::DiagramLockAcquiredEvent evt;
    REQUIRE(evt.diagram_id.empty());
    REQUIRE(evt.user_id.empty());
}

TEST_CASE("P04-W19 DiagramLockReleasedEvent defaults", "[p04-w19][events]")
{
    markamp::core::events::DiagramLockReleasedEvent evt;
    REQUIRE(evt.diagram_id.empty());
}

TEST_CASE("P04-W19 acquire_diagram_lock workbench", "[p04-w19][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.acquire_diagram_lock("diag_1", "user_alice");
}

TEST_CASE("P04-W19 release_diagram_lock workbench", "[p04-w19][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.release_diagram_lock("diag_1");
}

TEST_CASE("P04-W19 lock then release", "[p04-w19][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.acquire_diagram_lock("diag_2", "user_bob");
    wb.release_diagram_lock("diag_2");
}

TEST_CASE("P04-W19 event fields populated", "[p04-w19][events]")
{
    markamp::core::events::DiagramLockAcquiredEvent evt;
    evt.diagram_id = "arch_diag";
    evt.user_id = "user_charlie";
    REQUIRE(evt.diagram_id == "arch_diag");
    REQUIRE(evt.user_id == "user_charlie");
}

// ============================================================================
// V17 Phase 04 W20: Diagram Test Matrix
// ============================================================================

TEST_CASE("P04-W20 DiagramTestSuiteRunEvent defaults", "[p04-w20][events]")
{
    markamp::core::events::DiagramTestSuiteRunEvent evt;
    REQUIRE(evt.tests_passed == 0);
    REQUIRE(evt.tests_failed == 0);
}

TEST_CASE("P04-W20 DiagramRegressionDetectedEvent defaults", "[p04-w20][events]")
{
    markamp::core::events::DiagramRegressionDetectedEvent evt;
    REQUIRE(evt.test_name.empty());
    REQUIRE(evt.description.empty());
}

TEST_CASE("P04-W20 run_diagram_test_suite workbench", "[p04-w20][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.run_diagram_test_suite(45, 0);
}

TEST_CASE("P04-W20 report_diagram_regression workbench", "[p04-w20][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.report_diagram_regression("connector_routing", "Path overlaps node");
}

TEST_CASE("P04-W20 suite then regression", "[p04-w20][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.run_diagram_test_suite(44, 1);
    wb.report_diagram_regression("label_positioning", "Label clips border");
}

TEST_CASE("P04-W20 event fields populated", "[p04-w20][events]")
{
    markamp::core::events::DiagramTestSuiteRunEvent evt;
    evt.tests_passed = 99;
    evt.tests_failed = 1;
    REQUIRE(evt.tests_passed == 99);
    REQUIRE(evt.tests_failed == 1);
}

// ============================================================================
// V17 Phase 05 W01: Image Placement
// ============================================================================

TEST_CASE("P05-W01 ImagePlacedEvent defaults", "[p05-w01][events]")
{
    markamp::core::events::ImagePlacedEvent evt;
    REQUIRE(evt.image_path.empty());
    REQUIRE(evt.scale == 1.0);
}

TEST_CASE("P05-W01 ImageReplacedEvent defaults", "[p05-w01][events]")
{
    markamp::core::events::ImageReplacedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.new_image_path.empty());
}

TEST_CASE("P05-W01 place_image workbench", "[p05-w01][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.place_image("/photos/hero.png", 0.5);
}

TEST_CASE("P05-W01 replace_image workbench", "[p05-w01][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.replace_image("img_1", "/photos/new_hero.png");
}

TEST_CASE("P05-W01 place then replace", "[p05-w01][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.place_image("/assets/logo.svg", 1.0);
    wb.replace_image("img_1", "/assets/logo_v2.svg");
}

TEST_CASE("P05-W01 event fields populated", "[p05-w01][events]")
{
    markamp::core::events::ImagePlacedEvent evt;
    evt.image_path = "/wallpaper.jpg";
    evt.scale = 2.0;
    REQUIRE(evt.image_path == "/wallpaper.jpg");
    REQUIRE(evt.scale == 2.0);
}

// ============================================================================
// V17 Phase 05 W02: Video & Web Embeds
// ============================================================================

TEST_CASE("P05-W02 VideoEmbedAddedEvent defaults", "[p05-w02][events]")
{
    markamp::core::events::VideoEmbedAddedEvent evt;
    REQUIRE(evt.embed_url.empty());
    REQUIRE(evt.embed_type.empty());
}

TEST_CASE("P05-W02 WebEmbedResizedEvent defaults", "[p05-w02][events]")
{
    markamp::core::events::WebEmbedResizedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.width == 0.0);
    REQUIRE(evt.height == 0.0);
}

TEST_CASE("P05-W02 add_video_embed workbench", "[p05-w02][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_video_embed("https://youtube.com/watch?v=abc", "youtube");
}

TEST_CASE("P05-W02 resize_web_embed workbench", "[p05-w02][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.resize_web_embed("embed_1", 640.0, 480.0);
}

TEST_CASE("P05-W02 embed then resize", "[p05-w02][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_video_embed("https://vimeo.com/123", "vimeo");
    wb.resize_web_embed("embed_1", 800.0, 600.0);
}

TEST_CASE("P05-W02 event fields populated", "[p05-w02][events]")
{
    markamp::core::events::VideoEmbedAddedEvent evt;
    evt.embed_url = "https://example.com";
    evt.embed_type = "iframe";
    REQUIRE(evt.embed_url == "https://example.com");
    REQUIRE(evt.embed_type == "iframe");
}

// ============================================================================
// V17 Phase 05 W03: PDF Pages
// ============================================================================

TEST_CASE("P05-W03 PdfPageAddedEvent defaults", "[p05-w03][events]")
{
    markamp::core::events::PdfPageAddedEvent evt;
    REQUIRE(evt.pdf_path.empty());
    REQUIRE(evt.page_number == 1);
}

TEST_CASE("P05-W03 CanvasPdfPageNavigatedEvent defaults", "[p05-w03][events]")
{
    markamp::core::events::CanvasPdfPageNavigatedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.target_page == 1);
}

TEST_CASE("P05-W03 add_pdf_page workbench", "[p05-w03][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_pdf_page("/docs/report.pdf", 3);
}

TEST_CASE("P05-W03 navigate_canvas_pdf_page workbench", "[p05-w03][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.navigate_canvas_pdf_page("pdf_1", 5);
}

TEST_CASE("P05-W03 add then navigate", "[p05-w03][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_pdf_page("/specs/design.pdf", 1);
    wb.navigate_canvas_pdf_page("pdf_1", 10);
}

TEST_CASE("P05-W03 event fields populated", "[p05-w03][events]")
{
    markamp::core::events::PdfPageAddedEvent evt;
    evt.pdf_path = "/manual.pdf";
    evt.page_number = 42;
    REQUIRE(evt.pdf_path == "/manual.pdf");
    REQUIRE(evt.page_number == 42);
}

// ============================================================================
// V17 Phase 05 W04: Bookmark Cards
// ============================================================================

TEST_CASE("P05-W04 BookmarkCardCreatedEvent defaults", "[p05-w04][events]")
{
    markamp::core::events::BookmarkCardCreatedEvent evt;
    REQUIRE(evt.url.empty());
    REQUIRE(evt.title.empty());
}

TEST_CASE("P05-W04 BookmarkCardRefreshedEvent defaults", "[p05-w04][events]")
{
    markamp::core::events::BookmarkCardRefreshedEvent evt;
    REQUIRE(evt.object_id.empty());
}

TEST_CASE("P05-W04 create_bookmark_card workbench", "[p05-w04][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_bookmark_card("https://github.com", "GitHub");
}

TEST_CASE("P05-W04 refresh_bookmark_card workbench", "[p05-w04][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.refresh_bookmark_card("bm_1");
}

TEST_CASE("P05-W04 create then refresh", "[p05-w04][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_bookmark_card("https://docs.rs", "Rust Docs");
    wb.refresh_bookmark_card("bm_1");
}

TEST_CASE("P05-W04 event fields populated", "[p05-w04][events]")
{
    markamp::core::events::BookmarkCardCreatedEvent evt;
    evt.url = "https://markamp.dev";
    evt.title = "MarkAmp";
    REQUIRE(evt.url == "https://markamp.dev");
    REQUIRE(evt.title == "MarkAmp");
}

// ============================================================================
// V17 Phase 05 W05: App Widgets
// ============================================================================

TEST_CASE("P05-W05 AppWidgetAddedEvent defaults", "[p05-w05][events]")
{
    markamp::core::events::AppWidgetAddedEvent evt;
    REQUIRE(evt.widget_type.empty());
    REQUIRE(evt.widget_config.empty());
}

TEST_CASE("P05-W05 AppWidgetConfiguredEvent defaults", "[p05-w05][events]")
{
    markamp::core::events::AppWidgetConfiguredEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.config_key.empty());
    REQUIRE(evt.config_value.empty());
}

TEST_CASE("P05-W05 add_app_widget workbench", "[p05-w05][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_app_widget("timer", "duration=25m");
}

TEST_CASE("P05-W05 configure_app_widget workbench", "[p05-w05][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.configure_app_widget("widget_1", "theme", "dark");
}

TEST_CASE("P05-W05 add then configure", "[p05-w05][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_app_widget("calculator", "mode=scientific");
    wb.configure_app_widget("widget_1", "precision", "8");
}

TEST_CASE("P05-W05 event fields populated", "[p05-w05][events]")
{
    markamp::core::events::AppWidgetConfiguredEvent evt;
    evt.object_id = "w_99";
    evt.config_key = "refresh_rate";
    evt.config_value = "30s";
    REQUIRE(evt.object_id == "w_99");
    REQUIRE(evt.config_key == "refresh_rate");
    REQUIRE(evt.config_value == "30s");
}

// ============================================================================
// V17 Phase 05 W06: Icon Insertion
// ============================================================================

TEST_CASE("P05-W06 CanvasIconInsertedEvent defaults", "[p05-w06][events]")
{
    markamp::core::events::CanvasIconInsertedEvent evt;
    REQUIRE(evt.icon_name.empty());
    REQUIRE(evt.icon_set.empty());
}

TEST_CASE("P05-W06 IconColorChangedEvent defaults", "[p05-w06][events]")
{
    markamp::core::events::IconColorChangedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.new_color.empty());
}

TEST_CASE("P05-W06 insert_canvas_icon workbench", "[p05-w06][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.insert_canvas_icon("arrow-right", "lucide");
}

TEST_CASE("P05-W06 change_icon_color workbench", "[p05-w06][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.change_icon_color("icon_1", "#FF5500");
}

TEST_CASE("P05-W06 insert then recolor", "[p05-w06][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.insert_canvas_icon("check-circle", "lucide");
    wb.change_icon_color("icon_1", "#00CC44");
}

TEST_CASE("P05-W06 event fields populated", "[p05-w06][events]")
{
    markamp::core::events::CanvasIconInsertedEvent evt;
    evt.icon_name = "star";
    evt.icon_set = "material";
    REQUIRE(evt.icon_name == "star");
    REQUIRE(evt.icon_set == "material");
}

// ============================================================================
// V17 Phase 05 W07: Drag & Drop Intake
// ============================================================================

TEST_CASE("P05-W07 ContentDroppedEvent defaults", "[p05-w07][events]")
{
    markamp::core::events::ContentDroppedEvent evt;
    REQUIRE(evt.content_type.empty());
    REQUIRE(evt.source_path.empty());
}

TEST_CASE("P05-W07 DropIntakeCompletedEvent defaults", "[p05-w07][events]")
{
    markamp::core::events::DropIntakeCompletedEvent evt;
    REQUIRE(evt.items_ingested == 0);
}

TEST_CASE("P05-W07 handle_content_drop workbench", "[p05-w07][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.handle_content_drop("image", "/Desktop/photo.jpg");
}

TEST_CASE("P05-W07 complete_drop_intake workbench", "[p05-w07][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.complete_drop_intake(3);
}

TEST_CASE("P05-W07 drop then complete", "[p05-w07][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.handle_content_drop("file", "/docs/notes.md");
    wb.complete_drop_intake(1);
}

TEST_CASE("P05-W07 event fields populated", "[p05-w07][events]")
{
    markamp::core::events::ContentDroppedEvent evt;
    evt.content_type = "url";
    evt.source_path = "https://example.com";
    REQUIRE(evt.content_type == "url");
    REQUIRE(evt.source_path == "https://example.com");
}

// ============================================================================
// V17 Phase 05 W08: Clipboard Ingest
// ============================================================================

TEST_CASE("P05-W08 ClipboardPastedEvent defaults", "[p05-w08][events]")
{
    markamp::core::events::ClipboardPastedEvent evt;
    REQUIRE(evt.paste_format.empty());
}

TEST_CASE("P05-W08 RichPasteConvertedEvent defaults", "[p05-w08][events]")
{
    markamp::core::events::RichPasteConvertedEvent evt;
    REQUIRE(evt.source_format.empty());
    REQUIRE(evt.target_format.empty());
}

TEST_CASE("P05-W08 paste_clipboard workbench", "[p05-w08][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.paste_clipboard("html");
}

TEST_CASE("P05-W08 convert_rich_paste workbench", "[p05-w08][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.convert_rich_paste("rtf", "markdown");
}

TEST_CASE("P05-W08 paste then convert", "[p05-w08][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.paste_clipboard("image");
    wb.convert_rich_paste("png", "webp");
}

TEST_CASE("P05-W08 event fields populated", "[p05-w08][events]")
{
    markamp::core::events::RichPasteConvertedEvent evt;
    evt.source_format = "html";
    evt.target_format = "markdown";
    REQUIRE(evt.source_format == "html");
    REQUIRE(evt.target_format == "markdown");
}

// ============================================================================
// V17 Phase 05 W09: Media Optimization
// ============================================================================

TEST_CASE("P05-W09 MediaOptimizationRunEvent defaults", "[p05-w09][events]")
{
    markamp::core::events::MediaOptimizationRunEvent evt;
    REQUIRE(evt.assets_optimized == 0);
    REQUIRE(evt.size_reduction_pct == 0.0);
}

TEST_CASE("P05-W09 MediaQualitySetEvent defaults", "[p05-w09][events]")
{
    markamp::core::events::MediaQualitySetEvent evt;
    REQUIRE(evt.quality_level.empty());
}

TEST_CASE("P05-W09 run_media_optimization workbench", "[p05-w09][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.run_media_optimization(15, 32.5);
}

TEST_CASE("P05-W09 set_media_quality workbench", "[p05-w09][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.set_media_quality("high");
}

TEST_CASE("P05-W09 optimize then set quality", "[p05-w09][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.run_media_optimization(25, 45.0);
    wb.set_media_quality("medium");
}

TEST_CASE("P05-W09 event fields populated", "[p05-w09][events]")
{
    markamp::core::events::MediaOptimizationRunEvent evt;
    evt.assets_optimized = 42;
    evt.size_reduction_pct = 55.5;
    REQUIRE(evt.assets_optimized == 42);
    REQUIRE(evt.size_reduction_pct == 55.5);
}

// ============================================================================
// V17 Phase 05 W10: Cropping & Masking
// ============================================================================

TEST_CASE("P05-W10 ImageCroppedEvent defaults", "[p05-w10][events]")
{
    markamp::core::events::ImageCroppedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.crop_x == 0.0);
    REQUIRE(evt.crop_y == 0.0);
    REQUIRE(evt.crop_width == 0.0);
    REQUIRE(evt.crop_height == 0.0);
}

TEST_CASE("P05-W10 ImageMaskAppliedEvent defaults", "[p05-w10][events]")
{
    markamp::core::events::ImageMaskAppliedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.mask_type.empty());
}

TEST_CASE("P05-W10 crop_image workbench", "[p05-w10][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.crop_image("img_1", 10.0, 20.0, 200.0, 150.0);
}

TEST_CASE("P05-W10 apply_image_mask workbench", "[p05-w10][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.apply_image_mask("img_1", "circle");
}

TEST_CASE("P05-W10 crop then mask", "[p05-w10][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.crop_image("img_2", 0.0, 0.0, 100.0, 100.0);
    wb.apply_image_mask("img_2", "rounded_rect");
}

TEST_CASE("P05-W10 event fields populated", "[p05-w10][events]")
{
    markamp::core::events::ImageCroppedEvent evt;
    evt.object_id = "img_X";
    evt.crop_x = 50.0;
    evt.crop_y = 25.0;
    evt.crop_width = 300.0;
    evt.crop_height = 200.0;
    REQUIRE(evt.object_id == "img_X");
    REQUIRE(evt.crop_x == 50.0);
    REQUIRE(evt.crop_width == 300.0);
}

// ============================================================================
// V17 Phase 05 W11: Captions & Metadata
// ============================================================================

TEST_CASE("P05-W11 AssetCaptionSetEvent defaults", "[p05-w11][events]")
{
    markamp::core::events::AssetCaptionSetEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.caption_text.empty());
}

TEST_CASE("P05-W11 AssetAltTextSetEvent defaults", "[p05-w11][events]")
{
    markamp::core::events::AssetAltTextSetEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.alt_text.empty());
}

TEST_CASE("P05-W11 set_asset_caption workbench", "[p05-w11][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.set_asset_caption("img_1", "Team photo from Q4 offsite");
}

TEST_CASE("P05-W11 set_asset_alt_text workbench", "[p05-w11][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.set_asset_alt_text("img_1", "Group of people standing in front of office");
}

TEST_CASE("P05-W11 caption then alt text", "[p05-w11][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.set_asset_caption("img_2", "Architecture diagram");
    wb.set_asset_alt_text("img_2", "System architecture overview");
}

TEST_CASE("P05-W11 event fields populated", "[p05-w11][events]")
{
    markamp::core::events::AssetCaptionSetEvent evt;
    evt.object_id = "asset_42";
    evt.caption_text = "Logo v2";
    REQUIRE(evt.object_id == "asset_42");
    REQUIRE(evt.caption_text == "Logo v2");
}

// ============================================================================
// V17 Phase 05 W12: Asset Library Panel
// ============================================================================

TEST_CASE("P05-W12 AssetLibraryAddedEvent defaults", "[p05-w12][events]")
{
    markamp::core::events::AssetLibraryAddedEvent evt;
    REQUIRE(evt.asset_name.empty());
    REQUIRE(evt.asset_category.empty());
}

TEST_CASE("P05-W12 AssetLibraryRemovedEvent defaults", "[p05-w12][events]")
{
    markamp::core::events::AssetLibraryRemovedEvent evt;
    REQUIRE(evt.asset_id.empty());
}

TEST_CASE("P05-W12 add_to_asset_library workbench", "[p05-w12][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_to_asset_library("company_logo.svg", "branding");
}

TEST_CASE("P05-W12 remove_from_asset_library workbench", "[p05-w12][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.remove_from_asset_library("asset_old");
}

TEST_CASE("P05-W12 add then remove", "[p05-w12][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_to_asset_library("icon_pack.zip", "icons");
    wb.remove_from_asset_library("icon_pack");
}

TEST_CASE("P05-W12 event fields populated", "[p05-w12][events]")
{
    markamp::core::events::AssetLibraryAddedEvent evt;
    evt.asset_name = "hero_banner.png";
    evt.asset_category = "marketing";
    REQUIRE(evt.asset_name == "hero_banner.png");
    REQUIRE(evt.asset_category == "marketing");
}

// ============================================================================
// V17 Phase 05 W13: Asset Styling
// ============================================================================

TEST_CASE("P05-W13 AssetFrameAppliedEvent defaults", "[p05-w13][events]")
{
    markamp::core::events::AssetFrameAppliedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.frame_style.empty());
}

TEST_CASE("P05-W13 AssetShadowChangedEvent defaults", "[p05-w13][events]")
{
    markamp::core::events::AssetShadowChangedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.shadow_offset == 0.0);
    REQUIRE(evt.shadow_blur == 0.0);
}

TEST_CASE("P05-W13 apply_asset_frame workbench", "[p05-w13][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.apply_asset_frame("img_1", "rounded");
}

TEST_CASE("P05-W13 change_asset_shadow workbench", "[p05-w13][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.change_asset_shadow("img_1", 4.0, 8.0);
}

TEST_CASE("P05-W13 frame then shadow", "[p05-w13][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.apply_asset_frame("img_2", "border");
    wb.change_asset_shadow("img_2", 2.0, 6.0);
}

TEST_CASE("P05-W13 event fields populated", "[p05-w13][events]")
{
    markamp::core::events::AssetShadowChangedEvent evt;
    evt.object_id = "card_1";
    evt.shadow_offset = 5.0;
    evt.shadow_blur = 10.0;
    REQUIRE(evt.shadow_offset == 5.0);
    REQUIRE(evt.shadow_blur == 10.0);
}

// ============================================================================
// V17 Phase 05 W14: Mixed Media Boards
// ============================================================================

TEST_CASE("P05-W14 MediaCompositionCreatedEvent defaults", "[p05-w14][events]")
{
    markamp::core::events::MediaCompositionCreatedEvent evt;
    REQUIRE(evt.object_count == 0);
    REQUIRE(evt.layout_mode.empty());
}

TEST_CASE("P05-W14 MediaLayerReorderedEvent defaults", "[p05-w14][events]")
{
    markamp::core::events::MediaLayerReorderedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.new_z_index == 0);
}

TEST_CASE("P05-W14 create_media_composition workbench", "[p05-w14][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_media_composition(5, "grid");
}

TEST_CASE("P05-W14 reorder_media_layer workbench", "[p05-w14][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.reorder_media_layer("obj_3", 10);
}

TEST_CASE("P05-W14 compose then reorder", "[p05-w14][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_media_composition(3, "freeform");
    wb.reorder_media_layer("obj_1", 5);
}

TEST_CASE("P05-W14 event fields populated", "[p05-w14][events]")
{
    markamp::core::events::MediaCompositionCreatedEvent evt;
    evt.object_count = 12;
    evt.layout_mode = "stack";
    REQUIRE(evt.object_count == 12);
    REQUIRE(evt.layout_mode == "stack");
}

// ============================================================================
// V17 Phase 05 W15: Import & Convert
// ============================================================================

TEST_CASE("P05-W15 ExternalFileImportedEvent defaults", "[p05-w15][events]")
{
    markamp::core::events::ExternalFileImportedEvent evt;
    REQUIRE(evt.source_path.empty());
    REQUIRE(evt.converted_format.empty());
}

TEST_CASE("P05-W15 ImportConversionCompletedEvent defaults", "[p05-w15][events]")
{
    markamp::core::events::ImportConversionCompletedEvent evt;
    REQUIRE(evt.files_converted == 0);
    REQUIRE(evt.all_succeeded == true);
}

TEST_CASE("P05-W15 import_external_file workbench", "[p05-w15][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.import_external_file("/docs/wireframe.fig", "svg");
}

TEST_CASE("P05-W15 complete_import_conversion workbench", "[p05-w15][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.complete_import_conversion(5, true);
}

TEST_CASE("P05-W15 import then complete", "[p05-w15][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.import_external_file("/slides/deck.pptx", "png");
    wb.complete_import_conversion(12, false);
}

TEST_CASE("P05-W15 event fields populated", "[p05-w15][events]")
{
    markamp::core::events::ImportConversionCompletedEvent evt;
    evt.files_converted = 8;
    evt.all_succeeded = false;
    REQUIRE(evt.files_converted == 8);
    REQUIRE(evt.all_succeeded == false);
}

// ============================================================================
// V17 Phase 05 W16: Media Review Flows
// ============================================================================

TEST_CASE("P05-W16 MediaReviewStartedEvent defaults", "[p05-w16][events]")
{
    markamp::core::events::MediaReviewStartedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.reviewer.empty());
}

TEST_CASE("P05-W16 MediaReviewCompletedEvent defaults", "[p05-w16][events]")
{
    markamp::core::events::MediaReviewCompletedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.approved == false);
}

TEST_CASE("P05-W16 start_media_review workbench", "[p05-w16][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.start_media_review("img_1", "alice");
}

TEST_CASE("P05-W16 complete_media_review workbench", "[p05-w16][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.complete_media_review("img_1", true);
}

TEST_CASE("P05-W16 review then approve", "[p05-w16][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.start_media_review("img_2", "bob");
    wb.complete_media_review("img_2", false);
}

TEST_CASE("P05-W16 event fields populated", "[p05-w16][events]")
{
    markamp::core::events::MediaReviewCompletedEvent evt;
    evt.object_id = "vid_3";
    evt.approved = true;
    REQUIRE(evt.object_id == "vid_3");
    REQUIRE(evt.approved == true);
}

// ============================================================================
// V17 Phase 05 W17: Licensing & Security
// ============================================================================

TEST_CASE("P05-W17 ContentLicenseSetEvent defaults", "[p05-w17][events]")
{
    markamp::core::events::ContentLicenseSetEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.license_type.empty());
}

TEST_CASE("P05-W17 SecurityScanCompletedEvent defaults", "[p05-w17][events]")
{
    markamp::core::events::SecurityScanCompletedEvent evt;
    REQUIRE(evt.assets_scanned == 0);
    REQUIRE(evt.threats_found == 0);
}

TEST_CASE("P05-W17 set_content_license workbench", "[p05-w17][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.set_content_license("img_1", "cc-by");
}

TEST_CASE("P05-W17 run_security_scan workbench", "[p05-w17][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.run_security_scan(50, 0);
}

TEST_CASE("P05-W17 license then scan", "[p05-w17][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.set_content_license("vid_1", "proprietary");
    wb.run_security_scan(10, 2);
}

TEST_CASE("P05-W17 event fields populated", "[p05-w17][events]")
{
    markamp::core::events::SecurityScanCompletedEvent evt;
    evt.assets_scanned = 100;
    evt.threats_found = 3;
    REQUIRE(evt.assets_scanned == 100);
    REQUIRE(evt.threats_found == 3);
}

// ============================================================================
// V17 Phase 05 W18: Asset Serialization & Sync
// ============================================================================

TEST_CASE("P05-W18 AssetMetadataSerializedEvent defaults", "[p05-w18][events]")
{
    markamp::core::events::AssetMetadataSerializedEvent evt;
    REQUIRE(evt.assets_serialized == 0);
    REQUIRE(evt.format.empty());
}

TEST_CASE("P05-W18 AssetSyncCompletedEvent defaults", "[p05-w18][events]")
{
    markamp::core::events::AssetSyncCompletedEvent evt;
    REQUIRE(evt.assets_synced == 0);
    REQUIRE(evt.conflicts_detected == 0);
}

TEST_CASE("P05-W18 serialize_asset_metadata workbench", "[p05-w18][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.serialize_asset_metadata(25, "json");
}

TEST_CASE("P05-W18 complete_asset_sync workbench", "[p05-w18][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.complete_asset_sync(20, 1);
}

TEST_CASE("P05-W18 serialize then sync", "[p05-w18][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.serialize_asset_metadata(30, "yaml");
    wb.complete_asset_sync(30, 0);
}

TEST_CASE("P05-W18 event fields populated", "[p05-w18][events]")
{
    markamp::core::events::AssetSyncCompletedEvent evt;
    evt.assets_synced = 42;
    evt.conflicts_detected = 5;
    REQUIRE(evt.assets_synced == 42);
    REQUIRE(evt.conflicts_detected == 5);
}

// ============================================================================
// V17 Phase 05 W19: Rich Content Export
// ============================================================================

TEST_CASE("P05-W19 RichContentExportStartedEvent defaults", "[p05-w19][events]")
{
    markamp::core::events::RichContentExportStartedEvent evt;
    REQUIRE(evt.export_format.empty());
    REQUIRE(evt.objects_to_export == 0);
}

TEST_CASE("P05-W19 RichContentExportCompletedEvent defaults", "[p05-w19][events]")
{
    markamp::core::events::RichContentExportCompletedEvent evt;
    REQUIRE(evt.output_path.empty());
    REQUIRE(evt.success == true);
}

TEST_CASE("P05-W19 start_rich_content_export workbench", "[p05-w19][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.start_rich_content_export("pdf", 15);
}

TEST_CASE("P05-W19 complete_rich_content_export workbench", "[p05-w19][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.complete_rich_content_export("/output/board.pdf", true);
}

TEST_CASE("P05-W19 export start then complete", "[p05-w19][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.start_rich_content_export("html", 8);
    wb.complete_rich_content_export("/output/board.html", false);
}

TEST_CASE("P05-W19 event fields populated", "[p05-w19][events]")
{
    markamp::core::events::RichContentExportStartedEvent evt;
    evt.export_format = "svg";
    evt.objects_to_export = 20;
    REQUIRE(evt.export_format == "svg");
    REQUIRE(evt.objects_to_export == 20);
}

// ============================================================================
// V17 Phase 05 W20: Asset Test Coverage
// ============================================================================

TEST_CASE("P05-W20 AssetTestSuiteRunEvent defaults", "[p05-w20][events]")
{
    markamp::core::events::AssetTestSuiteRunEvent evt;
    REQUIRE(evt.tests_run == 0);
    REQUIRE(evt.tests_passed == 0);
}

TEST_CASE("P05-W20 AssetRegressionDetectedEvent defaults", "[p05-w20][events]")
{
    markamp::core::events::AssetRegressionDetectedEvent evt;
    REQUIRE(evt.test_name.empty());
    REQUIRE(evt.description.empty());
}

TEST_CASE("P05-W20 run_asset_test_suite workbench", "[p05-w20][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.run_asset_test_suite(100, 98);
}

TEST_CASE("P05-W20 report_asset_regression workbench", "[p05-w20][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.report_asset_regression("test_image_crop", "Crop bounds off by 1px");
}

TEST_CASE("P05-W20 test then report regression", "[p05-w20][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.run_asset_test_suite(50, 49);
    wb.report_asset_regression("test_svg_render", "SVG gradient missing");
}

TEST_CASE("P05-W20 event fields populated", "[p05-w20][events]")
{
    markamp::core::events::AssetTestSuiteRunEvent evt;
    evt.tests_run = 200;
    evt.tests_passed = 195;
    REQUIRE(evt.tests_run == 200);
    REQUIRE(evt.tests_passed == 195);
}

// ============================================================================
// V17 Phase 06 W01: Live Presence
// ============================================================================

TEST_CASE("P06-W01 CollaboratorJoinedEvent defaults", "[p06-w01][events]")
{
    markamp::core::events::CollaboratorJoinedEvent evt;
    REQUIRE(evt.user_id.empty());
    REQUIRE(evt.display_name.empty());
}

TEST_CASE("P06-W01 CollaboratorLeftEvent defaults", "[p06-w01][events]")
{
    markamp::core::events::CollaboratorLeftEvent evt;
    REQUIRE(evt.user_id.empty());
    REQUIRE(evt.reason.empty());
}

TEST_CASE("P06-W01 collaborator_joined workbench", "[p06-w01][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.collaborator_joined("user_42", "Alice");
}

TEST_CASE("P06-W01 collaborator_left workbench", "[p06-w01][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.collaborator_left("user_42", "disconnect");
}

TEST_CASE("P06-W01 join then leave", "[p06-w01][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.collaborator_joined("user_7", "Bob");
    wb.collaborator_left("user_7", "timeout");
}

TEST_CASE("P06-W01 event fields populated", "[p06-w01][events]")
{
    markamp::core::events::CollaboratorJoinedEvent evt;
    evt.user_id = "u_99";
    evt.display_name = "Charlie";
    REQUIRE(evt.user_id == "u_99");
    REQUIRE(evt.display_name == "Charlie");
}

// ============================================================================
// V17 Phase 06 W02: Remote Selections & Follow
// ============================================================================

TEST_CASE("P06-W02 RemoteSelectionChangedEvent defaults", "[p06-w02][events]")
{
    markamp::core::events::RemoteSelectionChangedEvent evt;
    REQUIRE(evt.user_id.empty());
    REQUIRE(evt.selected_object_count == 0);
}

TEST_CASE("P06-W02 FollowModeToggledEvent defaults", "[p06-w02][events]")
{
    markamp::core::events::FollowModeToggledEvent evt;
    REQUIRE(evt.target_user_id.empty());
    REQUIRE(evt.following == false);
}

TEST_CASE("P06-W02 update_remote_selection workbench", "[p06-w02][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.update_remote_selection("user_1", 3);
}

TEST_CASE("P06-W02 toggle_follow_mode workbench", "[p06-w02][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.toggle_follow_mode("user_2", true);
}

TEST_CASE("P06-W02 select then follow", "[p06-w02][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.update_remote_selection("user_5", 2);
    wb.toggle_follow_mode("user_5", true);
}

TEST_CASE("P06-W02 event fields populated", "[p06-w02][events]")
{
    markamp::core::events::FollowModeToggledEvent evt;
    evt.target_user_id = "user_10";
    evt.following = true;
    REQUIRE(evt.target_user_id == "user_10");
    REQUIRE(evt.following == true);
}

// ============================================================================
// V17 Phase 06 W03: Conflict Handling
// ============================================================================

TEST_CASE("P06-W03 EditConflictDetectedEvent defaults", "[p06-w03][events]")
{
    markamp::core::events::EditConflictDetectedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.conflicting_user.empty());
}

TEST_CASE("P06-W03 ConflictResolvedEvent defaults", "[p06-w03][events]")
{
    markamp::core::events::ConflictResolvedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.resolution_strategy.empty());
}

TEST_CASE("P06-W03 report_edit_conflict workbench", "[p06-w03][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.report_edit_conflict("obj_1", "alice");
}

TEST_CASE("P06-W03 resolve_conflict workbench", "[p06-w03][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.resolve_conflict("obj_1", "last_write_wins");
}

TEST_CASE("P06-W03 conflict then resolve", "[p06-w03][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.report_edit_conflict("obj_2", "bob");
    wb.resolve_conflict("obj_2", "merge");
}

TEST_CASE("P06-W03 event fields populated", "[p06-w03][events]")
{
    markamp::core::events::ConflictResolvedEvent evt;
    evt.object_id = "obj_X";
    evt.resolution_strategy = "manual";
    REQUIRE(evt.object_id == "obj_X");
    REQUIRE(evt.resolution_strategy == "manual");
}

// ============================================================================
// V17 Phase 06 W04: Comment Threads
// ============================================================================

TEST_CASE("P06-W04 CommentThreadCreatedEvent defaults", "[p06-w04][events]")
{
    markamp::core::events::CommentThreadCreatedEvent evt;
    REQUIRE(evt.thread_id.empty());
    REQUIRE(evt.anchor_object_id.empty());
}

TEST_CASE("P06-W04 CommentReplyAddedEvent defaults", "[p06-w04][events]")
{
    markamp::core::events::CommentReplyAddedEvent evt;
    REQUIRE(evt.thread_id.empty());
    REQUIRE(evt.author.empty());
}

TEST_CASE("P06-W04 create_comment_thread workbench", "[p06-w04][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_comment_thread("thread_1", "sticky_note_3");
}

TEST_CASE("P06-W04 add_comment_reply workbench", "[p06-w04][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_comment_reply("thread_1", "alice");
}

TEST_CASE("P06-W04 create then reply", "[p06-w04][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.create_comment_thread("thread_2", "img_1");
    wb.add_comment_reply("thread_2", "bob");
}

TEST_CASE("P06-W04 event fields populated", "[p06-w04][events]")
{
    markamp::core::events::CommentThreadCreatedEvent evt;
    evt.thread_id = "t_99";
    evt.anchor_object_id = "shape_5";
    REQUIRE(evt.thread_id == "t_99");
    REQUIRE(evt.anchor_object_id == "shape_5");
}

// ============================================================================
// V17 Phase 06 W05: Voting & Reactions
// ============================================================================

TEST_CASE("P06-W05 VoteCastEvent defaults", "[p06-w05][events]")
{
    markamp::core::events::VoteCastEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.voter.empty());
}

TEST_CASE("P06-W05 ReactionAddedEvent defaults", "[p06-w05][events]")
{
    markamp::core::events::ReactionAddedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.reaction_type.empty());
}

TEST_CASE("P06-W05 cast_vote workbench", "[p06-w05][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.cast_vote("idea_1", "alice");
}

TEST_CASE("P06-W05 add_reaction workbench", "[p06-w05][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_reaction("idea_1", "thumbs_up");
}

TEST_CASE("P06-W05 vote then react", "[p06-w05][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.cast_vote("idea_2", "bob");
    wb.add_reaction("idea_2", "fire");
}

TEST_CASE("P06-W05 event fields populated", "[p06-w05][events]")
{
    markamp::core::events::ReactionAddedEvent evt;
    evt.object_id = "card_7";
    evt.reaction_type = "heart";
    REQUIRE(evt.object_id == "card_7");
    REQUIRE(evt.reaction_type == "heart");
}

// ============================================================================
// V17 Phase 06 W06: Timer & Facilitation Controls
// ============================================================================

TEST_CASE("P06-W06 FacilitationTimerStartedEvent defaults", "[p06-w06][events]")
{
    markamp::core::events::FacilitationTimerStartedEvent evt;
    REQUIRE(evt.duration_seconds == 0);
    REQUIRE(evt.facilitator.empty());
}

TEST_CASE("P06-W06 FacilitationTimerExpiredEvent defaults", "[p06-w06][events]")
{
    markamp::core::events::FacilitationTimerExpiredEvent evt;
    REQUIRE(evt.session_id.empty());
    REQUIRE(evt.auto_extended == false);
}

TEST_CASE("P06-W06 start_facilitation_timer workbench", "[p06-w06][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.start_facilitation_timer(300, "alice");
}

TEST_CASE("P06-W06 expire_facilitation_timer workbench", "[p06-w06][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.expire_facilitation_timer("session_1", false);
}

TEST_CASE("P06-W06 start then expire", "[p06-w06][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.start_facilitation_timer(600, "bob");
    wb.expire_facilitation_timer("session_2", true);
}

TEST_CASE("P06-W06 event fields populated", "[p06-w06][events]")
{
    markamp::core::events::FacilitationTimerStartedEvent evt;
    evt.duration_seconds = 120;
    evt.facilitator = "carol";
    REQUIRE(evt.duration_seconds == 120);
    REQUIRE(evt.facilitator == "carol");
}

// ============================================================================
// V17 Phase 06 W07: Private Reveal & Presentation
// ============================================================================

TEST_CASE("P06-W07 PrivateRevealInitiatedEvent defaults", "[p06-w07][events]")
{
    markamp::core::events::PrivateRevealInitiatedEvent evt;
    REQUIRE(evt.presenter.empty());
    REQUIRE(evt.hidden_object_count == 0);
}

TEST_CASE("P06-W07 ContentRevealedEvent defaults", "[p06-w07][events]")
{
    markamp::core::events::ContentRevealedEvent evt;
    REQUIRE(evt.revealed_count == 0);
    REQUIRE(evt.reveal_mode.empty());
}

TEST_CASE("P06-W07 initiate_private_reveal workbench", "[p06-w07][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.initiate_private_reveal("alice", 5);
}

TEST_CASE("P06-W07 reveal_content workbench", "[p06-w07][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.reveal_content(5, "all_at_once");
}

TEST_CASE("P06-W07 hide then reveal", "[p06-w07][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.initiate_private_reveal("bob", 10);
    wb.reveal_content(10, "sequential");
}

TEST_CASE("P06-W07 event fields populated", "[p06-w07][events]")
{
    markamp::core::events::ContentRevealedEvent evt;
    evt.revealed_count = 8;
    evt.reveal_mode = "by_group";
    REQUIRE(evt.revealed_count == 8);
    REQUIRE(evt.reveal_mode == "by_group");
}

// ============================================================================
// V17 Phase 06 W08: Permissions & Shared Locks
// ============================================================================

TEST_CASE("P06-W08 ObjectPermissionChangedEvent defaults", "[p06-w08][events]")
{
    markamp::core::events::ObjectPermissionChangedEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.permission_level.empty());
}

TEST_CASE("P06-W08 SharedLockAcquiredEvent defaults", "[p06-w08][events]")
{
    markamp::core::events::SharedLockAcquiredEvent evt;
    REQUIRE(evt.object_id.empty());
    REQUIRE(evt.locked_by.empty());
}

TEST_CASE("P06-W08 change_object_permission workbench", "[p06-w08][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.change_object_permission("obj_1", "view");
}

TEST_CASE("P06-W08 acquire_shared_lock workbench", "[p06-w08][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.acquire_shared_lock("obj_1", "alice");
}

TEST_CASE("P06-W08 lock then permission", "[p06-w08][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.acquire_shared_lock("obj_2", "bob");
    wb.change_object_permission("obj_2", "locked");
}

TEST_CASE("P06-W08 event fields populated", "[p06-w08][events]")
{
    markamp::core::events::SharedLockAcquiredEvent evt;
    evt.object_id = "shape_9";
    evt.locked_by = "carol";
    REQUIRE(evt.object_id == "shape_9");
    REQUIRE(evt.locked_by == "carol");
}

// ============================================================================
// V17 Phase 06 W09: Offline Queue
// ============================================================================

TEST_CASE("P06-W09 OfflineOperationQueuedEvent defaults", "[p06-w09][events]")
{
    markamp::core::events::OfflineOperationQueuedEvent evt;
    REQUIRE(evt.queue_depth == 0);
    REQUIRE(evt.operation_type.empty());
}

TEST_CASE("P06-W09 OfflineQueueFlushedEvent defaults", "[p06-w09][events]")
{
    markamp::core::events::OfflineQueueFlushedEvent evt;
    REQUIRE(evt.operations_applied == 0);
    REQUIRE(evt.operations_conflicted == 0);
}

TEST_CASE("P06-W09 queue_offline_operation workbench", "[p06-w09][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.queue_offline_operation(1, "create");
}

TEST_CASE("P06-W09 flush_offline_queue workbench", "[p06-w09][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.flush_offline_queue(10, 0);
}

TEST_CASE("P06-W09 queue then flush", "[p06-w09][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.queue_offline_operation(5, "update");
    wb.flush_offline_queue(5, 1);
}

TEST_CASE("P06-W09 event fields populated", "[p06-w09][events]")
{
    markamp::core::events::OfflineQueueFlushedEvent evt;
    evt.operations_applied = 20;
    evt.operations_conflicted = 3;
    REQUIRE(evt.operations_applied == 20);
    REQUIRE(evt.operations_conflicted == 3);
}

// ============================================================================
// V17 Phase 06 W10: Activity Feed
// ============================================================================

TEST_CASE("P06-W10 ActivityFeedEntryAddedEvent defaults", "[p06-w10][events]")
{
    markamp::core::events::ActivityFeedEntryAddedEvent evt;
    REQUIRE(evt.actor.empty());
    REQUIRE(evt.action_description.empty());
}

TEST_CASE("P06-W10 ActivityFeedClearedEvent defaults", "[p06-w10][events]")
{
    markamp::core::events::ActivityFeedClearedEvent evt;
    REQUIRE(evt.entries_cleared == 0);
    REQUIRE(evt.cleared_by.empty());
}

TEST_CASE("P06-W10 add_activity_feed_entry workbench", "[p06-w10][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_activity_feed_entry("alice", "moved sticky note");
}

TEST_CASE("P06-W10 clear_activity_feed workbench", "[p06-w10][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.clear_activity_feed(50, "admin");
}

TEST_CASE("P06-W10 add then clear", "[p06-w10][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    markamp::canvas::CanvasWorkbench wb(bus);
    wb.add_activity_feed_entry("bob", "added image");
    wb.clear_activity_feed(100, "moderator");
}

TEST_CASE("P06-W10 event fields populated", "[p06-w10][events]")
{
    markamp::core::events::ActivityFeedEntryAddedEvent evt;
    evt.actor = "dave";
    evt.action_description = "created connector";
    REQUIRE(evt.actor == "dave");
    REQUIRE(evt.action_description == "created connector");
}

// ============================================================================
// V17 Phase 06 W11: Change Highlights
// ============================================================================
TEST_CASE("P06-W11 ChangeHighlightDetectedEvent defaults", "[p06-w11][events]") { markamp::core::events::ChangeHighlightDetectedEvent evt; REQUIRE(evt.changes_since_last_visit == 0); REQUIRE(evt.last_visitor.empty()); }
TEST_CASE("P06-W11 ChangeHighlightDismissedEvent defaults", "[p06-w11][events]") { markamp::core::events::ChangeHighlightDismissedEvent evt; REQUIRE(evt.user_id.empty()); REQUIRE(evt.dismissed_count == 0); }
TEST_CASE("P06-W11 detect_change_highlights workbench", "[p06-w11][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.detect_change_highlights(5, "alice"); }
TEST_CASE("P06-W11 dismiss_change_highlights workbench", "[p06-w11][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.dismiss_change_highlights("alice", 5); }
TEST_CASE("P06-W11 detect then dismiss", "[p06-w11][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.detect_change_highlights(3, "bob"); wb.dismiss_change_highlights("bob", 3); }
TEST_CASE("P06-W11 event fields populated", "[p06-w11][events]") { markamp::core::events::ChangeHighlightDetectedEvent evt; evt.changes_since_last_visit = 10; evt.last_visitor = "carol"; REQUIRE(evt.changes_since_last_visit == 10); REQUIRE(evt.last_visitor == "carol"); }

// ============================================================================
// V17 Phase 06 W12: Invites & Session Entry
// ============================================================================
TEST_CASE("P06-W12 SessionInviteSentEvent defaults", "[p06-w12][events]") { markamp::core::events::SessionInviteSentEvent evt; REQUIRE(evt.invitee_email.empty()); REQUIRE(evt.session_id.empty()); }
TEST_CASE("P06-W12 SessionJoinedViaInviteEvent defaults", "[p06-w12][events]") { markamp::core::events::SessionJoinedViaInviteEvent evt; REQUIRE(evt.user_id.empty()); REQUIRE(evt.invite_code.empty()); }
TEST_CASE("P06-W12 send_session_invite workbench", "[p06-w12][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.send_session_invite("alice@co.com", "sess_1"); }
TEST_CASE("P06-W12 join_session_via_invite workbench", "[p06-w12][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.join_session_via_invite("user_1", "INV_ABC"); }
TEST_CASE("P06-W12 invite then join", "[p06-w12][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.send_session_invite("bob@co.com", "sess_2"); wb.join_session_via_invite("user_2", "INV_XYZ"); }
TEST_CASE("P06-W12 event fields populated", "[p06-w12][events]") { markamp::core::events::SessionInviteSentEvent evt; evt.invitee_email = "t@t.com"; evt.session_id = "s1"; REQUIRE(evt.invitee_email == "t@t.com"); REQUIRE(evt.session_id == "s1"); }

// ============================================================================
// V17 Phase 06 W13: Collaborator Panels
// ============================================================================
TEST_CASE("P06-W13 CollaboratorPanelOpenedEvent defaults", "[p06-w13][events]") { markamp::core::events::CollaboratorPanelOpenedEvent evt; REQUIRE(evt.panel_type.empty()); REQUIRE(evt.active_users == 0); }
TEST_CASE("P06-W13 CollaboratorPanelActionEvent defaults", "[p06-w13][events]") { markamp::core::events::CollaboratorPanelActionEvent evt; REQUIRE(evt.action_type.empty()); REQUIRE(evt.target_user.empty()); }
TEST_CASE("P06-W13 open_collaborator_panel workbench", "[p06-w13][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.open_collaborator_panel("awareness", 5); }
TEST_CASE("P06-W13 take_collaborator_panel_action workbench", "[p06-w13][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.take_collaborator_panel_action("mute", "user_3"); }
TEST_CASE("P06-W13 open then action", "[p06-w13][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.open_collaborator_panel("moderation", 3); wb.take_collaborator_panel_action("kick", "user_4"); }
TEST_CASE("P06-W13 event fields populated", "[p06-w13][events]") { markamp::core::events::CollaboratorPanelOpenedEvent evt; evt.panel_type = "facilitation"; evt.active_users = 8; REQUIRE(evt.panel_type == "facilitation"); REQUIRE(evt.active_users == 8); }

// ============================================================================
// V17 Phase 06 W14: Co-Editing Text
// ============================================================================
TEST_CASE("P06-W14 CoEditingStartedEvent defaults", "[p06-w14][events]") { markamp::core::events::CoEditingStartedEvent evt; REQUIRE(evt.object_id.empty()); REQUIRE(evt.editors_count == 0); }
TEST_CASE("P06-W14 CoEditingCursorMovedEvent defaults", "[p06-w14][events]") { markamp::core::events::CoEditingCursorMovedEvent evt; REQUIRE(evt.user_id.empty()); REQUIRE(evt.cursor_position == 0); }
TEST_CASE("P06-W14 start_co_editing workbench", "[p06-w14][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.start_co_editing("text_1", 3); }
TEST_CASE("P06-W14 move_co_editing_cursor workbench", "[p06-w14][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.move_co_editing_cursor("user_1", 42); }
TEST_CASE("P06-W14 start then move cursor", "[p06-w14][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.start_co_editing("note_1", 2); wb.move_co_editing_cursor("user_2", 100); }
TEST_CASE("P06-W14 event fields populated", "[p06-w14][events]") { markamp::core::events::CoEditingStartedEvent evt; evt.object_id = "tb_5"; evt.editors_count = 4; REQUIRE(evt.object_id == "tb_5"); REQUIRE(evt.editors_count == 4); }

// ============================================================================
// V17 Phase 06 W15: Workshop Templates
// ============================================================================
TEST_CASE("P06-W15 WorkshopTemplateAppliedEvent defaults", "[p06-w15][events]") { markamp::core::events::WorkshopTemplateAppliedEvent evt; REQUIRE(evt.template_name.empty()); REQUIRE(evt.objects_created == 0); }
TEST_CASE("P06-W15 WorkshopTemplateSavedEvent defaults", "[p06-w15][events]") { markamp::core::events::WorkshopTemplateSavedEvent evt; REQUIRE(evt.template_name.empty()); REQUIRE(evt.author.empty()); }
TEST_CASE("P06-W15 apply_workshop_template workbench", "[p06-w15][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_workshop_template("retro_board", 12); }
TEST_CASE("P06-W15 save_workshop_template workbench", "[p06-w15][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.save_workshop_template("sprint_review", "alice"); }
TEST_CASE("P06-W15 apply then save", "[p06-w15][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_workshop_template("brainstorm", 20); wb.save_workshop_template("brainstorm_v2", "bob"); }
TEST_CASE("P06-W15 event fields populated", "[p06-w15][events]") { markamp::core::events::WorkshopTemplateAppliedEvent evt; evt.template_name = "standup"; evt.objects_created = 6; REQUIRE(evt.template_name == "standup"); REQUIRE(evt.objects_created == 6); }

// ============================================================================
// V17 Phase 06 W16: Async Review
// ============================================================================
TEST_CASE("P06-W16 AsyncReviewRequestedEvent defaults", "[p06-w16][events]") { markamp::core::events::AsyncReviewRequestedEvent evt; REQUIRE(evt.reviewer_id.empty()); REQUIRE(evt.board_id.empty()); }
TEST_CASE("P06-W16 AsyncReviewCompletedEvent defaults", "[p06-w16][events]") { markamp::core::events::AsyncReviewCompletedEvent evt; REQUIRE(evt.reviewer_id.empty()); REQUIRE(evt.approved == false); }
TEST_CASE("P06-W16 request_async_review workbench", "[p06-w16][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.request_async_review("alice", "board_1"); }
TEST_CASE("P06-W16 complete_async_review workbench", "[p06-w16][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.complete_async_review("alice", true); }
TEST_CASE("P06-W16 request then complete", "[p06-w16][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.request_async_review("bob", "board_2"); wb.complete_async_review("bob", false); }
TEST_CASE("P06-W16 event fields populated", "[p06-w16][events]") { markamp::core::events::AsyncReviewCompletedEvent evt; evt.reviewer_id = "r_1"; evt.approved = true; REQUIRE(evt.reviewer_id == "r_1"); REQUIRE(evt.approved == true); }

// ============================================================================
// V17 Phase 06 W17: Moderation & Recovery
// ============================================================================
TEST_CASE("P06-W17 UserModeratedEvent defaults", "[p06-w17][events]") { markamp::core::events::UserModeratedEvent evt; REQUIRE(evt.target_user.empty()); REQUIRE(evt.moderation_action.empty()); }
TEST_CASE("P06-W17 BoardStateRecoveredEvent defaults", "[p06-w17][events]") { markamp::core::events::BoardStateRecoveredEvent evt; REQUIRE(evt.recovery_point.empty()); REQUIRE(evt.objects_restored == 0); }
TEST_CASE("P06-W17 moderate_user workbench", "[p06-w17][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.moderate_user("troll_1", "mute"); }
TEST_CASE("P06-W17 recover_board_state workbench", "[p06-w17][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.recover_board_state("snap_001", 15); }
TEST_CASE("P06-W17 moderate then recover", "[p06-w17][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.moderate_user("spam_user", "remove"); wb.recover_board_state("snap_002", 30); }
TEST_CASE("P06-W17 event fields populated", "[p06-w17][events]") { markamp::core::events::BoardStateRecoveredEvent evt; evt.recovery_point = "rp_1"; evt.objects_restored = 50; REQUIRE(evt.recovery_point == "rp_1"); REQUIRE(evt.objects_restored == 50); }

// ============================================================================
// V17 Phase 06 W18: Multiplayer Performance
// ============================================================================
TEST_CASE("P06-W18 PresenceThrottledEvent defaults", "[p06-w18][events]") { markamp::core::events::PresenceThrottledEvent evt; REQUIRE(evt.active_connections == 0); REQUIRE(evt.throttle_interval_ms == 0); }
TEST_CASE("P06-W18 BatchUpdateSentEvent defaults", "[p06-w18][events]") { markamp::core::events::BatchUpdateSentEvent evt; REQUIRE(evt.operations_batched == 0); REQUIRE(evt.batch_size_bytes == 0); }
TEST_CASE("P06-W18 throttle_presence workbench", "[p06-w18][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.throttle_presence(50, 200); }
TEST_CASE("P06-W18 send_batch_update workbench", "[p06-w18][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.send_batch_update(10, 4096); }
TEST_CASE("P06-W18 throttle then batch", "[p06-w18][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.throttle_presence(100, 500); wb.send_batch_update(25, 8192); }
TEST_CASE("P06-W18 event fields populated", "[p06-w18][events]") { markamp::core::events::PresenceThrottledEvent evt; evt.active_connections = 75; evt.throttle_interval_ms = 300; REQUIRE(evt.active_connections == 75); REQUIRE(evt.throttle_interval_ms == 300); }

// ============================================================================
// V17 Phase 06 W19: Collaboration Analytics
// ============================================================================
TEST_CASE("P06-W19 CollabMetricsSnapshotEvent defaults", "[p06-w19][events]") { markamp::core::events::CollabMetricsSnapshotEvent evt; REQUIRE(evt.total_edits == 0); REQUIRE(evt.unique_contributors == 0); }
TEST_CASE("P06-W19 EngagementScoreCalculatedEvent defaults", "[p06-w19][events]") { markamp::core::events::EngagementScoreCalculatedEvent evt; REQUIRE(evt.engagement_score == 0.0); REQUIRE(evt.session_id.empty()); }
TEST_CASE("P06-W19 snapshot_collab_metrics workbench", "[p06-w19][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.snapshot_collab_metrics(200, 8); }
TEST_CASE("P06-W19 calculate_engagement_score workbench", "[p06-w19][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.calculate_engagement_score(0.85, "sess_1"); }
TEST_CASE("P06-W19 snapshot then score", "[p06-w19][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.snapshot_collab_metrics(500, 12); wb.calculate_engagement_score(0.92, "sess_2"); }
TEST_CASE("P06-W19 event fields populated", "[p06-w19][events]") { markamp::core::events::EngagementScoreCalculatedEvent evt; evt.engagement_score = 0.75; evt.session_id = "s_99"; REQUIRE(evt.engagement_score == 0.75); REQUIRE(evt.session_id == "s_99"); }

// ============================================================================
// V17 Phase 06 W20: Collaboration Coverage
// ============================================================================
TEST_CASE("P06-W20 CollabTestSuiteRunEvent defaults", "[p06-w20][events]") { markamp::core::events::CollabTestSuiteRunEvent evt; REQUIRE(evt.tests_run == 0); REQUIRE(evt.tests_passed == 0); }
TEST_CASE("P06-W20 CollabRegressionDetectedEvent defaults", "[p06-w20][events]") { markamp::core::events::CollabRegressionDetectedEvent evt; REQUIRE(evt.test_name.empty()); REQUIRE(evt.failure_detail.empty()); }
TEST_CASE("P06-W20 run_collab_test_suite workbench", "[p06-w20][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.run_collab_test_suite(50, 48); }
TEST_CASE("P06-W20 report_collab_regression workbench", "[p06-w20][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.report_collab_regression("test_presence", "cursor flicker"); }
TEST_CASE("P06-W20 test then report regression", "[p06-w20][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.run_collab_test_suite(100, 97); wb.report_collab_regression("test_sync", "missed update"); }
TEST_CASE("P06-W20 event fields populated", "[p06-w20][events]") { markamp::core::events::CollabTestSuiteRunEvent evt; evt.tests_run = 200; evt.tests_passed = 195; REQUIRE(evt.tests_run == 200); REQUIRE(evt.tests_passed == 195); }

// ============================================================================
// V17 Phase 07 W01: Minimap Navigation
// ============================================================================
TEST_CASE("P07-W01 MinimapViewportChangedEvent defaults", "[p07-w01][events]") { markamp::core::events::MinimapViewportChangedEvent evt; REQUIRE(evt.viewport_x == 0.0); REQUIRE(evt.viewport_y == 0.0); }
TEST_CASE("P07-W01 MinimapVisibilityToggledEvent defaults", "[p07-w01][events]") { markamp::core::events::MinimapVisibilityToggledEvent evt; REQUIRE(evt.visible == false); REQUIRE(evt.toggle_source.empty()); }
TEST_CASE("P07-W01 change_minimap_viewport workbench", "[p07-w01][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.change_minimap_viewport(100.0, 200.0); }
TEST_CASE("P07-W01 toggle_minimap_visibility workbench", "[p07-w01][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.toggle_minimap_visibility(true, "menu"); }
TEST_CASE("P07-W01 viewport then toggle", "[p07-w01][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.change_minimap_viewport(50.0, 75.0); wb.toggle_minimap_visibility(false, "shortcut"); }
TEST_CASE("P07-W01 event fields populated", "[p07-w01][events]") { markamp::core::events::MinimapViewportChangedEvent evt; evt.viewport_x = 42.5; evt.viewport_y = 99.9; REQUIRE(evt.viewport_x == 42.5); REQUIRE(evt.viewport_y == 99.9); }

// ============================================================================
// V17 Phase 07 W02: Outline Navigation
// ============================================================================
TEST_CASE("P07-W02 OutlineNodeSelectedEvent defaults", "[p07-w02][events]") { markamp::core::events::OutlineNodeSelectedEvent evt; REQUIRE(evt.node_id.empty()); REQUIRE(evt.node_type.empty()); }
TEST_CASE("P07-W02 OutlineTreeRefreshedEvent defaults", "[p07-w02][events]") { markamp::core::events::OutlineTreeRefreshedEvent evt; REQUIRE(evt.total_nodes == 0); REQUIRE(evt.depth_levels == 0); }
TEST_CASE("P07-W02 select_outline_node workbench", "[p07-w02][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.select_outline_node("node_1", "section"); }
TEST_CASE("P07-W02 refresh_outline_tree workbench", "[p07-w02][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.refresh_outline_tree(50, 4); }
TEST_CASE("P07-W02 select then refresh", "[p07-w02][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.select_outline_node("node_2", "frame"); wb.refresh_outline_tree(100, 5); }
TEST_CASE("P07-W02 event fields populated", "[p07-w02][events]") { markamp::core::events::OutlineNodeSelectedEvent evt; evt.node_id = "n_5"; evt.node_type = "group"; REQUIRE(evt.node_id == "n_5"); REQUIRE(evt.node_type == "group"); }

// ============================================================================
// V17 Phase 07 W03: Search & Find
// ============================================================================
TEST_CASE("P07-W03 BoardSearchExecutedEvent defaults", "[p07-w03][events]") { markamp::core::events::BoardSearchExecutedEvent evt; REQUIRE(evt.query.empty()); REQUIRE(evt.results_found == 0); }
TEST_CASE("P07-W03 BoardSearchResultNavigatedEvent defaults", "[p07-w03][events]") { markamp::core::events::BoardSearchResultNavigatedEvent evt; REQUIRE(evt.object_id.empty()); REQUIRE(evt.result_index == 0); }
TEST_CASE("P07-W03 execute_board_search workbench", "[p07-w03][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.execute_board_search("todo", 5); }
TEST_CASE("P07-W03 navigate_search_result workbench", "[p07-w03][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.navigate_search_result("obj_1", 0); }
TEST_CASE("P07-W03 search then navigate", "[p07-w03][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.execute_board_search("bug", 3); wb.navigate_search_result("obj_2", 1); }
TEST_CASE("P07-W03 event fields populated", "[p07-w03][events]") { markamp::core::events::BoardSearchExecutedEvent evt; evt.query = "design"; evt.results_found = 10; REQUIRE(evt.query == "design"); REQUIRE(evt.results_found == 10); }

// ============================================================================
// V17 Phase 07 W04: Tagging & Filters
// ============================================================================
TEST_CASE("P07-W04 TagFilterAppliedEvent defaults", "[p07-w04][events]") { markamp::core::events::TagFilterAppliedEvent evt; REQUIRE(evt.tag_name.empty()); REQUIRE(evt.matching_objects == 0); }
TEST_CASE("P07-W04 TagFilterClearedEvent defaults", "[p07-w04][events]") { markamp::core::events::TagFilterClearedEvent evt; REQUIRE(evt.filters_removed == 0); REQUIRE(evt.total_objects_visible == 0); }
TEST_CASE("P07-W04 apply_tag_filter workbench", "[p07-w04][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_tag_filter("urgent", 5); }
TEST_CASE("P07-W04 clear_tag_filter workbench", "[p07-w04][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.clear_tag_filter(2, 50); }
TEST_CASE("P07-W04 filter then clear", "[p07-w04][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_tag_filter("review", 10); wb.clear_tag_filter(1, 100); }
TEST_CASE("P07-W04 event fields populated", "[p07-w04][events]") { markamp::core::events::TagFilterAppliedEvent evt; evt.tag_name = "done"; evt.matching_objects = 20; REQUIRE(evt.tag_name == "done"); REQUIRE(evt.matching_objects == 20); }

// ============================================================================
// V17 Phase 07 W05: Sections & Landmarks
// ============================================================================
TEST_CASE("P07-W05 SectionLandmarkCreatedEvent defaults", "[p07-w05][events]") { markamp::core::events::SectionLandmarkCreatedEvent evt; REQUIRE(evt.section_name.empty()); REQUIRE(evt.section_id.empty()); }
TEST_CASE("P07-W05 LandmarkNavigatedEvent defaults", "[p07-w05][events]") { markamp::core::events::LandmarkNavigatedEvent evt; REQUIRE(evt.landmark_id.empty()); REQUIRE(evt.target_zoom == 1.0); }
TEST_CASE("P07-W05 create_section_landmark workbench", "[p07-w05][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.create_section_landmark("Design Sprint", "sec_1"); }
TEST_CASE("P07-W05 navigate_to_landmark workbench", "[p07-w05][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.navigate_to_landmark("sec_1", 0.5); }
TEST_CASE("P07-W05 create then navigate", "[p07-w05][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.create_section_landmark("Retro", "sec_2"); wb.navigate_to_landmark("sec_2", 1.0); }
TEST_CASE("P07-W05 event fields populated", "[p07-w05][events]") { markamp::core::events::SectionLandmarkCreatedEvent evt; evt.section_name = "Ideas"; evt.section_id = "s_9"; REQUIRE(evt.section_name == "Ideas"); REQUIRE(evt.section_id == "s_9"); }

// ============================================================================
// V17 Phase 07 W06: Large Board Streaming
// ============================================================================
TEST_CASE("P07-W06 TileLoadedEvent defaults", "[p07-w06][events]") { markamp::core::events::TileLoadedEvent evt; REQUIRE(evt.tile_x == 0); REQUIRE(evt.tile_y == 0); }
TEST_CASE("P07-W06 StreamingLodChangedEvent defaults", "[p07-w06][events]") { markamp::core::events::StreamingLodChangedEvent evt; REQUIRE(evt.lod_level == 0); REQUIRE(evt.visible_objects == 0); }
TEST_CASE("P07-W06 load_tile workbench", "[p07-w06][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.load_tile(3, 5); }
TEST_CASE("P07-W06 change_streaming_lod workbench", "[p07-w06][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.change_streaming_lod(2, 200); }
TEST_CASE("P07-W06 load then lod", "[p07-w06][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.load_tile(1, 1); wb.change_streaming_lod(3, 500); }
TEST_CASE("P07-W06 event fields populated", "[p07-w06][events]") { markamp::core::events::TileLoadedEvent evt; evt.tile_x = 10; evt.tile_y = 20; REQUIRE(evt.tile_x == 10); REQUIRE(evt.tile_y == 20); }

// ============================================================================
// V17 Phase 07 W07: Breadcrumbs & History
// ============================================================================
TEST_CASE("P07-W07 BreadcrumbPushedEvent defaults", "[p07-w07][events]") { markamp::core::events::BreadcrumbPushedEvent evt; REQUIRE(evt.location_label.empty()); REQUIRE(evt.stack_depth == 0); }
TEST_CASE("P07-W07 BreadcrumbNavigatedBackEvent defaults", "[p07-w07][events]") { markamp::core::events::BreadcrumbNavigatedBackEvent evt; REQUIRE(evt.steps_back == 0); REQUIRE(evt.destination_label.empty()); }
TEST_CASE("P07-W07 push_breadcrumb workbench", "[p07-w07][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.push_breadcrumb("Design Zone", 1); }
TEST_CASE("P07-W07 navigate_breadcrumb_back workbench", "[p07-w07][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.navigate_breadcrumb_back(2, "Home"); }
TEST_CASE("P07-W07 push then back", "[p07-w07][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.push_breadcrumb("Ideas", 1); wb.push_breadcrumb("Sub-Ideas", 2); wb.navigate_breadcrumb_back(1, "Ideas"); }
TEST_CASE("P07-W07 event fields populated", "[p07-w07][events]") { markamp::core::events::BreadcrumbPushedEvent evt; evt.location_label = "Tasks"; evt.stack_depth = 3; REQUIRE(evt.location_label == "Tasks"); REQUIRE(evt.stack_depth == 3); }

// ============================================================================
// V17 Phase 07 W08: Zoom Presets
// ============================================================================
TEST_CASE("P07-W08 ZoomPresetAppliedEvent defaults", "[p07-w08][events]") { markamp::core::events::ZoomPresetAppliedEvent evt; REQUIRE(evt.preset_name.empty()); REQUIRE(evt.zoom_level == 1.0); }
TEST_CASE("P07-W08 ZoomPresetSavedEvent defaults", "[p07-w08][events]") { markamp::core::events::ZoomPresetSavedEvent evt; REQUIRE(evt.preset_name.empty()); REQUIRE(evt.zoom_level == 1.0); }
TEST_CASE("P07-W08 apply_zoom_preset workbench", "[p07-w08][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_zoom_preset("fit_all", 0.25); }
TEST_CASE("P07-W08 save_zoom_preset workbench", "[p07-w08][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.save_zoom_preset("my_zoom", 1.5); }
TEST_CASE("P07-W08 apply then save", "[p07-w08][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_zoom_preset("100%", 1.0); wb.save_zoom_preset("detail", 2.0); }
TEST_CASE("P07-W08 event fields populated", "[p07-w08][events]") { markamp::core::events::ZoomPresetAppliedEvent evt; evt.preset_name = "50%"; evt.zoom_level = 0.5; REQUIRE(evt.preset_name == "50%"); REQUIRE(evt.zoom_level == 0.5); }

// ============================================================================
// V17 Phase 07 W09: Board Bookmarks
// ============================================================================
TEST_CASE("P07-W09 BoardBookmarkCreatedEvent defaults", "[p07-w09][events]") { markamp::core::events::BoardBookmarkCreatedEvent evt; REQUIRE(evt.bookmark_name.empty()); REQUIRE(evt.bookmark_id.empty()); }
TEST_CASE("P07-W09 BoardBookmarkNavigatedEvent defaults", "[p07-w09][events]") { markamp::core::events::BoardBookmarkNavigatedEvent evt; REQUIRE(evt.bookmark_id.empty()); REQUIRE(evt.viewport_zoom == 1.0); }
TEST_CASE("P07-W09 create_board_bookmark workbench", "[p07-w09][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.create_board_bookmark("Key Meeting Area", "bm_1"); }
TEST_CASE("P07-W09 navigate_board_bookmark workbench", "[p07-w09][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.navigate_board_bookmark("bm_1", 0.75); }
TEST_CASE("P07-W09 create then navigate", "[p07-w09][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.create_board_bookmark("Sprint Board", "bm_2"); wb.navigate_board_bookmark("bm_2", 1.25); }
TEST_CASE("P07-W09 event fields populated", "[p07-w09][events]") { markamp::core::events::BoardBookmarkCreatedEvent evt; evt.bookmark_name = "Overview"; evt.bookmark_id = "b_99"; REQUIRE(evt.bookmark_name == "Overview"); REQUIRE(evt.bookmark_id == "b_99"); }

// ============================================================================
// V17 Phase 07 W10: Metadata Driven Navigation
// ============================================================================
TEST_CASE("P07-W10 MetadataFilterAppliedEvent defaults", "[p07-w10][events]") { markamp::core::events::MetadataFilterAppliedEvent evt; REQUIRE(evt.filter_key.empty()); REQUIRE(evt.filter_value.empty()); }
TEST_CASE("P07-W10 MetadataNavJumpEvent defaults", "[p07-w10][events]") { markamp::core::events::MetadataNavJumpEvent evt; REQUIRE(evt.target_object_id.empty()); REQUIRE(evt.metadata_key.empty()); }
TEST_CASE("P07-W10 apply_metadata_filter workbench", "[p07-w10][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_metadata_filter("owner", "alice"); }
TEST_CASE("P07-W10 jump_to_metadata_nav workbench", "[p07-w10][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.jump_to_metadata_nav("obj_5", "status"); }
TEST_CASE("P07-W10 filter then jump", "[p07-w10][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_metadata_filter("status", "done"); wb.jump_to_metadata_nav("obj_9", "type"); }
TEST_CASE("P07-W10 event fields populated", "[p07-w10][events]") { markamp::core::events::MetadataFilterAppliedEvent evt; evt.filter_key = "type"; evt.filter_value = "sticky_note"; REQUIRE(evt.filter_key == "type"); REQUIRE(evt.filter_value == "sticky_note"); }

// ============================================================================
// V17 Phase 07 W11: Cross Board Traversal
// ============================================================================
TEST_CASE("P07-W11 CrossBoardLinkFollowedEvent defaults", "[p07-w11][events]") { markamp::core::events::CrossBoardLinkFollowedEvent evt; REQUIRE(evt.source_board_id.empty()); REQUIRE(evt.target_board_id.empty()); }
TEST_CASE("P07-W11 CrossBoardBackNavigatedEvent defaults", "[p07-w11][events]") { markamp::core::events::CrossBoardBackNavigatedEvent evt; REQUIRE(evt.returning_to_board_id.empty()); REQUIRE(evt.boards_traversed == 0); }
TEST_CASE("P07-W11 follow_cross_board_link workbench", "[p07-w11][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.follow_cross_board_link("board_a", "board_b"); }
TEST_CASE("P07-W11 navigate_cross_board_back workbench", "[p07-w11][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.navigate_cross_board_back("board_a", 2); }
TEST_CASE("P07-W11 follow then back", "[p07-w11][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.follow_cross_board_link("b1", "b2"); wb.navigate_cross_board_back("b1", 1); }
TEST_CASE("P07-W11 event fields populated", "[p07-w11][events]") { markamp::core::events::CrossBoardLinkFollowedEvent evt; evt.source_board_id = "src"; evt.target_board_id = "tgt"; REQUIRE(evt.source_board_id == "src"); REQUIRE(evt.target_board_id == "tgt"); }

// ============================================================================
// V17 Phase 07 W12: Selection Sync
// ============================================================================
TEST_CASE("P07-W12 SelectionSyncedToPanelEvent defaults", "[p07-w12][events]") { markamp::core::events::SelectionSyncedToPanelEvent evt; REQUIRE(evt.panel_id.empty()); REQUIRE(evt.synced_objects == 0); }
TEST_CASE("P07-W12 SelectionSyncToggledEvent defaults", "[p07-w12][events]") { markamp::core::events::SelectionSyncToggledEvent evt; REQUIRE(evt.sync_enabled == false); REQUIRE(evt.panel_id.empty()); }
TEST_CASE("P07-W12 sync_selection_to_panel workbench", "[p07-w12][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.sync_selection_to_panel("props", 3); }
TEST_CASE("P07-W12 toggle_selection_sync workbench", "[p07-w12][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.toggle_selection_sync(true, "outline"); }
TEST_CASE("P07-W12 sync then toggle", "[p07-w12][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.sync_selection_to_panel("tags", 5); wb.toggle_selection_sync(false, "tags"); }
TEST_CASE("P07-W12 event fields populated", "[p07-w12][events]") { markamp::core::events::SelectionSyncedToPanelEvent evt; evt.panel_id = "p1"; evt.synced_objects = 10; REQUIRE(evt.panel_id == "p1"); REQUIRE(evt.synced_objects == 10); }

// ============================================================================
// V17 Phase 07 W13: Saved Views
// ============================================================================
TEST_CASE("P07-W13 NamedViewSavedEvent defaults", "[p07-w13][events]") { markamp::core::events::NamedViewSavedEvent evt; REQUIRE(evt.view_name.empty()); REQUIRE(evt.view_id.empty()); }
TEST_CASE("P07-W13 NamedViewRestoredEvent defaults", "[p07-w13][events]") { markamp::core::events::NamedViewRestoredEvent evt; REQUIRE(evt.view_id.empty()); REQUIRE(evt.restored_zoom == 1.0); }
TEST_CASE("P07-W13 save_named_view workbench", "[p07-w13][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.save_named_view("Sprint Overview", "v_1"); }
TEST_CASE("P07-W13 restore_named_view workbench", "[p07-w13][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.restore_named_view("v_1", 0.5); }
TEST_CASE("P07-W13 save then restore", "[p07-w13][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.save_named_view("Detail View", "v_2"); wb.restore_named_view("v_2", 2.0); }
TEST_CASE("P07-W13 event fields populated", "[p07-w13][events]") { markamp::core::events::NamedViewSavedEvent evt; evt.view_name = "Retro"; evt.view_id = "v_3"; REQUIRE(evt.view_name == "Retro"); REQUIRE(evt.view_id == "v_3"); }

// ============================================================================
// V17 Phase 07 W14: Presentation Navigation
// ============================================================================
TEST_CASE("P07-W14 PresentationSlideAdvancedEvent defaults", "[p07-w14][events]") { markamp::core::events::PresentationSlideAdvancedEvent evt; REQUIRE(evt.slide_index == 0); REQUIRE(evt.total_slides == 0); }
TEST_CASE("P07-W14 PresentationModeToggledEvent defaults", "[p07-w14][events]") { markamp::core::events::PresentationModeToggledEvent evt; REQUIRE(evt.presentation_active == false); REQUIRE(evt.presenter_id.empty()); }
TEST_CASE("P07-W14 advance_presentation_slide workbench", "[p07-w14][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.advance_presentation_slide(2, 10); }
TEST_CASE("P07-W14 toggle_presentation_mode workbench", "[p07-w14][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.toggle_presentation_mode(true, "alice"); }
TEST_CASE("P07-W14 toggle then advance", "[p07-w14][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.toggle_presentation_mode(true, "bob"); wb.advance_presentation_slide(1, 5); }
TEST_CASE("P07-W14 event fields populated", "[p07-w14][events]") { markamp::core::events::PresentationSlideAdvancedEvent evt; evt.slide_index = 4; evt.total_slides = 8; REQUIRE(evt.slide_index == 4); REQUIRE(evt.total_slides == 8); }

// ============================================================================
// V17 Phase 07 W15: Semantic Navigation
// ============================================================================
TEST_CASE("P07-W15 SemanticClusterNavigatedEvent defaults", "[p07-w15][events]") { markamp::core::events::SemanticClusterNavigatedEvent evt; REQUIRE(evt.cluster_label.empty()); REQUIRE(evt.objects_in_cluster == 0); }
TEST_CASE("P07-W15 SemanticGroupingRecalculatedEvent defaults", "[p07-w15][events]") { markamp::core::events::SemanticGroupingRecalculatedEvent evt; REQUIRE(evt.clusters_found == 0); REQUIRE(evt.ungrouped_objects == 0); }
TEST_CASE("P07-W15 navigate_semantic_cluster workbench", "[p07-w15][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.navigate_semantic_cluster("Design Ideas", 12); }
TEST_CASE("P07-W15 recalculate_semantic_grouping workbench", "[p07-w15][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.recalculate_semantic_grouping(5, 3); }
TEST_CASE("P07-W15 navigate then recalculate", "[p07-w15][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.navigate_semantic_cluster("Bugs", 8); wb.recalculate_semantic_grouping(6, 2); }
TEST_CASE("P07-W15 event fields populated", "[p07-w15][events]") { markamp::core::events::SemanticClusterNavigatedEvent evt; evt.cluster_label = "Tasks"; evt.objects_in_cluster = 20; REQUIRE(evt.cluster_label == "Tasks"); REQUIRE(evt.objects_in_cluster == 20); }

// ============================================================================
// V17 Phase 07 W16: Quick Action Navigation
// ============================================================================
TEST_CASE("P07-W16 QuickActionInvokedEvent defaults", "[p07-w16][events]") { markamp::core::events::QuickActionInvokedEvent evt; REQUIRE(evt.action_name.empty()); REQUIRE(evt.action_source.empty()); }
TEST_CASE("P07-W16 QuickActionResultSelectedEvent defaults", "[p07-w16][events]") { markamp::core::events::QuickActionResultSelectedEvent evt; REQUIRE(evt.result_id.empty()); REQUIRE(evt.result_rank == 0); }
TEST_CASE("P07-W16 invoke_quick_action workbench", "[p07-w16][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.invoke_quick_action("goto_section", "palette"); }
TEST_CASE("P07-W16 select_quick_action_result workbench", "[p07-w16][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.select_quick_action_result("res_1", 0); }
TEST_CASE("P07-W16 invoke then select", "[p07-w16][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.invoke_quick_action("find_object", "shortcut"); wb.select_quick_action_result("res_3", 2); }
TEST_CASE("P07-W16 event fields populated", "[p07-w16][events]") { markamp::core::events::QuickActionInvokedEvent evt; evt.action_name = "jump"; evt.action_source = "contextmenu"; REQUIRE(evt.action_name == "jump"); REQUIRE(evt.action_source == "contextmenu"); }

// ============================================================================
// V17 Phase 07 W17: Discoverability
// ============================================================================
TEST_CASE("P07-W17 FeatureHintShownEvent defaults", "[p07-w17][events]") { markamp::core::events::FeatureHintShownEvent evt; REQUIRE(evt.hint_id.empty()); REQUIRE(evt.feature_area.empty()); }
TEST_CASE("P07-W17 FeatureHintDismissedEvent defaults", "[p07-w17][events]") { markamp::core::events::FeatureHintDismissedEvent evt; REQUIRE(evt.hint_id.empty()); REQUIRE(evt.dont_show_again == false); }
TEST_CASE("P07-W17 show_feature_hint workbench", "[p07-w17][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.show_feature_hint("hint_minimap", "navigation"); }
TEST_CASE("P07-W17 dismiss_feature_hint workbench", "[p07-w17][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.dismiss_feature_hint("hint_minimap", true); }
TEST_CASE("P07-W17 show then dismiss", "[p07-w17][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.show_feature_hint("hint_zoom", "editing"); wb.dismiss_feature_hint("hint_zoom", false); }
TEST_CASE("P07-W17 event fields populated", "[p07-w17][events]") { markamp::core::events::FeatureHintShownEvent evt; evt.hint_id = "h1"; evt.feature_area = "collaboration"; REQUIRE(evt.hint_id == "h1"); REQUIRE(evt.feature_area == "collaboration"); }

// ============================================================================
// V17 Phase 07 W18: Responsive Layouts
// ============================================================================
TEST_CASE("P07-W18 LayoutBreakpointChangedEvent defaults", "[p07-w18][events]") { markamp::core::events::LayoutBreakpointChangedEvent evt; REQUIRE(evt.breakpoint_name.empty()); REQUIRE(evt.window_width == 0); }
TEST_CASE("P07-W18 NavPanelRepositionedEvent defaults", "[p07-w18][events]") { markamp::core::events::NavPanelRepositionedEvent evt; REQUIRE(evt.panel_position.empty()); REQUIRE(evt.trigger.empty()); }
TEST_CASE("P07-W18 change_layout_breakpoint workbench", "[p07-w18][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.change_layout_breakpoint("compact", 800); }
TEST_CASE("P07-W18 reposition_nav_panel workbench", "[p07-w18][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.reposition_nav_panel("bottom", "auto"); }
TEST_CASE("P07-W18 breakpoint then reposition", "[p07-w18][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.change_layout_breakpoint("wide", 1920); wb.reposition_nav_panel("left", "manual"); }
TEST_CASE("P07-W18 event fields populated", "[p07-w18][events]") { markamp::core::events::LayoutBreakpointChangedEvent evt; evt.breakpoint_name = "medium"; evt.window_width = 1200; REQUIRE(evt.breakpoint_name == "medium"); REQUIRE(evt.window_width == 1200); }

// ============================================================================
// V17 Phase 07 W19: Wayfinding Telemetry
// ============================================================================
TEST_CASE("P07-W19 NavConfusionDetectedEvent defaults", "[p07-w19][events]") { markamp::core::events::NavConfusionDetectedEvent evt; REQUIRE(evt.rapid_pans == 0); REQUIRE(evt.zoom_reversals == 0); }
TEST_CASE("P07-W19 WayfindingMetricRecordedEvent defaults", "[p07-w19][events]") { markamp::core::events::WayfindingMetricRecordedEvent evt; REQUIRE(evt.metric_name.empty()); REQUIRE(evt.metric_value == 0.0); }
TEST_CASE("P07-W19 detect_nav_confusion workbench", "[p07-w19][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.detect_nav_confusion(5, 3); }
TEST_CASE("P07-W19 record_wayfinding_metric workbench", "[p07-w19][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.record_wayfinding_metric("time_to_find", 4.5); }
TEST_CASE("P07-W19 confusion then metric", "[p07-w19][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.detect_nav_confusion(10, 7); wb.record_wayfinding_metric("lost_rate", 0.15); }
TEST_CASE("P07-W19 event fields populated", "[p07-w19][events]") { markamp::core::events::NavConfusionDetectedEvent evt; evt.rapid_pans = 20; evt.zoom_reversals = 8; REQUIRE(evt.rapid_pans == 20); REQUIRE(evt.zoom_reversals == 8); }

// ============================================================================
// V17 Phase 07 W20: Navigation Coverage
// ============================================================================
TEST_CASE("P07-W20 NavTestSuiteRunEvent defaults", "[p07-w20][events]") { markamp::core::events::NavTestSuiteRunEvent evt; REQUIRE(evt.tests_run == 0); REQUIRE(evt.tests_passed == 0); }
TEST_CASE("P07-W20 NavRegressionDetectedEvent defaults", "[p07-w20][events]") { markamp::core::events::NavRegressionDetectedEvent evt; REQUIRE(evt.test_name.empty()); REQUIRE(evt.failure_detail.empty()); }
TEST_CASE("P07-W20 run_nav_test_suite workbench", "[p07-w20][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.run_nav_test_suite(50, 48); }
TEST_CASE("P07-W20 report_nav_regression workbench", "[p07-w20][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.report_nav_regression("test_minimap", "viewport drift"); }
TEST_CASE("P07-W20 test then report regression", "[p07-w20][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.run_nav_test_suite(100, 97); wb.report_nav_regression("test_search", "stale results"); }
TEST_CASE("P07-W20 event fields populated", "[p07-w20][events]") { markamp::core::events::NavTestSuiteRunEvent evt; evt.tests_run = 200; evt.tests_passed = 195; REQUIRE(evt.tests_run == 200); REQUIRE(evt.tests_passed == 195); }

// ============================================================================
// V18 Phase 08 W01: Board Templates
// ============================================================================
TEST_CASE("P08-W01 BoardTemplateAppliedEvent defaults", "[p08-w01][events]") { markamp::core::events::BoardTemplateAppliedEvent evt; REQUIRE(evt.template_name.empty()); REQUIRE(evt.template_category.empty()); }
TEST_CASE("P08-W01 BoardTemplateSavedEvent defaults", "[p08-w01][events]") { markamp::core::events::BoardTemplateSavedEvent evt; REQUIRE(evt.template_name.empty()); REQUIRE(evt.objects_in_template == 0); }
TEST_CASE("P08-W01 apply_board_template workbench", "[p08-w01][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_board_template("Sprint Planning", "sprint"); }
TEST_CASE("P08-W01 save_board_template workbench", "[p08-w01][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.save_board_template("My Retro", 15); }
TEST_CASE("P08-W01 apply then save", "[p08-w01][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_board_template("Brainstorm", "brainstorm"); wb.save_board_template("Custom Board", 20); }
TEST_CASE("P08-W01 event fields populated", "[p08-w01][events]") { markamp::core::events::BoardTemplateAppliedEvent evt; evt.template_name = "Kanban"; evt.template_category = "agile"; REQUIRE(evt.template_name == "Kanban"); REQUIRE(evt.template_category == "agile"); }

// ============================================================================
// V18 Phase 08 W02: Object Templates
// ============================================================================
TEST_CASE("P08-W02 ObjectTemplateInsertedEvent defaults", "[p08-w02][events]") { markamp::core::events::ObjectTemplateInsertedEvent evt; REQUIRE(evt.template_id.empty()); REQUIRE(evt.objects_inserted == 0); }
TEST_CASE("P08-W02 ObjectTemplateRegisteredEvent defaults", "[p08-w02][events]") { markamp::core::events::ObjectTemplateRegisteredEvent evt; REQUIRE(evt.template_name.empty()); REQUIRE(evt.template_type.empty()); }
TEST_CASE("P08-W02 insert_object_template workbench", "[p08-w02][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.insert_object_template("tpl_1", 5); }
TEST_CASE("P08-W02 register_object_template workbench", "[p08-w02][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.register_object_template("Design Card", "cluster"); }
TEST_CASE("P08-W02 register then insert", "[p08-w02][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.register_object_template("Sprint Card", "module"); wb.insert_object_template("tpl_2", 3); }
TEST_CASE("P08-W02 event fields populated", "[p08-w02][events]") { markamp::core::events::ObjectTemplateInsertedEvent evt; evt.template_id = "t_5"; evt.objects_inserted = 8; REQUIRE(evt.template_id == "t_5"); REQUIRE(evt.objects_inserted == 8); }

// ============================================================================
// V18 Phase 08 W03: Advanced Style Presets
// ============================================================================
TEST_CASE("P08-W03 BoardStylePresetAppliedEvent defaults", "[p08-w03][events]") { markamp::core::events::BoardStylePresetAppliedEvent evt; REQUIRE(evt.preset_name.empty()); REQUIRE(evt.objects_affected == 0); }
TEST_CASE("P08-W03 StylePresetCreatedEvent defaults", "[p08-w03][events]") { markamp::core::events::StylePresetCreatedEvent evt; REQUIRE(evt.preset_name.empty()); REQUIRE(evt.preset_scope.empty()); }
TEST_CASE("P08-W03 apply_style_preset workbench", "[p08-w03][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_style_preset("Dark Theme", 10); }
TEST_CASE("P08-W03 create_style_preset workbench", "[p08-w03][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.create_style_preset("Brand Colors", "team"); }
TEST_CASE("P08-W03 create then apply", "[p08-w03][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.create_style_preset("Minimal", "global"); wb.apply_style_preset("Minimal", 25); }
TEST_CASE("P08-W03 event fields populated", "[p08-w03][events]") { markamp::core::events::BoardStylePresetAppliedEvent evt; evt.preset_name = "Neon"; evt.objects_affected = 15; REQUIRE(evt.preset_name == "Neon"); REQUIRE(evt.objects_affected == 15); }

// ============================================================================
// V18 Phase 08 W04: Automation Rules
// ============================================================================
TEST_CASE("P08-W04 AutomationRuleTriggeredEvent defaults", "[p08-w04][events]") { markamp::core::events::AutomationRuleTriggeredEvent evt; REQUIRE(evt.rule_name.empty()); REQUIRE(evt.actions_executed == 0); }
TEST_CASE("P08-W04 AutomationRuleCreatedEvent defaults", "[p08-w04][events]") { markamp::core::events::AutomationRuleCreatedEvent evt; REQUIRE(evt.rule_name.empty()); REQUIRE(evt.trigger_type.empty()); }
TEST_CASE("P08-W04 trigger_automation_rule workbench", "[p08-w04][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.trigger_automation_rule("auto_tag", 3); }
TEST_CASE("P08-W04 create_automation_rule workbench", "[p08-w04][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.create_automation_rule("color_by_status", "on_tag"); }
TEST_CASE("P08-W04 create then trigger", "[p08-w04][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.create_automation_rule("auto_align", "on_create"); wb.trigger_automation_rule("auto_align", 5); }
TEST_CASE("P08-W04 event fields populated", "[p08-w04][events]") { markamp::core::events::AutomationRuleCreatedEvent evt; evt.rule_name = "snap_grid"; evt.trigger_type = "on_move"; REQUIRE(evt.rule_name == "snap_grid"); REQUIRE(evt.trigger_type == "on_move"); }

// ============================================================================
// V18 Phase 08 W05: AI Board Generation
// ============================================================================
TEST_CASE("P08-W05 AIBoardGenerationRequestedEvent defaults", "[p08-w05][events]") { markamp::core::events::AIBoardGenerationRequestedEvent evt; REQUIRE(evt.prompt_text.empty()); REQUIRE(evt.board_type.empty()); }
TEST_CASE("P08-W05 AIBoardGenerationCompletedEvent defaults", "[p08-w05][events]") { markamp::core::events::AIBoardGenerationCompletedEvent evt; REQUIRE(evt.objects_generated == 0); REQUIRE(evt.generation_time_ms == 0.0); }
TEST_CASE("P08-W05 request_ai_board_generation workbench", "[p08-w05][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.request_ai_board_generation("Create sprint board", "kanban"); }
TEST_CASE("P08-W05 complete_ai_board_generation workbench", "[p08-w05][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.complete_ai_board_generation(20, 1500.0); }
TEST_CASE("P08-W05 request then complete", "[p08-w05][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.request_ai_board_generation("Mind map for project", "mind_map"); wb.complete_ai_board_generation(15, 800.0); }
TEST_CASE("P08-W05 event fields populated", "[p08-w05][events]") { markamp::core::events::AIBoardGenerationRequestedEvent evt; evt.prompt_text = "Design flow"; evt.board_type = "flowchart"; REQUIRE(evt.prompt_text == "Design flow"); REQUIRE(evt.board_type == "flowchart"); }

// ============================================================================
// V18 Phase 08 W06: AI Cleanup & Refinement
// ============================================================================
TEST_CASE("P08-W06 AICleanupRequestedEvent defaults", "[p08-w06][events]") { markamp::core::events::AICleanupRequestedEvent evt; REQUIRE(evt.cleanup_type.empty()); REQUIRE(evt.objects_in_scope == 0); }
TEST_CASE("P08-W06 AICleanupCompletedEvent defaults", "[p08-w06][events]") { markamp::core::events::AICleanupCompletedEvent evt; REQUIRE(evt.objects_modified == 0); REQUIRE(evt.objects_removed == 0); }
TEST_CASE("P08-W06 request_ai_cleanup workbench", "[p08-w06][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.request_ai_cleanup("normalize", 50); }
TEST_CASE("P08-W06 complete_ai_cleanup workbench", "[p08-w06][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.complete_ai_cleanup(30, 5); }
TEST_CASE("P08-W06 request then complete", "[p08-w06][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.request_ai_cleanup("align", 40); wb.complete_ai_cleanup(35, 2); }
TEST_CASE("P08-W06 event fields populated", "[p08-w06][events]") { markamp::core::events::AICleanupRequestedEvent evt; evt.cleanup_type = "restructure"; evt.objects_in_scope = 100; REQUIRE(evt.cleanup_type == "restructure"); REQUIRE(evt.objects_in_scope == 100); }

// ============================================================================
// V18 Phase 08 W07: AI Summaries & Explanations
// ============================================================================
TEST_CASE("P08-W07 AISummaryRequestedEvent defaults", "[p08-w07][events]") { markamp::core::events::AISummaryRequestedEvent evt; REQUIRE(evt.scope.empty()); REQUIRE(evt.objects_summarized == 0); }
TEST_CASE("P08-W07 AISummaryGeneratedEvent defaults", "[p08-w07][events]") { markamp::core::events::AISummaryGeneratedEvent evt; REQUIRE(evt.word_count == 0); REQUIRE(evt.output_format.empty()); }
TEST_CASE("P08-W07 request_ai_summary workbench", "[p08-w07][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.request_ai_summary("board", 80); }
TEST_CASE("P08-W07 generate_ai_summary workbench", "[p08-w07][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.generate_ai_summary(250, "bullets"); }
TEST_CASE("P08-W07 request then generate", "[p08-w07][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.request_ai_summary("selection", 10); wb.generate_ai_summary(50, "text"); }
TEST_CASE("P08-W07 event fields populated", "[p08-w07][events]") { markamp::core::events::AISummaryGeneratedEvent evt; evt.word_count = 500; evt.output_format = "outline"; REQUIRE(evt.word_count == 500); REQUIRE(evt.output_format == "outline"); }

// ============================================================================
// V18 Phase 08 W08: AI Tags & Links
// ============================================================================
TEST_CASE("P08-W08 AITagSuggestionsGeneratedEvent defaults", "[p08-w08][events]") { markamp::core::events::AITagSuggestionsGeneratedEvent evt; REQUIRE(evt.tags_suggested == 0); REQUIRE(evt.objects_analyzed == 0); }
TEST_CASE("P08-W08 AILinkSuggestionsGeneratedEvent defaults", "[p08-w08][events]") { markamp::core::events::AILinkSuggestionsGeneratedEvent evt; REQUIRE(evt.links_suggested == 0); REQUIRE(evt.cross_board_links == 0); }
TEST_CASE("P08-W08 generate_ai_tag_suggestions workbench", "[p08-w08][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.generate_ai_tag_suggestions(10, 50); }
TEST_CASE("P08-W08 generate_ai_link_suggestions workbench", "[p08-w08][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.generate_ai_link_suggestions(5, 2); }
TEST_CASE("P08-W08 tags then links", "[p08-w08][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.generate_ai_tag_suggestions(15, 75); wb.generate_ai_link_suggestions(8, 3); }
TEST_CASE("P08-W08 event fields populated", "[p08-w08][events]") { markamp::core::events::AITagSuggestionsGeneratedEvent evt; evt.tags_suggested = 20; evt.objects_analyzed = 100; REQUIRE(evt.tags_suggested == 20); REQUIRE(evt.objects_analyzed == 100); }

// ============================================================================
// V18 Phase 08 W09: Batch Operations
// ============================================================================
TEST_CASE("P08-W09 BatchOperationExecutedEvent defaults", "[p08-w09][events]") { markamp::core::events::BatchOperationExecutedEvent evt; REQUIRE(evt.operation_type.empty()); REQUIRE(evt.objects_affected == 0); }
TEST_CASE("P08-W09 BatchOperationUndoneEvent defaults", "[p08-w09][events]") { markamp::core::events::BatchOperationUndoneEvent evt; REQUIRE(evt.operation_type.empty()); REQUIRE(evt.objects_restored == 0); }
TEST_CASE("P08-W09 execute_batch_operation workbench", "[p08-w09][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.execute_batch_operation("style", 20); }
TEST_CASE("P08-W09 undo_batch_operation workbench", "[p08-w09][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.undo_batch_operation("move", 15); }
TEST_CASE("P08-W09 execute then undo", "[p08-w09][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.execute_batch_operation("tag", 30); wb.undo_batch_operation("tag", 30); }
TEST_CASE("P08-W09 event fields populated", "[p08-w09][events]") { markamp::core::events::BatchOperationExecutedEvent evt; evt.operation_type = "delete"; evt.objects_affected = 10; REQUIRE(evt.operation_type == "delete"); REQUIRE(evt.objects_affected == 10); }

// ============================================================================
// V18 Phase 08 W10: Markdown To Canvas
// ============================================================================
TEST_CASE("P08-W10 MarkdownToCanvasStartedEvent defaults", "[p08-w10][events]") { markamp::core::events::MarkdownToCanvasStartedEvent evt; REQUIRE(evt.source_file.empty()); REQUIRE(evt.lines_to_parse == 0); }
TEST_CASE("P08-W10 MarkdownToCanvasCompletedEvent defaults", "[p08-w10][events]") { markamp::core::events::MarkdownToCanvasCompletedEvent evt; REQUIRE(evt.objects_created == 0); REQUIRE(evt.connectors_created == 0); }
TEST_CASE("P08-W10 start_markdown_to_canvas workbench", "[p08-w10][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.start_markdown_to_canvas("README.md", 200); }
TEST_CASE("P08-W10 complete_markdown_to_canvas workbench", "[p08-w10][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.complete_markdown_to_canvas(15, 8); }
TEST_CASE("P08-W10 start then complete", "[p08-w10][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.start_markdown_to_canvas("docs.md", 500); wb.complete_markdown_to_canvas(40, 20); }
TEST_CASE("P08-W10 event fields populated", "[p08-w10][events]") { markamp::core::events::MarkdownToCanvasStartedEvent evt; evt.source_file = "notes.md"; evt.lines_to_parse = 100; REQUIRE(evt.source_file == "notes.md"); REQUIRE(evt.lines_to_parse == 100); }

// ============================================================================
// V18 Phase 08 W11: CSV & Database Imports
// ============================================================================
TEST_CASE("P08-W11 CsvImportStartedEvent defaults", "[p08-w11][events]") { markamp::core::events::CsvImportStartedEvent evt; REQUIRE(evt.source_file.empty()); REQUIRE(evt.rows_to_import == 0); }
TEST_CASE("P08-W11 CsvImportCompletedEvent defaults", "[p08-w11][events]") { markamp::core::events::CsvImportCompletedEvent evt; REQUIRE(evt.objects_created == 0); REQUIRE(evt.columns_mapped == 0); }
TEST_CASE("P08-W11 start_csv_import workbench", "[p08-w11][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.start_csv_import("data.csv", 100); }
TEST_CASE("P08-W11 complete_csv_import workbench", "[p08-w11][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.complete_csv_import(50, 5); }
TEST_CASE("P08-W11 start then complete", "[p08-w11][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.start_csv_import("users.csv", 500); wb.complete_csv_import(200, 8); }
TEST_CASE("P08-W11 event fields populated", "[p08-w11][events]") { markamp::core::events::CsvImportStartedEvent evt; evt.source_file = "items.csv"; evt.rows_to_import = 250; REQUIRE(evt.source_file == "items.csv"); REQUIRE(evt.rows_to_import == 250); }

// ============================================================================
// V18 Phase 08 W12: Export Workflows
// ============================================================================
TEST_CASE("P08-W12 BoardExportStartedEvent defaults", "[p08-w12][events]") { markamp::core::events::BoardExportStartedEvent evt; REQUIRE(evt.export_format.empty()); REQUIRE(evt.export_scope.empty()); }
TEST_CASE("P08-W12 BoardExportCompletedEvent defaults", "[p08-w12][events]") { markamp::core::events::BoardExportCompletedEvent evt; REQUIRE(evt.output_path.empty()); REQUIRE(evt.objects_exported == 0); }
TEST_CASE("P08-W12 start_board_export workbench", "[p08-w12][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.start_board_export("png", "full"); }
TEST_CASE("P08-W12 complete_board_export workbench", "[p08-w12][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.complete_board_export("/tmp/board.png", 80); }
TEST_CASE("P08-W12 start then complete", "[p08-w12][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.start_board_export("pdf", "selection"); wb.complete_board_export("/tmp/out.pdf", 30); }
TEST_CASE("P08-W12 event fields populated", "[p08-w12][events]") { markamp::core::events::BoardExportStartedEvent evt; evt.export_format = "svg"; evt.export_scope = "viewport"; REQUIRE(evt.export_format == "svg"); REQUIRE(evt.export_scope == "viewport"); }

// ============================================================================
// V18 Phase 08 W13: Reusable Components
// ============================================================================
TEST_CASE("P08-W13 ReusableComponentSavedEvent defaults", "[p08-w13][events]") { markamp::core::events::ReusableComponentSavedEvent evt; REQUIRE(evt.component_name.empty()); REQUIRE(evt.child_objects == 0); }
TEST_CASE("P08-W13 ReusableComponentInstantiatedEvent defaults", "[p08-w13][events]") { markamp::core::events::ReusableComponentInstantiatedEvent evt; REQUIRE(evt.component_id.empty()); REQUIRE(evt.instance_id.empty()); }
TEST_CASE("P08-W13 save_reusable_component workbench", "[p08-w13][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.save_reusable_component("User Card", 4); }
TEST_CASE("P08-W13 instantiate_reusable_component workbench", "[p08-w13][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.instantiate_reusable_component("cmp_1", "inst_1"); }
TEST_CASE("P08-W13 save then instantiate", "[p08-w13][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.save_reusable_component("Header", 3); wb.instantiate_reusable_component("cmp_2", "inst_2"); }
TEST_CASE("P08-W13 event fields populated", "[p08-w13][events]") { markamp::core::events::ReusableComponentSavedEvent evt; evt.component_name = "Footer"; evt.child_objects = 6; REQUIRE(evt.component_name == "Footer"); REQUIRE(evt.child_objects == 6); }

// ============================================================================
// V18 Phase 08 W14: Plugin Hooks
// ============================================================================
TEST_CASE("P08-W14 PluginHookRegisteredEvent defaults", "[p08-w14][events]") { markamp::core::events::PluginHookRegisteredEvent evt; REQUIRE(evt.hook_name.empty()); REQUIRE(evt.plugin_id.empty()); }
TEST_CASE("P08-W14 PluginHookInvokedEvent defaults", "[p08-w14][events]") { markamp::core::events::PluginHookInvokedEvent evt; REQUIRE(evt.hook_name.empty()); REQUIRE(evt.listeners_notified == 0); }
TEST_CASE("P08-W14 register_plugin_hook workbench", "[p08-w14][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.register_plugin_hook("on_create", "mermaid"); }
TEST_CASE("P08-W14 invoke_plugin_hook workbench", "[p08-w14][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.invoke_plugin_hook("on_create", 3); }
TEST_CASE("P08-W14 register then invoke", "[p08-w14][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.register_plugin_hook("on_delete", "linter"); wb.invoke_plugin_hook("on_delete", 1); }
TEST_CASE("P08-W14 event fields populated", "[p08-w14][events]") { markamp::core::events::PluginHookRegisteredEvent evt; evt.hook_name = "on_move"; evt.plugin_id = "snap_plugin"; REQUIRE(evt.hook_name == "on_move"); REQUIRE(evt.plugin_id == "snap_plugin"); }

// ============================================================================
// V18 Phase 08 W15: Command Macros
// ============================================================================
TEST_CASE("P08-W15 CommandMacroRecordedEvent defaults", "[p08-w15][events]") { markamp::core::events::CommandMacroRecordedEvent evt; REQUIRE(evt.macro_name.empty()); REQUIRE(evt.steps_recorded == 0); }
TEST_CASE("P08-W15 CommandMacroReplayedEvent defaults", "[p08-w15][events]") { markamp::core::events::CommandMacroReplayedEvent evt; REQUIRE(evt.macro_name.empty()); REQUIRE(evt.objects_affected == 0); }
TEST_CASE("P08-W15 record_command_macro workbench", "[p08-w15][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.record_command_macro("align_grid", 5); }
TEST_CASE("P08-W15 replay_command_macro workbench", "[p08-w15][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.replay_command_macro("align_grid", 20); }
TEST_CASE("P08-W15 record then replay", "[p08-w15][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.record_command_macro("color_all", 3); wb.replay_command_macro("color_all", 50); }
TEST_CASE("P08-W15 event fields populated", "[p08-w15][events]") { markamp::core::events::CommandMacroRecordedEvent evt; evt.macro_name = "resize"; evt.steps_recorded = 7; REQUIRE(evt.macro_name == "resize"); REQUIRE(evt.steps_recorded == 7); }

// ============================================================================
// V18 Phase 08 W16: Quick Insert
// ============================================================================
TEST_CASE("P08-W16 QuickInsertMenuOpenedEvent defaults", "[p08-w16][events]") { markamp::core::events::QuickInsertMenuOpenedEvent evt; REQUIRE(evt.trigger_source.empty()); REQUIRE(evt.items_available == 0); }
TEST_CASE("P08-W16 QuickInsertItemSelectedEvent defaults", "[p08-w16][events]") { markamp::core::events::QuickInsertItemSelectedEvent evt; REQUIRE(evt.item_type.empty()); REQUIRE(evt.search_rank == 0); }
TEST_CASE("P08-W16 open_quick_insert_menu workbench", "[p08-w16][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.open_quick_insert_menu("slash", 15); }
TEST_CASE("P08-W16 select_quick_insert_item workbench", "[p08-w16][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.select_quick_insert_item("sticky_note", 0); }
TEST_CASE("P08-W16 open then select", "[p08-w16][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.open_quick_insert_menu("toolbar", 20); wb.select_quick_insert_item("connector", 3); }
TEST_CASE("P08-W16 event fields populated", "[p08-w16][events]") { markamp::core::events::QuickInsertMenuOpenedEvent evt; evt.trigger_source = "shortcut"; evt.items_available = 25; REQUIRE(evt.trigger_source == "shortcut"); REQUIRE(evt.items_available == 25); }

// ============================================================================
// V18 Phase 08 W17: Smart Defaults
// ============================================================================
TEST_CASE("P08-W17 SmartDefaultAppliedEvent defaults", "[p08-w17][events]") { markamp::core::events::SmartDefaultAppliedEvent evt; REQUIRE(evt.default_type.empty()); REQUIRE(evt.context.empty()); }
TEST_CASE("P08-W17 SmartDefaultSuggestionShownEvent defaults", "[p08-w17][events]") { markamp::core::events::SmartDefaultSuggestionShownEvent evt; REQUIRE(evt.suggestions_count == 0); REQUIRE(evt.suggestion_accepted == false); }
TEST_CASE("P08-W17 apply_smart_default workbench", "[p08-w17][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_smart_default("color", "sticky_note"); }
TEST_CASE("P08-W17 show_smart_default_suggestion workbench", "[p08-w17][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.show_smart_default_suggestion(3, true); }
TEST_CASE("P08-W17 apply then suggest", "[p08-w17][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_smart_default("font", "text_box"); wb.show_smart_default_suggestion(5, false); }
TEST_CASE("P08-W17 event fields populated", "[p08-w17][events]") { markamp::core::events::SmartDefaultAppliedEvent evt; evt.default_type = "layout"; evt.context = "kanban"; REQUIRE(evt.default_type == "layout"); REQUIRE(evt.context == "kanban"); }

// ============================================================================
// V18 Phase 08 W18: Profile & Preference Sync
// ============================================================================
TEST_CASE("P08-W18 ProfileSyncedEvent defaults", "[p08-w18][events]") { markamp::core::events::ProfileSyncedEvent evt; REQUIRE(evt.profile_id.empty()); REQUIRE(evt.preferences_synced == 0); }
TEST_CASE("P08-W18 PreferenceConflictResolvedEvent defaults", "[p08-w18][events]") { markamp::core::events::PreferenceConflictResolvedEvent evt; REQUIRE(evt.preference_key.empty()); REQUIRE(evt.resolution.empty()); }
TEST_CASE("P08-W18 sync_profile workbench", "[p08-w18][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.sync_profile("user_1", 12); }
TEST_CASE("P08-W18 resolve_preference_conflict workbench", "[p08-w18][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.resolve_preference_conflict("theme", "local"); }
TEST_CASE("P08-W18 sync then resolve", "[p08-w18][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.sync_profile("user_2", 20); wb.resolve_preference_conflict("font_size", "remote"); }
TEST_CASE("P08-W18 event fields populated", "[p08-w18][events]") { markamp::core::events::ProfileSyncedEvent evt; evt.profile_id = "p_99"; evt.preferences_synced = 30; REQUIRE(evt.profile_id == "p_99"); REQUIRE(evt.preferences_synced == 30); }

// ============================================================================
// V18 Phase 08 W19: Onboarding Programs
// ============================================================================
TEST_CASE("P08-W19 CanvasOnboardingStepCompletedEvent defaults", "[p08-w19][events]") { markamp::core::events::CanvasOnboardingStepCompletedEvent evt; REQUIRE(evt.step_id.empty()); REQUIRE(evt.steps_remaining == 0); }
TEST_CASE("P08-W19 OnboardingProgramFinishedEvent defaults", "[p08-w19][events]") { markamp::core::events::OnboardingProgramFinishedEvent evt; REQUIRE(evt.program_id.empty()); REQUIRE(evt.total_steps_completed == 0); }
TEST_CASE("P08-W19 complete_onboarding_step workbench", "[p08-w19][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.complete_onboarding_step("step_1", 4); }
TEST_CASE("P08-W19 finish_onboarding_program workbench", "[p08-w19][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.finish_onboarding_program("canvas_intro", 5); }
TEST_CASE("P08-W19 step then finish", "[p08-w19][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.complete_onboarding_step("step_5", 0); wb.finish_onboarding_program("canvas_intro", 5); }
TEST_CASE("P08-W19 event fields populated", "[p08-w19][events]") { markamp::core::events::CanvasOnboardingStepCompletedEvent evt; evt.step_id = "s_3"; evt.steps_remaining = 2; REQUIRE(evt.step_id == "s_3"); REQUIRE(evt.steps_remaining == 2); }

// ============================================================================
// V18 Phase 08 W20: Agent Ready Scaffolds
// ============================================================================
TEST_CASE("P08-W20 CreationTestSuiteRunEvent defaults", "[p08-w20][events]") { markamp::core::events::CreationTestSuiteRunEvent evt; REQUIRE(evt.tests_run == 0); REQUIRE(evt.tests_passed == 0); }
TEST_CASE("P08-W20 CreationRegressionDetectedEvent defaults", "[p08-w20][events]") { markamp::core::events::CreationRegressionDetectedEvent evt; REQUIRE(evt.test_name.empty()); REQUIRE(evt.failure_detail.empty()); }
TEST_CASE("P08-W20 run_creation_test_suite workbench", "[p08-w20][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.run_creation_test_suite(60, 58); }
TEST_CASE("P08-W20 report_creation_regression workbench", "[p08-w20][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.report_creation_regression("test_template", "wrong layout"); }
TEST_CASE("P08-W20 test then report regression", "[p08-w20][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.run_creation_test_suite(100, 95); wb.report_creation_regression("test_ai_gen", "timeout"); }
TEST_CASE("P08-W20 event fields populated", "[p08-w20][events]") { markamp::core::events::CreationTestSuiteRunEvent evt; evt.tests_run = 200; evt.tests_passed = 198; REQUIRE(evt.tests_run == 200); REQUIRE(evt.tests_passed == 198); }

// ============================================================================
// V19 Phase 09 W01: Keyboard Only Canvas
// ============================================================================
TEST_CASE("P09-W01 KeyboardNavActionEvent defaults", "[p09-w01][events]") { markamp::core::events::KeyboardNavActionEvent evt; REQUIRE(evt.action.empty()); REQUIRE(evt.direction.empty()); }
TEST_CASE("P09-W01 KeyboardShortcutConflictEvent defaults", "[p09-w01][events]") { markamp::core::events::KeyboardShortcutConflictEvent evt; REQUIRE(evt.shortcut_key.empty()); REQUIRE(evt.conflicting_action.empty()); }
TEST_CASE("P09-W01 perform_keyboard_nav_action workbench", "[p09-w01][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.perform_keyboard_nav_action("move_focus", "right"); }
TEST_CASE("P09-W01 detect_keyboard_shortcut_conflict workbench", "[p09-w01][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.detect_keyboard_shortcut_conflict("Ctrl+S", "save_board"); }
TEST_CASE("P09-W01 nav then conflict", "[p09-w01][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.perform_keyboard_nav_action("select", "down"); wb.detect_keyboard_shortcut_conflict("Ctrl+Z", "undo"); }
TEST_CASE("P09-W01 event fields populated", "[p09-w01][events]") { markamp::core::events::KeyboardNavActionEvent evt; evt.action = "pan"; evt.direction = "left"; REQUIRE(evt.action == "pan"); REQUIRE(evt.direction == "left"); }

// ============================================================================
// V19 Phase 09 W02: Screen Reader Semantics
// ============================================================================
TEST_CASE("P09-W02 ScreenReaderAnnouncementEvent defaults", "[p09-w02][events]") { markamp::core::events::ScreenReaderAnnouncementEvent evt; REQUIRE(evt.announcement_text.empty()); REQUIRE(evt.priority.empty()); }
TEST_CASE("P09-W02 AccessibleLabelUpdatedEvent defaults", "[p09-w02][events]") { markamp::core::events::AccessibleLabelUpdatedEvent evt; REQUIRE(evt.object_id.empty()); REQUIRE(evt.new_label.empty()); }
TEST_CASE("P09-W02 queue_screen_reader_announcement workbench", "[p09-w02][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.queue_screen_reader_announcement("Object selected", "polite"); }
TEST_CASE("P09-W02 update_accessible_label workbench", "[p09-w02][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.update_accessible_label("obj_1", "Task Card"); }
TEST_CASE("P09-W02 announce then label", "[p09-w02][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.queue_screen_reader_announcement("Board loaded", "assertive"); wb.update_accessible_label("obj_2", "Header"); }
TEST_CASE("P09-W02 event fields populated", "[p09-w02][events]") { markamp::core::events::ScreenReaderAnnouncementEvent evt; evt.announcement_text = "Moved"; evt.priority = "assertive"; REQUIRE(evt.announcement_text == "Moved"); REQUIRE(evt.priority == "assertive"); }

// ============================================================================
// V19 Phase 09 W03: Contrast & Color Blindness
// ============================================================================
TEST_CASE("P09-W03 HighContrastModeToggledEvent defaults", "[p09-w03][events]") { markamp::core::events::HighContrastModeToggledEvent evt; REQUIRE(evt.high_contrast_enabled == false); REQUIRE(evt.contrast_profile.empty()); }
TEST_CASE("P09-W03 ColorAccessibilityCheckEvent defaults", "[p09-w03][events]") { markamp::core::events::ColorAccessibilityCheckEvent evt; REQUIRE(evt.elements_checked == 0); REQUIRE(evt.issues_found == 0); }
TEST_CASE("P09-W03 toggle_high_contrast_mode workbench", "[p09-w03][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.toggle_high_contrast_mode(true, "deuteranopia"); }
TEST_CASE("P09-W03 run_color_accessibility_check workbench", "[p09-w03][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.run_color_accessibility_check(100, 3); }
TEST_CASE("P09-W03 toggle then check", "[p09-w03][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.toggle_high_contrast_mode(true, "standard"); wb.run_color_accessibility_check(50, 1); }
TEST_CASE("P09-W03 event fields populated", "[p09-w03][events]") { markamp::core::events::HighContrastModeToggledEvent evt; evt.high_contrast_enabled = true; evt.contrast_profile = "protanopia"; REQUIRE(evt.high_contrast_enabled == true); REQUIRE(evt.contrast_profile == "protanopia"); }

// ============================================================================
// V19 Phase 09 W04: Focus & Announcements
// ============================================================================
TEST_CASE("P09-W04 FocusTransferredEvent defaults", "[p09-w04][events]") { markamp::core::events::FocusTransferredEvent evt; REQUIRE(evt.from_object_id.empty()); REQUIRE(evt.to_object_id.empty()); }
TEST_CASE("P09-W04 LiveRegionUpdateEvent defaults", "[p09-w04][events]") { markamp::core::events::LiveRegionUpdateEvent evt; REQUIRE(evt.region_id.empty()); REQUIRE(evt.update_text.empty()); }
TEST_CASE("P09-W04 transfer_focus workbench", "[p09-w04][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.transfer_focus("obj_1", "obj_2"); }
TEST_CASE("P09-W04 update_live_region workbench", "[p09-w04][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.update_live_region("status", "3 items selected"); }
TEST_CASE("P09-W04 focus then region", "[p09-w04][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.transfer_focus("a", "b"); wb.update_live_region("toolbar", "Tool changed"); }
TEST_CASE("P09-W04 event fields populated", "[p09-w04][events]") { markamp::core::events::FocusTransferredEvent evt; evt.from_object_id = "x"; evt.to_object_id = "y"; REQUIRE(evt.from_object_id == "x"); REQUIRE(evt.to_object_id == "y"); }

// ============================================================================
// V19 Phase 09 W05: Touch & Pen Parity
// ============================================================================
TEST_CASE("P09-W05 TouchGestureRecognizedEvent defaults", "[p09-w05][events]") { markamp::core::events::TouchGestureRecognizedEvent evt; REQUIRE(evt.gesture_type.empty()); REQUIRE(evt.touch_points == 0); }
TEST_CASE("P09-W05 PenPressureAppliedEvent defaults", "[p09-w05][events]") { markamp::core::events::PenPressureAppliedEvent evt; REQUIRE(evt.pressure_level == 0.0); REQUIRE(evt.pen_tool.empty()); }
TEST_CASE("P09-W05 recognize_touch_gesture workbench", "[p09-w05][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.recognize_touch_gesture("pinch", 2); }
TEST_CASE("P09-W05 apply_pen_pressure workbench", "[p09-w05][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_pen_pressure(0.75, "draw"); }
TEST_CASE("P09-W05 touch then pen", "[p09-w05][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.recognize_touch_gesture("rotate", 2); wb.apply_pen_pressure(0.5, "highlight"); }
TEST_CASE("P09-W05 event fields populated", "[p09-w05][events]") { markamp::core::events::TouchGestureRecognizedEvent evt; evt.gesture_type = "two_finger_pan"; evt.touch_points = 2; REQUIRE(evt.gesture_type == "two_finger_pan"); REQUIRE(evt.touch_points == 2); }

// ============================================================================
// V19 Phase 09 W06: Performance Budgets
// ============================================================================
TEST_CASE("P09-W06 PerfBudgetExceededEvent defaults", "[p09-w06][events]") { markamp::core::events::PerfBudgetExceededEvent evt; REQUIRE(evt.operation.empty()); REQUIRE(evt.elapsed_ms == 0.0); }
TEST_CASE("P09-W06 FrameRateMetricEvent defaults", "[p09-w06][events]") { markamp::core::events::FrameRateMetricEvent evt; REQUIRE(evt.fps == 0.0); REQUIRE(evt.objects_rendered == 0); }
TEST_CASE("P09-W06 report_perf_budget_exceeded workbench", "[p09-w06][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.report_perf_budget_exceeded("render", 32.5); }
TEST_CASE("P09-W06 record_frame_rate_metric workbench", "[p09-w06][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.record_frame_rate_metric(59.8, 500); }
TEST_CASE("P09-W06 budget then metric", "[p09-w06][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.report_perf_budget_exceeded("layout", 50.0); wb.record_frame_rate_metric(30.0, 1000); }
TEST_CASE("P09-W06 event fields populated", "[p09-w06][events]") { markamp::core::events::PerfBudgetExceededEvent evt; evt.operation = "save"; evt.elapsed_ms = 120.5; REQUIRE(evt.operation == "save"); REQUIRE(evt.elapsed_ms == 120.5); }

// ============================================================================
// V19 Phase 09 W07: Virtualization
// ============================================================================
TEST_CASE("P09-W07 ViewportCullingUpdatedEvent defaults", "[p09-w07][events]") { markamp::core::events::ViewportCullingUpdatedEvent evt; REQUIRE(evt.visible_objects == 0); REQUIRE(evt.total_objects == 0); }
TEST_CASE("P09-W07 TileCacheMetricEvent defaults", "[p09-w07][events]") { markamp::core::events::TileCacheMetricEvent evt; REQUIRE(evt.cache_hits == 0); REQUIRE(evt.cache_misses == 0); }
TEST_CASE("P09-W07 update_viewport_culling workbench", "[p09-w07][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.update_viewport_culling(50, 500); }
TEST_CASE("P09-W07 record_tile_cache_metric workbench", "[p09-w07][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.record_tile_cache_metric(90, 10); }
TEST_CASE("P09-W07 culling then cache", "[p09-w07][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.update_viewport_culling(100, 1000); wb.record_tile_cache_metric(80, 20); }
TEST_CASE("P09-W07 event fields populated", "[p09-w07][events]") { markamp::core::events::ViewportCullingUpdatedEvent evt; evt.visible_objects = 200; evt.total_objects = 2000; REQUIRE(evt.visible_objects == 200); REQUIRE(evt.total_objects == 2000); }

// ============================================================================
// V19 Phase 09 W08: Crash Recovery
// ============================================================================
TEST_CASE("P09-W08 CrashRecoveryCheckpointEvent defaults", "[p09-w08][events]") { markamp::core::events::CrashRecoveryCheckpointEvent evt; REQUIRE(evt.checkpoint_id.empty()); REQUIRE(evt.objects_saved == 0); }
TEST_CASE("P09-W08 CrashBoardStateRecoveredEvent defaults", "[p09-w08][events]") { markamp::core::events::CrashBoardStateRecoveredEvent evt; REQUIRE(evt.recovery_source.empty()); REQUIRE(evt.objects_recovered == 0); }
TEST_CASE("P09-W08 save_crash_recovery_checkpoint workbench", "[p09-w08][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.save_crash_recovery_checkpoint("cp_1", 200); }
TEST_CASE("P09-W08 recover_crashed_board_state workbench", "[p09-w08][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.recover_crashed_board_state("autosave", 180); }
TEST_CASE("P09-W08 checkpoint then recover", "[p09-w08][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.save_crash_recovery_checkpoint("cp_2", 150); wb.recover_crashed_board_state("checkpoint", 145); }
TEST_CASE("P09-W08 event fields populated", "[p09-w08][events]") { markamp::core::events::CrashRecoveryCheckpointEvent evt; evt.checkpoint_id = "c_5"; evt.objects_saved = 300; REQUIRE(evt.checkpoint_id == "c_5"); REQUIRE(evt.objects_saved == 300); }

// ============================================================================
// V19 Phase 09 W09: Data Integrity
// ============================================================================
TEST_CASE("P09-W09 BoardIntegrityCheckEvent defaults", "[p09-w09][events]") { markamp::core::events::BoardIntegrityCheckEvent evt; REQUIRE(evt.objects_validated == 0); REQUIRE(evt.corruption_found == 0); }
TEST_CASE("P09-W09 DataRepairAppliedEvent defaults", "[p09-w09][events]") { markamp::core::events::DataRepairAppliedEvent evt; REQUIRE(evt.repair_type.empty()); REQUIRE(evt.items_repaired == 0); }
TEST_CASE("P09-W09 run_board_integrity_check workbench", "[p09-w09][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.run_board_integrity_check(500, 2); }
TEST_CASE("P09-W09 apply_data_repair workbench", "[p09-w09][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_data_repair("orphan_cleanup", 3); }
TEST_CASE("P09-W09 check then repair", "[p09-w09][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.run_board_integrity_check(1000, 5); wb.apply_data_repair("ref_fix", 5); }
TEST_CASE("P09-W09 event fields populated", "[p09-w09][events]") { markamp::core::events::BoardIntegrityCheckEvent evt; evt.objects_validated = 800; evt.corruption_found = 1; REQUIRE(evt.objects_validated == 800); REQUIRE(evt.corruption_found == 1); }

// ============================================================================
// V19 Phase 09 W10: Accessibility Tooling
// ============================================================================
TEST_CASE("P09-W10 AccessibilityAuditRunEvent defaults", "[p09-w10][events]") { markamp::core::events::AccessibilityAuditRunEvent evt; REQUIRE(evt.elements_audited == 0); REQUIRE(evt.violations_found == 0); }
TEST_CASE("P09-W10 AccessibilityRegressionEvent defaults", "[p09-w10][events]") { markamp::core::events::AccessibilityRegressionEvent evt; REQUIRE(evt.test_name.empty()); REQUIRE(evt.violation_type.empty()); }
TEST_CASE("P09-W10 run_accessibility_audit workbench", "[p09-w10][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.run_accessibility_audit(200, 4); }
TEST_CASE("P09-W10 detect_accessibility_regression workbench", "[p09-w10][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.detect_accessibility_regression("test_labels", "missing_alt"); }
TEST_CASE("P09-W10 audit then regression", "[p09-w10][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.run_accessibility_audit(300, 2); wb.detect_accessibility_regression("test_focus", "trap"); }
TEST_CASE("P09-W10 event fields populated", "[p09-w10][events]") { markamp::core::events::AccessibilityAuditRunEvent evt; evt.elements_audited = 500; evt.violations_found = 0; REQUIRE(evt.elements_audited == 500); REQUIRE(evt.violations_found == 0); }

// ============================================================================
// V19 Phase 09 W11: Safe Degradation
// ============================================================================
TEST_CASE("P09-W11 SafeDegradationActivatedEvent defaults", "[p09-w11][events]") { markamp::core::events::SafeDegradationActivatedEvent evt; REQUIRE(evt.feature_name.empty()); REQUIRE(evt.fallback_reason.empty()); }
TEST_CASE("P09-W11 DegradedModeExitedEvent defaults", "[p09-w11][events]") { markamp::core::events::DegradedModeExitedEvent evt; REQUIRE(evt.feature_name.empty()); REQUIRE(evt.degraded_duration_ms == 0.0); }
TEST_CASE("P09-W11 activate_safe_degradation workbench", "[p09-w11][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.activate_safe_degradation("3d_render", "slow_gpu"); }
TEST_CASE("P09-W11 exit_degraded_mode workbench", "[p09-w11][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.exit_degraded_mode("3d_render", 5000.0); }
TEST_CASE("P09-W11 activate then exit", "[p09-w11][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.activate_safe_degradation("embed", "failed_embed"); wb.exit_degraded_mode("embed", 2000.0); }
TEST_CASE("P09-W11 event fields populated", "[p09-w11][events]") { markamp::core::events::SafeDegradationActivatedEvent evt; evt.feature_name = "plugin"; evt.fallback_reason = "plugin_crash"; REQUIRE(evt.feature_name == "plugin"); REQUIRE(evt.fallback_reason == "plugin_crash"); }

// ============================================================================
// V19 Phase 09 W12: Observability
// ============================================================================
TEST_CASE("P09-W12 TelemetrySpanRecordedEvent defaults", "[p09-w12][events]") { markamp::core::events::TelemetrySpanRecordedEvent evt; REQUIRE(evt.span_name.empty()); REQUIRE(evt.duration_ms == 0.0); }
TEST_CASE("P09-W12 ErrorTelemetryEmittedEvent defaults", "[p09-w12][events]") { markamp::core::events::ErrorTelemetryEmittedEvent evt; REQUIRE(evt.error_category.empty()); REQUIRE(evt.error_message.empty()); }
TEST_CASE("P09-W12 record_telemetry_span workbench", "[p09-w12][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.record_telemetry_span("render_frame", 16.5); }
TEST_CASE("P09-W12 emit_error_telemetry workbench", "[p09-w12][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.emit_error_telemetry("io", "file not found"); }
TEST_CASE("P09-W12 span then error", "[p09-w12][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.record_telemetry_span("save", 120.0); wb.emit_error_telemetry("network", "timeout"); }
TEST_CASE("P09-W12 event fields populated", "[p09-w12][events]") { markamp::core::events::TelemetrySpanRecordedEvent evt; evt.span_name = "load"; evt.duration_ms = 50.0; REQUIRE(evt.span_name == "load"); REQUIRE(evt.duration_ms == 50.0); }

// ============================================================================
// V19 Phase 09 W13: Feature Flags
// ============================================================================
TEST_CASE("P09-W13 FeatureFlagEvaluatedEvent defaults", "[p09-w13][events]") { markamp::core::events::FeatureFlagEvaluatedEvent evt; REQUIRE(evt.flag_name.empty()); REQUIRE(evt.flag_value == false); }
TEST_CASE("P09-W13 FeatureFlagOverrideEvent defaults", "[p09-w13][events]") { markamp::core::events::FeatureFlagOverrideEvent evt; REQUIRE(evt.flag_name.empty()); REQUIRE(evt.override_source.empty()); }
TEST_CASE("P09-W13 evaluate_feature_flag workbench", "[p09-w13][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.evaluate_feature_flag("dark_mode_v2", true); }
TEST_CASE("P09-W13 apply_feature_flag_override workbench", "[p09-w13][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_feature_flag_override("dark_mode_v2", "admin"); }
TEST_CASE("P09-W13 evaluate then override", "[p09-w13][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.evaluate_feature_flag("new_toolbar", false); wb.apply_feature_flag_override("new_toolbar", "experiment"); }
TEST_CASE("P09-W13 event fields populated", "[p09-w13][events]") { markamp::core::events::FeatureFlagEvaluatedEvent evt; evt.flag_name = "collab_v2"; evt.flag_value = true; REQUIRE(evt.flag_name == "collab_v2"); REQUIRE(evt.flag_value == true); }

// ============================================================================
// V19 Phase 09 W14: Snapshot & Harnesses
// ============================================================================
TEST_CASE("P09-W14 VisualSnapshotCapturedEvent defaults", "[p09-w14][events]") { markamp::core::events::VisualSnapshotCapturedEvent evt; REQUIRE(evt.snapshot_id.empty()); REQUIRE(evt.pixels_diffed == 0); }
TEST_CASE("P09-W14 SnapshotComparisonResultEvent defaults", "[p09-w14][events]") { markamp::core::events::SnapshotComparisonResultEvent evt; REQUIRE(evt.baseline_id.empty()); REQUIRE(evt.diff_percentage == 0.0); }
TEST_CASE("P09-W14 capture_visual_snapshot workbench", "[p09-w14][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.capture_visual_snapshot("snap_1", 100); }
TEST_CASE("P09-W14 compare_snapshot workbench", "[p09-w14][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.compare_snapshot("baseline_1", 0.5); }
TEST_CASE("P09-W14 capture then compare", "[p09-w14][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.capture_visual_snapshot("snap_2", 200); wb.compare_snapshot("baseline_2", 1.2); }
TEST_CASE("P09-W14 event fields populated", "[p09-w14][events]") { markamp::core::events::VisualSnapshotCapturedEvent evt; evt.snapshot_id = "s_5"; evt.pixels_diffed = 50; REQUIRE(evt.snapshot_id == "s_5"); REQUIRE(evt.pixels_diffed == 50); }

// ============================================================================
// V19 Phase 09 W15: Unit & Integration Gaps
// ============================================================================
TEST_CASE("P09-W15 TestCoverageGapEvent defaults", "[p09-w15][events]") { markamp::core::events::TestCoverageGapEvent evt; REQUIRE(evt.module_name.empty()); REQUIRE(evt.uncovered_lines == 0); }
TEST_CASE("P09-W15 IntegrationTestAddedEvent defaults", "[p09-w15][events]") { markamp::core::events::IntegrationTestAddedEvent evt; REQUIRE(evt.test_name.empty()); REQUIRE(evt.covered_module.empty()); }
TEST_CASE("P09-W15 identify_test_coverage_gap workbench", "[p09-w15][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.identify_test_coverage_gap("canvas_renderer", 45); }
TEST_CASE("P09-W15 add_integration_test workbench", "[p09-w15][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.add_integration_test("test_render_flow", "canvas_renderer"); }
TEST_CASE("P09-W15 gap then add test", "[p09-w15][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.identify_test_coverage_gap("board_serializer", 20); wb.add_integration_test("test_serialize", "board_serializer"); }
TEST_CASE("P09-W15 event fields populated", "[p09-w15][events]") { markamp::core::events::TestCoverageGapEvent evt; evt.module_name = "event_bus"; evt.uncovered_lines = 10; REQUIRE(evt.module_name == "event_bus"); REQUIRE(evt.uncovered_lines == 10); }

// ============================================================================
// V19 Phase 09 W16: End To End Determinism
// ============================================================================
TEST_CASE("P09-W16 E2EFlakinessDetectedEvent defaults", "[p09-w16][events]") { markamp::core::events::E2EFlakinessDetectedEvent evt; REQUIRE(evt.test_name.empty()); REQUIRE(evt.flaky_runs == 0); }
TEST_CASE("P09-W16 E2ETimingStabilizedEvent defaults", "[p09-w16][events]") { markamp::core::events::E2ETimingStabilizedEvent evt; REQUIRE(evt.test_name.empty()); REQUIRE(evt.variance_ms == 0.0); }
TEST_CASE("P09-W16 detect_e2e_flakiness workbench", "[p09-w16][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.detect_e2e_flakiness("test_open_board", 5); }
TEST_CASE("P09-W16 stabilize_e2e_timing workbench", "[p09-w16][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.stabilize_e2e_timing("test_open_board", 15.0); }
TEST_CASE("P09-W16 flaky then stabilize", "[p09-w16][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.detect_e2e_flakiness("test_save", 3); wb.stabilize_e2e_timing("test_save", 5.0); }
TEST_CASE("P09-W16 event fields populated", "[p09-w16][events]") { markamp::core::events::E2EFlakinessDetectedEvent evt; evt.test_name = "test_collab"; evt.flaky_runs = 8; REQUIRE(evt.test_name == "test_collab"); REQUIRE(evt.flaky_runs == 8); }

// ============================================================================
// V19 Phase 09 W17: Security & Privacy
// ============================================================================
TEST_CASE("P09-W17 SecurityAuditFindingEvent defaults", "[p09-w17][events]") { markamp::core::events::SecurityAuditFindingEvent evt; REQUIRE(evt.finding_type.empty()); REQUIRE(evt.severity.empty()); }
TEST_CASE("P09-W17 PrivacyDataScrubbedEvent defaults", "[p09-w17][events]") { markamp::core::events::PrivacyDataScrubbedEvent evt; REQUIRE(evt.fields_scrubbed == 0); REQUIRE(evt.scrub_scope.empty()); }
TEST_CASE("P09-W17 report_security_audit_finding workbench", "[p09-w17][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.report_security_audit_finding("xss", "high"); }
TEST_CASE("P09-W17 scrub_privacy_data workbench", "[p09-w17][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.scrub_privacy_data(12, "export"); }
TEST_CASE("P09-W17 finding then scrub", "[p09-w17][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.report_security_audit_finding("leak", "medium"); wb.scrub_privacy_data(5, "telemetry"); }
TEST_CASE("P09-W17 event fields populated", "[p09-w17][events]") { markamp::core::events::SecurityAuditFindingEvent evt; evt.finding_type = "injection"; evt.severity = "critical"; REQUIRE(evt.finding_type == "injection"); REQUIRE(evt.severity == "critical"); }

// ============================================================================
// V19 Phase 09 W18: Localization & IME
// ============================================================================
TEST_CASE("P09-W18 LocaleSwitchedEvent defaults", "[p09-w18][events]") { markamp::core::events::LocaleSwitchedEvent evt; REQUIRE(evt.from_locale.empty()); REQUIRE(evt.to_locale.empty()); }
TEST_CASE("P09-W18 IMECompositionEvent defaults", "[p09-w18][events]") { markamp::core::events::IMECompositionEvent evt; REQUIRE(evt.ime_state.empty()); REQUIRE(evt.composition_length == 0); }
TEST_CASE("P09-W18 switch_locale workbench", "[p09-w18][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.switch_locale("en_US", "ja_JP"); }
TEST_CASE("P09-W18 handle_ime_composition workbench", "[p09-w18][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.handle_ime_composition("start", 3); }
TEST_CASE("P09-W18 locale then ime", "[p09-w18][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.switch_locale("en_US", "zh_CN"); wb.handle_ime_composition("update", 5); }
TEST_CASE("P09-W18 event fields populated", "[p09-w18][events]") { markamp::core::events::LocaleSwitchedEvent evt; evt.from_locale = "de_DE"; evt.to_locale = "fr_FR"; REQUIRE(evt.from_locale == "de_DE"); REQUIRE(evt.to_locale == "fr_FR"); }

// ============================================================================
// V19 Phase 09 W19: Compliance Checklists
// ============================================================================
TEST_CASE("P09-W19 ComplianceCheckRunEvent defaults", "[p09-w19][events]") { markamp::core::events::ComplianceCheckRunEvent evt; REQUIRE(evt.checks_passed == 0); REQUIRE(evt.checks_failed == 0); }
TEST_CASE("P09-W19 ComplianceViolationFlaggedEvent defaults", "[p09-w19][events]") { markamp::core::events::ComplianceViolationFlaggedEvent evt; REQUIRE(evt.rule_id.empty()); REQUIRE(evt.violation_detail.empty()); }
TEST_CASE("P09-W19 run_compliance_check workbench", "[p09-w19][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.run_compliance_check(95, 5); }
TEST_CASE("P09-W19 flag_compliance_violation workbench", "[p09-w19][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.flag_compliance_violation("WCAG-2.1", "missing alt text"); }
TEST_CASE("P09-W19 check then flag", "[p09-w19][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.run_compliance_check(48, 2); wb.flag_compliance_violation("ARIA-1", "invalid role"); }
TEST_CASE("P09-W19 event fields populated", "[p09-w19][events]") { markamp::core::events::ComplianceCheckRunEvent evt; evt.checks_passed = 100; evt.checks_failed = 0; REQUIRE(evt.checks_passed == 100); REQUIRE(evt.checks_failed == 0); }

// ============================================================================
// V19 Phase 09 W20: Operational Dashboards
// ============================================================================
TEST_CASE("P09-W20 DashboardMetricPublishedEvent defaults", "[p09-w20][events]") { markamp::core::events::DashboardMetricPublishedEvent evt; REQUIRE(evt.metric_name.empty()); REQUIRE(evt.metric_value == 0.0); }
TEST_CASE("P09-W20 CanvasHealthCheckCompletedEvent defaults", "[p09-w20][events]") { markamp::core::events::CanvasHealthCheckCompletedEvent evt; REQUIRE(evt.healthy_systems == 0); REQUIRE(evt.degraded_systems == 0); }
TEST_CASE("P09-W20 publish_dashboard_metric workbench", "[p09-w20][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.publish_dashboard_metric("active_users", 42.0); }
TEST_CASE("P09-W20 complete_health_check workbench", "[p09-w20][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.complete_health_check(10, 1); }
TEST_CASE("P09-W20 metric then health", "[p09-w20][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.publish_dashboard_metric("fps", 60.0); wb.complete_health_check(12, 0); }
TEST_CASE("P09-W20 event fields populated", "[p09-w20][events]") { markamp::core::events::DashboardMetricPublishedEvent evt; evt.metric_name = "latency"; evt.metric_value = 15.3; REQUIRE(evt.metric_name == "latency"); REQUIRE(evt.metric_value == 15.3); }

// ============================================================================
// V20 Phase 10 W01: Parity Audit
// ============================================================================
TEST_CASE("P10-W01 ParityAuditItemCheckedEvent defaults", "[p10-w01][events]") { markamp::core::events::ParityAuditItemCheckedEvent evt; REQUIRE(evt.competitor_feature.empty()); REQUIRE(evt.parity_status.empty()); }
TEST_CASE("P10-W01 ParityGapIdentifiedEvent defaults", "[p10-w01][events]") { markamp::core::events::ParityGapIdentifiedEvent evt; REQUIRE(evt.feature_name.empty()); REQUIRE(evt.gap_severity.empty()); }
TEST_CASE("P10-W01 check_parity_audit_item workbench", "[p10-w01][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.check_parity_audit_item("drag_select", "matched"); }
TEST_CASE("P10-W01 identify_parity_gap workbench", "[p10-w01][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.identify_parity_gap("comment_threads", "important"); }
TEST_CASE("P10-W01 check then gap", "[p10-w01][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.check_parity_audit_item("zoom_fit", "partial"); wb.identify_parity_gap("zoom_fit", "nice_to_have"); }
TEST_CASE("P10-W01 event fields populated", "[p10-w01][events]") { markamp::core::events::ParityAuditItemCheckedEvent evt; evt.competitor_feature = "lasso"; evt.parity_status = "missing"; REQUIRE(evt.competitor_feature == "lasso"); REQUIRE(evt.parity_status == "missing"); }

// ============================================================================
// V20 Phase 10 W02: Control Polish
// ============================================================================
TEST_CASE("P10-W02 ControlDensityAdjustedEvent defaults", "[p10-w02][events]") { markamp::core::events::ControlDensityAdjustedEvent evt; REQUIRE(evt.control_group.empty()); REQUIRE(evt.density_level.empty()); }
TEST_CASE("P10-W02 AffordanceRefinementAppliedEvent defaults", "[p10-w02][events]") { markamp::core::events::AffordanceRefinementAppliedEvent evt; REQUIRE(evt.element_id.empty()); REQUIRE(evt.refinement_type.empty()); }
TEST_CASE("P10-W02 adjust_control_density workbench", "[p10-w02][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.adjust_control_density("toolbar", "compact"); }
TEST_CASE("P10-W02 apply_affordance_refinement workbench", "[p10-w02][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_affordance_refinement("btn_save", "tooltip"); }
TEST_CASE("P10-W02 density then affordance", "[p10-w02][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.adjust_control_density("sidebar", "spacious"); wb.apply_affordance_refinement("btn_undo", "icon"); }
TEST_CASE("P10-W02 event fields populated", "[p10-w02][events]") { markamp::core::events::ControlDensityAdjustedEvent evt; evt.control_group = "panel"; evt.density_level = "default"; REQUIRE(evt.control_group == "panel"); REQUIRE(evt.density_level == "default"); }

// ============================================================================
// V20 Phase 10 W03: Motion & Microinteraction
// ============================================================================
TEST_CASE("P10-W03 MicrointeractionTriggeredEvent defaults", "[p10-w03][events]") { markamp::core::events::MicrointeractionTriggeredEvent evt; REQUIRE(evt.interaction_name.empty()); REQUIRE(evt.duration_ms == 0.0); }
TEST_CASE("P10-W03 MotionPreferenceAppliedEvent defaults", "[p10-w03][events]") { markamp::core::events::MotionPreferenceAppliedEvent evt; REQUIRE(evt.preference.empty()); REQUIRE(evt.system_prefers_reduced == false); }
TEST_CASE("P10-W03 trigger_microinteraction workbench", "[p10-w03][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.trigger_microinteraction("bounce", 200.0); }
TEST_CASE("P10-W03 apply_motion_preference workbench", "[p10-w03][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_motion_preference("reduced", true); }
TEST_CASE("P10-W03 trigger then preference", "[p10-w03][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.trigger_microinteraction("fade", 150.0); wb.apply_motion_preference("full", false); }
TEST_CASE("P10-W03 event fields populated", "[p10-w03][events]") { markamp::core::events::MicrointeractionTriggeredEvent evt; evt.interaction_name = "slide"; evt.duration_ms = 300.0; REQUIRE(evt.interaction_name == "slide"); REQUIRE(evt.duration_ms == 300.0); }

// ============================================================================
// V20 Phase 10 W04: Platform Conventions
// ============================================================================
TEST_CASE("P10-W04 PlatformConventionAppliedEvent defaults", "[p10-w04][events]") { markamp::core::events::PlatformConventionAppliedEvent evt; REQUIRE(evt.platform.empty()); REQUIRE(evt.convention_type.empty()); }
TEST_CASE("P10-W04 PlatformParityCheckEvent defaults", "[p10-w04][events]") { markamp::core::events::PlatformParityCheckEvent evt; REQUIRE(evt.platform.empty()); REQUIRE(evt.conventions_matched == 0); }
TEST_CASE("P10-W04 apply_platform_convention workbench", "[p10-w04][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_platform_convention("macos", "shortcut"); }
TEST_CASE("P10-W04 run_platform_parity_check workbench", "[p10-w04][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.run_platform_parity_check("windows", 45); }
TEST_CASE("P10-W04 convention then check", "[p10-w04][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_platform_convention("linux", "menu"); wb.run_platform_parity_check("linux", 40); }
TEST_CASE("P10-W04 event fields populated", "[p10-w04][events]") { markamp::core::events::PlatformConventionAppliedEvent evt; evt.platform = "macos"; evt.convention_type = "scroll"; REQUIRE(evt.platform == "macos"); REQUIRE(evt.convention_type == "scroll"); }

// ============================================================================
// V20 Phase 10 W05: Settings Migration
// ============================================================================
TEST_CASE("P10-W05 SettingsMigrationStartedEvent defaults", "[p10-w05][events]") { markamp::core::events::SettingsMigrationStartedEvent evt; REQUIRE(evt.from_version.empty()); REQUIRE(evt.to_version.empty()); }
TEST_CASE("P10-W05 SettingsMigrationCompletedEvent defaults", "[p10-w05][events]") { markamp::core::events::SettingsMigrationCompletedEvent evt; REQUIRE(evt.settings_migrated == 0); REQUIRE(evt.settings_defaulted == 0); }
TEST_CASE("P10-W05 start_settings_migration workbench", "[p10-w05][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.start_settings_migration("2.20", "2.21"); }
TEST_CASE("P10-W05 complete_settings_migration workbench", "[p10-w05][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.complete_settings_migration(50, 3); }
TEST_CASE("P10-W05 start then complete", "[p10-w05][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.start_settings_migration("1.0", "2.0"); wb.complete_settings_migration(100, 10); }
TEST_CASE("P10-W05 event fields populated", "[p10-w05][events]") { markamp::core::events::SettingsMigrationStartedEvent evt; evt.from_version = "2.0"; evt.to_version = "3.0"; REQUIRE(evt.from_version == "2.0"); REQUIRE(evt.to_version == "3.0"); }

// ============================================================================
// V20 Phase 10 W06: Documentation
// ============================================================================
TEST_CASE("P10-W06 DocPageGeneratedEvent defaults", "[p10-w06][events]") { markamp::core::events::DocPageGeneratedEvent evt; REQUIRE(evt.page_id.empty()); REQUIRE(evt.sections_written == 0); }
TEST_CASE("P10-W06 DocCoverageCheckedEvent defaults", "[p10-w06][events]") { markamp::core::events::DocCoverageCheckedEvent evt; REQUIRE(evt.features_documented == 0); REQUIRE(evt.features_undocumented == 0); }
TEST_CASE("P10-W06 generate_doc_page workbench", "[p10-w06][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.generate_doc_page("getting_started", 5); }
TEST_CASE("P10-W06 check_doc_coverage workbench", "[p10-w06][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.check_doc_coverage(80, 20); }
TEST_CASE("P10-W06 generate then check", "[p10-w06][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.generate_doc_page("api_reference", 12); wb.check_doc_coverage(90, 10); }
TEST_CASE("P10-W06 event fields populated", "[p10-w06][events]") { markamp::core::events::DocPageGeneratedEvent evt; evt.page_id = "canvas"; evt.sections_written = 8; REQUIRE(evt.page_id == "canvas"); REQUIRE(evt.sections_written == 8); }

// ============================================================================
// V20 Phase 10 W07: Benchmarks & Baselines
// ============================================================================
TEST_CASE("P10-W07 BenchmarkRunCompletedEvent defaults", "[p10-w07][events]") { markamp::core::events::BenchmarkRunCompletedEvent evt; REQUIRE(evt.benchmark_name.empty()); REQUIRE(evt.result_ms == 0.0); }
TEST_CASE("P10-W07 BenchmarkRegressionDetectedEvent defaults", "[p10-w07][events]") { markamp::core::events::BenchmarkRegressionDetectedEvent evt; REQUIRE(evt.benchmark_name.empty()); REQUIRE(evt.regression_pct == 0.0); }
TEST_CASE("P10-W07 complete_benchmark_run workbench", "[p10-w07][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.complete_benchmark_run("render_1k", 12.5); }
TEST_CASE("P10-W07 detect_benchmark_regression workbench", "[p10-w07][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.detect_benchmark_regression("render_1k", 15.0); }
TEST_CASE("P10-W07 run then regression", "[p10-w07][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.complete_benchmark_run("save_large", 200.0); wb.detect_benchmark_regression("save_large", 25.0); }
TEST_CASE("P10-W07 event fields populated", "[p10-w07][events]") { markamp::core::events::BenchmarkRunCompletedEvent evt; evt.benchmark_name = "load"; evt.result_ms = 50.0; REQUIRE(evt.benchmark_name == "load"); REQUIRE(evt.result_ms == 50.0); }

// ============================================================================
// V20 Phase 10 W08: Release Gates
// ============================================================================
TEST_CASE("P10-W08 ReleaseGateEvaluatedEvent defaults", "[p10-w08][events]") { markamp::core::events::ReleaseGateEvaluatedEvent evt; REQUIRE(evt.gate_name.empty()); REQUIRE(evt.gate_passed == false); }
TEST_CASE("P10-W08 ReleaseReadinessSummaryEvent defaults", "[p10-w08][events]") { markamp::core::events::ReleaseReadinessSummaryEvent evt; REQUIRE(evt.gates_passed == 0); REQUIRE(evt.gates_failed == 0); }
TEST_CASE("P10-W08 evaluate_release_gate workbench", "[p10-w08][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.evaluate_release_gate("tests_pass", true); }
TEST_CASE("P10-W08 summarize_release_readiness workbench", "[p10-w08][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.summarize_release_readiness(9, 1); }
TEST_CASE("P10-W08 gate then summary", "[p10-w08][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.evaluate_release_gate("perf_budget", true); wb.summarize_release_readiness(10, 0); }
TEST_CASE("P10-W08 event fields populated", "[p10-w08][events]") { markamp::core::events::ReleaseGateEvaluatedEvent evt; evt.gate_name = "a11y"; evt.gate_passed = true; REQUIRE(evt.gate_name == "a11y"); REQUIRE(evt.gate_passed == true); }

// ============================================================================
// V20 Phase 10 W09: Beta Feedback Loops
// ============================================================================
TEST_CASE("P10-W09 BetaFeedbackSubmittedEvent defaults", "[p10-w09][events]") { markamp::core::events::BetaFeedbackSubmittedEvent evt; REQUIRE(evt.feedback_category.empty()); REQUIRE(evt.board_state_id.empty()); }
TEST_CASE("P10-W09 BetaFeedbackTriagedEvent defaults", "[p10-w09][events]") { markamp::core::events::BetaFeedbackTriagedEvent evt; REQUIRE(evt.feedback_id.empty()); REQUIRE(evt.triage_priority.empty()); }
TEST_CASE("P10-W09 submit_beta_feedback workbench", "[p10-w09][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.submit_beta_feedback("ux", "board_abc"); }
TEST_CASE("P10-W09 triage_beta_feedback workbench", "[p10-w09][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.triage_beta_feedback("fb_1", "p1"); }
TEST_CASE("P10-W09 submit then triage", "[p10-w09][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.submit_beta_feedback("bug", "board_xyz"); wb.triage_beta_feedback("fb_2", "p0"); }
TEST_CASE("P10-W09 event fields populated", "[p10-w09][events]") { markamp::core::events::BetaFeedbackSubmittedEvent evt; evt.feedback_category = "perf"; evt.board_state_id = "s_99"; REQUIRE(evt.feedback_category == "perf"); REQUIRE(evt.board_state_id == "s_99"); }

// ============================================================================
// V20 Phase 10 W10: Extension Ecosystem
// ============================================================================
TEST_CASE("P10-W10 ExtensionCompatibilityCheckedEvent defaults", "[p10-w10][events]") { markamp::core::events::ExtensionCompatibilityCheckedEvent evt; REQUIRE(evt.extension_id.empty()); REQUIRE(evt.is_compatible == false); }
TEST_CASE("P10-W10 ExtensionEcosystemReportEvent defaults", "[p10-w10][events]") { markamp::core::events::ExtensionEcosystemReportEvent evt; REQUIRE(evt.total_extensions == 0); REQUIRE(evt.compatible_extensions == 0); }
TEST_CASE("P10-W10 check_extension_compatibility workbench", "[p10-w10][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.check_extension_compatibility("mermaid", true); }
TEST_CASE("P10-W10 generate_extension_ecosystem_report workbench", "[p10-w10][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.generate_extension_ecosystem_report(50, 45); }
TEST_CASE("P10-W10 check then report", "[p10-w10][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.check_extension_compatibility("linter", false); wb.generate_extension_ecosystem_report(100, 85); }
TEST_CASE("P10-W10 event fields populated", "[p10-w10][events]") { markamp::core::events::ExtensionCompatibilityCheckedEvent evt; evt.extension_id = "theme_gallery"; evt.is_compatible = true; REQUIRE(evt.extension_id == "theme_gallery"); REQUIRE(evt.is_compatible == true); }

// ============================================================================
// V20 Phase 10 W11: Advanced Drawing Polish
// ============================================================================
TEST_CASE("P10-W11 DrawingStrokeRefinedEvent defaults", "[p10-w11][events]") { markamp::core::events::DrawingStrokeRefinedEvent evt; REQUIRE(evt.tool_name.empty()); REQUIRE(evt.pressure_sensitivity == 0.0); }
TEST_CASE("P10-W11 DrawingToolCalibratedEvent defaults", "[p10-w11][events]") { markamp::core::events::DrawingToolCalibratedEvent evt; REQUIRE(evt.tool_name.empty()); REQUIRE(evt.calibration_points == 0); }
TEST_CASE("P10-W11 refine_drawing_stroke workbench", "[p10-w11][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.refine_drawing_stroke("pen", 0.8); }
TEST_CASE("P10-W11 calibrate_drawing_tool workbench", "[p10-w11][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.calibrate_drawing_tool("pen", 5); }
TEST_CASE("P10-W11 refine then calibrate", "[p10-w11][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.refine_drawing_stroke("shape", 0.5); wb.calibrate_drawing_tool("shape", 3); }
TEST_CASE("P10-W11 event fields populated", "[p10-w11][events]") { markamp::core::events::DrawingStrokeRefinedEvent evt; evt.tool_name = "text"; evt.pressure_sensitivity = 1.0; REQUIRE(evt.tool_name == "text"); REQUIRE(evt.pressure_sensitivity == 1.0); }

// ============================================================================
// V20 Phase 10 W12: Advanced Layout Polish
// ============================================================================
TEST_CASE("P10-W12 LayoutTransformRefinedEvent defaults", "[p10-w12][events]") { markamp::core::events::LayoutTransformRefinedEvent evt; REQUIRE(evt.transform_type.empty()); REQUIRE(evt.precision_delta == 0.0); }
TEST_CASE("P10-W12 AlignmentEdgeCaseResolvedEvent defaults", "[p10-w12][events]") { markamp::core::events::AlignmentEdgeCaseResolvedEvent evt; REQUIRE(evt.alignment_type.empty()); REQUIRE(evt.objects_affected == 0); }
TEST_CASE("P10-W12 refine_layout_transform workbench", "[p10-w12][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.refine_layout_transform("rotate", 0.01); }
TEST_CASE("P10-W12 resolve_alignment_edge_case workbench", "[p10-w12][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.resolve_alignment_edge_case("center", 5); }
TEST_CASE("P10-W12 transform then alignment", "[p10-w12][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.refine_layout_transform("scale", 0.1); wb.resolve_alignment_edge_case("distribute", 8); }
TEST_CASE("P10-W12 event fields populated", "[p10-w12][events]") { markamp::core::events::LayoutTransformRefinedEvent evt; evt.transform_type = "skew"; evt.precision_delta = 0.05; REQUIRE(evt.transform_type == "skew"); REQUIRE(evt.precision_delta == 0.05); }

// ============================================================================
// V20 Phase 10 W13: Advanced Collaboration Polish
// ============================================================================
TEST_CASE("P10-W13 CollabFrictionResolvedEvent defaults", "[p10-w13][events]") { markamp::core::events::CollabFrictionResolvedEvent evt; REQUIRE(evt.friction_type.empty()); REQUIRE(evt.latency_improvement_ms == 0.0); }
TEST_CASE("P10-W13 MultiUserSessionStabilizedEvent defaults", "[p10-w13][events]") { markamp::core::events::MultiUserSessionStabilizedEvent evt; REQUIRE(evt.concurrent_users == 0); REQUIRE(evt.session_uptime_hours == 0.0); }
TEST_CASE("P10-W13 resolve_collab_friction workbench", "[p10-w13][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.resolve_collab_friction("cursor_jitter", 50.0); }
TEST_CASE("P10-W13 stabilize_multi_user_session workbench", "[p10-w13][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.stabilize_multi_user_session(10, 2.5); }
TEST_CASE("P10-W13 friction then stabilize", "[p10-w13][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.resolve_collab_friction("sync_delay", 100.0); wb.stabilize_multi_user_session(5, 8.0); }
TEST_CASE("P10-W13 event fields populated", "[p10-w13][events]") { markamp::core::events::CollabFrictionResolvedEvent evt; evt.friction_type = "selection_conflict"; evt.latency_improvement_ms = 25.0; REQUIRE(evt.friction_type == "selection_conflict"); REQUIRE(evt.latency_improvement_ms == 25.0); }

// ============================================================================
// V20 Phase 10 W14: Advanced Navigation Polish
// ============================================================================
TEST_CASE("P10-W14 BoardTravelOptimizedEvent defaults", "[p10-w14][events]") { markamp::core::events::BoardTravelOptimizedEvent evt; REQUIRE(evt.navigation_mode.empty()); REQUIRE(evt.travel_time_ms == 0.0); }
TEST_CASE("P10-W14 NavigationWaypointSetEvent defaults", "[p10-w14][events]") { markamp::core::events::NavigationWaypointSetEvent evt; REQUIRE(evt.waypoint_id.empty()); REQUIRE(evt.x == 0.0); }
TEST_CASE("P10-W14 optimize_board_travel workbench", "[p10-w14][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.optimize_board_travel("minimap", 200.0); }
TEST_CASE("P10-W14 set_navigation_waypoint workbench", "[p10-w14][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.set_navigation_waypoint("wp_1", 500.0); }
TEST_CASE("P10-W14 travel then waypoint", "[p10-w14][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.optimize_board_travel("keyboard", 100.0); wb.set_navigation_waypoint("wp_2", 1000.0); }
TEST_CASE("P10-W14 event fields populated", "[p10-w14][events]") { markamp::core::events::BoardTravelOptimizedEvent evt; evt.navigation_mode = "search"; evt.travel_time_ms = 50.0; REQUIRE(evt.navigation_mode == "search"); REQUIRE(evt.travel_time_ms == 50.0); }

// ============================================================================
// V20 Phase 10 W15: Advanced Export Polish
// ============================================================================
TEST_CASE("P10-W15 ExportArtifactFinalizedEvent defaults", "[p10-w15][events]") { markamp::core::events::ExportArtifactFinalizedEvent evt; REQUIRE(evt.format.empty()); REQUIRE(evt.pages_exported == 0); }
TEST_CASE("P10-W15 ExportFidelityValidatedEvent defaults", "[p10-w15][events]") { markamp::core::events::ExportFidelityValidatedEvent evt; REQUIRE(evt.format.empty()); REQUIRE(evt.fidelity_score == 0.0); }
TEST_CASE("P10-W15 finalize_export_artifact workbench", "[p10-w15][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.finalize_export_artifact("pdf", 10); }
TEST_CASE("P10-W15 validate_export_fidelity workbench", "[p10-w15][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.validate_export_fidelity("pdf", 0.95); }
TEST_CASE("P10-W15 finalize then validate", "[p10-w15][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.finalize_export_artifact("png", 1); wb.validate_export_fidelity("png", 0.99); }
TEST_CASE("P10-W15 event fields populated", "[p10-w15][events]") { markamp::core::events::ExportArtifactFinalizedEvent evt; evt.format = "svg"; evt.pages_exported = 5; REQUIRE(evt.format == "svg"); REQUIRE(evt.pages_exported == 5); }

// ============================================================================
// V20 Phase 10 W16: Enterprise & Admin
// ============================================================================
TEST_CASE("P10-W16 AdminPolicyAppliedEvent defaults", "[p10-w16][events]") { markamp::core::events::AdminPolicyAppliedEvent evt; REQUIRE(evt.policy_name.empty()); REQUIRE(evt.users_affected == 0); }
TEST_CASE("P10-W16 GovernanceAuditLoggedEvent defaults", "[p10-w16][events]") { markamp::core::events::GovernanceAuditLoggedEvent evt; REQUIRE(evt.action_type.empty()); REQUIRE(evt.actor_id.empty()); }
TEST_CASE("P10-W16 apply_admin_policy workbench", "[p10-w16][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_admin_policy("readonly_boards", 50); }
TEST_CASE("P10-W16 log_governance_audit workbench", "[p10-w16][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.log_governance_audit("board_delete", "admin_1"); }
TEST_CASE("P10-W16 policy then audit", "[p10-w16][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.apply_admin_policy("no_export", 100); wb.log_governance_audit("policy_change", "admin_2"); }
TEST_CASE("P10-W16 event fields populated", "[p10-w16][events]") { markamp::core::events::AdminPolicyAppliedEvent evt; evt.policy_name = "sso_required"; evt.users_affected = 200; REQUIRE(evt.policy_name == "sso_required"); REQUIRE(evt.users_affected == 200); }

// ============================================================================
// V20 Phase 10 W17: Quality Backlog Triage
// ============================================================================
TEST_CASE("P10-W17 DefectTriagedEvent defaults", "[p10-w17][events]") { markamp::core::events::DefectTriagedEvent evt; REQUIRE(evt.defect_id.empty()); REQUIRE(evt.severity.empty()); }
TEST_CASE("P10-W17 QualityBacklogReducedEvent defaults", "[p10-w17][events]") { markamp::core::events::QualityBacklogReducedEvent evt; REQUIRE(evt.defects_resolved == 0); REQUIRE(evt.defects_remaining == 0); }
TEST_CASE("P10-W17 triage_defect workbench", "[p10-w17][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.triage_defect("BUG-123", "p1"); }
TEST_CASE("P10-W17 reduce_quality_backlog workbench", "[p10-w17][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.reduce_quality_backlog(10, 20); }
TEST_CASE("P10-W17 triage then reduce", "[p10-w17][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.triage_defect("BUG-456", "p0"); wb.reduce_quality_backlog(5, 15); }
TEST_CASE("P10-W17 event fields populated", "[p10-w17][events]") { markamp::core::events::DefectTriagedEvent evt; evt.defect_id = "BUG-789"; evt.severity = "p2"; REQUIRE(evt.defect_id == "BUG-789"); REQUIRE(evt.severity == "p2"); }

// ============================================================================
// V20 Phase 10 W18: Adoption Metrics
// ============================================================================
TEST_CASE("P10-W18 AdoptionMetricTrackedEvent defaults", "[p10-w18][events]") { markamp::core::events::AdoptionMetricTrackedEvent evt; REQUIRE(evt.metric_name.empty()); REQUIRE(evt.metric_value == 0.0); }
TEST_CASE("P10-W18 UsageTrendAnalyzedEvent defaults", "[p10-w18][events]") { markamp::core::events::UsageTrendAnalyzedEvent evt; REQUIRE(evt.feature_name.empty()); REQUIRE(evt.trend_direction == 0.0); }
TEST_CASE("P10-W18 track_adoption_metric workbench", "[p10-w18][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.track_adoption_metric("daily_active_boards", 150.0); }
TEST_CASE("P10-W18 analyze_usage_trend workbench", "[p10-w18][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.analyze_usage_trend("canvas_editor", 0.15); }
TEST_CASE("P10-W18 track then analyze", "[p10-w18][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.track_adoption_metric("sessions", 500.0); wb.analyze_usage_trend("collab", -0.05); }
TEST_CASE("P10-W18 event fields populated", "[p10-w18][events]") { markamp::core::events::AdoptionMetricTrackedEvent evt; evt.metric_name = "exports"; evt.metric_value = 75.0; REQUIRE(evt.metric_name == "exports"); REQUIRE(evt.metric_value == 75.0); }

// ============================================================================
// V20 Phase 10 W19: Support Playbooks
// ============================================================================
TEST_CASE("P10-W19 SupportPlaybookActivatedEvent defaults", "[p10-w19][events]") { markamp::core::events::SupportPlaybookActivatedEvent evt; REQUIRE(evt.playbook_id.empty()); REQUIRE(evt.issue_category.empty()); }
TEST_CASE("P10-W19 SupportResolutionLoggedEvent defaults", "[p10-w19][events]") { markamp::core::events::SupportResolutionLoggedEvent evt; REQUIRE(evt.ticket_id.empty()); REQUIRE(evt.resolution_time_hours == 0.0); }
TEST_CASE("P10-W19 activate_support_playbook workbench", "[p10-w19][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.activate_support_playbook("pb_crash", "crash"); }
TEST_CASE("P10-W19 log_support_resolution workbench", "[p10-w19][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.log_support_resolution("TKT-100", 2.5); }
TEST_CASE("P10-W19 playbook then resolution", "[p10-w19][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.activate_support_playbook("pb_slow", "performance"); wb.log_support_resolution("TKT-200", 1.0); }
TEST_CASE("P10-W19 event fields populated", "[p10-w19][events]") { markamp::core::events::SupportPlaybookActivatedEvent evt; evt.playbook_id = "pb_sync"; evt.issue_category = "sync"; REQUIRE(evt.playbook_id == "pb_sync"); REQUIRE(evt.issue_category == "sync"); }

// ============================================================================
// V20 Phase 10 W20: Architecture Follow Ups
// ============================================================================
TEST_CASE("P10-W20 TechDebtAddressedEvent defaults", "[p10-w20][events]") { markamp::core::events::TechDebtAddressedEvent evt; REQUIRE(evt.debt_item.empty()); REQUIRE(evt.resolution_type.empty()); }
TEST_CASE("P10-W20 PlatformInvestmentCompletedEvent defaults", "[p10-w20][events]") { markamp::core::events::PlatformInvestmentCompletedEvent evt; REQUIRE(evt.investment_area.empty()); REQUIRE(evt.files_affected == 0); }
TEST_CASE("P10-W20 address_tech_debt workbench", "[p10-w20][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.address_tech_debt("legacy_renderer", "refactor"); }
TEST_CASE("P10-W20 complete_platform_investment workbench", "[p10-w20][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.complete_platform_investment("linux_input", 15); }
TEST_CASE("P10-W20 debt then investment", "[p10-w20][workbench]") { auto bus = std::make_shared<markamp::core::EventBus>(); markamp::canvas::CanvasWorkbench wb(bus); wb.address_tech_debt("old_serializer", "remove"); wb.complete_platform_investment("wayland", 25); }
TEST_CASE("P10-W20 event fields populated", "[p10-w20][events]") { markamp::core::events::TechDebtAddressedEvent evt; evt.debt_item = "event_v1"; evt.resolution_type = "document"; REQUIRE(evt.debt_item == "event_v1"); REQUIRE(evt.resolution_type == "document"); }
