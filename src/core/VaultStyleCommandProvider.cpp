/// @file VaultStyleCommandProvider.cpp
/// @brief V9 Phase 38 — VaultStyleCommandProvider implementation.

#include "VaultStyleCommandProvider.h"

namespace markamp::core
{

void VaultStyleCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto VaultStyleCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "vaultStyle.editCss",
        "vaultStyle.resetCss",
        "vaultStyle.addSnippet",
        "vaultStyle.manageFonts",
        "vaultStyle.togglePreview",
        "vaultStyle.switchProfile",
        "vaultStyle.exportProfile",
        "vaultStyle.importProfile",
    };
}

auto VaultStyleCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
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

auto VaultStyleCommandProvider::command_count() -> int
{
    return 8;
}

auto VaultStyleCommandProvider::build_entries() -> std::vector<CommandEntry>
{
    std::vector<CommandEntry> entries;
    entries.reserve(8);

    {
        CommandEntry cmd;
        cmd.id = "vaultStyle.editCss";
        cmd.title = "Edit Vault CSS";
        cmd.category = "Vault Style";
        cmd.description = "Open the vault's custom CSS file for editing";
        cmd.shortcut = "Cmd+Shift+V";
        cmd.icon = "paintcan";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "vaultStyle.resetCss";
        cmd.title = "Reset Vault CSS";
        cmd.category = "Vault Style";
        cmd.description = "Reset vault CSS to default template";
        cmd.icon = "discard";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "vaultStyle.addSnippet";
        cmd.title = "Add CSS Snippet";
        cmd.category = "Vault Style";
        cmd.description = "Add a CSS snippet from the snippet library";
        cmd.icon = "code";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "vaultStyle.manageFonts";
        cmd.title = "Manage Custom Fonts";
        cmd.category = "Vault Style";
        cmd.description = "Register or remove custom fonts for this vault";
        cmd.icon = "symbol-text";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "vaultStyle.togglePreview";
        cmd.title = "Toggle Live CSS Preview";
        cmd.category = "Vault Style";
        cmd.description = "Toggle live preview of CSS changes";
        cmd.icon = "eye";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "vaultStyle.switchProfile";
        cmd.title = "Switch Workspace Profile";
        cmd.category = "Vault Style";
        cmd.description = "Switch to a different workspace profile";
        cmd.icon = "account";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "vaultStyle.exportProfile";
        cmd.title = "Export Profile";
        cmd.category = "Vault Style";
        cmd.description = "Export the current workspace profile as JSON";
        cmd.icon = "export";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "vaultStyle.importProfile";
        cmd.title = "Import Profile";
        cmd.category = "Vault Style";
        cmd.description = "Import a workspace profile from JSON";
        cmd.icon = "cloud-download";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    return entries;
}

} // namespace markamp::core
