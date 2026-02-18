/// @file SidebarCommandProvider.cpp
/// @brief V9 Phase 41 — SidebarCommandProvider implementation.

#include "SidebarCommandProvider.h"

namespace markamp::core
{

void SidebarCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto SidebarCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "sidebar.toggleLeft",
        "sidebar.toggleRight",
        "sidebar.showPanel",
        "sidebar.hidePanel",
        "sidebar.pinPanel",
        "sidebar.unpinPanel",
        "sidebar.movePanel",
        "sidebar.resetPanels",
    };
}

auto SidebarCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
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

auto SidebarCommandProvider::command_count() -> int
{
    return 8;
}

auto SidebarCommandProvider::build_entries() -> std::vector<CommandEntry>
{
    std::vector<CommandEntry> entries;
    entries.reserve(8);

    {
        CommandEntry cmd;
        cmd.id = "sidebar.toggleLeft";
        cmd.title = "Toggle Left Sidebar";
        cmd.category = "Sidebar";
        cmd.description = "Show or hide the left sidebar";
        cmd.shortcut = "Cmd+B";
        cmd.icon = "sidebar-left";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "sidebar.toggleRight";
        cmd.title = "Toggle Right Sidebar";
        cmd.category = "Sidebar";
        cmd.description = "Show or hide the right sidebar";
        cmd.shortcut = "Cmd+Shift+B";
        cmd.icon = "sidebar-right";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "sidebar.showPanel";
        cmd.title = "Show Sidebar Panel";
        cmd.category = "Sidebar";
        cmd.description = "Show a specific sidebar panel";
        cmd.icon = "panel";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "sidebar.hidePanel";
        cmd.title = "Hide Sidebar Panel";
        cmd.category = "Sidebar";
        cmd.description = "Hide a specific sidebar panel";
        cmd.icon = "eye-closed";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "sidebar.pinPanel";
        cmd.title = "Pin Sidebar Panel";
        cmd.category = "Sidebar";
        cmd.description = "Pin a panel to always show";
        cmd.icon = "pin";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "sidebar.unpinPanel";
        cmd.title = "Unpin Sidebar Panel";
        cmd.category = "Sidebar";
        cmd.description = "Unpin a pinned panel";
        cmd.icon = "unpin";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "sidebar.movePanel";
        cmd.title = "Move Panel Location";
        cmd.category = "Sidebar";
        cmd.description = "Move a panel to a different sidebar";
        cmd.icon = "move";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "sidebar.resetPanels";
        cmd.title = "Reset All Panels";
        cmd.category = "Sidebar";
        cmd.description = "Reset all panels to default positions";
        cmd.icon = "refresh";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    return entries;
}

} // namespace markamp::core
