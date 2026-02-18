// ============================================================================
// File: src/canvas/BoardLockController.cpp
// Phase 13: Canvas Collaboration — board and region locking
// ============================================================================
#include "canvas/BoardLockController.h"

#include "core/Events.h"

namespace markamp::canvas
{

BoardLockController::BoardLockController(core::EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ── Board Lock ────────────────────────────────────────────────────

auto BoardLockController::lock_board(const std::string& host_id) -> LockResult
{
    if (board_locked_)
    {
        return {false, "Board is already locked"};
    }

    board_locked_ = true;
    board_locked_by_ = host_id;
    publish_lock_event("canvas.collab.board_locked");
    return {true, ""};
}

auto BoardLockController::unlock_board(const std::string& host_id) -> LockResult
{
    if (!board_locked_)
    {
        return {false, "Board is not locked"};
    }

    if (board_locked_by_ != host_id)
    {
        return {false, "Only the host who locked the board can unlock it"};
    }

    board_locked_ = false;
    board_locked_by_.clear();
    publish_lock_event("canvas.collab.board_unlocked");
    return {true, ""};
}

auto BoardLockController::is_board_locked() const -> bool
{
    return board_locked_;
}

auto BoardLockController::locked_by() const -> const std::string&
{
    return board_locked_by_;
}

// ── Region Locks ──────────────────────────────────────────────────

auto BoardLockController::lock_region(const std::string& participant_id,
                                      const std::string& region_id,
                                      const AABB& bounds) -> LockResult
{
    if (regions_.contains(region_id))
    {
        return {false, "Region is already locked"};
    }

    LockedRegion region;
    region.region_id = region_id;
    region.bounds = bounds;
    region.locked_by = participant_id;
    regions_.emplace(region_id, std::move(region));

    publish_lock_event("canvas.collab.region_locked");
    return {true, ""};
}

auto BoardLockController::unlock_region(const std::string& participant_id,
                                        const std::string& region_id) -> LockResult
{
    auto iter = regions_.find(region_id);
    if (iter == regions_.end())
    {
        return {false, "Region not found"};
    }

    if (iter->second.locked_by != participant_id)
    {
        return {false, "Only the participant who locked the region can unlock it"};
    }

    regions_.erase(iter);
    publish_lock_event("canvas.collab.region_unlocked");
    return {true, ""};
}

auto BoardLockController::locked_regions() const
    -> const std::unordered_map<std::string, LockedRegion>&
{
    return regions_;
}

auto BoardLockController::is_point_locked(const Point2D& point,
                                          const std::string& participant_id) const -> bool
{
    for (const auto& [rid, region] : regions_)
    {
        if (region.locked_by != participant_id && region.bounds.contains(point))
        {
            return true;
        }
    }
    return false;
}

auto BoardLockController::is_object_locked(const AABB& object_bounds,
                                           const std::string& participant_id) const -> bool
{
    for (const auto& [rid, region] : regions_)
    {
        if (region.locked_by != participant_id && region.bounds.intersects(object_bounds))
        {
            return true;
        }
    }
    return false;
}

auto BoardLockController::region_lock_count() const -> size_t
{
    return regions_.size();
}

// ── Permission Check ──────────────────────────────────────────────

auto BoardLockController::can_edit(const std::string& participant_id, const Point2D& position) const
    -> bool
{
    // Full board lock blocks everyone except the host
    if (board_locked_ && participant_id != board_locked_by_)
    {
        return false;
    }

    // Check region locks
    return !is_point_locked(position, participant_id);
}

auto BoardLockController::can_edit_object(const std::string& participant_id,
                                          const AABB& object_bounds) const -> bool
{
    if (board_locked_ && participant_id != board_locked_by_)
    {
        return false;
    }

    return !is_object_locked(object_bounds, participant_id);
}

// ── Bulk Operations ───────────────────────────────────────────────

auto BoardLockController::clear_all_region_locks(const std::string& host_id) -> LockResult
{
    if (board_locked_ && board_locked_by_ != host_id)
    {
        return {false, "Only the board host can clear all region locks"};
    }

    regions_.clear();
    publish_lock_event("canvas.collab.all_regions_unlocked");
    return {true, ""};
}

auto BoardLockController::clear_participant_locks(const std::string& participant_id) -> void
{
    std::erase_if(regions_,
                  [&](const auto& pair) { return pair.second.locked_by == participant_id; });

    if (board_locked_ && board_locked_by_ == participant_id)
    {
        board_locked_ = false;
        board_locked_by_.clear();
    }
}

// ── Status ────────────────────────────────────────────────────────

auto BoardLockController::lock_scope() const -> LockScope
{
    if (board_locked_)
    {
        return LockScope::kFullBoard;
    }
    if (!regions_.empty())
    {
        return LockScope::kRegion;
    }
    return LockScope::kNone;
}

auto BoardLockController::scope_name(LockScope scope) -> std::string
{
    switch (scope)
    {
        case LockScope::kNone:
            return "Unlocked";
        case LockScope::kFullBoard:
            return "Full Board Locked";
        case LockScope::kRegion:
            return "Region Locked";
    }
    return "Unknown";
}

// ── Private ───────────────────────────────────────────────────────

auto BoardLockController::publish_lock_event(const std::string& event_name) -> void
{
    core::events::CanvasLockChangedEvent evt;
    evt.event_name = event_name;
    evt.participant_id = board_locked_by_;
    event_bus_.publish(evt);
}

} // namespace markamp::canvas
