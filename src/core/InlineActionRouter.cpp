/// @file InlineActionRouter.cpp
/// @brief V21 Phase 04 — InlineActionRouter implementation.

#include "InlineActionRouter.h"

#include <algorithm>

namespace markamp::core
{

// ── Registration ──

void InlineActionRouter::register_action(InlineActionBinding binding)
{
    const auto id = binding.action_id;
    if (actions_.contains(id))
    {
        actions_[id] = std::move(binding);
    }
    else
    {
        insertion_order_.push_back(id);
        actions_.emplace(id, std::move(binding));
    }
}

void InlineActionRouter::register_actions(std::vector<InlineActionBinding> bindings)
{
    for (auto& b : bindings) register_action(std::move(b));
}

auto InlineActionRouter::remove_action(const std::string& action_id) -> bool
{
    auto iter = actions_.find(action_id);
    if (iter == actions_.end()) return false;
    actions_.erase(iter);
    std::erase(insertion_order_, action_id);
    return true;
}

// ── Lookup ──

auto InlineActionRouter::get_action(const std::string& action_id) const
    -> const InlineActionBinding*
{
    auto iter = actions_.find(action_id);
    return iter != actions_.end() ? &iter->second : nullptr;
}

auto InlineActionRouter::all_actions() const -> std::vector<const InlineActionBinding*>
{
    std::vector<const InlineActionBinding*> result;
    result.reserve(insertion_order_.size());
    for (const auto& id : insertion_order_)
    {
        auto iter = actions_.find(id);
        if (iter != actions_.end()) result.push_back(&iter->second);
    }
    return result;
}

auto InlineActionRouter::actions_for_surface(const std::string& surface) const
    -> std::vector<const InlineActionBinding*>
{
    std::vector<const InlineActionBinding*> result;
    for (const auto& id : insertion_order_)
    {
        auto iter = actions_.find(id);
        if (iter != actions_.end() && iter->second.surface == surface)
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

auto InlineActionRouter::actions_for_host(const std::string& host_id) const
    -> std::vector<const InlineActionBinding*>
{
    std::vector<const InlineActionBinding*> result;
    for (const auto& id : insertion_order_)
    {
        auto iter = actions_.find(id);
        if (iter != actions_.end() && iter->second.host_id == host_id)
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

auto InlineActionRouter::action_count() const -> std::size_t
{
    return actions_.size();
}

// ── Dispatch ──

auto InlineActionRouter::dispatch(const std::string& action_id,
                                   ControlActionManifest& manifest) -> bool
{
    auto iter = actions_.find(action_id);
    if (iter == actions_.end()) return false;
    return manifest.execute_action(action_id);
}

// ── Enablement ──

void InlineActionRouter::refresh_enablement(const ControlActionManifest& manifest,
                                             const ContextKeyService& context)
{
    for (auto& [id, binding] : actions_)
    {
        const auto* action = manifest.get_action(id);
        if (action != nullptr)
        {
            binding.is_enabled = action->is_enabled(context);
            binding.is_visible = action->is_visible(context);
            binding.is_bound = action->has_handler();
        }
        else
        {
            binding.is_enabled = false;
            binding.is_visible = false;
            binding.is_bound = false;
        }
    }
}

// ── Diagnostics ──

auto InlineActionRouter::diagnose(const ControlActionManifest& manifest) const
    -> std::vector<InlineActionDiagnostic>
{
    std::vector<InlineActionDiagnostic> diagnostics;

    for (const auto& id : insertion_order_)
    {
        auto iter = actions_.find(id);
        if (iter == actions_.end()) continue;

        const auto& binding = iter->second;
        const auto* action = manifest.get_action(binding.action_id);

        if (action == nullptr)
        {
            InlineActionDiagnostic diag;
            diag.action_id = binding.action_id;
            diag.surface = binding.surface;
            diag.host_id = binding.host_id;
            diag.issue = "Inline action has no manifest action";
            diag.is_orphaned = true;
            diagnostics.push_back(std::move(diag));
            continue;
        }

        if (!action->has_handler())
        {
            InlineActionDiagnostic diag;
            diag.action_id = binding.action_id;
            diag.surface = binding.surface;
            diag.host_id = binding.host_id;
            diag.issue = "Inline action has no handler";
            diag.is_dead = true;
            diagnostics.push_back(std::move(diag));
        }
    }

    return diagnostics;
}

auto InlineActionRouter::live_count() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& [id, binding] : actions_)
    {
        if (binding.is_bound && binding.is_enabled) ++count;
    }
    return count;
}

auto InlineActionRouter::dead_count() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& [id, binding] : actions_)
    {
        if (!binding.is_bound) ++count;
    }
    return count;
}

} // namespace markamp::core
