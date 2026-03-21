/// @file test_v20_canvas_integration.cpp
/// @brief V20 Phase 04 – End-to-end canvas integration tests.

#include "core/ArtifactCreationService.h"
#include "core/ArtifactRegistry.h"
#include "core/CanvasArtifactLifecycle.h"
#include "core/CanvasEventBridge.h"
#include "core/CanvasShellIntegration.h"
#include "core/Config.h"
#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

struct CanvasIntegrationFixture
{
    EventBus bus;
    ArtifactRegistry registry{bus};
    Config config;
    ArtifactCreationService creation{bus, registry, config};
    CanvasArtifactLifecycle lifecycle{bus, registry, creation, config};
    CanvasEventBridge event_bridge{bus};
    CanvasShellIntegration shell{registry};
};

// ============================================================================
// Full create → save → shell flow
// ============================================================================

TEST_CASE("Integration: Create → Focus → Save → Shell", "[v20][canvas-integration]")
{
    CanvasIntegrationFixture fix;

    // 1. Create board
    CanvasPlacementContext ctx;
    ctx.entry_point = "palette";
    auto result = fix.lifecycle.create_board(ctx, "Design Board");
    REQUIRE(result.ok());

    // 2. Focus canvas
    CanvasContext canvas_ctx;
    canvas_ctx.active_board_id = result.artifact_id;
    canvas_ctx.active_tool = "select";
    canvas_ctx.is_canvas_focused = true;
    fix.event_bridge.set_context(canvas_ctx);
    REQUIRE(fix.event_bridge.is_canvas_active());
    REQUIRE(fix.event_bridge.should_save_canvas());

    // 3. Save
    auto save_result = fix.lifecycle.save_as(result.artifact_id, "/workspace/design.canvas");
    REQUIRE(save_result.ok());

    // 4. Verify shell sees it
    auto tabs = fix.shell.build_tab_descriptors();
    REQUIRE(tabs.size() == 1);
    REQUIRE(tabs[0].display_name == "Design Board"); // promote_to_saved preserves name
    REQUIRE(fix.shell.tab_count() == 1);
}

// ============================================================================
// Open → Focus → Rename
// ============================================================================

TEST_CASE("Integration: Open → Focus → Rename", "[v20][canvas-integration]")
{
    CanvasIntegrationFixture fix;

    auto result = fix.lifecycle.open_board("/workspace/my-board.canvas");
    REQUIRE(result.ok());

    // Focus
    CanvasContext ctx;
    ctx.active_board_id = result.artifact_id;
    ctx.is_canvas_focused = true;
    fix.event_bridge.set_context(ctx);

    // Rename
    auto rename_result = fix.lifecycle.rename(result.artifact_id, "Renamed Board");
    REQUIRE(rename_result.ok());
    REQUIRE(fix.registry.find(result.artifact_id)->display_name == "Renamed Board");
}

// ============================================================================
// Context switch: editor ↔ canvas
// ============================================================================

TEST_CASE("Integration: Context switch editor ↔ canvas", "[v20][canvas-integration]")
{
    CanvasIntegrationFixture fix;

    // Start in editor mode
    REQUIRE_FALSE(fix.event_bridge.is_canvas_active());

    // Create and focus canvas
    CanvasPlacementContext placement;
    placement.entry_point = "menu";
    auto board = fix.lifecycle.create_board(placement, "Board");

    CanvasContext ctx;
    ctx.active_board_id = board.artifact_id;
    ctx.is_canvas_focused = true;
    fix.event_bridge.set_context(ctx);
    REQUIRE(fix.event_bridge.is_canvas_active());
    REQUIRE(fix.event_bridge.should_save_canvas());

    // Switch back to editor
    fix.event_bridge.clear_context();
    REQUIRE_FALSE(fix.event_bridge.is_canvas_active());
}

// ============================================================================
// Duplicate with shell
// ============================================================================

TEST_CASE("Integration: Duplicate shows in shell", "[v20][canvas-integration]")
{
    CanvasIntegrationFixture fix;

    CanvasPlacementContext ctx;
    ctx.entry_point = "menu";
    auto original = fix.lifecycle.create_board(ctx, "Original Board");

    auto dup = fix.lifecycle.duplicate(original.artifact_id);
    REQUIRE(dup.ok());

    // Both should appear in tabs
    REQUIRE(fix.shell.tab_count() == 2);

    auto tabs = fix.shell.build_tab_descriptors();
    REQUIRE(tabs.size() == 2);
}

// ============================================================================
// Multiple boards
// ============================================================================

TEST_CASE("Integration: Multiple boards in session", "[v20][canvas-integration]")
{
    CanvasIntegrationFixture fix;

    CanvasPlacementContext ctx;
    ctx.entry_point = "menu";

    fix.lifecycle.create_board(ctx, "Board A");
    fix.lifecycle.create_board(ctx, "Board B");
    auto opened = fix.lifecycle.open_board("/workspace/existing.canvas");

    REQUIRE(fix.lifecycle.open_board_count() == 3);
    REQUIRE(fix.lifecycle.unsaved_board_count() == 2);
    REQUIRE(fix.shell.tab_count() == 3);

    // Close one
    fix.lifecycle.close(opened.artifact_id);
    REQUIRE(fix.lifecycle.open_board_count() == 2);
    REQUIRE(fix.shell.tab_count() == 2);
}

// ============================================================================
// Shared bus verification
// ============================================================================

TEST_CASE("Integration: Bridge uses shared bus", "[v20][canvas-integration]")
{
    CanvasIntegrationFixture fix;

    // The bridge must use the shared bus, not a private one
    REQUIRE(fix.event_bridge.is_using_shared_bus());
    REQUIRE(&fix.event_bridge.shared_bus() == &fix.bus);
}

// ============================================================================
// Tree visibility
// ============================================================================

TEST_CASE("Integration: Saved boards visible in tree", "[v20][canvas-integration]")
{
    CanvasIntegrationFixture fix;

    CanvasPlacementContext ctx;
    ctx.entry_point = "menu";
    auto unsaved = fix.lifecycle.create_board(ctx, "Unsaved");
    auto saved = fix.lifecycle.open_board("/workspace/saved.canvas");

    // Only saved board should be visible in tree
    REQUIRE_FALSE(fix.shell.is_visible_in_tree(unsaved.artifact_id));
    REQUIRE(fix.shell.is_visible_in_tree(saved.artifact_id));

    auto nodes = fix.shell.build_tree_nodes();
    REQUIRE(nodes.size() == 2); // Both exist as nodes
}

// ============================================================================
// Active board tab integration
// ============================================================================

TEST_CASE("Integration: Active board tab follows registry", "[v20][canvas-integration]")
{
    CanvasIntegrationFixture fix;

    CanvasPlacementContext ctx;
    ctx.entry_point = "menu";
    auto b1 = fix.lifecycle.create_board(ctx, "Board 1");
    auto b2 = fix.lifecycle.create_board(ctx, "Board 2");

    // Last created is active
    auto tab = fix.shell.active_board_tab();
    REQUIRE(tab.has_value());
    REQUIRE(tab->artifact_id == b2.artifact_id);

    // Switch active
    fix.registry.set_active_artifact(b1.artifact_id);
    tab = fix.shell.active_board_tab();
    REQUIRE(tab.has_value());
    REQUIRE(tab->artifact_id == b1.artifact_id);
}
