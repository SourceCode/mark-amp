/// @file WorkspaceCustomizationCommandProvider.h
/// @brief V9 Phase 38 — Workspace customization commands for command palette.
#pragma once

#include "CommandRegistry.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Provides 8 workspace customization commands for the command palette.
///
/// Commands:
///   - workspace.saveProfile — Save Workspace Profile (Cmd+Shift+W)
///   - workspace.loadProfile — Load Workspace Profile
///   - workspace.resetDefaults — Reset to Defaults
///   - workspace.themeOverrides — Theme Token Overrides
///   - workspace.fontSettings — Font Settings
///   - workspace.layoutPreset — Apply Layout Preset
///   - workspace.compareProfiles — Compare Profiles
///   - workspace.snippetLibrary — CSS Snippet Library
class WorkspaceCustomizationCommandProvider
{
public:
    WorkspaceCustomizationCommandProvider() = default;

    /// Register all commands in the given registry.
    void register_commands(CommandRegistry& registry) const;

    /// Get the list of command IDs provided.
    [[nodiscard]] static auto command_ids() -> std::vector<std::string>;

    /// Get the command entry for a specific command.
    [[nodiscard]] auto get_command(const std::string& command_id) const -> CommandEntry;

    /// Total number of commands provided.
    [[nodiscard]] static auto command_count() -> int;

private:
    [[nodiscard]] static auto build_entries() -> std::vector<CommandEntry>;
};

} // namespace markamp::core
