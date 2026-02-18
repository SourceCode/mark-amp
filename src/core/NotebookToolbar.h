/// @file NotebookToolbar.h
/// @brief V8 Phase 15 – Notebook-specific command registry and toolbar.

#pragma once

#include "core/EventBus.h"

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Notebook command
// ============================================================================

/// A notebook-specific command registered for toolbar and keyboard shortcuts.
struct NotebookCommand
{
    std::string id;       ///< Unique command identifier, e.g. "notebook.runCell"
    std::string label;    ///< Human-readable label, e.g. "Run Cell"
    std::string shortcut; ///< Keyboard shortcut, e.g. "Shift+Enter"
    std::string icon;     ///< Icon name (optional)
    std::string category; ///< Command category, e.g. "execution", "cell", "navigation"
    bool enabled{true};   ///< Whether the command is currently enabled

    /// Callback invoked when the command is executed.
    std::function<void()> handler;
};

// ============================================================================
// NotebookToolbar
// ============================================================================

class NotebookToolbar
{
public:
    explicit NotebookToolbar(EventBus& event_bus);

    /// Register a notebook command.
    auto register_command(const NotebookCommand& cmd) -> void;

    /// Unregister a command by ID.
    auto unregister_command(const std::string& command_id) -> void;

    /// Execute a command by its ID. Returns true if the command was found and executed.
    auto execute_command(const std::string& command_id) -> bool;

    /// Get all commands in a specific category.
    [[nodiscard]] auto commands_in_category(const std::string& category) const
        -> std::vector<NotebookCommand>;

    /// Enable or disable a command.
    auto set_enabled(const std::string& command_id, bool enabled) -> void;

    /// Get all registered commands.
    [[nodiscard]] auto all_commands() const -> const std::vector<NotebookCommand>&;

    /// Find a command by its keyboard shortcut.
    [[nodiscard]] auto find_by_shortcut(const std::string& shortcut) const
        -> std::optional<NotebookCommand>;

    /// Find a command by its ID.
    [[nodiscard]] auto find_by_id(const std::string& command_id) const
        -> std::optional<NotebookCommand>;

    /// Get all unique categories.
    [[nodiscard]] auto categories() const -> std::vector<std::string>;

    /// Get the total number of registered commands.
    [[nodiscard]] auto command_count() const -> int;

    /// Clear all registered commands.
    auto clear() -> void;

private:
    [[maybe_unused]] EventBus& event_bus_;
    std::vector<NotebookCommand> commands_;

    /// Find a command iterator by ID.
    [[nodiscard]] auto find_command(const std::string& command_id)
        -> std::vector<NotebookCommand>::iterator;

    [[nodiscard]] auto find_command(const std::string& command_id) const
        -> std::vector<NotebookCommand>::const_iterator;
};

} // namespace markamp::core
