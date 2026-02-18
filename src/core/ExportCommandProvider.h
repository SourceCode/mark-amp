/// @file ExportCommandProvider.h
/// @brief V9 Phase 24 – Command palette integration for export & publishing.
/// Provides 8 export-related commands with keybindings.

#pragma once

#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Command types
// ============================================================================

/// Info for a single export command entry.
struct ExportCommandInfo
{
    std::string cmd_id;
    std::string label;
    std::string description;
    std::string category;
    std::optional<std::string> keybinding;
    bool enabled{true};
};

// ============================================================================
// ExportCommandProvider
// ============================================================================

class ExportCommandProvider
{
public:
    ExportCommandProvider();

    /// Get all available export commands.
    [[nodiscard]] auto get_commands() const -> const std::vector<ExportCommandInfo>&;

    /// Check if a specific command is enabled.
    [[nodiscard]] auto is_enabled(const std::string& command_id) const -> bool;

    /// Get a specific command by ID.
    [[nodiscard]] auto get_command(const std::string& command_id) const
        -> std::optional<ExportCommandInfo>;

    /// Get the keybinding for a command.
    [[nodiscard]] auto get_keybinding(const std::string& command_id) const
        -> std::optional<std::string>;

private:
    std::vector<ExportCommandInfo> commands_;

    /// Initialize the default command set.
    auto init_commands() -> void;
};

} // namespace markamp::core
