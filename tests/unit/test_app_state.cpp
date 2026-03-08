// test_app_state.cpp — 10 tests for AppState and AppStateManager
#include "core/AppState.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("AppState default values", "[app_state]")
{
    AppState state;
    CHECK(state.active_file_id.empty());
    CHECK(state.cursor_line == 1);
    CHECK(state.cursor_column == 1);
    CHECK(state.sidebar_visible);
    CHECK(state.encoding == "UTF-8");
}

TEST_CASE("AppState is_modified returns false by default", "[app_state]")
{
    AppState state;
    CHECK_FALSE(state.is_modified());
}

TEST_CASE("AppStateManager stores state", "[app_state]")
{
    EventBus bus;
    AppStateManager mgr(bus);
    CHECK(mgr.state().cursor_line == 1);
}

TEST_CASE("AppStateManager set_active_file updates state", "[app_state]")
{
    EventBus bus;
    AppStateManager mgr(bus);
    mgr.set_active_file("file1.md");
    CHECK(mgr.state().active_file_id == "file1.md");
}

TEST_CASE("AppStateManager set_cursor_position updates", "[app_state]")
{
    EventBus bus;
    AppStateManager mgr(bus);
    mgr.set_cursor_position(10, 5);
    CHECK(mgr.state().cursor_line == 10);
    CHECK(mgr.state().cursor_column == 5);
}

TEST_CASE("AppStateManager set_sidebar_visible updates", "[app_state]")
{
    EventBus bus;
    AppStateManager mgr(bus);
    mgr.set_sidebar_visible(false);
    CHECK_FALSE(mgr.state().sidebar_visible);
}

TEST_CASE("AppStateManager set_modified updates", "[app_state]")
{
    EventBus bus;
    AppStateManager mgr(bus);
    mgr.set_modified(true);
    CHECK(mgr.state().is_modified());
}

TEST_CASE("AppStateManager update_content updates content", "[app_state]")
{
    EventBus bus;
    AppStateManager mgr(bus);
    mgr.update_content("# Hello");
    CHECK(mgr.state().active_file_content == "# Hello");
}

TEST_CASE("AppStateManager set_view_mode updates", "[app_state]")
{
    EventBus bus;
    AppStateManager mgr(bus);
    mgr.set_view_mode(events::ViewMode::Editor);
    CHECK(mgr.state().view_mode == events::ViewMode::Editor);
}

TEST_CASE("AppState default theme is midnight-neon", "[app_state]")
{
    AppState state;
    CHECK(state.current_theme_id == "midnight-neon");
}
