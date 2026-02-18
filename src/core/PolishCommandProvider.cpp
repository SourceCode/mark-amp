/// @file PolishCommandProvider.cpp
/// @brief V9 Phase 50 — PolishCommandProvider implementation.

#include "PolishCommandProvider.h"

namespace markamp::core
{

void PolishCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto PolishCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "polish.addChangelog",
        "polish.viewChangelog",
        "polish.trackDeprecation",
        "polish.listDeprecations",
        "polish.generateChangelog",
        "polish.checkCompat",
        "polish.runPolish",
        "polish.exportReport",
    };
}

auto PolishCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
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

auto PolishCommandProvider::command_count() -> int
{
    return 8;
}

auto PolishCommandProvider::build_entries() -> std::vector<CommandEntry>
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
        cmd.category = "Polish";
        cmd.description = desc;
        cmd.icon = icon;
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    };

    make("polish.addChangelog", "Add Changelog", "Add a changelog entry", "plus-circle");
    make("polish.viewChangelog", "View Changelog", "View the full changelog", "list");
    make("polish.trackDeprecation",
         "Track Deprecation",
         "Track a deprecated feature",
         "alert-triangle");
    make("polish.listDeprecations", "List Deprecations", "List all deprecations", "alert-circle");
    make("polish.generateChangelog",
         "Generate Changelog",
         "Generate changelog markdown",
         "file-text");
    make(
        "polish.checkCompat", "Check Compatibility", "Check version compatibility", "check-circle");
    make("polish.runPolish", "Run Polish", "Run release polish checks", "star");
    make("polish.exportReport", "Export Report", "Export polish report", "download");

    return entries;
}

} // namespace markamp::core
