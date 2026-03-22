/// @file ActionManifest.cpp
/// @brief V24 P04-T01: ActionManifest implementation.
#include "ActionManifest.h"

#include <algorithm>

namespace markamp::core
{

auto ActionManifest::register_action(ManifestAction action) -> bool
{
    if (action.action_id.empty()) return false;
    auto id = action.action_id;
    auto [it, inserted] = actions_.emplace(id, std::move(action));
    if (inserted) insertion_order_.push_back(id);
    return inserted;
}

auto ActionManifest::execute(const std::string& action_id) -> ActionExecutionResult
{
    auto it = actions_.find(action_id);
    if (it == actions_.end()) {
        return {.success = false, .error_message = "Action not found: " + action_id};
    }

    auto& action = it->second;
    if (!action.has_handler()) {
        return {.success = false, .error_message = "Action has no handler: " + action_id};
    }

    if (!action.is_enabled()) {
        return {.success = false, .error_message = "Action is disabled: " + action_id};
    }

    action.handler();
    return {.success = true};
}

auto ActionManifest::is_enabled(const std::string& action_id) const -> bool
{
    auto it = actions_.find(action_id);
    if (it == actions_.end()) return false;
    return it->second.is_enabled();
}

auto ActionManifest::get_action(const std::string& action_id) const
    -> const ManifestAction*
{
    auto it = actions_.find(action_id);
    return it != actions_.end() ? &it->second : nullptr;
}

auto ActionManifest::all_actions() const -> std::vector<const ManifestAction*>
{
    std::vector<const ManifestAction*> result;
    result.reserve(insertion_order_.size());
    for (const auto& id : insertion_order_) {
        auto it = actions_.find(id);
        if (it != actions_.end()) result.push_back(&it->second);
    }
    return result;
}

auto ActionManifest::actions_by_category(ActionCategory cat) const
    -> std::vector<const ManifestAction*>
{
    std::vector<const ManifestAction*> result;
    for (const auto& id : insertion_order_) {
        auto it = actions_.find(id);
        if (it != actions_.end() && it->second.category == cat) {
            result.push_back(&it->second);
        }
    }
    return result;
}

void ActionManifest::clear()
{
    actions_.clear();
    insertion_order_.clear();
}

} // namespace markamp::core
