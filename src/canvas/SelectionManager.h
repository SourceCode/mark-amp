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
