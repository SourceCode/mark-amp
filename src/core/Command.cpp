#include "Command.h"

namespace markamp::core
{

auto Command::is_mergeable_with([[maybe_unused]] const Command& other) const -> bool
{
    return false;
}

void Command::merge_with([[maybe_unused]] Command& other)
{
    // Default: no merge behavior
}

void CommandHistory::execute(std::unique_ptr<Command> cmd)
{
    cmd->execute();

    // Discard any redo history beyond the current position
    if (current_index_ < history_.size())
    {
        history_.erase(history_.begin() + static_cast<std::ptrdiff_t>(current_index_),
                       history_.end());
    }

    // Try to merge with previous command
    if (!history_.empty() && history_.back()->is_mergeable_with(*cmd))
    {
        history_.back()->merge_with(*cmd);
    }
    else
    {
        history_.push_back(std::move(cmd));

        // Enforce max history size
        if (history_.size() > kMaxHistorySize)
        {
            history_.erase(history_.begin());
        }
    }

    current_index_ = history_.size();
}

void CommandHistory::undo()
{
    if (!can_undo())
    {
        return;
    }
    --current_index_;
    history_[current_index_]->undo();
}

void CommandHistory::redo()
{
    if (!can_redo())
    {
        return;
    }
    history_[current_index_]->execute();
    ++current_index_;
}

auto CommandHistory::can_undo() const -> bool
{
    return current_index_ > 0;
}

auto CommandHistory::can_redo() const -> bool
{
    return current_index_ < history_.size();
}

auto CommandHistory::undo_description() const -> std::string
{
    if (!can_undo())
    {
        return "";
    }
    return history_[current_index_ - 1]->description();
}

auto CommandHistory::redo_description() const -> std::string
{
    if (!can_redo())
    {
        return "";
    }
    return history_[current_index_]->description();
}

void CommandHistory::clear()
{
    history_.clear();
    current_index_ = 0;
}

} // namespace markamp::core
