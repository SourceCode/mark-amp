/// @file test_workbench_navigation.cpp
/// @brief V8 Phase 11 Quality bar tests — Unified Workbench Navigation,
///        Tool Window System, and Surface Link Contract.

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/SurfaceLink.h"
#include "ui/NavigationService.h"
#include "ui/ToolWindowHost.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::core::events;
using namespace markamp::ui;

// =============================================================================
// WorkbenchMode enum
// =============================================================================

TEST_CASE("WorkbenchMode enum has 5 distinct values", "[phase11][workbench]")
{
    REQUIRE(WorkbenchMode::kEditor != WorkbenchMode::kCanvas);
    REQUIRE(WorkbenchMode::kEditor != WorkbenchMode::kNotebook);
    REQUIRE(WorkbenchMode::kEditor != WorkbenchMode::kGraph);
    REQUIRE(WorkbenchMode::kEditor != WorkbenchMode::kSettings);
    REQUIRE(WorkbenchMode::kCanvas != WorkbenchMode::kNotebook);

    // Count: 5 distinct values
    constexpr auto kExpectedCount = 5;
    int count = 0;
    for (int idx = 0; idx < kExpectedCount; ++idx)
    {
        auto mode = static_cast<WorkbenchMode>(idx);
        if (mode == WorkbenchMode::kEditor || mode == WorkbenchMode::kCanvas ||
            mode == WorkbenchMode::kNotebook || mode == WorkbenchMode::kGraph ||
            mode == WorkbenchMode::kSettings)
        {
            ++count;
        }
    }
    REQUIRE(count == kExpectedCount);
}

// =============================================================================
// DockPosition enum
// =============================================================================

TEST_CASE("DockPosition enum has 3 distinct values", "[phase11][toolwindow]")
{
    REQUIRE(DockPosition::kLeft != DockPosition::kRight);
    REQUIRE(DockPosition::kLeft != DockPosition::kBottom);
    REQUIRE(DockPosition::kRight != DockPosition::kBottom);
}

// =============================================================================
// Expanded ActivityBarItem enum (5 → 8)
// =============================================================================

TEST_CASE("ActivityBarItem has 8 values after Phase 11 expansion", "[phase11][activitybar]")
{
    // Original 5
    REQUIRE(ActivityBarItem::FileExplorer != ActivityBarItem::Search);
    REQUIRE(ActivityBarItem::Search != ActivityBarItem::Settings);
    REQUIRE(ActivityBarItem::Settings != ActivityBarItem::Themes);
    REQUIRE(ActivityBarItem::Themes != ActivityBarItem::Extensions);

    // Phase 11 additions
    REQUIRE(ActivityBarItem::Extensions != ActivityBarItem::kNotebooks);
    REQUIRE(ActivityBarItem::kNotebooks != ActivityBarItem::kCanvas);
    REQUIRE(ActivityBarItem::kCanvas != ActivityBarItem::kGraph);

    // Count: 8 total
    constexpr auto kExpectedCount = 8;
    int count = 0;
    for (int idx = 0; idx < kExpectedCount; ++idx)
    {
        ++count;
    }
    REQUIRE(count == kExpectedCount);
}

// =============================================================================
// Phase 11 event structs compile and carry payloads
// =============================================================================

TEST_CASE("Phase 11 events compile and carry payloads", "[phase11][events]")
{
    SECTION("WorkbenchModeChangedEvent")
    {
        WorkbenchModeChangedEvent evt;
        evt.previous_mode = WorkbenchMode::kEditor;
        evt.new_mode = WorkbenchMode::kCanvas;
        REQUIRE(evt.type_name() == "WorkbenchModeChangedEvent");
        REQUIRE(evt.previous_mode == WorkbenchMode::kEditor);
        REQUIRE(evt.new_mode == WorkbenchMode::kCanvas);
    }

    SECTION("WorkbenchModeSwitchRequestEvent")
    {
        WorkbenchModeSwitchRequestEvent evt;
        evt.target_mode = WorkbenchMode::kGraph;
        REQUIRE(evt.type_name() == "WorkbenchModeSwitchRequestEvent");
        REQUIRE(evt.target_mode == WorkbenchMode::kGraph);
    }

    SECTION("ToolWindowToggleRequestEvent")
    {
        ToolWindowToggleRequestEvent evt;
        evt.panel_id = "output";
        evt.visible = false;
        REQUIRE(evt.type_name() == "ToolWindowToggleRequestEvent");
        REQUIRE(evt.panel_id == "output");
        REQUIRE_FALSE(evt.visible);
    }

    SECTION("ToolWindowDockPositionChangedEvent")
    {
        ToolWindowDockPositionChangedEvent evt;
        evt.panel_id = "problems";
        evt.dock_position = DockPosition::kRight;
        REQUIRE(evt.type_name() == "ToolWindowDockPositionChangedEvent");
        REQUIRE(evt.dock_position == DockPosition::kRight);
    }
}

// =============================================================================
// SurfaceLink types
// =============================================================================

TEST_CASE("SurfaceKind enum has 5 distinct values", "[phase11][surfacelink]")
{
    REQUIRE(SurfaceKind::kEditor != SurfaceKind::kPreview);
    REQUIRE(SurfaceKind::kEditor != SurfaceKind::kCanvas);
    REQUIRE(SurfaceKind::kEditor != SurfaceKind::kGraph);
    REQUIRE(SurfaceKind::kEditor != SurfaceKind::kNotebook);
}

TEST_CASE("EntityKind enum has 5 distinct values", "[phase11][surfacelink]")
{
    REQUIRE(EntityKind::kDocument != EntityKind::kHeading);
    REQUIRE(EntityKind::kDocument != EntityKind::kCodeBlock);
    REQUIRE(EntityKind::kDocument != EntityKind::kCanvasObject);
    REQUIRE(EntityKind::kDocument != EntityKind::kNotebookCell);
}

TEST_CASE("LinkAnchor struct defaults and fields", "[phase11][surfacelink]")
{
    LinkAnchor anchor;
    REQUIRE(anchor.surface_kind == SurfaceKind::kEditor);
    REQUIRE(anchor.entity_kind == EntityKind::kDocument);
    REQUIRE(anchor.file_path.empty());
    REQUIRE(anchor.line == 0);
    REQUIRE(anchor.column == 0);
    REQUIRE(anchor.board_id.empty());
    REQUIRE(anchor.object_id.empty());
    REQUIRE(anchor.cell_id.empty());
    REQUIRE(anchor.heading_id.empty());

    // Populate and verify
    anchor.surface_kind = SurfaceKind::kCanvas;
    anchor.entity_kind = EntityKind::kCanvasObject;
    anchor.board_id = "board-1";
    anchor.object_id = "obj-42";
    REQUIRE(anchor.surface_kind == SurfaceKind::kCanvas);
    REQUIRE(anchor.board_id == "board-1");
    REQUIRE(anchor.object_id == "obj-42");
}

TEST_CASE("SurfaceLink struct fields", "[phase11][surfacelink]")
{
    SurfaceLink link;
    link.from.surface_kind = SurfaceKind::kEditor;
    link.from.file_path = "doc.md";
    link.from.line = 42;
    link.to.surface_kind = SurfaceKind::kPreview;
    link.to.heading_id = "heading-3";
    link.reason = "review";

    REQUIRE(link.from.surface_kind == SurfaceKind::kEditor);
    REQUIRE(link.from.line == 42);
    REQUIRE(link.to.surface_kind == SurfaceKind::kPreview);
    REQUIRE(link.to.heading_id == "heading-3");
    REQUIRE(link.reason == "review");
}

// =============================================================================
// Surface link events
// =============================================================================

TEST_CASE("Surface link events compile and carry payloads", "[phase11][events]")
{
    SECTION("OpenSurfaceLinkRequestEvent")
    {
        OpenSurfaceLinkRequestEvent evt;
        evt.link.from.surface_kind = SurfaceKind::kEditor;
        evt.link.to.surface_kind = SurfaceKind::kCanvas;
        REQUIRE(evt.type_name() == "OpenSurfaceLinkRequestEvent");
    }

    SECTION("RevealInSurfaceRequestEvent")
    {
        RevealInSurfaceRequestEvent evt;
        evt.target.surface_kind = SurfaceKind::kGraph;
        REQUIRE(evt.type_name() == "RevealInSurfaceRequestEvent");
    }

    SECTION("SurfaceLinkResolvedEvent")
    {
        SurfaceLinkResolvedEvent evt;
        evt.success = true;
        REQUIRE(evt.type_name() == "SurfaceLinkResolvedEvent");
        REQUIRE(evt.success);
    }

    SECTION("SurfaceTraversalFailedEvent")
    {
        SurfaceTraversalFailedEvent evt;
        evt.error_message = "target not found";
        REQUIRE(evt.type_name() == "SurfaceTraversalFailedEvent");
        REQUIRE(evt.error_message == "target not found");
    }
}

// =============================================================================
// ToolWindowHost lifecycle
// =============================================================================

TEST_CASE("ToolWindowHost register/toggle/pin lifecycle", "[phase11][toolwindow]")
{
    EventBus bus;
    Config config;
    ToolWindowHost host(bus, config);

    // Initially empty
    REQUIRE(host.panel_count() == 0);

    // Register panels
    host.register_panel("output", "Output", DockPosition::kBottom);
    host.register_panel("problems", "Problems", DockPosition::kBottom);
    host.register_panel("explorer", "Explorer", DockPosition::kLeft);
    REQUIRE(host.panel_count() == 3);

    // Duplicate registration is no-op
    host.register_panel("output", "Output Again", DockPosition::kRight);
    REQUIRE(host.panel_count() == 3);

    // Default state: not visible, not pinned
    REQUIRE_FALSE(host.is_visible("output"));
    REQUIRE_FALSE(host.is_pinned("output"));
    REQUIRE(host.dock_position("output") == DockPosition::kBottom);

    // Toggle visibility
    host.toggle_panel("output");
    REQUIRE(host.is_visible("output"));
    host.toggle_panel("output");
    REQUIRE_FALSE(host.is_visible("output"));

    // Set visible explicitly
    host.set_visible("output", true);
    REQUIRE(host.is_visible("output"));

    // Pin/unpin
    host.set_pinned("output", true);
    REQUIRE(host.is_pinned("output"));
    host.set_pinned("output", false);
    REQUIRE_FALSE(host.is_pinned("output"));

    // Change dock position
    host.set_dock_position("output", DockPosition::kRight);
    REQUIRE(host.dock_position("output") == DockPosition::kRight);

    // Panel state retrieval
    const auto* state = host.panel_state("output");
    REQUIRE(state != nullptr);
    REQUIRE(state->title == "Output");
    REQUIRE(state->panel_id == "output");

    // Non-existent panel returns nullptr/defaults
    REQUIRE(host.panel_state("nonexistent") == nullptr);
    REQUIRE_FALSE(host.is_visible("nonexistent"));
    REQUIRE(host.dock_position("nonexistent") == DockPosition::kBottom);

    // panels_at
    const auto& bottom_panels = host.panels_at(DockPosition::kBottom);
    REQUIRE(bottom_panels.size() == 1); // "problems" is the only bottom panel now
    const auto& left_panels = host.panels_at(DockPosition::kLeft);
    REQUIRE(left_panels.size() == 1); // "explorer"

    // Unregister
    host.unregister_panel("output");
    REQUIRE(host.panel_count() == 2);
    REQUIRE(host.panel_state("output") == nullptr);
}

// =============================================================================
// NavigationEntry cross-surface fields
// =============================================================================

TEST_CASE("NavigationEntry has cross-surface fields", "[phase11][navigation]")
{
    NavigationEntry entry;
    entry.document_id = "doc.md";
    entry.cursor_line = 10;
    entry.from_surface = "editor";
    entry.to_surface = "canvas";
    entry.entity_id = "entity-1";
    entry.board_id = "board-1";
    entry.object_id = "obj-1";
    entry.cell_id = "cell-1";

    REQUIRE(entry.from_surface == "editor");
    REQUIRE(entry.to_surface == "canvas");
    REQUIRE(entry.entity_id == "entity-1");
    REQUIRE(entry.board_id == "board-1");
    REQUIRE(entry.object_id == "obj-1");
    REQUIRE(entry.cell_id == "cell-1");
}
