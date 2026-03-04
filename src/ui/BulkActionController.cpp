#include "BulkActionController.h"

#include <sstream>

namespace markamp::ui
{

auto BulkAction::requires_confirmation() const -> bool
{
    return severity == ActionSeverity::kDestructive || severity == ActionSeverity::kIrreversible;
}

auto BulkOperationResult::is_success() const -> bool
{
    return failed_count == 0 && affected_count > 0;
}

void BulkActionController::register_action(const BulkAction& action)
{
    actions_.push_back(action);
}

auto BulkActionController::all_actions() const -> const std::vector<BulkAction>&
{
    return actions_;
}

auto BulkActionController::enabled_actions(int selection_count) const -> std::vector<BulkAction>
{
    std::vector<BulkAction> result;
    for (const auto& action : actions_)
    {
        if (selection_count >= action.min_selection)
        {
            result.push_back(action);
        }
    }
    return result;
}

auto BulkActionController::find_action(const std::string& action_id) const -> const BulkAction*
{
    for (const auto& action : actions_)
    {
        if (action.action_id == action_id)
        {
            return &action;
        }
    }
    return nullptr;
}

auto BulkActionController::action_count() const -> int
{
    return static_cast<int>(actions_.size());
}

auto BulkActionController::execute(const std::string& action_id, int selection_count)
    -> BulkOperationResult
{
    const auto* action = find_action(action_id);
    if (action == nullptr)
    {
        last_result_ = {0, 0, "Action not found: " + action_id};
        return last_result_;
    }

    if (action->requires_confirmation())
    {
        last_result_ = {0, 0, "Action requires confirmation"};
        return last_result_;
    }

    last_result_ = {selection_count,
                    0,
                    "Executed " + action->label + " on " + std::to_string(selection_count) +
                        " items"};
    return last_result_;
}

auto BulkActionController::confirm_and_execute(const std::string& action_id, int selection_count)
    -> BulkOperationResult
{
    const auto* action = find_action(action_id);
    if (action == nullptr)
    {
        last_result_ = {0, 0, "Action not found: " + action_id};
        return last_result_;
    }

    last_result_ = {selection_count,
                    0,
                    "Executed " + action->label + " on " + std::to_string(selection_count) +
                        " items"};
    return last_result_;
}

auto BulkActionController::last_result() const -> const BulkOperationResult&
{
    return last_result_;
}

auto BulkActionController::needs_confirmation(const std::string& action_id) const -> bool
{
    const auto* action = find_action(action_id);
    return action != nullptr && action->requires_confirmation();
}

auto BulkActionController::confirmation_message(const std::string& action_id,
                                                int selection_count) const -> std::string
{
    const auto* action = find_action(action_id);
    if (action == nullptr)
    {
        return "";
    }

    std::ostringstream oss;
    oss << "Are you sure you want to " << action->label << " " << selection_count << " item";
    if (selection_count != 1)
    {
        oss << "s";
    }
    oss << "?";

    if (action->severity == ActionSeverity::kIrreversible)
    {
        oss << " This action cannot be undone.";
    }

    return oss.str();
}

void BulkActionController::register_standard_actions()
{
    register_action({"bulk.delete", "Delete", "trash", ActionSeverity::kDestructive, 1});
    register_action({"bulk.move", "Move", "folder-move", ActionSeverity::kNormal, 1});
    register_action({"bulk.copy", "Copy", "copy", ActionSeverity::kNormal, 1});
    register_action({"bulk.rename", "Rename", "rename", ActionSeverity::kNormal, 1});
    register_action({"bulk.export", "Export", "export", ActionSeverity::kNormal, 2});
}

} // namespace markamp::ui
