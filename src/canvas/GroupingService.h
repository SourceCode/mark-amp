#pragma once

#include "canvas/Board.h"
#include "canvas/CanvasTypes.h"

#include <vector>

namespace markamp::canvas
{

/// Static utility class for group operations on a Board.
class GroupingService
{
public:
    GroupingService() = delete;

    /// Create a GroupObject containing the given objects. Sets parent_id_ on each child.
    /// Returns the new group's ObjectId.
    static auto group_objects(Board& board, const std::vector<ObjectId>& object_ids) -> ObjectId;

    /// Remove a GroupObject and clear children's parent_id_. Returns child IDs.
    static auto ungroup(Board& board, ObjectId group_id) -> std::vector<ObjectId>;

    /// Walk the parent chain to find the top-level group containing the given object.
    /// Returns kInvalidObjectId if the object has no parent.
    [[nodiscard]] static auto get_top_level_group(const Board& board, ObjectId obj_id) -> ObjectId;

    /// Recursively ungroup all nested groups under the given group.
    static auto flatten_group(Board& board, ObjectId group_id) -> void;
};

} // namespace markamp::canvas
