#include "Board.h"

#include <algorithm>

namespace markamp::canvas
{

Board::Board()
{
    metadata_.created_at = std::chrono::system_clock::now();
    metadata_.modified_at = metadata_.created_at;
}

Board::Board(const std::string& name)
    : Board()
{
    metadata_.name = name;
}

auto Board::metadata() const -> const BoardMetadata&
{
    return metadata_;
}

auto Board::metadata_mut() -> BoardMetadata&
{
    dirty_ = true;
    return metadata_;
}

// ── Object Management ──────────────────────────────────────────

auto Board::add_object(std::unique_ptr<CanvasObject> obj) -> ObjectId
{
    const auto obj_id = obj->id();
    id_to_index_[obj_id] = objects_.size();
    objects_.push_back(std::move(obj));
    dirty_ = true;
    return obj_id;
}

auto Board::remove_object(ObjectId obj_id) -> std::unique_ptr<CanvasObject>
{
    const auto idx_iter = id_to_index_.find(obj_id);
    if (idx_iter == id_to_index_.end())
    {
        return nullptr;
    }

    const auto index = idx_iter->second;
    auto removed = std::move(objects_[index]);

    // Swap-and-pop for O(1) removal.
    if (index < objects_.size() - 1)
    {
        objects_[index] = std::move(objects_.back());
        id_to_index_[objects_[index]->id()] = index;
    }
    objects_.pop_back();
    id_to_index_.erase(obj_id);
    dirty_ = true;

    return removed;
}

auto Board::get_object(ObjectId obj_id) const -> const CanvasObject*
{
    const auto idx_iter = id_to_index_.find(obj_id);
    if (idx_iter == id_to_index_.end())
    {
        return nullptr;
    }
    return objects_[idx_iter->second].get();
}

auto Board::get_object_mut(ObjectId obj_id) -> CanvasObject*
{
    const auto idx_iter = id_to_index_.find(obj_id);
    if (idx_iter == id_to_index_.end())
    {
        return nullptr;
    }
    dirty_ = true;
    return objects_[idx_iter->second].get();
}

auto Board::object_count() const -> size_t
{
    return objects_.size();
}

auto Board::all_object_ids() const -> std::vector<ObjectId>
{
    std::vector<ObjectId> ids;
    ids.reserve(objects_.size());
    for (const auto& obj : objects_)
    {
        ids.push_back(obj->id());
    }
    return ids;
}

auto Board::objects() const -> const std::vector<std::unique_ptr<CanvasObject>>&
{
    return objects_;
}

auto Board::objects_mut() -> std::vector<std::unique_ptr<CanvasObject>>&
{
    dirty_ = true;
    return objects_;
}

// ── Z-Ordering ─────────────────────────────────────────────────

auto Board::bring_to_front(ObjectId obj_id) -> void
{
    auto* obj = get_object_mut(obj_id);
    if (obj == nullptr)
    {
        return;
    }

    int max_z = 0;
    for (const auto& other : objects_)
    {
        if (other->z_index() > max_z)
        {
            max_z = other->z_index();
        }
    }
    obj->set_z_index(max_z + 1);
    dirty_ = true;
}

auto Board::send_to_back(ObjectId obj_id) -> void
{
    auto* obj = get_object_mut(obj_id);
    if (obj == nullptr)
    {
        return;
    }

    int min_z = 0;
    for (const auto& other : objects_)
    {
        if (other->z_index() < min_z)
        {
            min_z = other->z_index();
        }
    }
    obj->set_z_index(min_z - 1);
    dirty_ = true;
}

auto Board::bring_forward(ObjectId obj_id) -> void
{
    auto* obj = get_object_mut(obj_id);
    if (obj == nullptr)
    {
        return;
    }
    obj->set_z_index(obj->z_index() + 1);
    dirty_ = true;
}

auto Board::send_backward(ObjectId obj_id) -> void
{
    auto* obj = get_object_mut(obj_id);
    if (obj == nullptr)
    {
        return;
    }
    obj->set_z_index(obj->z_index() - 1);
    dirty_ = true;
}

auto Board::z_ordered_ids() const -> std::vector<ObjectId>
{
    auto ids = all_object_ids();
    std::sort(ids.begin(),
              ids.end(),
              [this](ObjectId lhs_id, ObjectId rhs_id)
              {
                  const auto* lhs_obj = get_object(lhs_id);
                  const auto* rhs_obj = get_object(rhs_id);
                  const int lhs_z = (lhs_obj != nullptr) ? lhs_obj->z_index() : 0;
                  const int rhs_z = (rhs_obj != nullptr) ? rhs_obj->z_index() : 0;
                  return lhs_z < rhs_z;
              });
    return ids;
}

// ── Content Bounds ─────────────────────────────────────────────

auto Board::content_bounds() const -> AABB
{
    if (objects_.empty())
    {
        return AABB{0.0, 0.0, 0.0, 0.0};
    }

    AABB bounds = objects_[0]->world_bounds();
    for (size_t idx = 1; idx < objects_.size(); ++idx)
    {
        bounds = bounds.merged(objects_[idx]->world_bounds());
    }
    return bounds;
}

// ── Dirty Tracking ─────────────────────────────────────────────

auto Board::is_dirty() const -> bool
{
    return dirty_;
}
auto Board::mark_dirty() -> void
{
    dirty_ = true;
}
auto Board::clear_dirty() -> void
{
    dirty_ = false;
}

// ── Clone ──────────────────────────────────────────────────────

auto Board::deep_clone() const -> Board
{
    Board cloned;
    cloned.metadata_ = metadata_;

    for (const auto& obj : objects_)
    {
        auto cloned_obj = obj->clone();
        cloned.add_object(std::move(cloned_obj));
    }

    cloned.dirty_ = dirty_;
    return cloned;
}

auto Board::rebuild_index() -> void
{
    id_to_index_.clear();
    for (size_t idx = 0; idx < objects_.size(); ++idx)
    {
        id_to_index_[objects_[idx]->id()] = idx;
    }
}

} // namespace markamp::canvas
