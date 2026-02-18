/// @file NotebookToolbar.cpp
/// @brief V8 Phase 15 – Notebook toolbar command registry implementation.

#include "core/NotebookToolbar.h"

#include <algorithm>
#include <unordered_set>

namespace markamp::core
{

// ============================================================================
// Constructor
// ============================================================================

NotebookToolbar::NotebookToolbar(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ============================================================================
// Command registration
// ============================================================================

auto NotebookToolbar::register_command(const NotebookCommand& cmd) -> void
{
    // Avoid duplicate IDs.
    auto iter = find_command(cmd.id);
    if (iter != commands_.end())
    {
        *iter = cmd; // Update existing command.
        return;
    }
    commands_.push_back(cmd);
}

auto NotebookToolbar::unregister_command(const std::string& command_id) -> void
{
    commands_.erase(std::remove_if(commands_.begin(),
                                   commands_.end(),
                                   [&command_id](const NotebookCommand& cmd)
                                   { return cmd.id == command_id; }),
                    commands_.end());
}

// ============================================================================
// Command execution
// ============================================================================

auto NotebookToolbar::execute_command(const std::string& command_id) -> bool
{
    auto iter = find_command(command_id);
    if (iter == commands_.end())
    {
        return false;
    }

    if (!iter->enabled)
    {
        return false;
    }

    if (iter->handler)
    {
        iter->handler();
    }

    return true;
}

// ============================================================================
// Query methods
// ============================================================================

auto NotebookToolbar::commands_in_category(const std::string& category) const
    -> std::vector<NotebookCommand>
{
    std::vector<NotebookCommand> result;
    for (const auto& cmd : commands_)
    {
        if (cmd.category == category)
        {
            result.push_back(cmd);
        }
    }
    return result;
}

auto NotebookToolbar::set_enabled(const std::string& command_id, bool enabled) -> void
{
    auto iter = find_command(command_id);
    if (iter != commands_.end())
    {
        iter->enabled = enabled;
    }
}

auto NotebookToolbar::all_commands() const -> const std::vector<NotebookCommand>&
{
    return commands_;
}

auto NotebookToolbar::find_by_shortcut(const std::string& shortcut) const
    -> std::optional<NotebookCommand>
{
    for (const auto& cmd : commands_)
    {
        if (cmd.shortcut == shortcut)
        {
            return cmd;
        }
    }
    return std::nullopt;
}

auto NotebookToolbar::find_by_id(const std::string& command_id) const
    -> std::optional<NotebookCommand>
{
    auto iter = find_command(command_id);
    if (iter != commands_.end())
    {
        return *iter;
    }
    return std::nullopt;
}

auto NotebookToolbar::categories() const -> std::vector<std::string>
{
    std::unordered_set<std::string> unique_categories;
    for (const auto& cmd : commands_)
    {
        if (!cmd.category.empty())
        {
            unique_categories.insert(cmd.category);
        }
    }

    std::vector<std::string> result(unique_categories.begin(), unique_categories.end());
    std::sort(result.begin(), result.end());
    return result;
}

auto NotebookToolbar::command_count() const -> int
{
    return static_cast<int>(commands_.size());
}

auto NotebookToolbar::clear() -> void
{
    commands_.clear();
}

// ============================================================================
// Private helpers
// ============================================================================

auto NotebookToolbar::find_command(const std::string& command_id)
    -> std::vector<NotebookCommand>::iterator
{
    return std::find_if(commands_.begin(),
                        commands_.end(),
                        [&command_id](const NotebookCommand& cmd) { return cmd.id == command_id; });
}

auto NotebookToolbar::find_command(const std::string& command_id) const
    -> std::vector<NotebookCommand>::const_iterator
{
    return std::find_if(commands_.begin(),
                        commands_.end(),
                        [&command_id](const NotebookCommand& cmd) { return cmd.id == command_id; });
}

} // namespace markamp::core
