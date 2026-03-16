#include "LayeringService.h"

#include "canvas/GroupObject.h"

#include <algorithm>
#include <limits>

namespace markamp::canvas
{

namespace
{

/// Expand a set of IDs: if any ID is a Group, also add its children.
auto expand_groups(const Board& board, const std::vector<ObjectId>& ids) -> std::vector<ObjectId>
{
    std::vector<ObjectId> expanded;
    expanded.reserve(ids.size());

    for (const auto obj_id : ids)
    {
        expanded.push_back(obj_id);

        const auto* obj_ptr = board.get_object(obj_id);
        if (obj_ptr != nullptr && obj_ptr->type() == CanvasObjectType::Group)
        {
            const auto& group = static_cast<const GroupObject&>(*obj_ptr);
            for (const auto child_id : group.children())
            {
                expanded.push_back(child_id);
            }
        }
    }

    return expanded;
}

} // anonymous namespace

auto LayeringService::bring_to_front(Board& board, const std::vector<ObjectId>& ids) -> void
{
    const auto expanded = expand_groups(board, ids);
    for (const auto obj_id : expanded)
    {
        board.bring_to_front(obj_id);
    }
}

auto LayeringService::send_to_back(Board& board, const std::vector<ObjectId>& ids) -> void
{
    const auto expanded = expand_groups(board, ids);
    for (const auto obj_id : expanded)
    {
        board.send_to_back(obj_id);
    }
}

auto LayeringService::bring_forward(Board& board, const std::vector<ObjectId>& ids) -> void
{
    const auto expanded = expand_groups(board, ids);
    for (const auto obj_id : expanded)
    {
        board.bring_forward(obj_id);
    }
}

auto LayeringService::send_backward(Board& board, const std::vector<ObjectId>& ids) -> void
{
    const auto expanded = expand_groups(board, ids);
    for (const auto obj_id : expanded)
    {
        board.send_backward(obj_id);
    }
}

auto LayeringService::z_range(const Board& board) -> std::pair<int, int>
{
    if (board.object_count() == 0)
    {
        return {0, 0};
    }

    int min_z = std::numeric_limits<int>::max();
    int max_z = std::numeric_limits<int>::min();

    for (const auto& obj_ptr : board.objects())
    {
        if (obj_ptr)
        {
            min_z = std::min(min_z, obj_ptr->z_index());
            max_z = std::max(max_z, obj_ptr->z_index());
        }
    }

    return {min_z, max_z};
}

auto LayeringService::auto_distribute_z(Board& board) -> void
{
    // Collect all objects sorted by current z-index.
    auto& objs = board.objects_mut();

    std::vector<CanvasObject*> sorted;
    sorted.reserve(objs.size());
    for (auto& obj_ptr : objs)
    {
        if (obj_ptr)
        {
            sorted.push_back(obj_ptr.get());
        }
    }

    std::sort(sorted.begin(),
              sorted.end(),
              [](const CanvasObject* lhs, const CanvasObject* rhs)
              { return lhs->z_index() < rhs->z_index(); });

    // Reassign z-indices 0..N-1.
    for (size_t idx = 0; idx < sorted.size(); ++idx)
    {
        sorted[idx]->set_z_index(static_cast<int>(idx));
    }

    board.mark_dirty();
}

// (#79) Return object IDs sorted by their z-index (front to back).
auto LayeringService::get_z_order(const Board& board) -> std::vector<ObjectId>
{
    std::vector<std::pair<int, ObjectId>> z_list;
    z_list.reserve(board.object_count());
    for (const auto& obj_ptr : board.objects())
    {
        if (obj_ptr)
        {
            z_list.emplace_back(obj_ptr->z_index(), obj_ptr->id());
        }
    }
    std::sort(z_list.begin(), z_list.end(),
              [](const auto& lhs, const auto& rhs) { return lhs.first > rhs.first; });
    std::vector<ObjectId> result;
    result.reserve(z_list.size());
    for (const auto& [z_val, obj_id] : z_list)
    {
        result.push_back(obj_id);
    }
    return result;
}

} // namespace markamp::canvas
