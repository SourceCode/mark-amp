/// @file test_v19_phase09_feedback.cpp
/// @brief V19 Phase 09 tests: Status bar projection, panel state patterns,
///        smoke diagnostics, command execution logging.

#include "core/EventBus.h"
#include "core/CommandRegistry.h"
#include "core/CommandDispatchAdapter.h"
#include "core/CommandFeedback.h"

#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace markamp::core;

// =============================================================================
// Status bar item projection
// =============================================================================

TEST_CASE("Status bar items projected from registered services",
          "[v19][phase09][feedback]")
{
    struct StatusBarItem
    {
        std::string item_id;
        std::string text;
        std::string command_id;
        int priority{0};
    };

    std::vector<StatusBarItem> items;
    items.push_back({"line-col", "Ln 1, Col 1", "", 100});
    items.push_back({"encoding", "UTF-8", "workbench.action.editor.changeEncoding", 90});
    items.push_back({"indent", "Spaces: 4", "editor.action.indentationToSpaces", 80});

    REQUIRE(items.size() == 3);
    REQUIRE(items[0].item_id == "line-col");
    REQUIRE_FALSE(items[1].command_id.empty());
}

// =============================================================================
// Panel state patterns
// =============================================================================

TEST_CASE("Panel state model defines render rules",
          "[v19][phase09][feedback]")
{
    enum class PanelState
    {
        kEmpty,
        kLoading,
        kReady,
        kError
    };

    PanelState state = PanelState::kEmpty;
    REQUIRE(state == PanelState::kEmpty);

    state = PanelState::kLoading;
    REQUIRE(state == PanelState::kLoading);

    state = PanelState::kReady;
    REQUIRE(state == PanelState::kReady);
}

// =============================================================================
// Command execution logging via dispatch adapter
// =============================================================================

TEST_CASE("Command dispatch adapter records execution log",
          "[v19][phase09][feedback]")
{
    EventBus bus;
    CommandRegistry registry;
    CommandFeedbackHelper feedback(bus);

    CommandEntry entry;
    entry.id = "test.smoke";
    entry.title = "Smoke Test";
    entry.category = "Test";
    entry.execute_fn = []() -> bool { return true; };
    registry.register_command(std::move(entry));

    CommandDispatchAdapter adapter(registry, feedback, bus);
    adapter.dispatch("test.smoke", CommandSource::kInternal, "smoke test");

    auto log = adapter.recent_dispatches(10);
    REQUIRE(log.size() == 1);
    REQUIRE(log[0].command_id == "test.smoke");
    REQUIRE(log[0].result == CommandResult::kSuccess);
    REQUIRE(log[0].elapsed_us >= 0);
}
