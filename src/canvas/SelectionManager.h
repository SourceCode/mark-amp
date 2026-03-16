#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"
#include "core/EventBus.h"

#include <memory>
#include <optional>
#include <unordered_set>
#include <vector>

namespace markamp::canvas
{

/// Handle type for resize/rotate handles.
enum class HandleType : uint8_t
{
    TopLeft,
    TopCenter,
    TopRight,
    MiddleLeft,
    MiddleRight,
    BottomLeft,
    BottomCenter,
    BottomRight,
    Rotation,
    None
};

/// Snapshot of an object's transform, used for cancel-and-restore.
struct TransformSnapshot
{
    ObjectId id{kInvalidObjectId};
    Transform2D transform;
};

/// Manages object selection state and interactive transforms (move, resize, rotate).
class SelectionManager
{
public:
    explicit SelectionManager(std::shared_ptr<core::EventBus> event_bus);

    // ── Selection State ────────────────────────────────────────────

    auto select(ObjectId obj_id) -> void;
    auto add_to_selection(ObjectId obj_id) -> void;
    auto remove_from_selection(ObjectId obj_id) -> void;
    auto toggle_selection(ObjectId obj_id) -> void;
    auto clear_selection() -> void;
    auto select_all(const std::vector<ObjectId>& ids) -> void;

    [[nodiscard]] auto is_selected(ObjectId obj_id) const -> bool;
    [[nodiscard]] auto selected_ids() const -> std::vector<ObjectId>;
    [[nodiscard]] auto selection_count() const -> size_t;

    // ── Selection Bounds ───────────────────────────────────────────

    /// Compute the combined AABB of all selected objects.
    [[nodiscard]] auto
    selection_bounds(const std::vector<std::unique_ptr<CanvasObject>>& objects) const
        -> std::optional<AABB>;

    // ── Move Transform ─────────────────────────────────────────────

    auto begin_move(const std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;
    auto update_move(double delta_x,
                     double delta_y,
                     std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;
    auto end_move() -> void;
    auto cancel_move(std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;

    // ── Resize Transform ───────────────────────────────────────────

    auto begin_resize(const std::vector<std::unique_ptr<CanvasObject>>& objects, HandleType handle)
        -> void;
    auto update_resize(double delta_x,
                       double delta_y,
                       std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;
    auto end_resize() -> void;
    auto cancel_resize(std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;

    // ── Rotate Transform ───────────────────────────────────────────

    auto begin_rotate(const std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;
    auto update_rotate(double delta_radians, std::vector<std::unique_ptr<CanvasObject>>& objects)
        -> void;
    auto end_rotate() -> void;
    auto cancel_rotate(std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;

    // ── State Queries ──────────────────────────────────────────────

    [[nodiscard]] auto is_moving() const -> bool;
    [[nodiscard]] auto is_resizing() const -> bool;
    [[nodiscard]] auto is_rotating() const -> bool;
    [[nodiscard]] auto active_handle() const -> HandleType;

    // ── Alignment (#9–14) ──────────────────────────────────────────
    auto align_left(std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;
    auto align_center_h(std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;
    auto align_right(std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;
    auto align_top(std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;
    auto align_center_v(std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;
    auto align_bottom(std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;

    // ── Distribution (#15–16) ──────────────────────────────────────
    auto distribute_horizontal(std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;
    auto distribute_vertical(std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;

    // ── Batch 10 (#59-60) ─────────────────────────────────────────

    /// Select all objects matching a specific CanvasObjectType.
    auto select_by_type(CanvasObjectType type,
                        const std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;

    /// Deselect current selection and select all others.
    auto invert_selection(const std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;

    // ── Batch 4 (#19-22) ──────────────────────────────────────────

    /// Select all objects whose parent_id matches any currently selected object.
    auto select_connected(const std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;

    /// Select all objects that overlap a given AABB region.
    auto select_in_region(const AABB& region,
                          const std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;

    /// Expand selection to include objects adjacent (overlapping) to currently selected.
    auto grow_selection(const std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;

    /// Return the set of unique CanvasObjectTypes present in the current selection.
    [[nodiscard]] auto
    selected_types(const std::vector<std::unique_ptr<CanvasObject>>& objects) const
        -> std::vector<CanvasObjectType>;

    // ── Batch 10 (#100) ───────────────────────────────────────────

    /// Context menu action descriptor for multi-select operations.
    struct MultiSelectAction
    {
        std::string label;
        std::string icon_name;
        bool enabled{true};
        int action_id{0};
    };

    /// Build a list of applicable context-menu actions for the current multi-selection.
    [[nodiscard]] auto build_multi_select_menu(
        const std::vector<std::unique_ptr<CanvasObject>>& objects) const
        -> std::vector<MultiSelectAction>;

    /// Whether anything is selected.
    [[nodiscard]] auto has_selection() const noexcept -> bool
    {
        return !selection_.empty();
    }

    /// Whether exactly one object is selected.
    [[nodiscard]] auto is_single_selection() const noexcept -> bool
    {
        return selection_.size() == 1;
    }

    /// Whether multiple objects are selected.
    [[nodiscard]] auto is_multi_selection() const noexcept -> bool
    {
        return selection_.size() > 1;
    }

    /// Whether any interactive transform (move/resize/rotate) is active.
    [[nodiscard]] auto is_transforming() const noexcept -> bool
    {
        return is_moving_ || is_resizing_ || is_rotating_;
    }

    /// Whether transform snapshots exist (operation in progress).
    [[nodiscard]] auto has_snapshots() const noexcept -> bool
    {
        return !snapshots_.empty();
    }

    // ── Batch 6 (#51-60) ──────────────────────────────────────────

    /// (#51) First selected object ID (or kInvalidObjectId).
    [[nodiscard]] auto first_selected_id() const -> ObjectId
    {
        if (selection_.empty()) { return kInvalidObjectId; }
        return *selection_.begin();
    }

    /// (#52) Last selected object ID (or kInvalidObjectId).
    [[nodiscard]] auto last_selected_id() const -> ObjectId
    {
        if (selection_.empty()) { return kInvalidObjectId; }
        ObjectId last = kInvalidObjectId;
        for (const auto& oid : selection_) { last = oid; }
        return last;
    }

    /// (#53) Whether selection contains an object of a specific type.
    [[nodiscard]] auto contains_type(
        CanvasObjectType type,
        const std::vector<std::unique_ptr<CanvasObject>>& objects) const -> bool
    {
        for (const auto& obj : objects)
        {
            if (obj && is_selected(obj->id()) && obj->type() == type) { return true; }
        }
        return false;
    }

    /// (#54) Whether every selected object is locked.
    [[nodiscard]] auto all_locked(
        const std::vector<std::unique_ptr<CanvasObject>>& objects) const -> bool
    {
        if (selection_.empty()) { return false; }
        for (const auto& obj : objects)
        {
            if (obj && is_selected(obj->id()) && !obj->is_locked()) { return false; }
        }
        return true;
    }

    /// (#55) Whether any selected object is locked.
    [[nodiscard]] auto any_locked(
        const std::vector<std::unique_ptr<CanvasObject>>& objects) const -> bool
    {
        for (const auto& obj : objects)
        {
            if (obj && is_selected(obj->id()) && obj->is_locked()) { return true; }
        }
        return false;
    }

    /// (#56) Count of selected objects matching a specific type.
    [[nodiscard]] auto selected_count_of_type(
        CanvasObjectType type,
        const std::vector<std::unique_ptr<CanvasObject>>& objects) const -> size_t
    {
        size_t count = 0;
        for (const auto& obj : objects)
        {
            if (obj && is_selected(obj->id()) && obj->type() == type) { ++count; }
        }
        return count;
    }

    /// (#57) Remove locked objects from selection.
    auto deselect_locked(
        const std::vector<std::unique_ptr<CanvasObject>>& objects) -> void
    {
        std::vector<ObjectId> to_remove;
        for (const auto& obj : objects)
        {
            if (obj && is_selected(obj->id()) && obj->is_locked())
            {
                to_remove.push_back(obj->id());
            }
        }
        for (const auto& oid : to_remove)
        {
            remove_from_selection(oid);
        }
    }

    /// (#58) Alias for contains_type.
    [[nodiscard]] auto has_type(
        CanvasObjectType type,
        const std::vector<std::unique_ptr<CanvasObject>>& objects) const -> bool
    {
        return contains_type(type, objects);
    }

    /// (#59) Center point of the selection bounds.
    [[nodiscard]] auto selection_center(
        const std::vector<std::unique_ptr<CanvasObject>>& objects) const
        -> std::optional<Point2D>
    {
        auto bounds = selection_bounds(objects);
        if (!bounds.has_value()) { return std::nullopt; }
        return bounds->center();
    }

    /// (#60) Total number of snapshots saved.
    [[nodiscard]] auto snapshot_count() const noexcept -> size_t
    {
        return snapshots_.size();
    }

private:
    std::shared_ptr<core::EventBus> event_bus_;
    std::unordered_set<ObjectId> selection_;
    std::vector<TransformSnapshot> snapshots_;
    HandleType active_handle_{HandleType::None};
    bool is_moving_{false};
    bool is_resizing_{false};
    bool is_rotating_{false};

    auto save_snapshots(const std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;
    auto restore_snapshots(std::vector<std::unique_ptr<CanvasObject>>& objects) -> void;
    auto publish_selection_changed() -> void;
};

} // namespace markamp::canvas
