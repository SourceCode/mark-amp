#include "Board.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <random>
#include <set>
#include <sstream>

namespace markamp::canvas
{

namespace
{

/// Generate a board-unique ID: "b-<hex_timestamp>-<hex_random>"
auto generate_board_id() -> std::string
{
    const auto now = std::chrono::system_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now.time_since_epoch())
                        .count();

    static thread_local std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<uint32_t> dist;
    const auto rand_val = dist(rng);

    std::ostringstream oss;
    oss << "b-" << std::hex << std::setfill('0') << std::setw(12)
        << static_cast<uint64_t>(ms) << "-" << std::setw(8) << rand_val;
    return oss.str();
}

} // namespace

Board::Board()
{
    metadata_.id = generate_board_id();
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
    ++metadata_.version;
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

// ── Query Helpers ────────────────────────────────────────────────

auto Board::find_objects_by_type(CanvasObjectType type) const -> std::vector<ObjectId>
{
    std::vector<ObjectId> result;
    for (const auto& obj : objects_)
    {
        if (obj->type() == type)
        {
            result.push_back(obj->id());
        }
    }
    return result;
}

auto Board::find_objects_in_region(const AABB& region) const -> std::vector<ObjectId>
{
    std::vector<ObjectId> result;
    for (const auto& obj : objects_)
    {
        if (obj->world_bounds().intersects(region))
        {
            result.push_back(obj->id());
        }
    }
    return result;
}

auto Board::object_count_by_type() const -> std::unordered_map<uint8_t, size_t>
{
    std::unordered_map<uint8_t, size_t> counts;
    for (const auto& obj : objects_)
    {
        ++counts[static_cast<uint8_t>(obj->type())];
    }
    return counts;
}

// ── Bulk Operations ──────────────────────────────────────────────

auto Board::clear_all_objects() -> void
{
    objects_.clear();
    id_to_index_.clear();
    dirty_ = true;
    ++metadata_.version;
}

auto Board::sort_objects_by_position() -> void
{
    std::sort(objects_.begin(),
              objects_.end(),
              [](const std::unique_ptr<CanvasObject>& lhs, const std::unique_ptr<CanvasObject>& rhs)
              {
                  const auto lhs_bounds = lhs->world_bounds();
                  const auto rhs_bounds = rhs->world_bounds();
                  if (lhs_bounds.min_y != rhs_bounds.min_y)
                  {
                      return lhs_bounds.min_y < rhs_bounds.min_y;
                  }
                  return lhs_bounds.min_x < rhs_bounds.min_x;
              });
    rebuild_index();
    dirty_ = true;
}

// ── Analytics ────────────────────────────────────────────────────

auto Board::statistics() const -> BoardStatistics
{
    BoardStatistics stats;
    stats.total_objects = objects_.size();
    stats.type_counts = object_count_by_type();
    stats.bounds = content_bounds();
    stats.favorite = metadata_.favorite;
    stats.version = metadata_.version;
    return stats;
}

// ── Object Helpers (#38-40) ────────────────────────────────────

auto Board::duplicate_object(ObjectId obj_id, double offset_x, double offset_y) -> ObjectId
{
    const auto* original = get_object(obj_id);
    if (original == nullptr)
    {
        return kInvalidObjectId;
    }

    auto cloned = original->clone();
    auto xform = cloned->transform();
    xform.tx += offset_x;
    xform.ty += offset_y;
    cloned->set_transform(xform);
    cloned->set_name(original->name() + " (copy)");

    return add_object(std::move(cloned));
}

auto Board::layer_count() const -> int
{
    std::set<int> layers;
    for (const auto& obj : objects_)
    {
        layers.insert(obj->layer());
    }
    return static_cast<int>(layers.size());
}

auto Board::objects_on_layer(int layer_index) const -> std::vector<ObjectId>
{
    std::vector<ObjectId> result;
    for (const auto& obj : objects_)
    {
        if (obj->layer() == layer_index)
        {
            result.push_back(obj->id());
        }
    }
    return result;
}

// ── Utility (#25-30) ───────────────────────────────────────────────

auto Board::rename_object(ObjectId obj_id, const std::string& new_name) -> bool
{
    auto* obj = get_object_mut(obj_id);
    if (obj == nullptr)
    {
        return false;
    }
    obj->set_name(new_name);
    return true;
}

auto Board::move_object(ObjectId obj_id, double delta_x, double delta_y) -> bool
{
    auto* obj = get_object_mut(obj_id);
    if (obj == nullptr)
    {
        return false;
    }
    auto xform = obj->transform();
    xform.tx += delta_x;
    xform.ty += delta_y;
    obj->set_transform(xform);
    return true;
}

auto Board::resize_object(ObjectId obj_id, double scale_x, double scale_y) -> bool
{
    auto* obj = get_object_mut(obj_id);
    if (obj == nullptr)
    {
        return false;
    }
    auto xform = obj->transform();
    xform.scale_x *= scale_x;
    xform.scale_y *= scale_y;
    obj->set_transform(xform);
    return true;
}

auto Board::find_objects_by_name(const std::string& substring) const -> std::vector<ObjectId>
{
    std::vector<ObjectId> result;
    for (const auto& obj : objects_)
    {
        if (obj->name().find(substring) != std::string::npos)
        {
            result.push_back(obj->id());
        }
    }
    return result;
}

auto Board::total_bounds_area() const -> double
{
    const auto bounds = content_bounds();
    const double width = bounds.max_x - bounds.min_x;
    const double height = bounds.max_y - bounds.min_y;
    return width * height;
}

auto Board::has_object(ObjectId obj_id) const -> bool
{
    return get_object(obj_id) != nullptr;
}

// ── Batch 1 (#1-6) ────────────────────────────────────────────────

auto Board::find_objects_by_tag(const std::string& tag) const -> std::vector<ObjectId>
{
    std::vector<ObjectId> result;
    for (const auto& obj : objects_)
    {
        const auto& obj_tags = obj->tags();
        if (std::find(obj_tags.begin(), obj_tags.end(), tag) != obj_tags.end())
        {
            result.push_back(obj->id());
        }
    }
    return result;
}

auto Board::find_topmost_at(const Point2D& point) const -> ObjectId
{
    ObjectId topmost = kInvalidObjectId;
    int highest_z = std::numeric_limits<int>::min();

    for (const auto& obj : objects_)
    {
        if (obj->world_bounds().contains(point) && obj->z_index() > highest_z)
        {
            highest_z = obj->z_index();
            topmost = obj->id();
        }
    }
    return topmost;
}

auto Board::compact_z_indices() -> void
{
    // Gather objects sorted by current z-index.
    std::vector<CanvasObject*> sorted;
    sorted.reserve(objects_.size());
    for (const auto& obj : objects_)
    {
        sorted.push_back(obj.get());
    }
    std::sort(sorted.begin(),
              sorted.end(),
              [](const CanvasObject* lhs, const CanvasObject* rhs)
              { return lhs->z_index() < rhs->z_index(); });

    for (size_t idx = 0; idx < sorted.size(); ++idx)
    {
        sorted[idx]->set_z_index(static_cast<int>(idx));
    }
    dirty_ = true;
}

auto Board::swap_z_order(ObjectId first_id, ObjectId second_id) -> bool
{
    auto* first_obj = get_object_mut(first_id);
    auto* second_obj = get_object_mut(second_id);
    if (first_obj == nullptr || second_obj == nullptr)
    {
        return false;
    }
    const int temp_z = first_obj->z_index();
    first_obj->set_z_index(second_obj->z_index());
    second_obj->set_z_index(temp_z);
    dirty_ = true;
    return true;
}

auto Board::lock_all() -> void
{
    for (auto& obj : objects_)
    {
        obj->set_locked(true);
    }
    dirty_ = true;
}

auto Board::unlock_all() -> void
{
    for (auto& obj : objects_)
    {
        obj->set_locked(false);
    }
    dirty_ = true;
}

} // namespace markamp::canvas
