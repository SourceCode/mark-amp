#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <chrono>
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

    // ── Clone ──────────────────────────────────────────────────────

    [[nodiscard]] auto deep_clone() const -> Board;

private:
    BoardMetadata metadata_;
    std::vector<std::unique_ptr<CanvasObject>> objects_;
    std::unordered_map<ObjectId, size_t> id_to_index_;
    bool dirty_{false};

    auto rebuild_index() -> void;
};

} // namespace markamp::canvas
