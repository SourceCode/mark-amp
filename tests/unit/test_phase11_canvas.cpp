// ============================================================================
// File: tests/unit/test_phase11_canvas.cpp
// Phase 11: Canvas Workbench Shell — comprehensive unit tests
// ============================================================================
#include "canvas/CanvasAccessibility.h"
#include "canvas/CanvasContextMenu.h"
#include "canvas/CanvasInspector.h"
#include "canvas/CanvasObjectFactory.h"
#include "canvas/CanvasPerformanceMonitor.h"
#include "canvas/CanvasWorkbench.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

// ── CanvasWorkbench Tests ──────────────────────────────────────────

TEST_CASE("CanvasWorkbench — construction", "[canvas][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    CanvasWorkbench wb(bus);

    REQUIRE(wb.active_board() == nullptr);
    REQUIRE(wb.active_tool_mode() == ToolMode::Select);
    REQUIRE(wb.open_boards().empty());
    REQUIRE(wb.recent_boards().empty());
}

TEST_CASE("CanvasWorkbench — create and open board", "[canvas][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    CanvasWorkbench wb(bus);

    auto board_id = wb.create_board("My Board");
    REQUIRE_FALSE(board_id.empty());
    REQUIRE(wb.active_board() != nullptr);
    REQUIRE(wb.board_exists(board_id));
    REQUIRE(wb.open_boards().size() == 1);
    REQUIRE(wb.open_boards()[0].name == "My Board");
}

TEST_CASE("CanvasWorkbench — close board", "[canvas][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    CanvasWorkbench wb(bus);

    auto id = wb.create_board("Board 1");
    REQUIRE(wb.board_exists(id));

    REQUIRE(wb.close_board(id));
    REQUIRE_FALSE(wb.board_exists(id));
    REQUIRE(wb.active_board() == nullptr);
}

TEST_CASE("CanvasWorkbench — tool switching", "[canvas][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    CanvasWorkbench wb(bus);

    REQUIRE(wb.active_tool_mode() == ToolMode::Select);
    wb.set_tool(ToolMode::Pan);
    REQUIRE(wb.active_tool_mode() == ToolMode::Pan);
    wb.set_tool(ToolMode::Draw);
    REQUIRE(wb.active_tool_mode() == ToolMode::Draw);
}

TEST_CASE("CanvasWorkbench — state persistence", "[canvas][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    CanvasWorkbench wb(bus);

    wb.set_tool(ToolMode::Pan);
    const auto& state = wb.state();
    REQUIRE(state.active_tool == ToolMode::Pan);
    REQUIRE(state.grid_visible);
    REQUIRE(state.snap_enabled);
}

TEST_CASE("CanvasWorkbench — recent boards tracking", "[canvas][workbench]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    CanvasWorkbench wb(bus);

    auto id1 = wb.create_board("B1");
    auto id2 = wb.create_board("B2");

    const auto& recent = wb.recent_boards();
    REQUIRE(recent.size() == 2);
    REQUIRE(recent[0] == id2); // Most recent first
    REQUIRE(recent[1] == id1);
}

TEST_CASE("CanvasWorkbench — surface name", "[canvas][workbench]")
{
    REQUIRE(CanvasWorkbench::surface_name() == "Canvas");
    REQUIRE(CanvasWorkbench::kSurfaceKindCanvas == 3);
}

// ── CanvasObjectFactory Tests ──────────────────────────────────────

TEST_CASE("CanvasObjectFactory — create all types", "[canvas][factory]")
{
    CanvasObjectFactory factory;
    const auto types = CanvasObjectFactory::creatable_types();

    for (const auto type : types)
    {
        auto result = factory.create(type);
        REQUIRE(result.success);
        REQUIRE(result.object != nullptr);
    }
}

TEST_CASE("CanvasObjectFactory — custom params", "[canvas][factory]")
{
    CanvasObjectFactory factory;

    ObjectCreationParams params;
    params.position = {100.0, 200.0};
    params.size = {300.0, 150.0};
    params.name = "Test Note";
    params.fill_color = {255, 200, 100, 255};

    auto result = factory.create(CanvasObjectType::StickyNote, params);
    REQUIRE(result.success);
    REQUIRE(result.object != nullptr);
    REQUIRE(result.object->name() == "Test Note");
    REQUIRE(result.object->transform().tx == 100.0);
    REQUIRE(result.object->transform().ty == 200.0);
}

TEST_CASE("CanvasObjectFactory — default sizes", "[canvas][factory]")
{
    auto sn_size = CanvasObjectFactory::default_size(CanvasObjectType::StickyNote);
    REQUIRE(sn_size.width == 200.0);
    REQUIRE(sn_size.height == 200.0);

    auto tb_size = CanvasObjectFactory::default_size(CanvasObjectType::TextBox);
    REQUIRE(tb_size.width == 250.0);
    REQUIRE(tb_size.height == 100.0);
}

TEST_CASE("CanvasObjectFactory — type display names", "[canvas][factory]")
{
    REQUIRE(CanvasObjectFactory::type_display_name(CanvasObjectType::StickyNote) == "Sticky Note");
    REQUIRE(CanvasObjectFactory::type_display_name(CanvasObjectType::TextBox) == "Text Box");
    REQUIRE(CanvasObjectFactory::type_display_name(CanvasObjectType::Table) == "Table");
}

TEST_CASE("CanvasObjectFactory — grid creation", "[canvas][factory]")
{
    CanvasObjectFactory factory;
    auto objects = factory.create_grid(CanvasObjectType::StickyNote, 4, {0.0, 0.0});
    REQUIRE(objects.size() == 4);
    for (const auto& obj : objects)
    {
        REQUIRE(obj != nullptr);
    }
}

TEST_CASE("CanvasObjectFactory — duplicate", "[canvas][factory]")
{
    CanvasObjectFactory factory;
    auto result = factory.create(CanvasObjectType::StickyNote);
    REQUIRE(result.success);

    auto clone = CanvasObjectFactory::duplicate(*result.object);
    REQUIRE(clone != nullptr);
    // Clone should be offset
    REQUIRE(clone->transform().tx == result.object->transform().tx + 20.0);
    REQUIRE(clone->transform().ty == result.object->transform().ty + 20.0);
}

// ── CanvasContextMenu Tests ────────────────────────────────────────

TEST_CASE("CanvasContextMenu — background menu", "[canvas][context-menu]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    Board board("test");
    SelectionManager sel(bus);
    UndoRedoStack undo;

    CanvasContextMenu menu(board, sel, undo);
    auto groups = menu.build_background_menu({100.0, 100.0});

    REQUIRE_FALSE(groups.empty());
    // Should have Clipboard, Insert, View groups
    REQUIRE(groups.size() >= 3);
}

TEST_CASE("CanvasContextMenu — action availability", "[canvas][context-menu]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    Board board("test");
    SelectionManager sel(bus);
    UndoRedoStack undo;

    CanvasContextMenu menu(board, sel, undo);

    // No selection — cut/copy should be unavailable
    REQUIRE_FALSE(menu.is_action_available(CanvasContextMenu::kActionCut));
    REQUIRE_FALSE(menu.is_action_available(CanvasContextMenu::kActionCopy));
    REQUIRE_FALSE(menu.is_action_available(CanvasContextMenu::kActionDelete));
    // Paste and zoom-to-fit should always be available
    REQUIRE(menu.is_action_available(CanvasContextMenu::kActionPaste));
    REQUIRE(menu.is_action_available(CanvasContextMenu::kActionZoomToFit));
}

TEST_CASE("CanvasContextMenu — action handler", "[canvas][context-menu]")
{
    auto bus = std::make_shared<markamp::core::EventBus>();
    Board board("test");
    SelectionManager sel(bus);
    UndoRedoStack undo;

    CanvasContextMenu menu(board, sel, undo);

    std::string executed_action;
    menu.set_action_handler([&](const std::string& id) { executed_action = id; });

    menu.execute_action(CanvasContextMenu::kActionPaste);
    REQUIRE(executed_action == "canvas.paste");
}

// ── CanvasPerformanceMonitor Tests ─────────────────────────────────

TEST_CASE("CanvasPerformanceMonitor — default construction", "[canvas][perf]")
{
    CanvasPerformanceMonitor mon;
    REQUIRE(mon.fps() > 0.0);
    REQUIRE(mon.frame_count() == 0);
    REQUIRE(mon.health_status() == CanvasHealthStatus::kHealthy);
}

TEST_CASE("CanvasPerformanceMonitor — frame timing", "[canvas][perf]")
{
    CanvasPerformanceMonitor mon;

    mon.begin_frame();
    mon.begin_render();
    // Simulate some work
    mon.end_render();
    mon.end_frame();

    REQUIRE(mon.frame_count() == 1);
    REQUIRE(mon.frame_time_ms() >= 0.0);
    REQUIRE(mon.render_time_ms() >= 0.0);
}

TEST_CASE("CanvasPerformanceMonitor — budget violations", "[canvas][perf]")
{
    CanvasPerformanceBudget budget;
    budget.max_visible_objects = 100;
    budget.max_total_objects = 200;

    CanvasPerformanceMonitor mon(budget);
    mon.set_object_counts(300, 150);

    auto violations = mon.check_budget();
    REQUIRE(violations.size() >= 2);
}

TEST_CASE("CanvasPerformanceMonitor — health status", "[canvas][perf]")
{
    CanvasPerformanceBudget budget;
    budget.max_total_objects = 100;
    CanvasPerformanceMonitor mon(budget);

    // Healthy with no objects
    REQUIRE(mon.health_status() == CanvasHealthStatus::kHealthy);

    // Warning when over budget but < 1.5x
    mon.set_object_counts(120, 50);
    REQUIRE(mon.health_status() == CanvasHealthStatus::kWarning);
}

TEST_CASE("CanvasPerformanceMonitor — snapshot", "[canvas][perf]")
{
    CanvasPerformanceMonitor mon;
    mon.set_object_counts(42, 30);
    mon.set_memory_estimate(128.5);
    mon.set_undo_depth(15);

    auto snap = mon.snapshot();
    REQUIRE(snap.total_objects == 42);
    REQUIRE(snap.visible_objects == 30);
    REQUIRE(snap.estimated_memory_mb == 128.5);
    REQUIRE(snap.undo_stack_depth == 15);
}

TEST_CASE("CanvasPerformanceMonitor — reset", "[canvas][perf]")
{
    CanvasPerformanceMonitor mon;
    mon.set_object_counts(100, 50);
    mon.begin_frame();
    mon.end_frame();

    REQUIRE(mon.frame_count() == 1);

    mon.reset();
    REQUIRE(mon.frame_count() == 0);
    auto snap = mon.snapshot();
    REQUIRE(snap.total_objects == 0);
    REQUIRE(snap.visible_objects == 0);
}

// ── CanvasAccessibility Tests ──────────────────────────────────────

TEST_CASE("CanvasAccessibility — default construction", "[canvas][a11y]")
{
    CanvasAccessibility a11y;
    REQUIRE(a11y.focused_object() == kInvalidObjectId);
    REQUIRE(a11y.recent_announcements().empty());
}

TEST_CASE("CanvasAccessibility — announce zoom", "[canvas][a11y]")
{
    CanvasAccessibility a11y;
    auto ann = a11y.announce_zoom(150.0);
    REQUIRE(ann.message == "Zoom 150%");
    REQUIRE(ann.priority == AnnouncementPriority::kPolite);
}

TEST_CASE("CanvasAccessibility — announce tool change", "[canvas][a11y]")
{
    CanvasAccessibility a11y;
    auto ann = a11y.announce_tool_change("Select");
    REQUIRE(ann.message == "Select tool active");
}

TEST_CASE("CanvasAccessibility — announce deletion", "[canvas][a11y]")
{
    CanvasAccessibility a11y;
    auto ann1 = a11y.announce_deletion(1);
    REQUIRE(ann1.message == "Deleted 1 object");

    auto ann2 = a11y.announce_deletion(5);
    REQUIRE(ann2.message == "Deleted 5 objects");
    REQUIRE(ann2.priority == AnnouncementPriority::kAssertive);
}

TEST_CASE("CanvasAccessibility — keyboard focus ring", "[canvas][a11y]")
{
    CanvasAccessibility a11y;

    std::vector<ObjectId> tab_order = {10, 20, 30, 40};

    // First focus_next should focus on first item
    REQUIRE(a11y.focus_next(tab_order) == 10);
    REQUIRE(a11y.focused_object() == 10);

    // Next
    REQUIRE(a11y.focus_next(tab_order) == 20);
    REQUIRE(a11y.focus_next(tab_order) == 30);
    REQUIRE(a11y.focus_next(tab_order) == 40);

    // Wrap around
    REQUIRE(a11y.focus_next(tab_order) == 10);
}

TEST_CASE("CanvasAccessibility — focus previous", "[canvas][a11y]")
{
    CanvasAccessibility a11y;
    std::vector<ObjectId> tab_order = {10, 20, 30};

    // First previous should go to last
    REQUIRE(a11y.focus_previous(tab_order) == 30);
    REQUIRE(a11y.focus_previous(tab_order) == 20);
    REQUIRE(a11y.focus_previous(tab_order) == 10);
    // Wrap
    REQUIRE(a11y.focus_previous(tab_order) == 30);
}

TEST_CASE("CanvasAccessibility — delete confirmation config", "[canvas][a11y]")
{
    DeleteConfirmationConfig config;
    config.threshold = 3;
    config.confirm_locked = true;
    config.enabled = true;

    CanvasAccessibility a11y(config);
    REQUIRE(a11y.delete_config().threshold == 3);
    REQUIRE(a11y.delete_config().confirm_locked);
}

TEST_CASE("CanvasAccessibility — announcement history", "[canvas][a11y]")
{
    CanvasAccessibility a11y;

    a11y.announce_zoom(100.0);
    a11y.announce_zoom(200.0);
    a11y.announce_tool_change("Pan");

    REQUIRE(a11y.recent_announcements().size() == 3);
}

TEST_CASE("CanvasAccessibility — static announce", "[canvas][a11y]")
{
    auto ann = CanvasAccessibility::announce("Board saved", AnnouncementPriority::kAssertive);
    REQUIRE(ann.message == "Board saved");
    REQUIRE(ann.priority == AnnouncementPriority::kAssertive);
}
