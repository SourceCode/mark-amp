#include "core/AppState.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("AppState: default values", "[core][appstate]")
{
    AppState state;

    REQUIRE(state.active_file_id.empty());
    REQUIRE(state.view_mode == events::ViewMode::Split);
    REQUIRE(state.sidebar_visible == true);
    REQUIRE(state.current_theme_id == "midnight-neon");
    REQUIRE(state.cursor_line == 1);
    REQUIRE(state.cursor_column == 1);
    REQUIRE(state.encoding == "UTF-8");
}

TEST_CASE("AppState: active_file_name returns basename", "[core][appstate]")
{
    AppState state;
    state.active_file_id = "/path/to/readme.md";

    REQUIRE(state.active_file_name() == "readme.md");
}

TEST_CASE("AppState: active_file_name returns Untitled when empty", "[core][appstate]")
{
    AppState state;
    REQUIRE(state.active_file_name() == "Untitled");
}

TEST_CASE("AppStateManager: set_active_file publishes event", "[core][appstate]")
{
    EventBus bus;
    AppStateManager manager(bus);
    std::string received_id;

    auto sub = bus.subscribe<events::ActiveFileChangedEvent>(
        [&](const events::ActiveFileChangedEvent& e) { received_id = e.file_id; });

    manager.set_active_file("test.md");

    REQUIRE(manager.state().active_file_id == "test.md");
    REQUIRE(received_id == "test.md");
}

TEST_CASE("AppStateManager: set_view_mode publishes event", "[core][appstate]")
{
    EventBus bus;
    AppStateManager manager(bus);
    events::ViewMode received_mode = events::ViewMode::Editor;

    auto sub = bus.subscribe<events::ViewModeChangedEvent>(
        [&](const events::ViewModeChangedEvent& e) { received_mode = e.mode; });

    manager.set_view_mode(events::ViewMode::Preview);

    REQUIRE(manager.state().view_mode == events::ViewMode::Preview);
    REQUIRE(received_mode == events::ViewMode::Preview);
}

TEST_CASE("AppStateManager: set_cursor_position publishes event", "[core][appstate]")
{
    EventBus bus;
    AppStateManager manager(bus);
    int received_line = 0;
    int received_col = 0;

    auto sub = bus.subscribe<events::CursorPositionChangedEvent>(
        [&](const events::CursorPositionChangedEvent& e)
        {
            received_line = e.line;
            received_col = e.column;
        });

    manager.set_cursor_position(10, 5);

    REQUIRE(manager.state().cursor_line == 10);
    REQUIRE(manager.state().cursor_column == 5);
    REQUIRE(received_line == 10);
    REQUIRE(received_col == 5);
}
