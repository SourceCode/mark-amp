#pragma once

#include "CanvasTypes.h"

#include <array>
#include <functional>
#include <memory>
#include <vector>

namespace markamp::canvas
{

/// Quadtree spatial index for efficient region queries and hit-testing.
/// Stores ObjectId + AABB pairs. Supports insert, remove, query-region, and
/// query-point (hit-test).
class Quadtree
{
public:
    struct Entry
    {
        ObjectId id{kInvalidObjectId};
        AABB bounds;
    };

    /// Construct a quadtree covering the given world-space bounds.
    explicit Quadtree(const AABB& bounds, int max_depth = 8, int max_entries_per_node = 16);

    /// Insert an object into the index.
    auto insert(ObjectId obj_id, const AABB& bounds) -> void;

    /// Remove an object by ID. Returns true if found and removed.
    auto remove(ObjectId obj_id) -> bool;

    /// Update the bounds of an existing object. Equivalent to remove + insert.
    auto update(ObjectId obj_id, const AABB& new_bounds) -> void;

    /// Query all objects whose AABB intersects the given region.
    [[nodiscard]] auto query_region(const AABB& region) const -> std::vector<ObjectId>;

    /// Query the object at a specific point (hit-test). Returns the topmost
    /// candidate by insertion order; caller resolves z-order.
    [[nodiscard]] auto query_point(const Point2D& point) const -> std::vector<ObjectId>;

    /// Clear all entries and reset.
    auto clear() -> void;

    /// Rebuild the tree from scratch (call after bulk modifications).
    auto rebuild(const std::vector<Entry>& entries) -> void;

    /// Total number of entries stored.
    [[nodiscard]] auto size() const -> size_t;

private:
    struct Node
    {
        AABB bounds;
        std::vector<Entry> entries;
        std::array<std::unique_ptr<Node>, 4> children;
        bool is_leaf{true};
    };

    std::unique_ptr<Node> root_;
    int max_depth_;
    int max_entries_per_node_;
    size_t total_entries_{0};

    static auto subdivide(Node& node, int depth) -> void;
    auto insert_into(Node& node, const Entry& entry, int depth) -> void;
    auto query_region_impl(const Node& node,
                           const AABB& region,
                           std::vector<ObjectId>& results) const -> void;
    auto remove_from(Node& node, ObjectId obj_id) -> bool;
    [[nodiscard]] static auto child_index(const Node& node, const AABB& bounds) -> int;
};

} // namespace markamp::canvas
