#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::canvas
{

/// Metadata describing a board.
struct BoardMetadata
{
    std::string id;
    std::string name{"Untitled Board"};
    std::string description;
    std::string author;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point modified_at;
    std::vector<std::string> tags;
    bool archived{false};
    bool favorite{false};
    int version{0};
    std::unordered_map<std::string, std::string> custom_properties;
    ObjectId last_selected_id{kInvalidObjectId};

    // Visual settings.
    CanvasColor background_color{255, 255, 255, 255};
    double default_zoom{1.0};
    bool grid_visible{true};
    double grid_spacing{20.0};
};

/// Board: owns all canvas objects, provides z-ordering, content bounds, dirty tracking.
class Board
{
public:
    Board();
    explicit Board(const std::string& name);

    // ── Metadata ───────────────────────────────────────────────────

    [[nodiscard]] auto metadata() const -> const BoardMetadata&;
    auto metadata_mut() -> BoardMetadata&;

    // ── Object Management ──────────────────────────────────────────

    auto add_object(std::unique_ptr<CanvasObject> obj) -> ObjectId;
    auto remove_object(ObjectId obj_id) -> std::unique_ptr<CanvasObject>;
    [[nodiscard]] auto get_object(ObjectId obj_id) const -> const CanvasObject*;
    auto get_object_mut(ObjectId obj_id) -> CanvasObject*;
    [[nodiscard]] auto object_count() const -> size_t;
    [[nodiscard]] auto all_object_ids() const -> std::vector<ObjectId>;

    /// Find all objects of a specific type.
    [[nodiscard]] auto find_objects_by_type(CanvasObjectType type) const -> std::vector<ObjectId>;

    /// Find all objects whose world_bounds intersect the given region.
    [[nodiscard]] auto find_objects_in_region(const AABB& region) const -> std::vector<ObjectId>;

    /// Return a count of objects grouped by type.
    [[nodiscard]] auto object_count_by_type() const -> std::unordered_map<uint8_t, size_t>;

    /// Direct access to the objects vector (for SelectionManager transforms).
    [[nodiscard]] auto objects() const -> const std::vector<std::unique_ptr<CanvasObject>>&;
    auto objects_mut() -> std::vector<std::unique_ptr<CanvasObject>>&;

    // ── Z-Ordering ─────────────────────────────────────────────────

    auto bring_to_front(ObjectId obj_id) -> void;
    auto send_to_back(ObjectId obj_id) -> void;
    auto bring_forward(ObjectId obj_id) -> void;
    auto send_backward(ObjectId obj_id) -> void;
    [[nodiscard]] auto z_ordered_ids() const -> std::vector<ObjectId>;

    // ── Content Bounds ─────────────────────────────────────────────

    [[nodiscard]] auto content_bounds() const -> AABB;

    // ── Dirty Tracking ─────────────────────────────────────────────

    [[nodiscard]] auto is_dirty() const -> bool;
    auto mark_dirty() -> void;
    auto clear_dirty() -> void;

    // ── Bulk Operations ────────────────────────────────────────────

    /// Remove all objects from the board.
    auto clear_all_objects() -> void;

    /// Sort objects left-to-right, top-to-bottom by world_bounds position.
    auto sort_objects_by_position() -> void;

    // ── Analytics ──────────────────────────────────────────────────

    /// Board statistics snapshot.
    struct BoardStatistics
    {
        size_t total_objects{0};
        std::unordered_map<uint8_t, size_t> type_counts;
        AABB bounds;
        bool favorite{false};
        int version{0};
    };

    /// Compute current board statistics.
    [[nodiscard]] auto statistics() const -> BoardStatistics;

    // ── Clone ──────────────────────────────────────────────────────

    [[nodiscard]] auto deep_clone() const -> Board;

    // ── Object Helpers (#38-40) ────────────────────────────────────

    /// Clone an object by ID and add the clone at an offset position.
    auto duplicate_object(ObjectId obj_id, double offset_x = 20.0, double offset_y = 20.0)
        -> ObjectId;

    /// Returns the number of distinct layers in use by objects.
    [[nodiscard]] auto layer_count() const -> int;

    /// Returns all objects assigned to a specific layer.
    [[nodiscard]] auto objects_on_layer(int layer_index) const -> std::vector<ObjectId>;

    // ── Utility (#25-30) ───────────────────────────────────────

    auto rename_object(ObjectId obj_id, const std::string& new_name) -> bool;
    auto move_object(ObjectId obj_id, double delta_x, double delta_y) -> bool;
    auto resize_object(ObjectId obj_id, double scale_x, double scale_y) -> bool;
    [[nodiscard]] auto find_objects_by_name(const std::string& substring) const
        -> std::vector<ObjectId>;
    [[nodiscard]] auto total_bounds_area() const -> double;
    [[nodiscard]] auto has_object(ObjectId obj_id) const -> bool;

    // ── Batch 1 (#1-6) ────────────────────────────────────────────

    /// Find all objects tagged with a specific tag string.
    [[nodiscard]] auto find_objects_by_tag(const std::string& tag) const -> std::vector<ObjectId>;

    /// Return the highest z-order object at a world point (hit-test).
    [[nodiscard]] auto find_topmost_at(const Point2D& point) const -> ObjectId;

    /// Re-index all z-indices to sequential 0..N with no gaps.
    auto compact_z_indices() -> void;

    /// Swap z-indices of two objects.
    auto swap_z_order(ObjectId first_id, ObjectId second_id) -> bool;

    /// Lock every object on the board.
    auto lock_all() -> void;

    /// Unlock every object on the board.
    auto unlock_all() -> void;

private:
    BoardMetadata metadata_;
    std::vector<std::unique_ptr<CanvasObject>> objects_;
    std::unordered_map<ObjectId, size_t> id_to_index_;
    bool dirty_{false};

    auto rebuild_index() -> void;
};

} // namespace markamp::canvas
