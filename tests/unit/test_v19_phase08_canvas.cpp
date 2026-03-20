/// @file test_v19_phase08_canvas.cpp
/// @brief V19 Phase 08 tests: Canvas event bus sharing, board open flow,
///        cross-surface navigation.

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/Config.h"
#include "core/CanvasWorkbenchMode.h"

#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace markamp::core;

// =============================================================================
// Canvas event bus sharing
// =============================================================================

TEST_CASE("Canvas uses shared app event bus",
          "[v19][phase08][canvas]")
{
    EventBus shared_bus;
    bool canvas_event_received = false;

    auto sub = shared_bus.subscribe<events::WorkbenchModeSwitchRequestEvent>(
        [&](const events::WorkbenchModeSwitchRequestEvent& mode_evt)
        {
            if (mode_evt.target_mode == events::WorkbenchMode::kCanvas)
            {
                canvas_event_received = true;
            }
        });

    events::WorkbenchModeSwitchRequestEvent req;
    req.target_mode = events::WorkbenchMode::kCanvas;
    shared_bus.publish(req);

    REQUIRE(canvas_event_received);
}

// =============================================================================
// Canvas workbench mode — board open flow
// =============================================================================

TEST_CASE("CanvasWorkbenchMode enter/exit lifecycle",
          "[v19][phase08][canvas]")
{
    EventBus bus;
    Config cfg;
    CanvasWorkbenchMode canvas_mode(bus, cfg);

    REQUIRE_FALSE(canvas_mode.is_active());

    canvas_mode.enter();
    REQUIRE(canvas_mode.is_active());

    canvas_mode.exit();
    REQUIRE_FALSE(canvas_mode.is_active());
}

// =============================================================================
// Cross-surface navigation
// =============================================================================

TEST_CASE("Cross-surface navigation transitions canvas to editor",
          "[v19][phase08][canvas]")
{
    EventBus bus;
    events::WorkbenchMode current_mode = events::WorkbenchMode::kCanvas;

    auto sub = bus.subscribe<events::WorkbenchModeChangedEvent>(
        [&](const events::WorkbenchModeChangedEvent& change_evt)
        {
            current_mode = change_evt.new_mode;
        });

    events::WorkbenchModeChangedEvent mode_change;
    mode_change.previous_mode = events::WorkbenchMode::kCanvas;
    mode_change.new_mode = events::WorkbenchMode::kEditor;
    bus.publish(mode_change);

    REQUIRE(current_mode == events::WorkbenchMode::kEditor);
}
