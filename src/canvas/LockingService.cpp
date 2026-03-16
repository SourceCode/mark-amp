#include "LockingService.h"

#include "canvas/GroupObject.h"

namespace markamp::canvas
{

namespace
{

/// Set lock state on an object. If it's a group, also set on all children.
auto set_lock_recursive(Board& board, ObjectId obj_id, bool locked) -> void
{
    auto* obj_ptr = board.get_object_mut(obj_id);
    if (obj_ptr == nullptr)
    {
        return;
    }

    obj_ptr->set_locked(locked);

    if (obj_ptr->type() == CanvasObjectType::Group)
    {
        const auto& group = static_cast<const GroupObject&>(*obj_ptr);
        for (const auto child_id : group.children())
        {
            set_lock_recursive(board, child_id, locked);
        }
    }
}

} // anonymous namespace

auto LockingService::lock_objects(Board& board, const std::vector<ObjectId>& ids) -> void
{
    for (const auto obj_id : ids)
    {
        set_lock_recursive(board, obj_id, true);
    }
}

auto LockingService::unlock_objects(Board& board, const std::vector<ObjectId>& ids) -> void
{
    for (const auto obj_id : ids)
    {
        set_lock_recursive(board, obj_id, false);
    }
}

auto LockingService::toggle_lock(Board& board, ObjectId obj_id) -> void
{
    auto* obj_ptr = board.get_object_mut(obj_id);
    if (obj_ptr == nullptr)
    {
        return;
    }

    const bool new_state = !obj_ptr->is_locked();
    set_lock_recursive(board, obj_id, new_state);
}

auto LockingService::is_any_locked(const Board& board, const std::vector<ObjectId>& ids) -> bool
{
    for (const auto obj_id : ids)
    {
        const auto* obj_ptr = board.get_object(obj_id);
        if (obj_ptr != nullptr && obj_ptr->is_locked())
        {
            return true;
        }
    }
    return false;
}

auto LockingService::locked_ids(const Board& board) -> std::vector<ObjectId>
{
    std::vector<ObjectId> result;

    for (const auto& obj_ptr : board.objects())
    {
        if (obj_ptr && obj_ptr->is_locked())
        {
            result.push_back(obj_ptr->id());
        }
    }

    return result;
}

// (#84) Return count of locked objects on the board.
auto LockingService::locked_count(const Board& board) -> std::size_t
{
    std::size_t count = 0;
    for (const auto& obj_ptr : board.objects())
    {
        if (obj_ptr && obj_ptr->is_locked())
        {
            ++count;
        }
    }
    return count;
}

} // namespace markamp::canvas
