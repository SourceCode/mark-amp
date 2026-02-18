/// @file ActivityCommandProvider.cpp
/// @brief V9 Phase 39 — ActivityCommandProvider implementation.

#include "ActivityCommandProvider.h"

namespace markamp::core
{

void ActivityCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto ActivityCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "activity.showFeed",
        "activity.showTimeline",
        "activity.searchHistory",
        "activity.exportTimeline",
        "activity.pinEntry",
        "activity.clearFeed",
        "activity.filterCategory",
        "activity.showStreaks",
    };
}

auto ActivityCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
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

auto ActivityCommandProvider::command_count() -> int
{
    return 8;
}

auto ActivityCommandProvider::build_entries() -> std::vector<CommandEntry>
{
    std::vector<CommandEntry> entries;
    entries.reserve(8);

    {
        CommandEntry cmd;
        cmd.id = "activity.showFeed";
        cmd.title = "Show Activity Feed";
        cmd.category = "Activity";
        cmd.description = "Open the activity feed panel";
        cmd.shortcut = "Cmd+Shift+A";
        cmd.icon = "pulse";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "activity.showTimeline";
        cmd.title = "Show Activity Timeline";
        cmd.category = "Activity";
        cmd.description = "Open the activity timeline heatmap";
        cmd.icon = "graph-line";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "activity.searchHistory";
        cmd.title = "Search Activity History";
        cmd.category = "Activity";
        cmd.description = "Search through past activities";
        cmd.icon = "search";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "activity.exportTimeline";
        cmd.title = "Export Activity Timeline";
        cmd.category = "Activity";
        cmd.description = "Export timeline data as JSON";
        cmd.icon = "export";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "activity.pinEntry";
        cmd.title = "Pin Activity Entry";
        cmd.category = "Activity";
        cmd.description = "Pin an activity entry to keep it visible";
        cmd.icon = "pin";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "activity.clearFeed";
        cmd.title = "Clear Activity Feed";
        cmd.category = "Activity";
        cmd.description = "Clear all entries from the activity feed";
        cmd.icon = "trash";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "activity.filterCategory";
        cmd.title = "Filter by Category";
        cmd.category = "Activity";
        cmd.description = "Filter activity feed by category";
        cmd.icon = "filter";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "activity.showStreaks";
        cmd.title = "Show Activity Streaks";
        cmd.category = "Activity";
        cmd.description = "View your activity streaks and statistics";
        cmd.icon = "flame";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    return entries;
}

} // namespace markamp::core
