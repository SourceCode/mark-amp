/// @file WorkspaceCustomizationCommandProvider.cpp
/// @brief V9 Phase 38 — WorkspaceCustomizationCommandProvider implementation.

#include "WorkspaceCustomizationCommandProvider.h"

namespace markamp::core
{

void WorkspaceCustomizationCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto WorkspaceCustomizationCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "workspace.saveProfile",
        "workspace.loadProfile",
        "workspace.resetDefaults",
        "workspace.themeOverrides",
        "workspace.fontSettings",
        "workspace.layoutPreset",
        "workspace.compareProfiles",
        "workspace.snippetLibrary",
    };
}

auto WorkspaceCustomizationCommandProvider::get_command(const std::string& command_id) const
    -> CommandEntry
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

auto WorkspaceCustomizationCommandProvider::command_count() -> int
{
    return 8;
}

auto WorkspaceCustomizationCommandProvider::build_entries() -> std::vector<CommandEntry>
{
    std::vector<CommandEntry> entries;
    entries.reserve(8);

    {
        CommandEntry cmd;
        cmd.id = "workspace.saveProfile";
        cmd.title = "Save Workspace Profile";
        cmd.category = "Workspace";
        cmd.description = "Save the current workspace configuration as a named profile";
        cmd.shortcut = "Cmd+Shift+W";
        cmd.icon = "save";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "workspace.loadProfile";
        cmd.title = "Load Workspace Profile";
        cmd.category = "Workspace";
        cmd.description = "Load a saved workspace profile";
        cmd.icon = "folder-opened";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "workspace.resetDefaults";
        cmd.title = "Reset to Defaults";
        cmd.category = "Workspace";
        cmd.description = "Reset all customization to default values";
        cmd.icon = "refresh";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "workspace.themeOverrides";
        cmd.title = "Theme Token Overrides";
        cmd.category = "Workspace";
        cmd.description = "Manage per-vault theme token overrides";
        cmd.icon = "symbol-color";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "workspace.fontSettings";
        cmd.title = "Font Settings";
        cmd.category = "Workspace";
        cmd.description = "Configure custom fonts for this workspace";
        cmd.icon = "symbol-text";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "workspace.layoutPreset";
        cmd.title = "Apply Layout Preset";
        cmd.category = "Workspace";
        cmd.description = "Apply a predefined panel layout preset";
        cmd.icon = "layout";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "workspace.compareProfiles";
        cmd.title = "Compare Profiles";
        cmd.category = "Workspace";
        cmd.description = "Compare two workspace profiles and see differences";
        cmd.icon = "diff";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "workspace.snippetLibrary";
        cmd.title = "CSS Snippet Library";
        cmd.category = "Workspace";
        cmd.description = "Browse and apply CSS snippets from the library";
        cmd.icon = "library";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    return entries;
}

} // namespace markamp::core
