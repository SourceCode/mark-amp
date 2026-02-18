/// @file TaskCommandProvider.h
/// @brief V9 Phase 23 – Command palette integration for task management.
/// Provides 8 task-related commands with keybindings.

#pragma once

#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Command types
// ============================================================================

/// Info for a single command entry.
struct TaskCommandInfo
{
    std::string cmd_id;
    std::string label;
    std::string description;
    std::string category;
    std::optional<std::string> keybinding;
    bool enabled{true};
};

// ============================================================================
// TaskCommandProvider
// ============================================================================

class TaskCommandProvider
{
public:
    TaskCommandProvider();

    /// Get all available task commands.
    [[nodiscard]] auto get_commands() const -> const std::vector<TaskCommandInfo>&;

    /// Check if a specific command is enabled.
    [[nodiscard]] auto is_enabled(const std::string& command_id) const -> bool;

    /// Get a specific command by ID.
    [[nodiscard]] auto get_command(const std::string& command_id) const
        -> std::optional<TaskCommandInfo>;

    /// Get the keybinding for a command.
    [[nodiscard]] auto get_keybinding(const std::string& command_id) const
        -> std::optional<std::string>;

private:
    std::vector<TaskCommandInfo> commands_;

    /// Initialize the default command set.
    auto init_commands() -> void;
};

} // namespace markamp::core
