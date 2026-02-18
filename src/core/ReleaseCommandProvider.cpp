/// @file ReleaseCommandProvider.cpp
/// @brief V9 Phase 50 — ReleaseCommandProvider implementation.

#include "ReleaseCommandProvider.h"

namespace markamp::core
{

void ReleaseCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto ReleaseCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "release.create",
        "release.addEntry",
        "release.generate",
        "release.bumpVersion",
        "release.viewHistory",
        "release.publish",
        "release.exportNotes",
        "release.clearReleases",
    };
}

auto ReleaseCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
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

auto ReleaseCommandProvider::command_count() -> int
{
    return 8;
}

auto ReleaseCommandProvider::build_entries() -> std::vector<CommandEntry>
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
        cmd.category = "Release";
        cmd.description = desc;
        cmd.icon = icon;
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    };

    make("release.create", "Create Release", "Create a new release", "package");
    make("release.addEntry", "Add Entry", "Add entry to release notes", "plus");
    make("release.generate", "Generate Notes", "Generate release notes markdown", "file-text");
    make("release.bumpVersion", "Bump Version", "Bump semantic version", "arrow-up-circle");
    make("release.viewHistory", "View History", "View version history", "clock");
    make("release.publish", "Publish Release", "Publish the current release", "send");
    make("release.exportNotes", "Export Notes", "Export release notes to file", "download");
    make("release.clearReleases", "Clear Releases", "Clear all releases", "trash-2");

    return entries;
}

} // namespace markamp::core
