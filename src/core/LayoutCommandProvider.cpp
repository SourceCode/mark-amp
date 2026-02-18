/// @file LayoutCommandProvider.cpp
/// @brief V9 Phase 46 — LayoutCommandProvider implementation.

#include "LayoutCommandProvider.h"

namespace markamp::core
{

void LayoutCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto LayoutCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "layout.save",
        "layout.restore",
        "layout.singleColumn",
        "layout.twoColumn",
        "layout.threePanel",
        "session.save",
        "session.restore",
        "layout.reset",
    };
}

auto LayoutCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
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

auto LayoutCommandProvider::command_count() -> int
{
    return 8;
}

auto LayoutCommandProvider::build_entries() -> std::vector<CommandEntry>
{
    std::vector<CommandEntry> entries;
    entries.reserve(8);

    auto make = [&](const std::string& cmd_id,
                    const std::string& cmd_title,
                    const std::string& desc,
                    const std::string& icon)
    {
        CommandEntry cmd;
        cmd.id = cmd_id;
        cmd.title = cmd_title;
        cmd.category = "Layout";
        cmd.description = desc;
        cmd.icon = icon;
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    };

    make("layout.save", "Save Layout", "Save current editor layout", "save");
    make("layout.restore", "Restore Layout", "Restore a saved layout preset", "refresh");
    make("layout.singleColumn", "Single Column", "Switch to single column layout", "square");
    make("layout.twoColumn", "Two Column", "Switch to two column layout", "columns");
    make("layout.threePanel", "Three Panel", "Switch to three panel layout", "grid");
    make("session.save", "Save Session", "Save current workspace session", "download");
    make("session.restore", "Restore Session", "Restore last workspace session", "upload");
    make("layout.reset", "Reset Layout", "Reset layout to default", "rotate-ccw");

    return entries;
}

} // namespace markamp::core
