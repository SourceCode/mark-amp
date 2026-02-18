/// @file WorkspaceHookManager.cpp
/// @brief V9 Phase 40 — WorkspaceHookManager implementation.

#include "WorkspaceHookManager.h"

#include <algorithm>
#include <set>

namespace markamp::core
{

auto hook_type_name(WorkspaceHookType hook_type) -> std::string
{
    switch (hook_type)
    {
        case WorkspaceHookType::kOnOpen:
            return "on_open";
        case WorkspaceHookType::kOnClose:
            return "on_close";
        case WorkspaceHookType::kOnSave:
            return "on_save";
        case WorkspaceHookType::kOnCreate:
            return "on_create";
        case WorkspaceHookType::kOnDelete:
            return "on_delete";
        case WorkspaceHookType::kOnRename:
            return "on_rename";
        case WorkspaceHookType::kOnFocusGain:
            return "on_focus_gain";
        case WorkspaceHookType::kOnFocusLost:
            return "on_focus_lost";
        case WorkspaceHookType::kOnStartup:
            return "on_startup";
        case WorkspaceHookType::kOnShutdown:
            return "on_shutdown";
    }
    return "unknown";
}

void WorkspaceHookManager::register_hook(HookRegistration registration)
{
    if (registration.hook_id.empty())
    {
        registration.hook_id = "hook_" + std::to_string(hooks_.size());
    }
    hooks_.push_back(std::move(registration));
}

auto WorkspaceHookManager::unregister_hook(const std::string& hook_id) -> bool
{
    auto iter = std::remove_if(hooks_.begin(),
                               hooks_.end(),
                               [&](const HookRegistration& reg) { return reg.hook_id == hook_id; });
    if (iter == hooks_.end())
    {
        return false;
    }
    hooks_.erase(iter, hooks_.end());
    return true;
}

auto WorkspaceHookManager::find_hook(const std::string& hook_id) const -> const HookRegistration*
{
    for (const auto& reg : hooks_)
    {
        if (reg.hook_id == hook_id)
        {
            return &reg;
        }
    }
    return nullptr;
}

auto WorkspaceHookManager::hook_count() const -> int
{
    return static_cast<int>(hooks_.size());
}

auto WorkspaceHookManager::enable_hook(const std::string& hook_id) -> bool
{
    for (auto& reg : hooks_)
    {
        if (reg.hook_id == hook_id)
        {
            reg.enabled = true;
            return true;
        }
    }
    return false;
}

auto WorkspaceHookManager::disable_hook(const std::string& hook_id) -> bool
{
    for (auto& reg : hooks_)
    {
        if (reg.hook_id == hook_id)
        {
            reg.enabled = false;
            return true;
        }
    }
    return false;
}

void WorkspaceHookManager::fire_hooks(WorkspaceHookType hook_type, const HookContext& context) const
{
    // Collect matching hooks
    std::vector<const HookRegistration*> matching;
    for (const auto& reg : hooks_)
    {
        if (reg.hook_type == hook_type && reg.enabled && reg.callback)
        {
            matching.push_back(&reg);
        }
    }

    // Sort by priority (higher first)
    std::sort(matching.begin(),
              matching.end(),
              [](const HookRegistration* lhs, const HookRegistration* rhs)
              { return lhs->priority > rhs->priority; });

    for (const auto* reg : matching)
    {
        reg->callback(context);
    }
}

auto WorkspaceHookManager::hooks_for_type(WorkspaceHookType hook_type) const
    -> std::vector<const HookRegistration*>
{
    std::vector<const HookRegistration*> result;
    for (const auto& reg : hooks_)
    {
        if (reg.hook_type == hook_type)
        {
            result.push_back(&reg);
        }
    }
    return result;
}

auto WorkspaceHookManager::all_hooks() const -> std::vector<const HookRegistration*>
{
    std::vector<const HookRegistration*> result;
    result.reserve(hooks_.size());
    for (const auto& reg : hooks_)
    {
        result.push_back(&reg);
    }
    return result;
}

auto WorkspaceHookManager::registered_types() const -> std::vector<WorkspaceHookType>
{
    std::set<WorkspaceHookType> seen;
    for (const auto& reg : hooks_)
    {
        seen.insert(reg.hook_type);
    }
    return {seen.begin(), seen.end()};
}

void WorkspaceHookManager::clear_all()
{
    hooks_.clear();
}

} // namespace markamp::core
