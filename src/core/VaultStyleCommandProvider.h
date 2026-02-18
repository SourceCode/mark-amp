/// @file VaultStyleCommandProvider.h
/// @brief V9 Phase 38 — Vault style commands for command palette.
#pragma once

#include "CommandRegistry.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Provides 8 vault-style-related commands for the command palette.
///
/// Commands:
///   - vaultStyle.editCss — Edit Vault CSS (Cmd+Shift+V)
///   - vaultStyle.resetCss — Reset Vault CSS
///   - vaultStyle.addSnippet — Add CSS Snippet
///   - vaultStyle.manageFonts — Manage Fonts
///   - vaultStyle.togglePreview — Toggle Live Preview
///   - vaultStyle.switchProfile — Switch Workspace Profile
///   - vaultStyle.exportProfile — Export Profile
///   - vaultStyle.importProfile — Import Profile
class VaultStyleCommandProvider
{
public:
    VaultStyleCommandProvider() = default;

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
