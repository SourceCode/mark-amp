/// @file VCCommandProvider.h
/// @brief Phase 25: Version Control Integration — VC command provider.
/// Registers 10 Git-related commands with keybindings for the command palette.

#pragma once

#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Data structures
// ============================================================================

/// Information about a single version-control command.
struct VCCommandInfo
{
    std::string id;          ///< Unique command identifier (e.g. "git.commit").
    std::string title;       ///< Display title (e.g. "Git: Commit").
    std::string category;    ///< Category group (e.g. "Git: Commit").
    std::string keybinding;  ///< Default keybinding (e.g. "Cmd+Shift+G").
    std::string description; ///< Human-readable description.
};

// ============================================================================
// VCCommandProvider
// ============================================================================

/// Provides 10 version-control commands organized into categories.
class VCCommandProvider
{
public:
    VCCommandProvider();

    /// Get all registered commands.
    [[nodiscard]] auto commands() const -> const std::vector<VCCommandInfo>&;

    /// Find a command by ID.
    [[nodiscard]] auto find_command(const std::string& command_id) const -> const VCCommandInfo*;

    /// Get commands for a specific category.
    [[nodiscard]] auto commands_for_category(const std::string& category) const
        -> std::vector<VCCommandInfo>;

    /// Get all unique category names.
    [[nodiscard]] auto categories() const -> std::vector<std::string>;

    /// Get the total number of registered commands.
    [[nodiscard]] auto command_count() const -> int;

private:
    std::vector<VCCommandInfo> commands_;

    /// Register all built-in VC commands.
    auto register_commands() -> void;
};

} // namespace markamp::core
