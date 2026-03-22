/// @file ToolbarCommandBinder.cpp
/// @brief V21 Phase 03 — ToolbarCommandBinder implementation.

#include "ToolbarCommandBinder.h"

#include <algorithm>

namespace markamp::core
{

// ── Toolbar Binding Registration ──

void ToolbarCommandBinder::bind(ToolbarButtonBinding binding)
{
    const auto id = binding.action_id;
    if (bindings_.contains(id))
    {
        bindings_[id] = std::move(binding);
    }
    else
    {
        insertion_order_.push_back(id);
        bindings_.emplace(id, std::move(binding));
    }
}

void ToolbarCommandBinder::bind_all(std::vector<ToolbarButtonBinding> bindings)
{
    for (auto& b : bindings)
    {
        bind(std::move(b));
    }
}

auto ToolbarCommandBinder::unbind(const std::string& action_id) -> bool
{
    auto iter = bindings_.find(action_id);
    if (iter == bindings_.end()) return false;
    bindings_.erase(iter);
    std::erase(insertion_order_, action_id);
    toggle_states_.erase(action_id);
    return true;
}

// ── Lookup ──

auto ToolbarCommandBinder::get_binding(const std::string& action_id) const
    -> const ToolbarButtonBinding*
{
    auto iter = bindings_.find(action_id);
    return iter != bindings_.end() ? &iter->second : nullptr;
}

auto ToolbarCommandBinder::all_bindings() const -> std::vector<const ToolbarButtonBinding*>
{
    std::vector<const ToolbarButtonBinding*> result;
    result.reserve(insertion_order_.size());
    for (const auto& id : insertion_order_)
    {
        auto iter = bindings_.find(id);
        if (iter != bindings_.end()) result.push_back(&iter->second);
    }
    return result;
}

auto ToolbarCommandBinder::bindings_for_mode(const std::string& mode) const
    -> std::vector<const ToolbarButtonBinding*>
{
    std::vector<const ToolbarButtonBinding*> result;
    for (const auto& id : insertion_order_)
    {
        auto iter = bindings_.find(id);
        if (iter != bindings_.end() && iter->second.context_mode == mode)
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

auto ToolbarCommandBinder::global_bindings() const -> std::vector<const ToolbarButtonBinding*>
{
    return bindings_for_mode("");
}

auto ToolbarCommandBinder::binding_count() const -> std::size_t
{
    return bindings_.size();
}

// ── Execution ──

auto ToolbarCommandBinder::dispatch_click(const std::string& action_id,
                                           ControlActionManifest& manifest) -> bool
{
    // If this is a toggle, flip the state before dispatching
    auto toggle_iter = toggle_states_.find(action_id);
    if (toggle_iter != toggle_states_.end())
    {
        toggle_iter->second.current_state = !toggle_iter->second.current_state;
        auto binding_iter = bindings_.find(action_id);
        if (binding_iter != bindings_.end())
        {
            binding_iter->second.is_toggled = toggle_iter->second.current_state;
        }
    }

    return manifest.execute_action(action_id);
}

// ── Enablement Sync ──

void ToolbarCommandBinder::refresh_enablement(const ControlActionManifest& manifest,
                                               const ContextKeyService& context)
{
    for (auto& [id, binding] : bindings_)
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

auto ToolbarCommandBinder::disabled_bindings() const -> std::vector<const ToolbarButtonBinding*>
{
    std::vector<const ToolbarButtonBinding*> result;
    for (const auto& id : insertion_order_)
    {
        auto iter = bindings_.find(id);
        if (iter != bindings_.end() && !iter->second.is_enabled)
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

// ── Toggle Sync ──

void ToolbarCommandBinder::register_toggle(ToggleStateEntry entry)
{
    const auto id = entry.action_id;
    toggle_states_[id] = std::move(entry);

    // Update the binding's toggle flag
    auto iter = bindings_.find(id);
    if (iter != bindings_.end())
    {
        iter->second.is_toggle = true;
        iter->second.is_toggled = toggle_states_[id].current_state;
    }
}

void ToolbarCommandBinder::sync_toggle_states()
{
    for (auto& [id, toggle] : toggle_states_)
    {
        auto iter = bindings_.find(id);
        if (iter != bindings_.end())
        {
            iter->second.is_toggled = toggle.current_state;
        }
    }
}

void ToolbarCommandBinder::set_toggle_state(const std::string& action_id, bool state)
{
    auto toggle_iter = toggle_states_.find(action_id);
    if (toggle_iter != toggle_states_.end())
    {
        toggle_iter->second.current_state = state;
    }

    auto binding_iter = bindings_.find(action_id);
    if (binding_iter != bindings_.end())
    {
        binding_iter->second.is_toggled = state;
    }
}

auto ToolbarCommandBinder::get_toggle_state(const std::string& action_id) const -> bool
{
    auto iter = toggle_states_.find(action_id);
    return iter != toggle_states_.end() ? iter->second.current_state : false;
}

auto ToolbarCommandBinder::toggle_entries() const -> std::vector<const ToggleStateEntry*>
{
    std::vector<const ToggleStateEntry*> result;
    for (const auto& [id, entry] : toggle_states_)
    {
        result.push_back(&entry);
    }
    return result;
}

// ── Manifest Sync ──

auto ToolbarCommandBinder::sync_from_manifest(const ControlActionManifest& manifest) -> int
{
    int count = 0;
    int order = 0;

    for (const auto* action : manifest.actions_for_surface(ControlSurface::kToolbar))
    {
        if (bindings_.contains(action->action_id))
        {
            auto& binding = bindings_[action->action_id];
            binding.label = action->label;
            binding.tooltip = action->tooltip;
            binding.icon = action->icon;
            binding.shortcut_hint = action->shortcut_hint;
            binding.is_bound = action->has_handler();
            ++count;
            continue;
        }

        ToolbarButtonBinding binding;
        binding.action_id = action->action_id;
        binding.label = action->label;
        binding.tooltip = action->tooltip;
        binding.icon = action->icon;
        binding.shortcut_hint = action->shortcut_hint;
        binding.is_bound = action->has_handler();
        binding.sort_order = order++;

        bind(std::move(binding));
        ++count;
    }

    return count;
}

// ── CTA Wiring ──

void ToolbarCommandBinder::register_cta(CTABinding cta)
{
    const auto id = cta.action_id;
    if (!ctas_.contains(id))
    {
        cta_insertion_order_.push_back(id);
    }
    ctas_[id] = std::move(cta);
}

auto ToolbarCommandBinder::all_ctas() const -> std::vector<const CTABinding*>
{
    std::vector<const CTABinding*> result;
    result.reserve(cta_insertion_order_.size());
    for (const auto& id : cta_insertion_order_)
    {
        auto iter = ctas_.find(id);
        if (iter != ctas_.end()) result.push_back(&iter->second);
    }
    return result;
}

auto ToolbarCommandBinder::ctas_for_surface(const std::string& surface) const
    -> std::vector<const CTABinding*>
{
    std::vector<const CTABinding*> result;
    for (const auto& id : cta_insertion_order_)
    {
        auto iter = ctas_.find(id);
        if (iter != ctas_.end() && iter->second.surface == surface)
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

auto ToolbarCommandBinder::dispatch_cta(const std::string& action_id,
                                         ControlActionManifest& manifest) -> bool
{
    auto cta_iter = ctas_.find(action_id);
    if (cta_iter == ctas_.end()) return false;

    bool result = manifest.execute_action(action_id);
    if (result)
    {
        cta_iter->second.was_executed = true;
    }
    return result;
}

auto ToolbarCommandBinder::live_cta_count() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& [id, cta] : ctas_)
    {
        if (cta.is_bound) ++count;
    }
    return count;
}

auto ToolbarCommandBinder::dead_cta_count() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& [id, cta] : ctas_)
    {
        if (!cta.is_bound) ++count;
    }
    return count;
}

// ── Diagnostics ──

auto ToolbarCommandBinder::diagnose(const ControlActionManifest& manifest) const
    -> std::vector<ToolbarDiagnostic>
{
    std::vector<ToolbarDiagnostic> diagnostics;

    for (const auto& id : insertion_order_)
    {
        auto iter = bindings_.find(id);
        if (iter == bindings_.end()) continue;

        const auto& binding = iter->second;
        const auto* action = manifest.get_action(binding.action_id);

        if (action == nullptr)
        {
            ToolbarDiagnostic diag;
            diag.action_id = binding.action_id;
            diag.issue = "Toolbar button has no manifest action";
            diag.is_wrong_target = true;
            diagnostics.push_back(std::move(diag));
            continue;
        }

        if (!action->has_handler())
        {
            ToolbarDiagnostic diag;
            diag.action_id = binding.action_id;
            diag.issue = "Toolbar button action has no handler (dead button)";
            diag.is_dead = true;
            diagnostics.push_back(std::move(diag));
        }
    }

    return diagnostics;
}

auto ToolbarCommandBinder::live_binding_count() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& [id, binding] : bindings_)
    {
        if (binding.is_live()) ++count;
    }
    return count;
}

auto ToolbarCommandBinder::dead_binding_count() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& [id, binding] : bindings_)
    {
        if (!binding.is_bound) ++count;
    }
    return count;
}

} // namespace markamp::core
