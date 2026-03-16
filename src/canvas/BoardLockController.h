// ============================================================================
// File: src/canvas/BoardLockController.h
// Phase 13: Canvas Collaboration — board and region locking
// ============================================================================
#pragma once

#include "canvas/CanvasTypes.h"
#include "core/EventBus.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::canvas
{

/// Lock scope: entire board or a specific region.
enum class LockScope : uint8_t
{
    kNone,
    kFullBoard,
    kRegion
};

/// A locked region on the canvas.
struct LockedRegion
{
    std::string region_id;
    AABB bounds;              ///< The locked area
    std::string locked_by;    ///< Participant who locked it
    bool allow_view{true};    ///< Others can still view
    bool allow_select{false}; ///< Others can select objects (read-only)

    /// Whether the region is view-only (can view but not select).
    [[nodiscard]] auto is_view_only() const noexcept -> bool
    {
        return allow_view && !allow_select;
    }

    /// Whether the region is fully locked (no view, no select).
    [[nodiscard]] auto is_fully_locked() const noexcept -> bool
    {
        return !allow_view && !allow_select;
    }
};

/// Result of a lock operation.
struct LockResult
{
    bool success{false};
    std::string error_message;

    /// Whether the lock operation failed.
    [[nodiscard]] auto failed() const noexcept -> bool
    {
        return !success;
    }
};

/// Controls board-level and region-level locking for collaborative sessions.
///
/// Features:
///   - Lock entire board (read-only for non-hosts)
///   - Lock specific regions while others remain editable
///   - Lock status broadcast via EventBus
///   - Host-only lock control (only the session host can lock/unlock)
class BoardLockController
{
public:
    explicit BoardLockController(core::EventBus& event_bus);

    // ── Board Lock ────────────────────────────────────────────────

    /// Lock the entire board. Only the host can do this.
    auto lock_board(const std::string& host_id) -> LockResult;

    /// Unlock the entire board.
    auto unlock_board(const std::string& host_id) -> LockResult;

    /// Is the board fully locked?
    [[nodiscard]] auto is_board_locked() const -> bool;

    /// Who locked the board.
    [[nodiscard]] auto locked_by() const -> const std::string&;

    // ── Region Locks ──────────────────────────────────────────────

    /// Lock a specific region on the canvas.
    auto lock_region(const std::string& participant_id,
                     const std::string& region_id,
                     const AABB& bounds) -> LockResult;

    /// Unlock a specific region.
    auto unlock_region(const std::string& participant_id, const std::string& region_id)
        -> LockResult;

    /// Get all locked regions.
    [[nodiscard]] auto locked_regions() const
        -> const std::unordered_map<std::string, LockedRegion>&;

    /// Check if a point is within any locked region.
    [[nodiscard]] auto is_point_locked(const Point2D& point,
                                       const std::string& participant_id) const -> bool;

    /// Check if a specific object (by bounds) is within a locked region.
    [[nodiscard]] auto is_object_locked(const AABB& object_bounds,
                                        const std::string& participant_id) const -> bool;

    /// Number of active region locks.
    [[nodiscard]] auto region_lock_count() const -> size_t;

    // ── Permission Check ──────────────────────────────────────────

    /// Can the given participant edit at the given position?
    [[nodiscard]] auto can_edit(const std::string& participant_id, const Point2D& position) const
        -> bool;

    /// Can the given participant edit the given object?
    [[nodiscard]] auto can_edit_object(const std::string& participant_id,
                                       const AABB& object_bounds) const -> bool;

    // ── Bulk Operations ───────────────────────────────────────────

    /// Clear all region locks (host only).
    auto clear_all_region_locks(const std::string& host_id) -> LockResult;

    /// Clear all locks for a specific participant (e.g. when they leave).
    auto clear_participant_locks(const std::string& participant_id) -> void;

    // ── Status ────────────────────────────────────────────────────

    [[nodiscard]] auto lock_scope() const -> LockScope;

    /// Human-readable lock scope name.
    [[nodiscard]] static auto scope_name(LockScope scope) -> std::string;

    /// Whether any region locks exist.
    [[nodiscard]] auto has_region_locks() const noexcept -> bool
    {
        return !regions_.empty();
    }

    /// Whether the board is completely unlocked.
    [[nodiscard]] auto is_unlocked() const noexcept -> bool
    {
        return !board_locked_ && regions_.empty();
    }

    // ── Round 2 Batch 10 (#96-98) ────────────────────────────────

    /// (#96) Whether the entire board is locked.
    [[nodiscard]] auto is_full_board_lock() const noexcept -> bool
    {
        return board_locked_;
    }

    /// (#97) Whether any regions (but not the full board) are locked.
    [[nodiscard]] auto is_region_lock_only() const noexcept -> bool
    {
        return !board_locked_ && !regions_.empty();
    }

    /// (#98) Whether no lock scope is active.
    [[nodiscard]] auto is_no_lock() const noexcept -> bool
    {
        return !board_locked_ && regions_.empty();
    }

private:
    core::EventBus& event_bus_;
    bool board_locked_{false};
    std::string board_locked_by_;
    std::unordered_map<std::string, LockedRegion> regions_;

    /// Publish lock state change via EventBus.
    auto publish_lock_event(const std::string& event_name) -> void;
};

} // namespace markamp::canvas
