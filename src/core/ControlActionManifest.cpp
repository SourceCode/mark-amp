/// @file ControlActionManifest.cpp
/// @brief V21 Phase 01 — ControlActionManifest implementation.

#include "ControlActionManifest.h"

#include <algorithm>
#include <set>

namespace markamp::core
{

// ── Registration ──

void ControlActionManifest::register_action(ActionEntry entry)
{
    const auto id = entry.action_id;

    if (actions_.contains(id))
    {
        actions_[id] = std::move(entry);
        return;
    }

    insertion_order_.push_back(id);
    actions_.emplace(id, std::move(entry));
}

void ControlActionManifest::register_actions(std::vector<ActionEntry> entries)
{
    for (auto& entry : entries)
    {
        register_action(std::move(entry));
    }
}

auto ControlActionManifest::unregister_action(const std::string& action_id) -> bool
{
    auto iter = actions_.find(action_id);
    if (iter == actions_.end())
    {
        return false;
    }
    actions_.erase(iter);
    std::erase(insertion_order_, action_id);
    return true;
}

// ── Lookup ──

auto ControlActionManifest::get_action(const std::string& action_id) const -> const ActionEntry*
{
    auto iter = actions_.find(action_id);
    if (iter != actions_.end())
    {
        return &iter->second;
    }
    return nullptr;
}

auto ControlActionManifest::get_action_mut(const std::string& action_id) -> ActionEntry*
{
    auto iter = actions_.find(action_id);
    if (iter != actions_.end())
    {
        return &iter->second;
    }
    return nullptr;
}

auto ControlActionManifest::all_actions() const -> std::vector<const ActionEntry*>
{
    std::vector<const ActionEntry*> result;
    result.reserve(insertion_order_.size());
    for (const auto& id : insertion_order_)
    {
        auto iter = actions_.find(id);
        if (iter != actions_.end())
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

auto ControlActionManifest::actions_for_surface(ControlSurface surface) const
    -> std::vector<const ActionEntry*>
{
    std::vector<const ActionEntry*> result;
    for (const auto& id : insertion_order_)
    {
        auto iter = actions_.find(id);
        if (iter != actions_.end() && iter->second.appears_on(surface))
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

auto ControlActionManifest::actions_for_category(const std::string& category) const
    -> std::vector<const ActionEntry*>
{
    std::vector<const ActionEntry*> result;
    for (const auto& id : insertion_order_)
    {
        auto iter = actions_.find(id);
        if (iter != actions_.end() && iter->second.category == category)
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

auto ControlActionManifest::active_actions(const ContextKeyService& context) const
    -> std::vector<const ActionEntry*>
{
    std::vector<const ActionEntry*> result;
    for (const auto& id : insertion_order_)
    {
        auto iter = actions_.find(id);
        if (iter != actions_.end() && iter->second.is_enabled(context) &&
            iter->second.is_visible(context))
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

auto ControlActionManifest::has_action(const std::string& action_id) const -> bool
{
    return actions_.contains(action_id);
}

auto ControlActionManifest::action_count() const -> std::size_t
{
    return actions_.size();
}

// ── Execution ──

auto ControlActionManifest::execute_action(const std::string& action_id) -> bool
{
    auto iter = actions_.find(action_id);
    if (iter == actions_.end() || !iter->second.handler)
    {
        return false;
    }
    return iter->second.handler();
}

// ── Validation & Audit ──

auto ControlActionManifest::actions_with_status(ActionValidationStatus status) const
    -> std::vector<const ActionEntry*>
{
    std::vector<const ActionEntry*> result;
    for (const auto& id : insertion_order_)
    {
        auto iter = actions_.find(id);
        if (iter != actions_.end() && iter->second.validation_status == status)
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

auto ControlActionManifest::unresolved_actions() const -> std::vector<const ActionEntry*>
{
    std::vector<const ActionEntry*> result;
    for (const auto& id : insertion_order_)
    {
        auto iter = actions_.find(id);
        if (iter != actions_.end() && !iter->second.has_handler())
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

auto ControlActionManifest::incomplete_actions() const -> std::vector<const ActionEntry*>
{
    std::vector<const ActionEntry*> result;
    for (const auto& id : insertion_order_)
    {
        auto iter = actions_.find(id);
        if (iter != actions_.end() &&
            (iter->second.validation_status == ActionValidationStatus::kStub ||
             iter->second.validation_status == ActionValidationStatus::kPartial))
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

auto ControlActionManifest::get_categories() const -> std::vector<std::string>
{
    std::set<std::string> categories;
    for (const auto& [id, entry] : actions_)
    {
        if (!entry.category.empty())
        {
            categories.insert(entry.category);
        }
    }
    return {categories.begin(), categories.end()};
}

auto ControlActionManifest::validation_summary() const -> ValidationSummary
{
    ValidationSummary summary;
    for (const auto& [id, entry] : actions_)
    {
        switch (entry.validation_status)
        {
        case ActionValidationStatus::kLive: ++summary.live; break;
        case ActionValidationStatus::kPartial: ++summary.partial; break;
        case ActionValidationStatus::kStub: ++summary.stub; break;
        case ActionValidationStatus::kDead: ++summary.dead; break;
        case ActionValidationStatus::kDuplicate: ++summary.duplicate; break;
        case ActionValidationStatus::kDeprecated: ++summary.deprecated; break;
        case ActionValidationStatus::kGated: ++summary.gated; break;
        }
    }
    return summary;
}

} // namespace markamp::core
