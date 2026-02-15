#include "GroupingService.h"

#include "canvas/GroupObject.h"

#include <memory>

namespace markamp::canvas
{

auto GroupingService::group_objects(Board& board, const std::vector<ObjectId>& object_ids)
    -> ObjectId
{
    if (object_ids.empty())
    {
        return kInvalidObjectId;
    }

    auto group = std::make_unique<GroupObject>();

    for (const auto obj_id : object_ids)
    {
        auto* obj_ptr = board.get_object_mut(obj_id);
        if (obj_ptr != nullptr)
        {
            group->add_child(obj_id);
            obj_ptr->set_parent_id(group->id());
        }
    }

    group->update_bounds(board);

    const auto group_id = group->id();
    board.add_object(std::move(group));
    return group_id;
}

auto GroupingService::ungroup(Board& board, ObjectId group_id) -> std::vector<ObjectId>
{
    const auto* group_obj = board.get_object(group_id);
    if (group_obj == nullptr || group_obj->type() != CanvasObjectType::Group)
    {
        return {};
    }

    // Safe cast: we verified the type above.
    const auto& group = static_cast<const GroupObject&>(*group_obj);
    auto child_ids = group.children();

    // Clear children's parent_id.
    for (const auto child_id : child_ids)
    {
        auto* child_ptr = board.get_object_mut(child_id);
        if (child_ptr != nullptr)
        {
            child_ptr->set_parent_id(kInvalidObjectId);
        }
    }

    // Remove the group object itself.
    board.remove_object(group_id);

    return child_ids;
}

auto GroupingService::get_top_level_group(const Board& board, ObjectId obj_id) -> ObjectId
{
    ObjectId current = obj_id;
    ObjectId top_group = kInvalidObjectId;

    // Walk up the parent chain.
    constexpr int kMaxDepth = 64; // Guard against cycles.
    for (int depth = 0; depth < kMaxDepth; ++depth)
    {
        const auto* obj_ptr = board.get_object(current);
        if (obj_ptr == nullptr)
        {
            break;
        }

        const auto parent = obj_ptr->parent_id();
        if (parent == kInvalidObjectId)
        {
            break;
        }

        top_group = parent;
        current = parent;
    }

    return top_group;
}

auto GroupingService::flatten_group(Board& board, ObjectId group_id) -> void
{
    const auto* group_obj = board.get_object(group_id);
    if (group_obj == nullptr || group_obj->type() != CanvasObjectType::Group)
    {
        return;
    }

    const auto& group = static_cast<const GroupObject&>(*group_obj);
    auto child_ids = group.children();

    // Recursively flatten any nested groups first.
    for (const auto child_id : child_ids)
    {
        const auto* child_ptr = board.get_object(child_id);
        if (child_ptr != nullptr && child_ptr->type() == CanvasObjectType::Group)
        {
            flatten_group(board, child_id);
        }
    }

    // Now ungroup this group.
    ungroup(board, group_id);
}

} // namespace markamp::canvas
