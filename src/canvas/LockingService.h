#pragma once

#include "canvas/Board.h"
#include "canvas/CanvasTypes.h"

#include <vector>

namespace markamp::canvas
{

/// Static utility class for batch lock/unlock operations, group-aware.
class LockingService
{
public:
    LockingService() = delete;

    /// Lock all given objects. If a GroupObject is included, locks all children too.
    static auto lock_objects(Board& board, const std::vector<ObjectId>& ids) -> void;

    /// Unlock all given objects. If a GroupObject is included, unlocks all children too.
    static auto unlock_objects(Board& board, const std::vector<ObjectId>& ids) -> void;

    /// Toggle lock state of a single object.
    static auto toggle_lock(Board& board, ObjectId obj_id) -> void;

    /// Check if any of the given objects are locked.
    [[nodiscard]] static auto is_any_locked(const Board& board, const std::vector<ObjectId>& ids)
        -> bool;

    /// Return all locked object IDs on the board.
    [[nodiscard]] static auto locked_ids(const Board& board) -> std::vector<ObjectId>;

    /// (#84) Return count of locked objects.
    [[nodiscard]] static auto locked_count(const Board& board) -> std::size_t;
};

} // namespace markamp::canvas
