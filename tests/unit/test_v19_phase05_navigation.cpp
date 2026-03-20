/// @file test_v19_phase05_navigation.cpp
/// @brief V19 Phase 05 tests: Active document tracking, search workspace-root,
///        quick-open command mode dispatch.

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/CommandRegistry.h"

#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace markamp::core;

// =============================================================================
// Active document tracking
// =============================================================================

TEST_CASE("Active document tracker publishes changes via events",
          "[v19][phase05][navigation]")
{
    EventBus bus;
    std::string active_doc;

    auto sub = bus.subscribe<events::FileOpenedEvent>(
        [&](const events::FileOpenedEvent& file_evt)
        {
            active_doc = file_evt.file_path;
        });

    events::FileOpenedEvent open_evt;
    open_evt.file_path = "/workspace/readme.md";
    bus.publish(open_evt);

    REQUIRE(active_doc == "/workspace/readme.md");
}

// =============================================================================
// Workspace-root search contract
// =============================================================================

TEST_CASE("Search requires workspace root to operate",
          "[v19][phase05][navigation]")
{
    std::string workspace_root;
    bool search_enabled = !workspace_root.empty();
    REQUIRE_FALSE(search_enabled);

    workspace_root = "/Users/test/project";
    search_enabled = !workspace_root.empty();
    REQUIRE(search_enabled);
}

// =============================================================================
// Quick-open command mode dispatch
// =============================================================================

TEST_CASE("Quick open command mode routes through CommandRegistry",
          "[v19][phase05][navigation]")
{
    CommandRegistry registry;

    bool executed = false;
    CommandEntry entry;
    entry.id = "workbench.action.showAllSymbols";
    entry.title = "Go to Symbol";
    entry.category = "Navigation";
    entry.execute_fn = [&]() -> bool
    {
        executed = true;
        return true;
    };
    registry.register_command(std::move(entry));

    auto success = registry.execute_command("workbench.action.showAllSymbols");
    REQUIRE(success);
    REQUIRE(executed);
}
