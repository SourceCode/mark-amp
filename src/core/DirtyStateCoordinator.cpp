/// @file DirtyStateCoordinator.cpp
/// @brief V19 P03: Surface-agnostic dirty-state aggregation implementation.

#include "DirtyStateCoordinator.h"

#include "Logger.h"

#include <algorithm>

namespace markamp::core
{

DirtyStateCoordinator::DirtyStateCoordinator(EventBus& event_bus)
    : event_bus_(event_bus)
{
    MARKAMP_LOG_DEBUG("DirtyStateCoordinator initialized");
}

void DirtyStateCoordinator::register_contributor(DirtyContributor contributor)
{
    const std::string& surface_id = contributor.surface_id;
    MARKAMP_LOG_DEBUG("DirtyStateCoordinator: registered contributor '{}'", surface_id);
    contributors_.insert_or_assign(surface_id, std::move(contributor));
}

auto DirtyStateCoordinator::unregister_contributor(const std::string& surface_id) -> bool
{
    auto erased = contributors_.erase(surface_id);
    if (erased > 0)
    {
        MARKAMP_LOG_DEBUG("DirtyStateCoordinator: unregistered contributor '{}'", surface_id);
        return true;
    }
    return false;
}

auto DirtyStateCoordinator::is_session_dirty() const -> bool
{
    return std::any_of(contributors_.begin(), contributors_.end(),
                       [](const auto& pair) -> bool
                       {
                           return pair.second.is_dirty_fn != nullptr && pair.second.is_dirty_fn();
                       });
}

auto DirtyStateCoordinator::get_dirty_contributors() const -> std::vector<std::string>
{
    std::vector<std::string> dirty;
    for (const auto& [surface_id, contributor] : contributors_)
    {
        if (contributor.is_dirty_fn != nullptr && contributor.is_dirty_fn())
        {
            dirty.push_back(surface_id);
        }
    }
    return dirty;
}

auto DirtyStateCoordinator::save_all() -> int
{
    int failures = 0;
    for (auto& [surface_id, contributor] : contributors_)
    {
        if (contributor.is_dirty_fn != nullptr && contributor.is_dirty_fn())
        {
            if (contributor.save_fn != nullptr)
            {
                const bool saved = contributor.save_fn();
                if (!saved)
                {
                    ++failures;
                    MARKAMP_LOG_WARN("DirtyStateCoordinator: save failed for '{}'", surface_id);
                }
                else
                {
                    MARKAMP_LOG_DEBUG("DirtyStateCoordinator: saved '{}'", surface_id);
                }
            }
        }
    }
    return failures;
}

auto DirtyStateCoordinator::close_check() -> bool
{
    auto dirty = get_dirty_contributors();
    if (dirty.empty())
    {
        return true; // Nothing dirty, safe to close
    }

    // If we have a close policy (testing/custom), use it
    if (close_policy_ != nullptr)
    {
        return close_policy_(dirty);
    }

    // Default: attempt to save all, close if no failures
    const int failures = save_all();
    return failures == 0;
}

void DirtyStateCoordinator::set_close_policy(ClosePolicyFn policy)
{
    close_policy_ = std::move(policy);
    MARKAMP_LOG_DEBUG("DirtyStateCoordinator: close policy {}",
                      close_policy_ ? "set" : "cleared");
}

} // namespace markamp::core
