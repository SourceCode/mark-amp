/// @file ContextMenuActionBinder.cpp
/// @brief V21 Phase 04 — ContextMenuActionBinder implementation.

#include "ContextMenuActionBinder.h"

#include <algorithm>

namespace markamp::core
{

// ── Context Menu Registration ──

void ContextMenuActionBinder::register_context(const std::string& context_type,
                                                std::vector<ContextMenuBinding> bindings)
{
    if (!contexts_.contains(context_type))
    {
        context_order_.push_back(context_type);
    }
    contexts_[context_type] = std::move(bindings);
}

auto ContextMenuActionBinder::registered_contexts() const -> std::vector<std::string>
{
    return context_order_;
}

auto ContextMenuActionBinder::has_context(const std::string& context_type) const -> bool
{
    return contexts_.contains(context_type);
}

auto ContextMenuActionBinder::bindings_for_context(const std::string& context_type) const
    -> std::vector<const ContextMenuBinding*>
{
    std::vector<const ContextMenuBinding*> result;
    auto iter = contexts_.find(context_type);
    if (iter != contexts_.end())
    {
        for (const auto& binding : iter->second)
        {
            result.push_back(&binding);
        }
    }
    return result;
}

auto ContextMenuActionBinder::context_count() const -> std::size_t
{
    return contexts_.size();
}

// ── Target-Aware Resolution ──

auto ContextMenuActionBinder::resolve(const std::string& context_type,
                                       const TargetContext& target,
                                       const ControlActionManifest& manifest) const
    -> std::vector<ContextMenuBinding>
{
    auto iter = contexts_.find(context_type);
    if (iter == contexts_.end()) return {};

    std::vector<ContextMenuBinding> resolved;
    resolved.reserve(iter->second.size());

    for (auto binding : iter->second) // Copy each binding for modification
    {
        // Check if blocked as no-op
        if (is_blocked(binding.action_id))
        {
            binding.is_enabled = false;
            binding.is_visible = false;
        }

        // Verify against manifest
        const auto* action = manifest.get_action(binding.action_id);
        if (action != nullptr)
        {
            binding.is_bound = action->has_handler();
            if (!action->has_handler())
            {
                binding.is_enabled = false;
            }
        }
        else
        {
            binding.is_bound = false;
            binding.is_enabled = false;
        }

        // Target-aware enablement rules
        switch (target.type)
        {
        case TargetType::kFile:
            if (binding.action_id.starts_with("folder."))
                binding.is_visible = false;
            if (target.is_readonly && binding.is_destructive)
                binding.is_enabled = false;
            break;

        case TargetType::kFolder:
            if (binding.action_id.starts_with("file.edit"))
                binding.is_visible = false;
            break;

        case TargetType::kEmptyArea:
            // Most item-specific actions should be hidden
            if (binding.action_id.starts_with("file.") ||
                binding.action_id.starts_with("folder."))
                binding.is_visible = false;
            break;

        case TargetType::kTab:
            if (target.is_dirty && binding.action_id == "tab.close")
                binding.is_destructive = true;
            break;

        case TargetType::kEditorSelection:
            if (!target.has_selection && binding.action_id.starts_with("selection."))
                binding.is_enabled = false;
            break;

        default:
            break;
        }

        resolved.push_back(std::move(binding));
    }

    return resolved;
}

// ── Dispatch ──

auto ContextMenuActionBinder::dispatch(const std::string& action_id,
                                        const TargetContext& /*target*/,
                                        ControlActionManifest& manifest) -> bool
{
    if (is_blocked(action_id)) return false;
    return manifest.execute_action(action_id);
}

// ── No-Op Blocking ──

void ContextMenuActionBinder::block_noop(const std::string& action_id,
                                          const std::string& reason)
{
    blocked_[action_id] = reason;
}

void ContextMenuActionBinder::unblock(const std::string& action_id)
{
    blocked_.erase(action_id);
}

auto ContextMenuActionBinder::is_blocked(const std::string& action_id) const -> bool
{
    return blocked_.contains(action_id);
}

auto ContextMenuActionBinder::block_reason(const std::string& action_id) const -> std::string
{
    auto iter = blocked_.find(action_id);
    return iter != blocked_.end() ? iter->second : "";
}

auto ContextMenuActionBinder::blocked_actions() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    result.reserve(blocked_.size());
    for (const auto& [id, reason] : blocked_)
    {
        result.push_back(id);
    }
    std::sort(result.begin(), result.end());
    return result;
}

// ── Keyboard Accessibility ──

void ContextMenuActionBinder::set_keyboard_accessible(const std::string& action_id,
                                                       bool accessible)
{
    keyboard_access_[action_id] = accessible;
}

auto ContextMenuActionBinder::is_keyboard_accessible(const std::string& action_id) const -> bool
{
    auto iter = keyboard_access_.find(action_id);
    return iter != keyboard_access_.end() ? iter->second : false;
}

auto ContextMenuActionBinder::keyboard_gaps() const -> std::vector<std::string>
{
    std::vector<std::string> gaps;
    for (const auto& [ctx_type, bindings] : contexts_)
    {
        for (const auto& binding : bindings)
        {
            if (binding.is_bound && binding.is_enabled)
            {
                auto iter = keyboard_access_.find(binding.action_id);
                if (iter == keyboard_access_.end() || !iter->second)
                {
                    gaps.push_back(binding.action_id);
                }
            }
        }
    }
    // Deduplicate
    std::sort(gaps.begin(), gaps.end());
    gaps.erase(std::unique(gaps.begin(), gaps.end()), gaps.end());
    return gaps;
}

// ── Diagnostics ──

auto ContextMenuActionBinder::diagnose(const ControlActionManifest& manifest) const
    -> std::vector<ContextMenuDiagnostic>
{
    std::vector<ContextMenuDiagnostic> diagnostics;

    for (const auto& [ctx_type, bindings] : contexts_)
    {
        for (const auto& binding : bindings)
        {
            const auto* action = manifest.get_action(binding.action_id);

            if (action == nullptr)
            {
                ContextMenuDiagnostic diag;
                diag.context_type = ctx_type;
                diag.action_id = binding.action_id;
                diag.issue = "Context menu item has no manifest action";
                diag.is_missing_target = true;
                diagnostics.push_back(std::move(diag));
                continue;
            }

            if (!action->has_handler())
            {
                ContextMenuDiagnostic diag;
                diag.context_type = ctx_type;
                diag.action_id = binding.action_id;
                diag.issue = "Context menu action has no handler";
                diag.is_noop = true;
                diagnostics.push_back(std::move(diag));
            }

            auto kb_iter = keyboard_access_.find(binding.action_id);
            if (kb_iter == keyboard_access_.end() || !kb_iter->second)
            {
                if (action->has_handler())
                {
                    ContextMenuDiagnostic diag;
                    diag.context_type = ctx_type;
                    diag.action_id = binding.action_id;
                    diag.issue = "Context action not keyboard-accessible";
                    diag.is_no_keyboard = true;
                    diagnostics.push_back(std::move(diag));
                }
            }
        }
    }

    return diagnostics;
}

auto ContextMenuActionBinder::total_live_bindings() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& [ctx, bindings] : contexts_)
    {
        for (const auto& b : bindings)
        {
            if (b.is_bound && b.is_enabled) ++count;
        }
    }
    return count;
}

auto ContextMenuActionBinder::total_dead_bindings() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& [ctx, bindings] : contexts_)
    {
        for (const auto& b : bindings)
        {
            if (!b.is_bound) ++count;
        }
    }
    return count;
}

} // namespace markamp::core
