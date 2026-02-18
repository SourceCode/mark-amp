// ============================================================================
// File: src/core/SyncCommandProvider.cpp
// Phase 27: Cloud Sync & Collaboration — Command palette commands
// ============================================================================

#include "SyncCommandProvider.h"

#include <algorithm>
#include <set>

namespace markamp::core
{

SyncCommandProvider::SyncCommandProvider()
{
    register_commands();
}

auto SyncCommandProvider::commands() const -> const std::vector<SyncCommand>&
{
    return commands_;
}

auto SyncCommandProvider::find_command(const std::string& command_id) const
    -> std::optional<SyncCommand>
{
    auto iter =
        std::find_if(commands_.begin(),
                     commands_.end(),
                     [&command_id](const SyncCommand& cmd) { return cmd.id == command_id; });

    if (iter != commands_.end())
    {
        return *iter;
    }
    return std::nullopt;
}

auto SyncCommandProvider::commands_in_category(const std::string& category) const
    -> std::vector<SyncCommand>
{
    std::vector<SyncCommand> results;
    for (const auto& cmd : commands_)
    {
        if (cmd.category == category)
        {
            results.push_back(cmd);
        }
    }
    return results;
}

auto SyncCommandProvider::categories() const -> std::vector<std::string>
{
    std::set<std::string> unique_categories;
    for (const auto& cmd : commands_)
    {
        unique_categories.insert(cmd.category);
    }
    return {unique_categories.begin(), unique_categories.end()};
}

auto SyncCommandProvider::command_count() const -> int32_t
{
    return static_cast<int32_t>(commands_.size());
}

auto SyncCommandProvider::available_commands(bool sync_configured) const -> std::vector<SyncCommand>
{
    std::vector<SyncCommand> results;
    for (const auto& cmd : commands_)
    {
        if (!cmd.requires_sync_configured || sync_configured)
        {
            results.push_back(cmd);
        }
    }
    return results;
}

auto SyncCommandProvider::register_commands() -> void
{
    commands_ = {
        // Sync category — primary sync operations
        {.id = "sync.sync_now",
         .title = "Sync: Sync Now",
         .category = "Sync",
         .keybinding = "Cmd+Shift+S",
         .description = "Trigger an immediate sync with cloud storage",
         .requires_sync_configured = true},

        {.id = "sync.configure",
         .title = "Sync: Configure",
         .category = "Sync",
         .keybinding = "",
         .description = "Open sync provider configuration settings",
         .requires_sync_configured = false},

        {.id = "sync.pause",
         .title = "Sync: Pause",
         .category = "Sync",
         .keybinding = "",
         .description = "Pause automatic sync scheduling",
         .requires_sync_configured = true},

        {.id = "sync.resume",
         .title = "Sync: Resume",
         .category = "Sync",
         .keybinding = "",
         .description = "Resume automatic sync scheduling",
         .requires_sync_configured = true},

        {.id = "sync.test_connection",
         .title = "Sync: Test Connection",
         .category = "Sync",
         .keybinding = "",
         .description = "Test connectivity to the sync provider",
         .requires_sync_configured = true},

        // Sync Management category — history, conflicts, sharing

        {.id = "sync.show_history",
         .title = "Sync: Show History",
         .category = "Sync Management",
         .keybinding = "",
         .description = "Show sync operation history and audit log",
         .requires_sync_configured = true},

        {.id = "sync.resolve_conflicts",
         .title = "Sync: Resolve Conflicts",
         .category = "Sync Management",
         .keybinding = "",
         .description = "Open conflict resolution panel for pending sync conflicts",
         .requires_sync_configured = true},

        {.id = "sync.share_workspace",
         .title = "Sync: Share Workspace",
         .category = "Sync Management",
         .keybinding = "",
         .description = "Share the current workspace with collaborators",
         .requires_sync_configured = true},

        {.id = "sync.show_ignore_patterns",
         .title = "Sync: Show Ignore Patterns",
         .category = "Sync Management",
         .keybinding = "",
         .description = "Edit the .markamp-syncignore file for selective sync",
         .requires_sync_configured = false},

        {.id = "sync.show_devices",
         .title = "Sync: Show Devices",
         .category = "Sync Management",
         .keybinding = "",
         .description = "Show connected devices and their sync status",
         .requires_sync_configured = true},
    };
}

} // namespace markamp::core
