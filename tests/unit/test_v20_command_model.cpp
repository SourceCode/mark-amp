/// @file test_v20_command_model.cpp
/// @brief V20 Phase 06 – CommandModel unit tests.

#include "core/CommandModel.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("CommandModel: construction", "[v20][command-model]")
{
    EventBus bus;
    CommandModel model(bus);
    REQUIRE(model.command_count() == 0);
    REQUIRE(model.execution_count() == 0);
}

TEST_CASE("CommandModel: register and find", "[v20][command-model]")
{
    EventBus bus;
    CommandModel model(bus);

    CommandDescriptor desc;
    desc.id = "workbench.action.files.newFile";
    desc.label = "New File";
    desc.category = CommandCategory::kFile;
    desc.shortcut = "Cmd+N";

    model.register_command(desc, [](const std::string&) {
        CommandResult r;
        r.success = true;
        return r;
    });

    REQUIRE(model.command_count() == 1);

    const auto* found = model.find_command("workbench.action.files.newFile");
    REQUIRE(found != nullptr);
    REQUIRE(found->label == "New File");
    REQUIRE(found->has_shortcut());
}

TEST_CASE("CommandModel: execute success", "[v20][command-model]")
{
    EventBus bus;
    CommandModel model(bus);

    CommandDescriptor desc;
    desc.id = "test.command";
    desc.label = "Test";
    model.register_command(desc, [](const std::string& source) {
        CommandResult r;
        r.success = true;
        r.source = source;
        return r;
    });

    auto result = model.execute("test.command", "palette");
    REQUIRE(result.ok());
    REQUIRE(result.source == "palette");
    REQUIRE(model.execution_count() == 1);
}

TEST_CASE("CommandModel: execute unknown command", "[v20][command-model]")
{
    EventBus bus;
    CommandModel model(bus);
    auto result = model.execute("nonexistent", "menu");
    REQUIRE_FALSE(result.ok());
}

TEST_CASE("CommandModel: disabled command", "[v20][command-model]")
{
    EventBus bus;
    CommandModel model(bus);

    CommandDescriptor desc;
    desc.id = "disabled.cmd";
    desc.label = "Disabled";
    desc.is_enabled = false;

    model.register_command(desc, [](const std::string&) {
        CommandResult r;
        r.success = true;
        return r;
    });

    auto result = model.execute("disabled.cmd", "shortcut");
    REQUIRE_FALSE(result.ok());
    REQUIRE_FALSE(model.is_enabled("disabled.cmd"));
}

TEST_CASE("CommandModel: list by category", "[v20][command-model]")
{
    EventBus bus;
    CommandModel model(bus);

    auto reg = [&](const std::string& id, CommandCategory cat) {
        CommandDescriptor d;
        d.id = id;
        d.label = id;
        d.category = cat;
        model.register_command(d, [](const std::string&) { return CommandResult{}; });
    };

    reg("file.new", CommandCategory::kFile);
    reg("file.save", CommandCategory::kFile);
    reg("edit.undo", CommandCategory::kEdit);
    reg("canvas.draw", CommandCategory::kCanvas);

    auto file_cmds = model.commands_by_category(CommandCategory::kFile);
    REQUIRE(file_cmds.size() == 2);

    auto all = model.all_commands();
    REQUIRE(all.size() == 4);
}

TEST_CASE("Phase 06 events: CommandExecutedEvent", "[v20][command-events]")
{
    events::CanonicalCommandExecutedEvent evt;
    evt.command_id = "test.save";
    evt.source = "shortcut";
    evt.success = true;
    REQUIRE(evt.success);
}
