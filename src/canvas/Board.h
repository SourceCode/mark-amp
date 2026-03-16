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

    /// Whether the board has any tags.
    [[nodiscard]] auto has_tags() const noexcept -> bool
    {
        return !tags.empty();
    }

    /// Number of tags on the board.
    [[nodiscard]] auto tag_count() const noexcept -> std::size_t
    {
        return tags.size();
    }
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

    /// Whether the board has no objects.
    [[nodiscard]] auto is_empty() const -> bool
    {
        return objects_.empty();
    }

    /// Whether the board is archived.
    [[nodiscard]] auto is_archived() const -> bool
    {
        return metadata_.archived;
    }

    /// Whether the board is favorited.
    [[nodiscard]] auto is_favorite() const -> bool
    {
        return metadata_.favorite;
    }

    // ── Batch 3 (#21-30) ──────────────────────────────────────────

    /// (#21) Count of visible objects.
    [[nodiscard]] auto visible_object_count() const -> size_t
    {
        size_t count = 0;
        for (const auto& obj : objects_)
        {
            if (obj && obj->is_visible()) { ++count; }
        }
        return count;
    }

    /// (#22) Count of locked objects.
    [[nodiscard]] auto locked_object_count() const -> size_t
    {
        size_t count = 0;
        for (const auto& obj : objects_)
        {
            if (obj && obj->is_locked()) { ++count; }
        }
        return count;
    }

    /// (#23) Count of dirty objects.
    [[nodiscard]] auto dirty_object_count() const -> size_t
    {
        size_t count = 0;
        for (const auto& obj : objects_)
        {
            if (obj && obj->is_dirty()) { ++count; }
        }
        return count;
    }

    /// (#24) Whether any objects are locked.
    [[nodiscard]] auto has_locked_objects() const -> bool
    {
        return locked_object_count() > 0;
    }

    /// (#25) Whether any object has a given tag.
    [[nodiscard]] auto has_tag(const std::string& tag) const -> bool
    {
        for (const auto& obj : objects_)
        {
            if (!obj) { continue; }
            for (const auto& obj_tag : obj->tags())
            {
                if (obj_tag == tag) { return true; }
            }
        }
        return false;
    }

    /// (#26) Collect all unique tags across all objects.
    [[nodiscard]] auto all_tags() const -> std::vector<std::string>
    {
        std::vector<std::string> result;
        for (const auto& obj : objects_)
        {
            if (!obj) { continue; }
            for (const auto& tag : obj->tags())
            {
                bool found = false;
                for (const auto& existing : result)
                {
                    if (existing == tag) { found = true; break; }
                }
                if (!found) { result.push_back(tag); }
            }
        }
        return result;
    }

    /// (#27) Highest z_index on the board.
    [[nodiscard]] auto topmost_z() const -> int
    {
        int max_z = 0;
        for (const auto& obj : objects_)
        {
            if (obj && obj->z_index() > max_z) { max_z = obj->z_index(); }
        }
        return max_z;
    }

    /// (#28) Lowest z_index on the board.
    [[nodiscard]] auto bottommost_z() const -> int
    {
        if (objects_.empty()) { return 0; }
        int min_z = std::numeric_limits<int>::max();
        for (const auto& obj : objects_)
        {
            if (obj && obj->z_index() < min_z) { min_z = obj->z_index(); }
        }
        return min_z == std::numeric_limits<int>::max() ? 0 : min_z;
    }

    /// (#29) Average area per object (total_bounds_area / object_count).
    [[nodiscard]] auto average_object_area() const -> double
    {
        if (objects_.empty()) { return 0.0; }
        return total_bounds_area() / static_cast<double>(objects_.size());
    }

    /// (#30) Shortcut for metadata().name.
    [[nodiscard]] auto board_name() const -> const std::string&
    {
        return metadata_.name;
    }

private:
    BoardMetadata metadata_;
    std::vector<std::unique_ptr<CanvasObject>> objects_;
    std::unordered_map<ObjectId, size_t> id_to_index_;
    bool dirty_{false};

    auto rebuild_index() -> void;
};

} // namespace markamp::canvas
