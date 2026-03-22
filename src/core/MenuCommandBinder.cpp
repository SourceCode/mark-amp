/// @file MenuCommandBinder.cpp
/// @brief V21 Phase 02 — MenuCommandBinder implementation.

#include "MenuCommandBinder.h"

#include <algorithm>
#include <set>

namespace markamp::core
{

// ── Binding Registration ──

void MenuCommandBinder::bind(MenuItemBinding binding)
{
    const auto id = binding.action_id;
    const auto menu_id = binding.menu_item_id;

    if (bindings_.contains(id))
    {
        // Remove old reverse lookup
        auto old_menu_id = bindings_[id].menu_item_id;
        menu_id_to_action_.erase(old_menu_id);
        bindings_[id] = std::move(binding);
    }
    else
    {
        insertion_order_.push_back(id);
        bindings_.emplace(id, std::move(binding));
    }

    if (menu_id != 0)
    {
        menu_id_to_action_[menu_id] = id;
    }
}

void MenuCommandBinder::bind_all(std::vector<MenuItemBinding> bindings)
{
    for (auto& b : bindings)
    {
        bind(std::move(b));
    }
}

auto MenuCommandBinder::unbind(const std::string& action_id) -> bool
{
    auto iter = bindings_.find(action_id);
    if (iter == bindings_.end())
    {
        return false;
    }

    menu_id_to_action_.erase(iter->second.menu_item_id);
    bindings_.erase(iter);
    std::erase(insertion_order_, action_id);
    return true;
}

// ── Lookup ──

auto MenuCommandBinder::get_binding(const std::string& action_id) const
    -> const MenuItemBinding*
{
    auto iter = bindings_.find(action_id);
    return iter != bindings_.end() ? &iter->second : nullptr;
}

auto MenuCommandBinder::get_binding_by_menu_id(int menu_item_id) const
    -> const MenuItemBinding*
{
    auto iter = menu_id_to_action_.find(menu_item_id);
    if (iter == menu_id_to_action_.end())
    {
        return nullptr;
    }
    return get_binding(iter->second);
}

auto MenuCommandBinder::all_bindings() const -> std::vector<const MenuItemBinding*>
{
    std::vector<const MenuItemBinding*> result;
    result.reserve(insertion_order_.size());
    for (const auto& id : insertion_order_)
    {
        auto iter = bindings_.find(id);
        if (iter != bindings_.end())
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

auto MenuCommandBinder::bindings_for_menu(const std::string& menu_prefix) const
    -> std::vector<const MenuItemBinding*>
{
    std::vector<const MenuItemBinding*> result;
    for (const auto& id : insertion_order_)
    {
        auto iter = bindings_.find(id);
        if (iter != bindings_.end() &&
            iter->second.menu_path.starts_with(menu_prefix))
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

auto MenuCommandBinder::binding_count() const -> std::size_t
{
    return bindings_.size();
}

// ── Execution ──

auto MenuCommandBinder::dispatch_menu_event(int menu_item_id,
                                             ControlActionManifest& manifest) -> bool
{
    auto action_iter = menu_id_to_action_.find(menu_item_id);
    if (action_iter == menu_id_to_action_.end())
    {
        return false;
    }
    return manifest.execute_action(action_iter->second);
}

// ── Enablement Sync ──

void MenuCommandBinder::refresh_enablement(const ControlActionManifest& manifest,
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

auto MenuCommandBinder::disabled_bindings() const -> std::vector<const MenuItemBinding*>
{
    std::vector<const MenuItemBinding*> result;
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

auto MenuCommandBinder::hidden_bindings() const -> std::vector<const MenuItemBinding*>
{
    std::vector<const MenuItemBinding*> result;
    for (const auto& id : insertion_order_)
    {
        auto iter = bindings_.find(id);
        if (iter != bindings_.end() && !iter->second.is_visible)
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

// ── Accelerator Generation ──

auto MenuCommandBinder::generate_accelerators() const -> std::vector<AcceleratorBinding>
{
    std::vector<AcceleratorBinding> accelerators;
    for (const auto& id : insertion_order_)
    {
        auto iter = bindings_.find(id);
        if (iter != bindings_.end() && !iter->second.shortcut_display.empty() &&
            iter->second.is_bound)
        {
            AcceleratorBinding accel;
            accel.action_id = id;
            accel.display = iter->second.shortcut_display;
            // key_code and modifiers would be derived from parsing the display string
            // in a wxWidgets integration; for now we expose the display
            accelerators.push_back(std::move(accel));
        }
    }
    return accelerators;
}

// ── Manifest Synchronization ──

auto MenuCommandBinder::sync_from_manifest(const ControlActionManifest& manifest) -> int
{
    int count = 0;
    int next_id = 10000; // Starting menu item ID range for manifest-driven items

    for (const auto* action : manifest.actions_for_surface(ControlSurface::kMenu))
    {
        if (bindings_.contains(action->action_id))
        {
            // Update existing binding
            auto& binding = bindings_[action->action_id];
            binding.label = action->label;
            binding.shortcut_display = action->shortcut_hint;
            binding.is_bound = action->has_handler();
            ++count;
            continue;
        }

        MenuItemBinding binding;
        binding.action_id = action->action_id;
        binding.label = action->label;
        binding.shortcut_display = action->shortcut_hint;
        binding.menu_path = action->category + "/" + action->label;
        binding.menu_item_id = next_id++;
        binding.is_bound = action->has_handler();
        binding.is_enabled = true;
        binding.is_visible = true;

        bind(std::move(binding));
        ++count;
    }

    return count;
}

// ── Diagnostics ──

auto MenuCommandBinder::diagnose(const ControlActionManifest& manifest) const
    -> std::vector<MenuBindingDiagnostic>
{
    std::vector<MenuBindingDiagnostic> diagnostics;
    std::set<std::string> seen_actions;

    for (const auto& id : insertion_order_)
    {
        auto iter = bindings_.find(id);
        if (iter == bindings_.end())
        {
            continue;
        }

        const auto& binding = iter->second;
        const auto* action = manifest.get_action(binding.action_id);

        MenuBindingDiagnostic diag;
        diag.action_id = binding.action_id;
        diag.menu_path = binding.menu_path;

        if (action == nullptr)
        {
            diag.issue = "Menu item has no corresponding manifest action";
            diag.is_orphaned = true;
            diagnostics.push_back(std::move(diag));
            continue;
        }

        if (!action->has_handler())
        {
            diag.issue = "Action has no handler (dead menu item)";
            diag.is_dead = true;
            diagnostics.push_back(std::move(diag));
            continue;
        }

        if (action->validation_status == ActionValidationStatus::kStub)
        {
            diag.issue = "Action handler is a stub";
            diag.is_stub = true;
            diagnostics.push_back(std::move(diag));
            continue;
        }

        if (!seen_actions.insert(binding.action_id).second)
        {
            diag.issue = "Duplicate menu binding for same action";
            diag.is_duplicate = true;
            diagnostics.push_back(std::move(diag));
        }
    }

    return diagnostics;
}

auto MenuCommandBinder::live_binding_count() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& [id, binding] : bindings_)
    {
        if (binding.is_live())
        {
            ++count;
        }
    }
    return count;
}

auto MenuCommandBinder::dead_binding_count() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& [id, binding] : bindings_)
    {
        if (!binding.is_bound)
        {
            ++count;
        }
    }
    return count;
}

// ── Settings Deep-Link ──

void MenuCommandBinder::register_deep_link(const std::string& action_id,
                                            DeepLinkTarget target)
{
    deep_links_[action_id] = std::move(target);
}

auto MenuCommandBinder::get_deep_link(const std::string& action_id) const
    -> const DeepLinkTarget*
{
    auto iter = deep_links_.find(action_id);
    return iter != deep_links_.end() ? &iter->second : nullptr;
}

auto MenuCommandBinder::is_deep_link(const std::string& action_id) const -> bool
{
    return deep_links_.contains(action_id);
}

} // namespace markamp::core
