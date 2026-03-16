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

// ── Alignment (#9–14) ──────────────────────────────────────────────

auto SelectionManager::align_left(std::vector<std::unique_ptr<CanvasObject>>& objects) -> void
{
    if (selection_.size() < 2)
    {
        return;
    }
    double min_left = std::numeric_limits<double>::max();
    for (const auto& obj : objects)
    {
        if (selection_.contains(obj->id()))
        {
            min_left = std::min(min_left, obj->world_bounds().min_x);
        }
    }
    for (auto& obj : objects)
    {
        if (selection_.contains(obj->id()))
        {
            const double delta = min_left - obj->world_bounds().min_x;
            obj->set_position(obj->position().x + delta, obj->position().y);
        }
    }
}

auto SelectionManager::align_center_h(std::vector<std::unique_ptr<CanvasObject>>& objects) -> void
{
    if (selection_.size() < 2)
    {
        return;
    }
    const auto bounds = selection_bounds(objects);
    if (!bounds)
    {
        return;
    }
    const double center_x = bounds->center().x;
    for (auto& obj : objects)
    {
        if (selection_.contains(obj->id()))
        {
            const auto wb = obj->world_bounds();
            const double obj_cx = (wb.min_x + wb.max_x) / 2.0;
            obj->set_position(obj->position().x + (center_x - obj_cx), obj->position().y);
        }
    }
}

auto SelectionManager::align_right(std::vector<std::unique_ptr<CanvasObject>>& objects) -> void
{
    if (selection_.size() < 2)
    {
        return;
    }
    double max_right = std::numeric_limits<double>::lowest();
    for (const auto& obj : objects)
    {
        if (selection_.contains(obj->id()))
        {
            max_right = std::max(max_right, obj->world_bounds().max_x);
        }
    }
    for (auto& obj : objects)
    {
        if (selection_.contains(obj->id()))
        {
            const double delta = max_right - obj->world_bounds().max_x;
            obj->set_position(obj->position().x + delta, obj->position().y);
        }
    }
}

auto SelectionManager::align_top(std::vector<std::unique_ptr<CanvasObject>>& objects) -> void
{
    if (selection_.size() < 2)
    {
        return;
    }
    double min_top = std::numeric_limits<double>::max();
    for (const auto& obj : objects)
    {
        if (selection_.contains(obj->id()))
        {
            min_top = std::min(min_top, obj->world_bounds().min_y);
        }
    }
    for (auto& obj : objects)
    {
        if (selection_.contains(obj->id()))
        {
            const double delta = min_top - obj->world_bounds().min_y;
            obj->set_position(obj->position().x, obj->position().y + delta);
        }
    }
}

auto SelectionManager::align_center_v(std::vector<std::unique_ptr<CanvasObject>>& objects) -> void
{
    if (selection_.size() < 2)
    {
        return;
    }
    const auto bounds = selection_bounds(objects);
    if (!bounds)
    {
        return;
    }
    const double center_y = bounds->center().y;
    for (auto& obj : objects)
    {
        if (selection_.contains(obj->id()))
        {
            const auto wb = obj->world_bounds();
            const double obj_cy = (wb.min_y + wb.max_y) / 2.0;
            obj->set_position(obj->position().x, obj->position().y + (center_y - obj_cy));
        }
    }
}

auto SelectionManager::align_bottom(std::vector<std::unique_ptr<CanvasObject>>& objects) -> void
{
    if (selection_.size() < 2)
    {
        return;
    }
    double max_bottom = std::numeric_limits<double>::lowest();
    for (const auto& obj : objects)
    {
        if (selection_.contains(obj->id()))
        {
            max_bottom = std::max(max_bottom, obj->world_bounds().max_y);
        }
    }
    for (auto& obj : objects)
    {
        if (selection_.contains(obj->id()))
        {
            const double delta = max_bottom - obj->world_bounds().max_y;
            obj->set_position(obj->position().x, obj->position().y + delta);
        }
    }
}

// ── Distribution (#15–16) ──────────────────────────────────────────

auto SelectionManager::distribute_horizontal(std::vector<std::unique_ptr<CanvasObject>>& objects)
    -> void
{
    if (selection_.size() < 3)
    {
        return;
    }

    // Gather selected objects sorted by left edge.
    std::vector<CanvasObject*> selected;
    for (auto& obj : objects)
    {
        if (selection_.contains(obj->id()))
        {
            selected.push_back(obj.get());
        }
    }
    std::sort(selected.begin(),
              selected.end(),
              [](const CanvasObject* lhs, const CanvasObject* rhs)
              { return lhs->world_bounds().min_x < rhs->world_bounds().min_x; });

    const double total_left = selected.front()->world_bounds().min_x;
    const double total_right = selected.back()->world_bounds().max_x;
    double total_obj_width = 0.0;
    for (const auto* sel_obj : selected)
    {
        total_obj_width += sel_obj->world_bounds().width();
    }
    const double gap =
        (total_right - total_left - total_obj_width) / static_cast<double>(selected.size() - 1);
    double cursor = total_left;
    for (auto* sel_obj : selected)
    {
        const double delta = cursor - sel_obj->world_bounds().min_x;
        sel_obj->set_position(sel_obj->position().x + delta, sel_obj->position().y);
        cursor += sel_obj->world_bounds().width() + gap;
    }
}

auto SelectionManager::distribute_vertical(std::vector<std::unique_ptr<CanvasObject>>& objects)
    -> void
{
    if (selection_.size() < 3)
    {
        return;
    }

    std::vector<CanvasObject*> selected;
    for (auto& obj : objects)
    {
        if (selection_.contains(obj->id()))
        {
            selected.push_back(obj.get());
        }
    }
    std::sort(selected.begin(),
              selected.end(),
              [](const CanvasObject* lhs, const CanvasObject* rhs)
              { return lhs->world_bounds().min_y < rhs->world_bounds().min_y; });

    const double total_top = selected.front()->world_bounds().min_y;
    const double total_bottom = selected.back()->world_bounds().max_y;
    double total_obj_height = 0.0;
    for (const auto* sel_obj : selected)
    {
        total_obj_height += sel_obj->world_bounds().height();
    }
    const double gap =
        (total_bottom - total_top - total_obj_height) / static_cast<double>(selected.size() - 1);
    double cursor = total_top;
    for (auto* sel_obj : selected)
    {
        const double delta = cursor - sel_obj->world_bounds().min_y;
        sel_obj->set_position(sel_obj->position().x, sel_obj->position().y + delta);
        cursor += sel_obj->world_bounds().height() + gap;
    }
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

// ── Batch 10 (#59-60) ─────────────────────────────────────────────

auto SelectionManager::select_by_type(CanvasObjectType type,
                                      const std::vector<std::unique_ptr<CanvasObject>>& objects)
    -> void
{
    selection_.clear();
    for (const auto& obj : objects)
    {
        if (obj && obj->type() == type)
        {
            selection_.insert(obj->id());
        }
    }
    publish_selection_changed();
}

auto SelectionManager::invert_selection(const std::vector<std::unique_ptr<CanvasObject>>& objects)
    -> void
{
    std::unordered_set<ObjectId> inverted;
    for (const auto& obj : objects)
    {
        if (obj && selection_.count(obj->id()) == 0)
        {
            inverted.insert(obj->id());
        }
    }
    selection_ = std::move(inverted);
    publish_selection_changed();
}

// ── Batch 4 (#19-22) ─────────────────────────────────────────────

auto SelectionManager::select_connected(const std::vector<std::unique_ptr<CanvasObject>>& objects)
    -> void
{
    std::unordered_set<ObjectId> connected = selection_;
    for (const auto& obj : objects)
    {
        if (obj && selection_.contains(obj->parent_id()))
        {
            connected.insert(obj->id());
        }
        if (obj && selection_.contains(obj->id()))
        {
            // Also select siblings (objects with the same parent).
            for (const auto& sibling : objects)
            {
                if (sibling && sibling->parent_id() == obj->parent_id() &&
                    obj->parent_id() != kInvalidObjectId)
                {
                    connected.insert(sibling->id());
                }
            }
        }
    }
    selection_ = std::move(connected);
    publish_selection_changed();
}

auto SelectionManager::select_in_region(const AABB& region,
                                        const std::vector<std::unique_ptr<CanvasObject>>& objects)
    -> void
{
    selection_.clear();
    for (const auto& obj : objects)
    {
        if (obj && obj->world_bounds().intersects(region))
        {
            selection_.insert(obj->id());
        }
    }
    publish_selection_changed();
}

auto SelectionManager::grow_selection(const std::vector<std::unique_ptr<CanvasObject>>& objects)
    -> void
{
    if (selection_.empty())
    {
        return;
    }

    // Compute combined bounds of current selection.
    AABB combined{0.0, 0.0, 0.0, 0.0};
    bool first = true;
    for (const auto& obj : objects)
    {
        if (obj && selection_.contains(obj->id()))
        {
            if (first)
            {
                combined = obj->world_bounds();
                first = false;
            }
            else
            {
                combined = combined.merged(obj->world_bounds());
            }
        }
    }
    if (first)
    {
        return;
    }

    // Select any object that overlaps the combined bounds.
    for (const auto& obj : objects)
    {
        if (obj && obj->world_bounds().intersects(combined))
        {
            selection_.insert(obj->id());
        }
    }
    publish_selection_changed();
}

auto SelectionManager::selected_types(const std::vector<std::unique_ptr<CanvasObject>>& objects)
    const -> std::vector<CanvasObjectType>
{
    std::unordered_set<uint8_t> type_set;
    for (const auto& obj : objects)
    {
        if (obj && selection_.contains(obj->id()))
        {
            type_set.insert(static_cast<uint8_t>(obj->type()));
        }
    }
    std::vector<CanvasObjectType> result;
    result.reserve(type_set.size());
    for (const auto type_val : type_set)
    {
        result.push_back(static_cast<CanvasObjectType>(type_val));
    }
    return result;
}

auto SelectionManager::build_multi_select_menu(
    const std::vector<std::unique_ptr<CanvasObject>>& objects) const
    -> std::vector<MultiSelectAction>
{
    std::vector<MultiSelectAction> actions;
    const auto count = selection_count();

    if (count < 2)
    {
        return actions; // Multi-select menu only for 2+ objects.
    }

    // ── Alignment (requires 2+) ──────────────────────────────────
    actions.push_back({.label = "Align Left", .icon_name = "align-left", .enabled = true, .action_id = 1});
    actions.push_back({.label = "Align Center", .icon_name = "align-center-horizontal", .enabled = true, .action_id = 2});
    actions.push_back({.label = "Align Right", .icon_name = "align-right", .enabled = true, .action_id = 3});
    actions.push_back({.label = "Align Top", .icon_name = "align-top", .enabled = true, .action_id = 4});
    actions.push_back({.label = "Align Middle", .icon_name = "align-center-vertical", .enabled = true, .action_id = 5});
    actions.push_back({.label = "Align Bottom", .icon_name = "align-bottom", .enabled = true, .action_id = 6});

    // ── Distribution (requires 3+) ───────────────────────────────
    const bool can_distribute = (count >= 3);
    actions.push_back({.label = "Distribute Horizontal", .icon_name = "distribute-horizontal", .enabled = can_distribute, .action_id = 7});
    actions.push_back({.label = "Distribute Vertical", .icon_name = "distribute-vertical", .enabled = can_distribute, .action_id = 8});

    // ── Separator placeholder ─────────────────────────────────────
    actions.push_back({.label = "---", .icon_name = "", .enabled = false, .action_id = 0});

    // ── Grouping ──────────────────────────────────────────────────
    actions.push_back({.label = "Group", .icon_name = "group", .enabled = true, .action_id = 10});

    // ── Lock/Unlock ───────────────────────────────────────────────
    // Check if any selected objects are unlocked.
    bool has_unlocked = false;
    bool has_locked = false;
    for (const auto& obj : objects)
    {
        if (obj && selection_.contains(obj->id()))
        {
            if (obj->is_locked())
            {
                has_locked = true;
            }
            else
            {
                has_unlocked = true;
            }
        }
    }
    if (has_unlocked)
    {
        actions.push_back({.label = "Lock All", .icon_name = "lock", .enabled = true, .action_id = 11});
    }
    if (has_locked)
    {
        actions.push_back({.label = "Unlock All", .icon_name = "unlock", .enabled = true, .action_id = 12});
    }

    // ── Separator ─────────────────────────────────────────────────
    actions.push_back({.label = "---", .icon_name = "", .enabled = false, .action_id = 0});

    // ── Batch operations ──────────────────────────────────────────
    actions.push_back({.label = "Set Color…", .icon_name = "palette", .enabled = true, .action_id = 20});
    actions.push_back({.label = "Delete Selected", .icon_name = "trash-2", .enabled = true, .action_id = 30});

    return actions;
}

} // namespace markamp::canvas
