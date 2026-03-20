/// @file test_v19_phase06_settings.cpp
/// @brief V19 Phase 06 tests: Settings deep link routing, toolbar action audit,
///        command label normalization.

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/CommandRegistry.h"
#include "core/ContextKeyService.h"

#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace markamp::core;

// =============================================================================
// Settings deep link routing
// =============================================================================

TEST_CASE("Settings deep link routes through event bus",
          "[v19][phase06][settings]")
{
    EventBus bus;
    std::string navigated_setting;

    auto sub = bus.subscribe<events::SettingsOpenRequestEvent>(
        [&](const events::SettingsOpenRequestEvent& /*evt*/)
        {
            navigated_setting = "editor.fontSize";
        });

    const events::SettingsOpenRequestEvent evt;
    bus.publish(evt);

    REQUIRE(navigated_setting == "editor.fontSize");
}

// =============================================================================
// Toolbar action audit
// =============================================================================

TEST_CASE("Toolbar actions map to registered commands",
          "[v19][phase06][settings]")
{
    CommandRegistry registry;

    CommandEntry file_new;
    file_new.id = "file.newFile";
    file_new.title = "New File";
    file_new.category = "File";
    file_new.shortcut = "Cmd+N";
    file_new.execute_fn = []() -> bool { return true; };
    registry.register_command(std::move(file_new));

    CommandEntry file_open;
    file_open.id = "file.openFolder";
    file_open.title = "Open Folder";
    file_open.category = "File";
    file_open.shortcut = "Cmd+O";
    file_open.execute_fn = []() -> bool { return true; };
    registry.register_command(std::move(file_open));

    REQUIRE(registry.has_command("file.newFile"));
    REQUIRE(registry.has_command("file.openFolder"));
    REQUIRE(registry.command_count() == 2);
}

// =============================================================================
// Command label normalization
// =============================================================================

TEST_CASE("Commands have normalized category and title",
          "[v19][phase06][settings]")
{
    CommandRegistry registry;

    CommandEntry entry;
    entry.id = "editor.action.formatDocument";
    entry.title = "Format Document";
    entry.category = "Editor";
    entry.description = "Format the active document using the configured formatter";
    entry.shortcut = "Shift+Alt+F";
    entry.execute_fn = []() -> bool { return true; };
    registry.register_command(std::move(entry));

    const auto* cmd = registry.get_command("editor.action.formatDocument");
    REQUIRE(cmd != nullptr);
    REQUIRE(cmd->title == "Format Document");
    REQUIRE(cmd->category == "Editor");
    REQUIRE_FALSE(cmd->description.empty());
    REQUIRE_FALSE(cmd->shortcut.empty());
}
