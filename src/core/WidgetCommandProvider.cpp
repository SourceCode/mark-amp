/// @file WidgetCommandProvider.cpp
/// @brief V9 Phase 41 — WidgetCommandProvider implementation.

#include "WidgetCommandProvider.h"

namespace markamp::core
{

void WidgetCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto WidgetCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "widgets.showAll",
        "widgets.addWidget",
        "widgets.removeWidget",
        "widgets.toggleCollapse",
        "widgets.resetLayout",
        "widgets.refreshData",
        "widgets.moveWidget",
        "widgets.configureWidget",
    };
}

auto WidgetCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
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

auto WidgetCommandProvider::command_count() -> int
{
    return 8;
}

auto WidgetCommandProvider::build_entries() -> std::vector<CommandEntry>
{
    std::vector<CommandEntry> entries;
    entries.reserve(8);

    {
        CommandEntry cmd;
        cmd.id = "widgets.showAll";
        cmd.title = "Show All Widgets";
        cmd.category = "Widgets";
        cmd.description = "View all registered sidebar widgets";
        cmd.icon = "grid";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "widgets.addWidget";
        cmd.title = "Add Widget";
        cmd.category = "Widgets";
        cmd.description = "Add a widget to the sidebar";
        cmd.icon = "add";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "widgets.removeWidget";
        cmd.title = "Remove Widget";
        cmd.category = "Widgets";
        cmd.description = "Remove a widget from the sidebar";
        cmd.icon = "trash";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "widgets.toggleCollapse";
        cmd.title = "Toggle Widget Collapse";
        cmd.category = "Widgets";
        cmd.description = "Expand or collapse a widget";
        cmd.icon = "chevron";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "widgets.resetLayout";
        cmd.title = "Reset Widget Layout";
        cmd.category = "Widgets";
        cmd.description = "Reset all widgets to default positions";
        cmd.icon = "refresh";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "widgets.refreshData";
        cmd.title = "Refresh Widget Data";
        cmd.category = "Widgets";
        cmd.description = "Refresh data for all widgets";
        cmd.icon = "sync";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "widgets.moveWidget";
        cmd.title = "Move Widget";
        cmd.category = "Widgets";
        cmd.description = "Move a widget to a different slot";
        cmd.icon = "move";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "widgets.configureWidget";
        cmd.title = "Configure Widget";
        cmd.category = "Widgets";
        cmd.description = "Open widget configuration";
        cmd.icon = "settings";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    return entries;
}

} // namespace markamp::core
