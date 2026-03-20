/// @file NotebookCommandProvider.h
/// @brief P08-T05: Notebook commands, menus, and activity bar entry points.
///
/// Registers notebook commands in the shared command model with context-aware
/// enable/disable and feedback when no notebook is active.
#pragma once

#include "EventBus.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// A registered notebook command.
struct NotebookCommand
{
    std::string command_id;
    std::string label;
    std::string category{"Notebook"};
    bool requires_active_notebook{true};
};

/// Provides notebook commands for palette, menu, and toolbar.
class NotebookCommandProvider
{
public:
    explicit NotebookCommandProvider(EventBus& bus);

    /// Register a notebook command.
    void register_command(const NotebookCommand& cmd);

    /// Get all registered commands.
    [[nodiscard]] auto commands() const -> const std::vector<NotebookCommand>&
    {
        return commands_;
    }

    /// Get command count.
    [[nodiscard]] auto command_count() const -> int
    {
        return static_cast<int>(commands_.size());
    }

    /// Check if a command is enabled (notebook context active).
    [[nodiscard]] auto is_enabled(const std::string& command_id) const -> bool;

    /// Set whether a notebook is active (for enable/disable state).
    void set_notebook_active(bool active) { notebook_active_ = active; }

    /// Check if a notebook is active.
    [[nodiscard]] auto has_active_notebook() const -> bool { return notebook_active_; }

private:
    void register_built_in_commands();

    EventBus& event_bus_;
    Subscription notebook_open_sub_;
    std::vector<NotebookCommand> commands_;
    bool notebook_active_{false};
};

} // namespace markamp::core
