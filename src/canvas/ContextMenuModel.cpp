#include "ContextMenuModel.h"

namespace markamp::canvas
{

void ContextMenuModel::set_scope(ContextScope scope)
{
    scope_ = scope;
}
auto ContextMenuModel::scope() const -> ContextScope
{
    return scope_;
}

void ContextMenuModel::set_actions(std::vector<ContextAction> actions)
{
    actions_ = std::move(actions);
}
auto ContextMenuModel::actions() const -> const std::vector<ContextAction>&
{
    return actions_;
}

auto ContextMenuModel::applicable_actions() const -> std::vector<ContextAction>
{
    std::vector<ContextAction> result;
    for (const auto& action : actions_)
    {
        if (action.applicable)
        {
            result.push_back(action);
        }
    }
    return result;
}

auto ContextMenuModel::actions_in_group(const std::string& group) const
    -> std::vector<ContextAction>
{
    std::vector<ContextAction> result;
    for (const auto& action : actions_)
    {
        if (action.group == group)
        {
            result.push_back(action);
        }
    }
    return result;
}

void ContextMenuModel::set_object_type(const std::string& object_type)
{
    object_type_ = object_type;
}
auto ContextMenuModel::object_type() const -> const std::string&
{
    return object_type_;
}

} // namespace markamp::canvas
