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

    // ── Batch 10 (#55-57) ──────────────────────────────────────────

    /// Returns true if the tree has no entries.
    [[nodiscard]] auto is_empty() const -> bool;

    /// Count entries within a specific region.
    [[nodiscard]] auto entry_count_in(const AABB& region) const -> size_t;

    /// Return the configured max depth of the tree.
    [[nodiscard]] auto depth() const -> int;

    /// Return the configured max entries per node.
    [[nodiscard]] auto max_entries_per_node() const noexcept -> int
    {
        return max_entries_per_node_;
    }

    /// Whether the tree contains any entries.
    [[nodiscard]] auto has_entries() const noexcept -> bool
    {
        return total_entries_ > 0;
    }

    // ── Batch 5 (#46-50) ──────────────────────────────────────────

    /// (#46) World-space bounds of the root node.
    [[nodiscard]] auto bounds() const noexcept -> const AABB&
    {
        return root_->bounds;
    }

    /// (#47) Configured maximum tree depth.
    [[nodiscard]] auto max_depth_limit() const noexcept -> int
    {
        return max_depth_;
    }

    /// (#48) Whether an object ID exists in the tree.
    [[nodiscard]] auto contains(ObjectId obj_id) const -> bool
    {
        const auto results = query_region(root_->bounds);
        for (const auto& oid : results)
        {
            if (oid == obj_id) { return true; }
        }
        return false;
    }

    /// (#49) Count of entries at a specific point.
    [[nodiscard]] auto query_point_count(const Point2D& point) const -> size_t
    {
        return query_point(point).size();
    }

    /// (#50) Alias for size().
    [[nodiscard]] auto entry_count() const -> size_t
    {
        return size();
    }

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
