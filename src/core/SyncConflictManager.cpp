// ============================================================================
// File: src/core/SyncConflictManager.cpp
// Phase 27: Cloud Sync & Collaboration — Conflict resolution management
// ============================================================================

#include "SyncConflictManager.h"

#include "EventBus.h"
#include "Events.h"
#include "SyncEngine.h"

#include <algorithm>
#include <chrono>

namespace markamp::core
{

SyncConflictManager::SyncConflictManager(EventBus& event_bus, SyncEngine& sync_engine)
    : event_bus_(event_bus)
    , sync_engine_(sync_engine)
{
}

auto SyncConflictManager::set_pending_conflicts(const std::vector<SyncConflict>& conflicts) -> void
{
    pending_conflicts_ = conflicts;

    if (!conflicts.empty())
    {
        events::SyncConflictDetectedEvent evt;
        evt.file_path = conflicts[0].relative_path.string();
        evt.conflict_count = static_cast<int32_t>(conflicts.size());
        event_bus_.publish(evt);
    }
}

auto SyncConflictManager::pending_conflicts() const -> const std::vector<SyncConflict>&
{
    return pending_conflicts_;
}

auto SyncConflictManager::pending_count() const -> int32_t
{
    return static_cast<int32_t>(pending_conflicts_.size());
}

auto SyncConflictManager::has_conflicts() const -> bool
{
    return !pending_conflicts_.empty();
}

auto SyncConflictManager::resolve_conflict(const std::string& file_path,
                                           SyncConflictResolution strategy) -> bool
{
    auto iter = std::find_if(pending_conflicts_.begin(),
                             pending_conflicts_.end(),
                             [&file_path](const SyncConflict& conflict)
                             { return conflict.relative_path.string() == file_path; });

    if (iter == pending_conflicts_.end())
    {
        return false;
    }

    iter->resolved = true;
    iter->resolution = strategy;

    record_resolution(file_path, strategy);

    events::SyncConflictResolvedEvent evt;
    evt.file_path = file_path;
    evt.resolution_strategy = static_cast<int>(strategy);
    event_bus_.publish(evt);

    // Remove from pending.
    pending_conflicts_.erase(iter);
    return true;
}

auto SyncConflictManager::resolve_all(SyncConflictResolution strategy) -> int32_t
{
    const int32_t count = static_cast<int32_t>(pending_conflicts_.size());

    for (auto& conflict : pending_conflicts_)
    {
        conflict.resolved = true;
        conflict.resolution = strategy;
        record_resolution(conflict.relative_path.string(), strategy);

        events::SyncConflictResolvedEvent evt;
        evt.file_path = conflict.relative_path.string();
        evt.resolution_strategy = static_cast<int>(strategy);
        event_bus_.publish(evt);
    }

    pending_conflicts_.clear();
    return count;
}

auto SyncConflictManager::auto_resolve() -> int32_t
{
    return resolve_all(default_strategy_);
}

auto SyncConflictManager::set_default_strategy(SyncConflictResolution strategy) -> void
{
    default_strategy_ = strategy;
}

auto SyncConflictManager::default_strategy() const -> SyncConflictResolution
{
    return default_strategy_;
}

auto SyncConflictManager::find_conflict(const std::string& file_path) const
    -> std::optional<SyncConflict>
{
    auto iter = std::find_if(pending_conflicts_.begin(),
                             pending_conflicts_.end(),
                             [&file_path](const SyncConflict& conflict)
                             { return conflict.relative_path.string() == file_path; });

    if (iter != pending_conflicts_.end())
    {
        return *iter;
    }
    return std::nullopt;
}

auto SyncConflictManager::resolution_history() const -> const std::vector<ConflictResolution>&
{
    return resolution_history_;
}

auto SyncConflictManager::resolution_count() const -> int32_t
{
    return static_cast<int32_t>(resolution_history_.size());
}

auto SyncConflictManager::clear_history() -> void
{
    resolution_history_.clear();
}

auto SyncConflictManager::clear_pending() -> void
{
    pending_conflicts_.clear();
}

auto SyncConflictManager::record_resolution(const std::string& file_path,
                                            SyncConflictResolution strategy) -> void
{
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    const auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now).count();

    ConflictResolution resolution;
    resolution.file_path = file_path;
    resolution.strategy = strategy;
    resolution.resolved_at = timestamp;

    switch (strategy)
    {
        case SyncConflictResolution::KeepLocal:
            resolution.kept_version = "local";
            break;
        case SyncConflictResolution::KeepRemote:
            resolution.kept_version = "remote";
            break;
        case SyncConflictResolution::KeepBoth:
            resolution.kept_version = "both";
            break;
        case SyncConflictResolution::KeepNewer:
            resolution.kept_version = "newer";
            break;
        case SyncConflictResolution::AskUser:
            resolution.kept_version = "manual";
            break;
    }

    resolution_history_.push_back(std::move(resolution));

    // Cap history at 500 entries.
    if (resolution_history_.size() > 500)
    {
        resolution_history_.erase(resolution_history_.begin());
    }
}

} // namespace markamp::core
