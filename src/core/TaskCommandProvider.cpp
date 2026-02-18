/// @file TaskCommandProvider.cpp
/// @brief V9 Phase 23 – Task command provider implementation.

#include "core/TaskCommandProvider.h"

namespace markamp::core
{

// ============================================================================
// Constructor
// ============================================================================

TaskCommandProvider::TaskCommandProvider()
{
    init_commands();
}

// ============================================================================
// Public API
// ============================================================================

auto TaskCommandProvider::get_commands() const -> const std::vector<TaskCommandInfo>&
{
    return commands_;
}

auto TaskCommandProvider::is_enabled(const std::string& command_id) const -> bool
{
    for (const auto& cmd : commands_)
    {
        if (cmd.cmd_id == command_id)
        {
            return cmd.enabled;
        }
    }
    return false;
}

auto TaskCommandProvider::get_command(const std::string& command_id) const
    -> std::optional<TaskCommandInfo>
{
    for (const auto& cmd : commands_)
    {
        if (cmd.cmd_id == command_id)
        {
            return cmd;
        }
    }
    return std::nullopt;
}

auto TaskCommandProvider::get_keybinding(const std::string& command_id) const
    -> std::optional<std::string>
{
    for (const auto& cmd : commands_)
    {
        if (cmd.cmd_id == command_id)
        {
            return cmd.keybinding;
        }
    }
    return std::nullopt;
}

// ============================================================================
// Private
// ============================================================================

auto TaskCommandProvider::init_commands() -> void
{
    commands_ = {
        {"task.view_board",
         "Task: View Board",
         "Open the Kanban task board",
         "Task",
         std::nullopt,
         true},

        {"task.view_gantt",
         "Task: View Timeline",
         "Open the Gantt timeline view",
         "Task",
         std::nullopt,
         true},

        {"task.add_task",
         "Task: Add Task",
         "Create a new task at cursor position",
         "Task",
         std::optional<std::string>{"Cmd+Shift+N"},
         true},

        {"task.toggle_task",
         "Task: Toggle Task",
         "Toggle the task status at cursor",
         "Task",
         std::optional<std::string>{"Cmd+Shift+T"},
         true},

        {"task.filter_overdue",
         "Task: Show Overdue",
         "Filter to show only overdue tasks",
         "Task",
         std::nullopt,
         true},

        {"task.open_calendar",
         "Task: Open Calendar",
         "Open the calendar view with task due dates",
         "Task",
         std::nullopt,
         true},

        {"task.set_reminder",
         "Task: Set Reminder",
         "Set a reminder for the current task",
         "Task",
         std::nullopt,
         true},

        {"task.refresh",
         "Task: Refresh Tasks",
         "Re-scan all documents for tasks",
         "Task",
         std::optional<std::string>{"Cmd+Shift+F5"},
         true},
    };
}

} // namespace markamp::core
