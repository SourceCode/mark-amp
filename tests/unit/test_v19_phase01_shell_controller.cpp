/// @file test_v19_phase01_shell_controller.cpp
/// @brief V19 Phase 01 tests: WorkbenchShellController surface transitions,
///        startup sequencing, event wiring, and structured logging.

#include "core/CanvasWorkbenchMode.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ShellLayoutState.h"
#include "core/WorkbenchShellController.h"
#include "core/WorkspaceLoadStateModel.h"
#include "core/WorkspaceOpenOrchestrator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// =============================================================================
// Mock IShellUIDelegate for testing
// =============================================================================

struct MockUIDelegate : IShellUIDelegate
{
    int show_editor_count{0};
    int show_canvas_count{0};
    int show_startup_count{0};
    int set_mode_count{0};
    int focus_editor_count{0};
    events::WorkbenchMode last_mode{events::WorkbenchMode::kEditor};
    std::string last_workspace_root;

    void ShowEditorWorkspace() override { ++show_editor_count; }
    void ShowCanvasWorkspace() override { ++show_canvas_count; }
    void ShowStartupScreen() override { ++show_startup_count; }
    void SetWorkbenchMode(events::WorkbenchMode mode) override
    {
        last_mode = mode;
        ++set_mode_count;
    }
    void SetWorkspaceRoot(const std::string& root_path) override
    {
        last_workspace_root = root_path;
    }
    void FocusEditor() override { ++focus_editor_count; }
};

// =============================================================================
// Construction and default state
// =============================================================================

TEST_CASE("WorkbenchShellController default state is kEditor", "[v19][phase01][shell]")
{
    EventBus bus;
    Config cfg;
    WorkbenchShellController controller(bus, cfg);

    REQUIRE(controller.active_surface() == events::WorkbenchMode::kEditor);
    REQUIRE_FALSE(controller.has_workspace());
    REQUIRE(controller.workspace_root().empty());
    REQUIRE(controller.transition_count() == 0);
    REQUIRE(controller.transition_history().empty());
}

TEST_CASE("WorkbenchShellController has_delegate is false before set_ui_delegate",
          "[v19][phase01][shell]")
{
    EventBus bus;
    Config cfg;
    WorkbenchShellController controller(bus, cfg);

    REQUIRE_FALSE(controller.has_delegate());

    MockUIDelegate delegate;
    controller.set_ui_delegate(&delegate);
    REQUIRE(controller.has_delegate());
}

// =============================================================================
// Surface transitions
// =============================================================================

TEST_CASE("switch_to changes active surface", "[v19][phase01][shell]")
{
    EventBus bus;
    Config cfg;
    WorkbenchShellController controller(bus, cfg);
    MockUIDelegate delegate;
    controller.set_ui_delegate(&delegate);

    controller.switch_to(events::WorkbenchMode::kCanvas, "test");

    REQUIRE(controller.active_surface() == events::WorkbenchMode::kCanvas);
    REQUIRE(controller.transition_count() == 1);
    REQUIRE(delegate.show_canvas_count == 1);
    REQUIRE(delegate.set_mode_count == 1);
    REQUIRE(delegate.last_mode == events::WorkbenchMode::kCanvas);
}

TEST_CASE("switch_to same surface is a no-op", "[v19][phase01][shell]")
{
    EventBus bus;
    Config cfg;
    WorkbenchShellController controller(bus, cfg);
    MockUIDelegate delegate;
    controller.set_ui_delegate(&delegate);

    // Already kEditor by default
    controller.switch_to(events::WorkbenchMode::kEditor, "test");

    REQUIRE(controller.transition_count() == 0);
    REQUIRE(delegate.set_mode_count == 0);
}

TEST_CASE("switch_to publishes WorkbenchModeChangedEvent", "[v19][phase01][shell][event]")
{
    EventBus bus;
    Config cfg;
    WorkbenchShellController controller(bus, cfg);
    MockUIDelegate delegate;
    controller.set_ui_delegate(&delegate);

    events::WorkbenchMode received_from{events::WorkbenchMode::kEditor};
    events::WorkbenchMode received_to{events::WorkbenchMode::kEditor};

    auto sub = bus.subscribe<events::WorkbenchModeChangedEvent>(
        [&](const events::WorkbenchModeChangedEvent& evt)
        {
            received_from = evt.previous_mode;
            received_to = evt.new_mode;
        });

    controller.switch_to(events::WorkbenchMode::kNotebook, "test_event");

    REQUIRE(received_from == events::WorkbenchMode::kEditor);
    REQUIRE(received_to == events::WorkbenchMode::kNotebook);
}

TEST_CASE("switch_to calls ShowEditorWorkspace for kEditor", "[v19][phase01][shell]")
{
    EventBus bus;
    Config cfg;
    WorkbenchShellController controller(bus, cfg);
    MockUIDelegate delegate;
    controller.set_ui_delegate(&delegate);

    // First go to canvas
    controller.switch_to(events::WorkbenchMode::kCanvas, "setup");
    // Then back to editor
    controller.switch_to(events::WorkbenchMode::kEditor, "test");

    REQUIRE(delegate.show_editor_count == 1);
}

// =============================================================================
// Transition history
// =============================================================================

TEST_CASE("Transition history records entries in order", "[v19][phase01][shell][history]")
{
    EventBus bus;
    Config cfg;
    WorkbenchShellController controller(bus, cfg);
    MockUIDelegate delegate;
    controller.set_ui_delegate(&delegate);

    controller.switch_to(events::WorkbenchMode::kCanvas, "step1");
    controller.switch_to(events::WorkbenchMode::kNotebook, "step2");
    controller.switch_to(events::WorkbenchMode::kEditor, "step3");

    REQUIRE(controller.transition_count() == 3);
    REQUIRE(controller.transition_history().size() == 3);

    const auto& h = controller.transition_history();
    REQUIRE(h[0].from == events::WorkbenchMode::kEditor);
    REQUIRE(h[0].to == events::WorkbenchMode::kCanvas);
    REQUIRE(h[0].trigger == "step1");

    REQUIRE(h[1].from == events::WorkbenchMode::kCanvas);
    REQUIRE(h[1].to == events::WorkbenchMode::kNotebook);
    REQUIRE(h[1].trigger == "step2");

    REQUIRE(h[2].from == events::WorkbenchMode::kNotebook);
    REQUIRE(h[2].to == events::WorkbenchMode::kEditor);
    REQUIRE(h[2].trigger == "step3");
}

// =============================================================================
// Event-driven transitions
// =============================================================================

TEST_CASE("WorkbenchModeSwitchRequestEvent triggers switch_to",
          "[v19][phase01][shell][event]")
{
    EventBus bus;
    Config cfg;
    WorkbenchShellController controller(bus, cfg);
    MockUIDelegate delegate;
    controller.set_ui_delegate(&delegate);

    events::WorkbenchModeSwitchRequestEvent req;
    req.target_mode = events::WorkbenchMode::kGraph;
    bus.publish(req);

    REQUIRE(controller.active_surface() == events::WorkbenchMode::kGraph);
    REQUIRE(controller.transition_count() == 1);
}

TEST_CASE("SettingsOpenRequestEvent switches to kSettings", "[v19][phase01][shell][event]")
{
    EventBus bus;
    Config cfg;
    WorkbenchShellController controller(bus, cfg);
    MockUIDelegate delegate;
    controller.set_ui_delegate(&delegate);

    events::SettingsOpenRequestEvent settings_evt;
    bus.publish(settings_evt);

    REQUIRE(controller.active_surface() == events::WorkbenchMode::kSettings);
}

// =============================================================================
// Startup screen
// =============================================================================

TEST_CASE("show_startup clears workspace state", "[v19][phase01][shell]")
{
    EventBus bus;
    Config cfg;
    WorkbenchShellController controller(bus, cfg);
    MockUIDelegate delegate;
    controller.set_ui_delegate(&delegate);

    controller.show_startup("test");

    REQUIRE_FALSE(controller.has_workspace());
    REQUIRE(controller.workspace_root().empty());
    REQUIRE(delegate.show_startup_count == 1);
}

// =============================================================================
// File open
// =============================================================================

TEST_CASE("open_file switches to editor and publishes FileOpenedEvent",
          "[v19][phase01][shell]")
{
    EventBus bus;
    Config cfg;
    WorkbenchShellController controller(bus, cfg);
    MockUIDelegate delegate;
    controller.set_ui_delegate(&delegate);

    // Start in canvas mode
    controller.switch_to(events::WorkbenchMode::kCanvas, "setup");

    std::string received_path;
    auto sub = bus.subscribe<events::FileOpenedEvent>(
        [&](const events::FileOpenedEvent& evt)
        { received_path = evt.file_path; });

    controller.open_file("/test/file.md", "drag_drop");

    REQUIRE(controller.active_surface() == events::WorkbenchMode::kEditor);
    REQUIRE(received_path == "/test/file.md");
    REQUIRE(delegate.focus_editor_count == 1);
}

// =============================================================================
// Workspace open (without orchestrator)
// =============================================================================

TEST_CASE("open_workspace without orchestrator sets state directly",
          "[v19][phase01][shell]")
{
    EventBus bus;
    Config cfg;
    WorkbenchShellController controller(bus, cfg);
    MockUIDelegate delegate;
    controller.set_ui_delegate(&delegate);

    // Start at startup
    controller.show_startup("setup");

    controller.open_workspace("/test/workspace", "test");

    REQUIRE(controller.has_workspace());
    REQUIRE(controller.workspace_root() == "/test/workspace");
    REQUIRE(delegate.last_workspace_root == "/test/workspace");
    REQUIRE(controller.active_surface() == events::WorkbenchMode::kEditor);
}

// =============================================================================
// Canvas mode integration
// =============================================================================

TEST_CASE("switch_to kCanvas enters CanvasWorkbenchMode", "[v19][phase01][shell][canvas]")
{
    EventBus bus;
    Config cfg;
    CanvasWorkbenchMode canvas_mode(bus, cfg);
    WorkbenchShellController controller(bus, cfg);
    MockUIDelegate delegate;
    controller.set_ui_delegate(&delegate);
    controller.set_canvas_mode(&canvas_mode);

    controller.switch_to(events::WorkbenchMode::kCanvas, "test");

    REQUIRE(canvas_mode.is_active());
}

TEST_CASE("switch_to kEditor from kCanvas exits CanvasWorkbenchMode",
          "[v19][phase01][shell][canvas]")
{
    EventBus bus;
    Config cfg;
    CanvasWorkbenchMode canvas_mode(bus, cfg);
    WorkbenchShellController controller(bus, cfg);
    MockUIDelegate delegate;
    controller.set_ui_delegate(&delegate);
    controller.set_canvas_mode(&canvas_mode);

    controller.switch_to(events::WorkbenchMode::kCanvas, "setup");
    REQUIRE(canvas_mode.is_active());

    controller.switch_to(events::WorkbenchMode::kEditor, "test");
    REQUIRE_FALSE(canvas_mode.is_active());
}

// =============================================================================
// Load state model integration
// =============================================================================

TEST_CASE("open_workspace transitions load state model", "[v19][phase01][shell]")
{
    EventBus bus;
    Config cfg;
    WorkspaceLoadStateModel load_model(bus);
    WorkbenchShellController controller(bus, cfg);
    MockUIDelegate delegate;
    controller.set_ui_delegate(&delegate);
    controller.set_load_state_model(&load_model);

    controller.open_workspace("/test/workspace", "test");

    // Without orchestrator, open_workspace succeeds directly
    REQUIRE(load_model.is_ready());
}
