#include "ErrorRecoveryModel.h"

namespace markamp::ui
{

void ErrorRecoveryModel::set_preflights(std::vector<PreflightCheck> checks)
{
    preflights_ = std::move(checks);
}

auto ErrorRecoveryModel::preflights() const -> const std::vector<PreflightCheck>&
{
    return preflights_;
}

auto ErrorRecoveryModel::is_blocked() const -> bool
{
    for (const auto& check : preflights_)
    {
        if (check.severity == PreflightSeverity::kBlock)
        {
            return true;
        }
    }
    return false;
}

auto ErrorRecoveryModel::blockers() const -> std::vector<PreflightCheck>
{
    std::vector<PreflightCheck> result;
    for (const auto& check : preflights_)
    {
        if (check.severity == PreflightSeverity::kBlock)
        {
            result.push_back(check);
        }
    }
    return result;
}

auto ErrorRecoveryModel::warnings() const -> std::vector<PreflightCheck>
{
    std::vector<PreflightCheck> result;
    for (const auto& check : preflights_)
    {
        if (check.severity == PreflightSeverity::kWarning)
        {
            result.push_back(check);
        }
    }
    return result;
}

void ErrorRecoveryModel::push_error(ErrorContext error)
{
    errors_.push_back(std::move(error));
}

auto ErrorRecoveryModel::errors() const -> const std::vector<ErrorContext>&
{
    return errors_;
}

auto ErrorRecoveryModel::latest_error() const -> const ErrorContext*
{
    if (errors_.empty())
    {
        return nullptr;
    }
    return &errors_.back();
}

void ErrorRecoveryModel::clear_errors()
{
    errors_.clear();
}

void ErrorRecoveryModel::push_undoable(const std::string& action_label)
{
    undo_stack_.push_back(action_label);
}

auto ErrorRecoveryModel::undo_stack_size() const -> int
{
    return static_cast<int>(undo_stack_.size());
}

auto ErrorRecoveryModel::last_undoable() const -> const std::string*
{
    if (undo_stack_.empty())
    {
        return nullptr;
    }
    return &undo_stack_.back();
}

void ErrorRecoveryModel::pop_undoable()
{
    if (!undo_stack_.empty())
    {
        undo_stack_.pop_back();
    }
}

} // namespace markamp::ui
