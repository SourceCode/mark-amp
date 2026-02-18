/// @file AICommandProvider.h
/// @brief Phase 26: AI Integration — AI command provider.
/// Registers 12 AI-related commands with keybindings for the command palette.

#pragma once

#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Data structures
// ============================================================================

/// Information about a single AI command.
struct AICommandInfo
{
    std::string id;                 ///< Unique command identifier (e.g. "ai.chat").
    std::string title;              ///< Display title (e.g. "AI: Chat").
    std::string category;           ///< Category group (e.g. "AI").
    std::string keybinding;         ///< Default keybinding (e.g. "Cmd+Shift+A").
    std::string description;        ///< Human-readable description.
    bool requires_selection{false}; ///< True if command needs selected text.
};

// ============================================================================
// AICommandProvider
// ============================================================================

/// Provides 12 AI commands organized into categories for the command palette.
/// Commands are disabled when no provider is configured.
class AICommandProvider
{
public:
    AICommandProvider();

    /// Get all registered commands.
    [[nodiscard]] auto commands() const -> const std::vector<AICommandInfo>&;

    /// Find a command by ID.
    [[nodiscard]] auto find_command(const std::string& command_id) const -> const AICommandInfo*;

    /// Get commands for a specific category.
    [[nodiscard]] auto commands_for_category(const std::string& category) const
        -> std::vector<AICommandInfo>;

    /// Get all unique category names.
    [[nodiscard]] auto categories() const -> std::vector<std::string>;

    /// Get the total number of registered commands.
    [[nodiscard]] auto command_count() const -> int;

    /// Get only commands that require text selection.
    [[nodiscard]] auto selection_commands() const -> std::vector<AICommandInfo>;

    /// Get only commands that do NOT require text selection.
    [[nodiscard]] auto global_commands() const -> std::vector<AICommandInfo>;

private:
    std::vector<AICommandInfo> commands_;

    /// Register all built-in AI commands.
    auto register_commands() -> void;
};

} // namespace markamp::core
