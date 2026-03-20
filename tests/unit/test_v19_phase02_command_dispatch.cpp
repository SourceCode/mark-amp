/// @file test_v19_phase02_command_dispatch.cpp
/// @brief V19 Phase 02 tests: CommandDispatchAdapter dispatch, source tracking,
///        feedback routing, and PromptHost mock responder.

#include "core/CommandDispatchAdapter.h"
#include "core/CommandFeedback.h"
#include "core/CommandRegistry.h"
#include "core/EventBus.h"
#include "ui/PromptHost.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// =============================================================================
// Helper: register a test command
// =============================================================================
static void register_test_command(CommandRegistry& registry,
                                   const std::string& cmd_id,
                                   bool should_succeed = true)
{
    CommandEntry entry;
    entry.id = cmd_id;
    entry.title = "Test: " + cmd_id;
    entry.category = "Test";
    entry.execute_fn = [should_succeed]() -> bool { return should_succeed; };
    registry.register_command(std::move(entry));
}

// =============================================================================
// CommandDispatchAdapter — basic dispatch
// =============================================================================

TEST_CASE("CommandDispatchAdapter dispatches existing command", "[v19][phase02][dispatch]")
{
    EventBus bus;
    CommandRegistry registry;
    CommandFeedbackHelper feedback(bus);
    register_test_command(registry, "test.echo");

    CommandDispatchAdapter adapter(registry, feedback, bus);
    auto result = adapter.dispatch("test.echo", CommandSource::kPalette);

    REQUIRE(result == CommandResult::kSuccess);
    REQUIRE(adapter.dispatch_count() == 1);
    REQUIRE(adapter.error_count() == 0);
}

TEST_CASE("CommandDispatchAdapter returns kError for unknown command",
          "[v19][phase02][dispatch]")
{
    EventBus bus;
    CommandRegistry registry;
    CommandFeedbackHelper feedback(bus);

    CommandDispatchAdapter adapter(registry, feedback, bus);
    auto result = adapter.dispatch("nonexistent.command", CommandSource::kShortcut);

    REQUIRE(result == CommandResult::kError);
    REQUIRE(adapter.dispatch_count() == 1);
    REQUIRE(adapter.error_count() == 1);
}

TEST_CASE("CommandDispatchAdapter returns kError when command execution fails",
          "[v19][phase02][dispatch]")
{
    EventBus bus;
    CommandRegistry registry;
    CommandFeedbackHelper feedback(bus);
    register_test_command(registry, "test.fail", false);

    CommandDispatchAdapter adapter(registry, feedback, bus);
    auto result = adapter.dispatch("test.fail", CommandSource::kMenu);

    REQUIRE(result == CommandResult::kError);
    REQUIRE(adapter.error_count() == 1);
}

// =============================================================================
// CommandDispatchAdapter — dispatch history
// =============================================================================

TEST_CASE("CommandDispatchAdapter records dispatch history", "[v19][phase02][dispatch][history]")
{
    EventBus bus;
    CommandRegistry registry;
    CommandFeedbackHelper feedback(bus);
    register_test_command(registry, "test.a");
    register_test_command(registry, "test.b");

    CommandDispatchAdapter adapter(registry, feedback, bus);
    adapter.dispatch("test.a", CommandSource::kShortcut);
    adapter.dispatch("test.b", CommandSource::kPalette);

    auto history = adapter.recent_dispatches(10);
    REQUIRE(history.size() == 2);
    REQUIRE(history[0].command_id == "test.a");
    REQUIRE(history[0].source == CommandSource::kShortcut);
    REQUIRE(history[1].command_id == "test.b");
    REQUIRE(history[1].source == CommandSource::kPalette);
}

// =============================================================================
// CommandDispatchAdapter — has_command
// =============================================================================

TEST_CASE("CommandDispatchAdapter has_command checks registry", "[v19][phase02][dispatch]")
{
    EventBus bus;
    CommandRegistry registry;
    CommandFeedbackHelper feedback(bus);
    register_test_command(registry, "test.exists");

    CommandDispatchAdapter adapter(registry, feedback, bus);
    REQUIRE(adapter.has_command("test.exists"));
    REQUIRE_FALSE(adapter.has_command("test.missing"));
}

// =============================================================================
// CommandDispatchAdapter — when-clause context filtering
// =============================================================================

TEST_CASE("dispatch_with_context returns kDisabled when when-clause fails",
          "[v19][phase02][dispatch][context]")
{
    EventBus bus;
    CommandRegistry registry;
    CommandFeedbackHelper feedback(bus);

    CommandEntry entry;
    entry.id = "editor.format";
    entry.title = "Format";
    entry.category = "Editor";
    entry.when_clause = "editorHasSelection";
    entry.execute_fn = []() -> bool { return true; };
    registry.register_command(std::move(entry));

    ContextKeyService context;
    // Don't set editorHasSelection — it should be false

    CommandDispatchAdapter adapter(registry, feedback, bus);
    auto result = adapter.dispatch_with_context("editor.format", CommandSource::kShortcut, context);

    REQUIRE(result == CommandResult::kDisabled);
}

// =============================================================================
// PromptHost — mock mode
// =============================================================================

TEST_CASE("PromptHost is not in mock mode by default", "[v19][phase02][prompt]")
{
    EventBus bus;
    PromptHost host(bus);

    REQUIRE_FALSE(host.is_mock_mode());
    REQUIRE(host.prompt_count() == 0);
}

TEST_CASE("PromptHost mock responder intercepts prompts", "[v19][phase02][prompt]")
{
    EventBus bus;
    PromptHost host(bus);

    host.set_mock_responder([](const PromptConfig& /*cfg*/) -> PromptResult
    {
        PromptResult result;
        result.response = PromptResponse::kYes;
        result.button_index = 0;
        return result;
    });

    REQUIRE(host.is_mock_mode());

    auto result = host.confirm("Save?", "Do you want to save?");
    REQUIRE(result == PromptResponse::kYes);
    REQUIRE(host.prompt_count() == 1);
}

TEST_CASE("PromptHost mock input returns custom value", "[v19][phase02][prompt]")
{
    EventBus bus;
    PromptHost host(bus);

    host.set_mock_responder([](const PromptConfig& /*cfg*/) -> PromptResult
    {
        PromptResult result;
        result.response = PromptResponse::kYes;
        result.input_value = "test_name";
        return result;
    });

    auto value = host.input("Rename", "Enter new name:", "default");
    REQUIRE(value.has_value());
    REQUIRE(value.value() == "test_name");
}

TEST_CASE("PromptHost info/warning/error increment prompt count",
          "[v19][phase02][prompt]")
{
    EventBus bus;
    PromptHost host(bus);

    host.set_mock_responder([](const PromptConfig& /*cfg*/) -> PromptResult
    {
        return PromptResult{};
    });

    host.info("Info", "Message");
    host.warning("Warning", "Message");
    host.error("Error", "Message");

    REQUIRE(host.prompt_count() == 3);
}
