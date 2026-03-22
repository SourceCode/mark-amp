/// @file StatusBarActionRouter.cpp
/// @brief V21 Phase 03 — StatusBarActionRouter implementation.

#include "StatusBarActionRouter.h"

#include <algorithm>

namespace markamp::core
{

// ── Registration ──

void StatusBarActionRouter::register_item(StatusBarBinding binding)
{
    const auto id = binding.item_id;
    if (items_.contains(id))
    {
        items_[id] = std::move(binding);
    }
    else
    {
        insertion_order_.push_back(id);
        items_.emplace(id, std::move(binding));
    }
}

void StatusBarActionRouter::register_items(std::vector<StatusBarBinding> bindings)
{
    for (auto& b : bindings)
    {
        register_item(std::move(b));
    }
}

auto StatusBarActionRouter::remove_item(const std::string& item_id) -> bool
{
    auto iter = items_.find(item_id);
    if (iter == items_.end()) return false;
    items_.erase(iter);
    std::erase(insertion_order_, item_id);
    return true;
}

// ── Lookup ──

auto StatusBarActionRouter::get_item(const std::string& item_id) const
    -> const StatusBarBinding*
{
    auto iter = items_.find(item_id);
    return iter != items_.end() ? &iter->second : nullptr;
}

auto StatusBarActionRouter::all_items() const -> std::vector<const StatusBarBinding*>
{
    std::vector<const StatusBarBinding*> result;
    result.reserve(insertion_order_.size());
    for (const auto& id : insertion_order_)
    {
        auto iter = items_.find(id);
        if (iter != items_.end()) result.push_back(&iter->second);
    }
    return result;
}

auto StatusBarActionRouter::actionable_items() const -> std::vector<const StatusBarBinding*>
{
    std::vector<const StatusBarBinding*> result;
    for (const auto& id : insertion_order_)
    {
        auto iter = items_.find(id);
        if (iter != items_.end() && iter->second.kind != StatusItemKind::kInformational)
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

auto StatusBarActionRouter::informational_items() const -> std::vector<const StatusBarBinding*>
{
    std::vector<const StatusBarBinding*> result;
    for (const auto& id : insertion_order_)
    {
        auto iter = items_.find(id);
        if (iter != items_.end() && iter->second.kind == StatusItemKind::kInformational)
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

auto StatusBarActionRouter::item_count() const -> std::size_t
{
    return items_.size();
}

// ── Click Dispatch ──

auto StatusBarActionRouter::dispatch_click(const std::string& item_id,
                                            ControlActionManifest& manifest) -> bool
{
    auto iter = items_.find(item_id);
    if (iter == items_.end()) return false;

    const auto& binding = iter->second;
    if (binding.kind == StatusItemKind::kInformational) return false;
    if (binding.action_id.empty()) return false;

    return manifest.execute_action(binding.action_id);
}

// ── Enablement Sync ──

void StatusBarActionRouter::refresh_enablement(const ControlActionManifest& manifest,
                                                const ContextKeyService& context)
{
    for (auto& [id, binding] : items_)
    {
        if (binding.kind == StatusItemKind::kInformational) continue;

        const auto* action = manifest.get_action(binding.action_id);
        if (action != nullptr)
        {
            binding.is_enabled = action->is_enabled(context);
            binding.is_visible = action->is_visible(context);
            binding.is_bound = action->has_handler();
        }
        else
        {
            binding.is_enabled = false;
            binding.is_bound = false;
        }
    }
}

// ── Manifest Sync ──

auto StatusBarActionRouter::sync_from_manifest(const ControlActionManifest& manifest) -> int
{
    int count = 0;

    for (const auto* action : manifest.actions_for_surface(ControlSurface::kStatusBar))
    {
        if (items_.contains(action->action_id))
        {
            auto& item = items_[action->action_id];
            item.label = action->label;
            item.tooltip = action->tooltip;
            item.is_bound = action->has_handler();
            ++count;
            continue;
        }

        StatusBarBinding binding;
        binding.item_id = action->action_id;
        binding.action_id = action->action_id;
        binding.label = action->label;
        binding.tooltip = action->tooltip;
        binding.kind = StatusItemKind::kActionable;
        binding.is_bound = action->has_handler();
        binding.is_enabled = true;
        binding.is_visible = true;

        register_item(std::move(binding));
        ++count;
    }

    return count;
}

// ── Diagnostics ──

auto StatusBarActionRouter::diagnose(const ControlActionManifest& manifest) const
    -> std::vector<StatusBarDiagnostic>
{
    std::vector<StatusBarDiagnostic> diagnostics;

    for (const auto& id : insertion_order_)
    {
        auto iter = items_.find(id);
        if (iter == items_.end()) continue;

        const auto& binding = iter->second;
        if (binding.kind == StatusItemKind::kInformational) continue;

        const auto* action = manifest.get_action(binding.action_id);

        if (action == nullptr)
        {
            StatusBarDiagnostic diag;
            diag.item_id = binding.item_id;
            diag.action_id = binding.action_id;
            diag.issue = "Status item targets non-existent action";
            diag.is_misleading = true;
            diagnostics.push_back(std::move(diag));
            continue;
        }

        if (!action->has_handler())
        {
            StatusBarDiagnostic diag;
            diag.item_id = binding.item_id;
            diag.action_id = binding.action_id;
            diag.issue = "Status item targets action with no handler";
            diag.is_dead_action = true;
            diagnostics.push_back(std::move(diag));
        }
    }

    return diagnostics;
}

auto StatusBarActionRouter::live_action_count() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& [id, binding] : items_)
    {
        if (binding.kind != StatusItemKind::kInformational && binding.is_bound)
        {
            ++count;
        }
    }
    return count;
}

auto StatusBarActionRouter::dead_action_count() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& [id, binding] : items_)
    {
        if (binding.kind != StatusItemKind::kInformational && !binding.is_bound)
        {
            ++count;
        }
    }
    return count;
}

} // namespace markamp::core
