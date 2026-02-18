/// @file HooksCommandProvider.cpp
/// @brief V9 Phase 40 — HooksCommandProvider implementation.

#include "HooksCommandProvider.h"

namespace markamp::core
{

void HooksCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto HooksCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "hooks.showAll",
        "hooks.registerHook",
        "hooks.unregisterHook",
        "hooks.toggleHook",
        "hooks.fireHook",
        "hooks.addFileWatch",
        "hooks.removeFileWatch",
        "hooks.showScheduledTasks",
    };
}

auto HooksCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
{
    for (auto& entry : build_entries())
    {
        if (entry.id == command_id)
        {
            return entry;
        }
    }
    return {};
}

auto HooksCommandProvider::command_count() -> int
{
    return 8;
}

auto HooksCommandProvider::build_entries() -> std::vector<CommandEntry>
{
    std::vector<CommandEntry> entries;
    entries.reserve(8);

    {
        CommandEntry cmd;
        cmd.id = "hooks.showAll";
        cmd.title = "Show All Hooks";
        cmd.category = "Hooks";
        cmd.description = "View all registered workspace hooks";
        cmd.shortcut = "Cmd+Shift+H";
        cmd.icon = "webhook";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "hooks.registerHook";
        cmd.title = "Register Hook";
        cmd.category = "Hooks";
        cmd.description = "Register a new workspace lifecycle hook";
        cmd.icon = "add";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "hooks.unregisterHook";
        cmd.title = "Unregister Hook";
        cmd.category = "Hooks";
        cmd.description = "Remove a registered hook";
        cmd.icon = "remove";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "hooks.toggleHook";
        cmd.title = "Toggle Hook";
        cmd.category = "Hooks";
        cmd.description = "Enable or disable a hook";
        cmd.icon = "toggle";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "hooks.fireHook";
        cmd.title = "Fire Hook Manually";
        cmd.category = "Hooks";
        cmd.description = "Manually trigger a hook for testing";
        cmd.icon = "play";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "hooks.addFileWatch";
        cmd.title = "Add File Watch";
        cmd.category = "Hooks";
        cmd.description = "Add a file system watch on a directory";
        cmd.icon = "eye";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "hooks.removeFileWatch";
        cmd.title = "Remove File Watch";
        cmd.category = "Hooks";
        cmd.description = "Remove an active file watch";
        cmd.icon = "eye-closed";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "hooks.showScheduledTasks";
        cmd.title = "Show Scheduled Tasks";
        cmd.category = "Hooks";
        cmd.description = "View all scheduled and running tasks";
        cmd.icon = "clock";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    return entries;
}

} // namespace markamp::core
