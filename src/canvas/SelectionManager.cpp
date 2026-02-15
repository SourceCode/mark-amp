#include "SelectionManager.h"

#include "core/Events.h"

#include <algorithm>

namespace markamp::canvas
{

SelectionManager::SelectionManager(std::shared_ptr<core::EventBus> event_bus)
    : event_bus_(std::move(event_bus))
{
}

// ── Selection State ────────────────────────────────────────────────

auto SelectionManager::select(ObjectId obj_id) -> void
{
    selection_.clear();
    selection_.insert(obj_id);
    publish_selection_changed();
}

auto SelectionManager::add_to_selection(ObjectId obj_id) -> void
{
    selection_.insert(obj_id);
    publish_selection_changed();
}

auto SelectionManager::remove_from_selection(ObjectId obj_id) -> void
{
    selection_.erase(obj_id);
    publish_selection_changed();
}

auto SelectionManager::toggle_selection(ObjectId obj_id) -> void
{
    if (selection_.contains(obj_id))
    {
        selection_.erase(obj_id);
    }
    else
    {
        selection_.insert(obj_id);
    }
    publish_selection_changed();
}

auto SelectionManager::clear_selection() -> void
{
    if (!selection_.empty())
    {
        selection_.clear();
        publish_selection_changed();
    }
}

auto SelectionManager::select_all(const std::vector<ObjectId>& ids) -> void
{
    selection_.clear();
    for (const auto obj_id : ids)
    {
        selection_.insert(obj_id);
    }
    publish_selection_changed();
}

auto SelectionManager::is_selected(ObjectId obj_id) const -> bool
{
    return selection_.contains(obj_id);
}

auto SelectionManager::selected_ids() const -> std::vector<ObjectId>
{
    return {selection_.begin(), selection_.end()};
}

auto SelectionManager::selection_count() const -> size_t
{
    return selection_.size();
}

// ── Selection Bounds ───────────────────────────────────────────────

auto SelectionManager::selection_bounds(
    const std::vector<std::unique_ptr<CanvasObject>>& objects) const -> std::optional<AABB>
{
    if (selection_.empty())
    {
        return std::nullopt;
    }

    bool found = false;
    AABB combined{0.0, 0.0, 0.0, 0.0};

    for (const auto& obj : objects)
    {
        if (!selection_.contains(obj->id()))
        {
            continue;
        }
        if (!found)
        {
            combined = obj->world_bounds();
            found = true;
        }
        else
        {
            combined = combined.merged(obj->world_bounds());
        }
    }

    return found ? std::optional<AABB>(combined) : std::nullopt;
}

// ── Move Transform ─────────────────────────────────────────────────

auto SelectionManager::begin_move(const std::vector<std::unique_ptr<CanvasObject>>& objects) -> void
{
    save_snapshots(objects);
    is_moving_ = true;
}

auto SelectionManager::update_move(double delta_x,
                                   double delta_y,
                                   std::vector<std::unique_ptr<CanvasObject>>& objects) -> void
{
    for (auto& obj : objects)
    {
        if (!selection_.contains(obj->id()))
        {
            continue;
        }

        // Find original transform from snapshot.
        for (const auto& snap : snapshots_)
        {
            if (snap.id == obj->id())
            {
                Transform2D new_transform = snap.transform;
                new_transform.tx += delta_x;
                new_transform.ty += delta_y;
                obj->set_transform(new_transform);
                break;
            }
        }
    }
}

auto SelectionManager::end_move() -> void
{
    is_moving_ = false;
    snapshots_.clear();

    if (event_bus_)
    {
        core::events::CanvasObjectMovedEvent evt;
        evt.object_ids = selected_ids();
        event_bus_->publish(evt);
    }
}

auto SelectionManager::cancel_move(std::vector<std::unique_ptr<CanvasObject>>& objects) -> void
{
    restore_snapshots(objects);
    is_moving_ = false;
    snapshots_.clear();
}

// ── Resize Transform ───────────────────────────────────────────────

auto SelectionManager::begin_resize(const std::vector<std::unique_ptr<CanvasObject>>& objects,
                                    HandleType handle) -> void
{
    save_snapshots(objects);
    active_handle_ = handle;
    is_resizing_ = true;
}

auto SelectionManager::update_resize(double delta_x,
                                     double delta_y,
                                     std::vector<std::unique_ptr<CanvasObject>>& objects) -> void
{
    for (auto& obj : objects)
    {
        if (!selection_.contains(obj->id()))
        {
            continue;
        }

        for (const auto& snap : snapshots_)
        {
            if (snap.id == obj->id())
            {
                Transform2D new_transform = snap.transform;
                // Simple scale adjustment based on handle.
                const double scale_factor = std::max(0.1, snap.transform.scale_x + delta_x * 0.01);
                new_transform.scale_x = scale_factor;
                new_transform.scale_y = scale_factor;
                (void)delta_y; // Reserved for proportional vs free resize.
                obj->set_transform(new_transform);
                break;
            }
        }
    }
}

auto SelectionManager::end_resize() -> void
{
    is_resizing_ = false;
    active_handle_ = HandleType::None;
    snapshots_.clear();

    if (event_bus_)
    {
        core::events::CanvasObjectResizedEvent evt;
        evt.object_ids = selected_ids();
        event_bus_->publish(evt);
    }
}

auto SelectionManager::cancel_resize(std::vector<std::unique_ptr<CanvasObject>>& objects) -> void
{
    restore_snapshots(objects);
    is_resizing_ = false;
    active_handle_ = HandleType::None;
    snapshots_.clear();
}

// ── Rotate Transform ───────────────────────────────────────────────

auto SelectionManager::begin_rotate(const std::vector<std::unique_ptr<CanvasObject>>& objects)
    -> void
{
    save_snapshots(objects);
    is_rotating_ = true;
}

auto SelectionManager::update_rotate(double delta_radians,
                                     std::vector<std::unique_ptr<CanvasObject>>& objects) -> void
{
    for (auto& obj : objects)
    {
        if (!selection_.contains(obj->id()))
        {
            continue;
        }

        for (const auto& snap : snapshots_)
        {
            if (snap.id == obj->id())
            {
                Transform2D new_transform = snap.transform;
                new_transform.rotation += delta_radians;
                obj->set_transform(new_transform);
                break;
            }
        }
    }
}

auto SelectionManager::end_rotate() -> void
{
    is_rotating_ = false;
    snapshots_.clear();

    if (event_bus_)
    {
        core::events::CanvasObjectRotatedEvent evt;
        evt.object_ids = selected_ids();
        event_bus_->publish(evt);
    }
}

auto SelectionManager::cancel_rotate(std::vector<std::unique_ptr<CanvasObject>>& objects) -> void
{
    restore_snapshots(objects);
    is_rotating_ = false;
    snapshots_.clear();
}

// ── State Queries ──────────────────────────────────────────────────

auto SelectionManager::is_moving() const -> bool
{
    return is_moving_;
}
auto SelectionManager::is_resizing() const -> bool
{
    return is_resizing_;
}
auto SelectionManager::is_rotating() const -> bool
{
    return is_rotating_;
}
auto SelectionManager::active_handle() const -> HandleType
{
    return active_handle_;
}

// ── Private Helpers ────────────────────────────────────────────────

auto SelectionManager::save_snapshots(const std::vector<std::unique_ptr<CanvasObject>>& objects)
    -> void
{
    snapshots_.clear();
    for (const auto& obj : objects)
    {
        if (selection_.contains(obj->id()))
        {
            snapshots_.push_back({obj->id(), obj->transform()});
        }
    }
}

auto SelectionManager::restore_snapshots(std::vector<std::unique_ptr<CanvasObject>>& objects)
    -> void
{
    for (auto& obj : objects)
    {
        for (const auto& snap : snapshots_)
        {
            if (snap.id == obj->id())
            {
                obj->set_transform(snap.transform);
                break;
            }
        }
    }
}

auto SelectionManager::publish_selection_changed() -> void
{
    if (event_bus_)
    {
        core::events::CanvasSelectionChangedEvent evt;
        evt.count = static_cast<int>(selection_.size());
        event_bus_->publish(evt);
    }
}

} // namespace markamp::canvas
