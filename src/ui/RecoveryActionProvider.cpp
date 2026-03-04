#include "RecoveryActionProvider.h"

namespace markamp::ui
{

auto RecoveryAction::type_name() const -> std::string
{
    switch (type)
    {
        case RecoveryActionType::kRetry:
            return "retry";
        case RecoveryActionType::kOpenLog:
            return "open_log";
        case RecoveryActionType::kOpenSettings:
            return "open_settings";
        case RecoveryActionType::kUndo:
            return "undo";
        case RecoveryActionType::kCustom:
            return "custom";
    }
    return "unknown";
}

void RecoveryActionProvider::register_recovery(const std::string& error_context,
                                               const RecoveryAction& action)
{
    RecoveryEntry entry;
    entry.error_context = error_context;
    entry.action = action;
    recoveries_.push_back(entry);
}

auto RecoveryActionProvider::recoveries_for(const std::string& error_context) const
    -> std::vector<RecoveryAction>
{
    std::vector<RecoveryAction> result;
    for (const auto& entry : recoveries_)
    {
        if (entry.error_context == error_context)
        {
            result.push_back(entry.action);
        }
    }
    return result;
}

auto RecoveryActionProvider::has_recoveries(const std::string& error_context) const -> bool
{
    for (const auto& entry : recoveries_)
    {
        if (entry.error_context == error_context)
        {
            return true;
        }
    }
    return false;
}

void RecoveryActionProvider::push_undo(const UndoEntry& entry)
{
    undo_stack_.push_back(entry);
    redo_stack_.clear(); // New action clears redo
}

auto RecoveryActionProvider::undo() -> bool
{
    if (undo_stack_.empty())
    {
        return false;
    }

    auto entry = std::move(undo_stack_.back());
    undo_stack_.pop_back();

    if (entry.undo_fn)
    {
        entry.undo_fn();
    }

    redo_stack_.push_back(std::move(entry));
    return true;
}

auto RecoveryActionProvider::redo() -> bool
{
    if (redo_stack_.empty())
    {
        return false;
    }

    auto entry = std::move(redo_stack_.back());
    redo_stack_.pop_back();

    if (entry.redo_fn)
    {
        entry.redo_fn();
    }

    undo_stack_.push_back(std::move(entry));
    return true;
}

auto RecoveryActionProvider::can_undo() const -> bool
{
    return !undo_stack_.empty();
}

auto RecoveryActionProvider::can_redo() const -> bool
{
    return !redo_stack_.empty();
}

auto RecoveryActionProvider::undo_description() const -> std::string
{
    if (undo_stack_.empty())
    {
        return "";
    }
    return undo_stack_.back().description;
}

auto RecoveryActionProvider::redo_description() const -> std::string
{
    if (redo_stack_.empty())
    {
        return "";
    }
    return redo_stack_.back().description;
}

auto RecoveryActionProvider::undo_depth() const -> int
{
    return static_cast<int>(undo_stack_.size());
}

void RecoveryActionProvider::clear_undo()
{
    undo_stack_.clear();
    redo_stack_.clear();
}

} // namespace markamp::ui
