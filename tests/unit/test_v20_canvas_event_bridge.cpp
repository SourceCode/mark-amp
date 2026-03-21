/// @file test_v20_canvas_event_bridge.cpp
/// @brief V20 Phase 04 – CanvasEventBridge unit tests.

#include "core/CanvasEventBridge.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// Construction
// ============================================================================

TEST_CASE("CanvasEventBridge: construction", "[v20][canvas-bridge]")
{
    EventBus bus;
    CanvasEventBridge bridge(bus);

    REQUIRE(bridge.update_count() == 0);
    REQUIRE_FALSE(bridge.is_canvas_active());
    REQUIRE(bridge.active_board_id().empty());
    REQUIRE(bridge.is_using_shared_bus());
}

// ============================================================================
// Set context
// ============================================================================

TEST_CASE("CanvasEventBridge: set context", "[v20][canvas-bridge]")
{
    EventBus bus;
    CanvasEventBridge bridge(bus);

    CanvasContext ctx;
    ctx.active_board_id = ArtifactId{"board-1"};
    ctx.active_tool = "select";
    ctx.selected_object_count = 3;
    ctx.is_canvas_focused = true;

    bridge.set_context(ctx);

    REQUIRE(bridge.is_canvas_active());
    REQUIRE(bridge.active_board_id() == ArtifactId{"board-1"});
    REQUIRE(bridge.should_save_canvas());
    REQUIRE(bridge.update_count() == 1);

    const auto& current = bridge.context();
    REQUIRE(current.active_tool == "select");
    REQUIRE(current.selected_object_count == 3);
}

// ============================================================================
// Clear context
// ============================================================================

TEST_CASE("CanvasEventBridge: clear context", "[v20][canvas-bridge]")
{
    EventBus bus;
    CanvasEventBridge bridge(bus);

    CanvasContext ctx;
    ctx.active_board_id = ArtifactId{"board-1"};
    ctx.is_canvas_focused = true;
    bridge.set_context(ctx);

    bridge.clear_context();

    REQUIRE_FALSE(bridge.is_canvas_active());
    REQUIRE(bridge.active_board_id().empty());
    REQUIRE_FALSE(bridge.should_save_canvas());
    REQUIRE(bridge.update_count() == 2);
}

// ============================================================================
// Shared bus reference
// ============================================================================

TEST_CASE("CanvasEventBridge: shared bus reference", "[v20][canvas-bridge]")
{
    EventBus bus;
    CanvasEventBridge bridge(bus);

    // Bridge should return the shared bus
    REQUIRE(&bridge.shared_bus() == &bus);
    REQUIRE(bridge.is_using_shared_bus());
}

// ============================================================================
// Save targeting
// ============================================================================

TEST_CASE("CanvasEventBridge: save targets canvas when focused", "[v20][canvas-bridge]")
{
    EventBus bus;
    CanvasEventBridge bridge(bus);

    // Not focused
    CanvasContext unfocused;
    unfocused.active_board_id = ArtifactId{"board-1"};
    unfocused.is_canvas_focused = false;
    bridge.set_context(unfocused);
    REQUIRE_FALSE(bridge.is_canvas_active());

    // Focused
    CanvasContext focused;
    focused.active_board_id = ArtifactId{"board-1"};
    focused.is_canvas_focused = true;
    bridge.set_context(focused);
    REQUIRE(bridge.is_canvas_active());
    REQUIRE(bridge.should_save_canvas());
}

// ============================================================================
// CanvasContext helpers
// ============================================================================

TEST_CASE("CanvasContext: has_active_board", "[v20][canvas-bridge]")
{
    CanvasContext ctx;
    REQUIRE_FALSE(ctx.has_active_board());

    ctx.active_board_id = ArtifactId{"board-1"};
    REQUIRE(ctx.has_active_board());
}

// ============================================================================
// Phase 04 events
// ============================================================================

TEST_CASE("Phase 04 events: CanvasArtifactCreatedEvent", "[v20][canvas-events]")
{
    events::CanvasArtifactCreatedEvent evt;
    evt.artifact_id = "board-1";
    evt.board_id = "board-1";
    evt.board_name = "My Board";
    evt.creation_source = "palette";

    REQUIRE(evt.board_name == "My Board");
    REQUIRE(evt.creation_source == "palette");
}

TEST_CASE("Phase 04 events: CanvasContextChangedEvent", "[v20][canvas-events]")
{
    events::CanvasContextChangedEvent evt;
    evt.artifact_id = "board-1";
    evt.active_tool = "draw";
    evt.selected_object_count = 5;
    evt.is_focused = true;

    REQUIRE(evt.active_tool == "draw");
    REQUIRE(evt.selected_object_count == 5);
}

TEST_CASE("Phase 04 events: CanvasEventBusUnifiedEvent", "[v20][canvas-events]")
{
    events::CanvasEventBusUnifiedEvent evt;
    evt.artifact_id = "board-1";
    evt.using_shared_bus = true;

    REQUIRE(evt.using_shared_bus);
}

TEST_CASE("Phase 04 events: CanvasShellTabChangedEvent", "[v20][canvas-events]")
{
    events::CanvasShellTabChangedEvent evt;
    evt.artifact_id = "board-1";
    evt.board_name = "Design Board";
    evt.is_dirty = true;
    evt.is_active = true;

    REQUIRE(evt.is_dirty);
    REQUIRE(evt.is_active);
}
