#include "GroupObject.h"

#include "canvas/Board.h"

#include <algorithm>
#include <sstream>

namespace markamp::canvas
{

GroupObject::GroupObject()
    : CanvasObject(CanvasObjectType::Group)
{
}

// ── Children ──────────────────────────────────────────────────

auto GroupObject::add_child(ObjectId child_id) -> void
{
    if (child_id == kInvalidObjectId)
    {
        return;
    }

    // Prevent duplicates.
    if (std::find(children_ids_.begin(), children_ids_.end(), child_id) != children_ids_.end())
    {
        return;
    }

    children_ids_.push_back(child_id);
    mark_dirty();
}

auto GroupObject::remove_child(ObjectId child_id) -> void
{
    auto iter = std::find(children_ids_.begin(), children_ids_.end(), child_id);
    if (iter != children_ids_.end())
    {
        children_ids_.erase(iter);
        mark_dirty();
    }
}

auto GroupObject::children() const -> const std::vector<ObjectId>&
{
    return children_ids_;
}

auto GroupObject::child_count() const -> size_t
{
    return children_ids_.size();
}

auto GroupObject::contains_child(ObjectId child_id) const -> bool
{
    return std::find(children_ids_.begin(), children_ids_.end(), child_id) != children_ids_.end();
}

// ── Bounds ────────────────────────────────────────────────────

auto GroupObject::update_bounds(const Board& board) -> void
{
    if (children_ids_.empty())
    {
        cached_bounds_ = AABB{0.0, 0.0, 0.0, 0.0};
        return;
    }

    bool first = true;
    AABB merged{0.0, 0.0, 0.0, 0.0};

    for (const auto child_id : children_ids_)
    {
        const auto* child_obj = board.get_object(child_id);
        if (child_obj == nullptr)
        {
            continue;
        }

        const auto child_bounds = child_obj->world_bounds();
        if (first)
        {
            merged = child_bounds;
            first = false;
        }
        else
        {
            merged = merged.merged(child_bounds);
        }
    }

    cached_bounds_ = merged;
}

// ── CanvasObject overrides ────────────────────────────────────

auto GroupObject::local_bounds() const -> AABB
{
    return cached_bounds_;
}

auto GroupObject::clone() const -> std::unique_ptr<CanvasObject>
{
    auto copy = std::make_unique<GroupObject>();
    copy->set_name(name());
    copy->set_transform(transform());
    copy->set_z_index(z_index());
    copy->set_locked(is_locked());
    copy->set_visible(is_visible());
    copy->set_opacity(opacity());

    for (const auto child_id : children_ids_)
    {
        copy->add_child(child_id);
    }
    copy->cached_bounds_ = cached_bounds_;

    return copy;
}

auto GroupObject::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"type\":\"Group\"";
    oss << ",\"id\":" << id();
    oss << ",\"name\":\"" << name() << "\"";
    oss << ",\"z_index\":" << z_index();
    oss << ",\"locked\":" << (is_locked() ? "true" : "false");
    oss << ",\"visible\":" << (is_visible() ? "true" : "false");
    oss << ",\"opacity\":" << opacity();

    oss << ",\"children\":[";
    for (size_t idx = 0; idx < children_ids_.size(); ++idx)
    {
        if (idx > 0)
        {
            oss << ",";
        }
        oss << children_ids_[idx];
    }
    oss << "]";

    oss << "}";
    return oss.str();
}

auto GroupObject::from_json(const std::string& /*json*/) -> void
{
    // Deserialization stub — would parse children array.
}

} // namespace markamp::canvas
