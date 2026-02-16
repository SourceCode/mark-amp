#include "Quadtree.h"

#include <algorithm>

namespace markamp::canvas
{

Quadtree::Quadtree(const AABB& bounds, int max_depth, int max_entries_per_node)
    : root_(std::make_unique<Node>())
    , max_depth_(max_depth)
    , max_entries_per_node_(max_entries_per_node)
{
    root_->bounds = bounds;
}

auto Quadtree::insert(ObjectId obj_id, const AABB& bounds) -> void
{
    insert_into(*root_, {obj_id, bounds}, 0);
    ++total_entries_;
}

auto Quadtree::remove(ObjectId obj_id) -> bool
{
    if (remove_from(*root_, obj_id))
    {
        --total_entries_;
        return true;
    }
    return false;
}

auto Quadtree::update(ObjectId obj_id, const AABB& new_bounds) -> void
{
    remove(obj_id);
    insert(obj_id, new_bounds);
}

auto Quadtree::query_region(const AABB& region) const -> std::vector<ObjectId>
{
    std::vector<ObjectId> results;
    query_region_impl(*root_, region, results);
    return results;
}

auto Quadtree::query_point(const Point2D& point) const -> std::vector<ObjectId>
{
    // A point query is a region query with a zero-area AABB.
    const AABB point_aabb{point.x, point.y, point.x, point.y};
    std::vector<ObjectId> results;
    query_region_impl(*root_, point_aabb, results);
    return results;
}

auto Quadtree::clear() -> void
{
    root_ = std::make_unique<Node>();
    root_->bounds = {-1e9, -1e9, 1e9, 1e9};
    total_entries_ = 0;
}

auto Quadtree::rebuild(const std::vector<Entry>& entries) -> void
{
    const AABB bounds = root_->bounds;
    root_ = std::make_unique<Node>();
    root_->bounds = bounds;
    total_entries_ = 0;

    for (const auto& entry : entries)
    {
        insert(entry.id, entry.bounds);
    }
}

auto Quadtree::size() const -> size_t
{
    return total_entries_;
}

// ============================================================================
// Private implementation
// ============================================================================

auto Quadtree::child_index(const Node& node, const AABB& bounds) -> int
{
    const double mid_x = (node.bounds.min_x + node.bounds.max_x) / 2.0;
    const double mid_y = (node.bounds.min_y + node.bounds.max_y) / 2.0;

    // Determine which quadrant the bounds fits entirely within.
    // 0: top-left, 1: top-right, 2: bottom-left, 3: bottom-right
    // Returns -1 if the bounds spans multiple quadrants.

    const bool in_left = bounds.max_x <= mid_x;
    const bool in_right = bounds.min_x >= mid_x;
    const bool in_top = bounds.max_y <= mid_y;
    const bool in_bottom = bounds.min_y >= mid_y;

    if (in_left && in_top)
    {
        return 0;
    }
    if (in_right && in_top)
    {
        return 1;
    }
    if (in_left && in_bottom)
    {
        return 2;
    }
    if (in_right && in_bottom)
    {
        return 3;
    }

    return -1; // Spans multiple quadrants.
}

auto Quadtree::subdivide(Node& node, int /*depth*/) -> void
{
    const double mid_x = (node.bounds.min_x + node.bounds.max_x) / 2.0;
    const double mid_y = (node.bounds.min_y + node.bounds.max_y) / 2.0;

    // Top-left
    node.children[0] = std::make_unique<Node>();
    node.children[0]->bounds = {node.bounds.min_x, node.bounds.min_y, mid_x, mid_y};

    // Top-right
    node.children[1] = std::make_unique<Node>();
    node.children[1]->bounds = {mid_x, node.bounds.min_y, node.bounds.max_x, mid_y};

    // Bottom-left
    node.children[2] = std::make_unique<Node>();
    node.children[2]->bounds = {node.bounds.min_x, mid_y, mid_x, node.bounds.max_y};

    // Bottom-right
    node.children[3] = std::make_unique<Node>();
    node.children[3]->bounds = {mid_x, mid_y, node.bounds.max_x, node.bounds.max_y};

    node.is_leaf = false;

    // Redistribute existing entries into children.
    std::vector<Entry> remaining;
    for (auto& entry : node.entries)
    {
        const int idx = child_index(node, entry.bounds);
        if (idx >= 0)
        {
            node.children[static_cast<size_t>(idx)]->entries.push_back(std::move(entry));
        }
        else
        {
            remaining.push_back(std::move(entry));
        }
    }
    node.entries = std::move(remaining);
}

auto Quadtree::insert_into(Node& node, const Entry& entry, int depth) -> void
{
    if (!node.is_leaf)
    {
        const int idx = child_index(node, entry.bounds);
        if (idx >= 0)
        {
            insert_into(*node.children[static_cast<size_t>(idx)], entry, depth + 1);
            return;
        }
        // Entry spans multiple quadrants, store in this node.
        node.entries.push_back(entry);
        return;
    }

    // Leaf node.
    node.entries.push_back(entry);

    // Subdivide if over capacity and haven't hit max depth.
    if (static_cast<int>(node.entries.size()) > max_entries_per_node_ && depth < max_depth_)
    {
        subdivide(node, depth);
    }
}

auto Quadtree::query_region_impl(const Node& node,
                                 const AABB& region,
                                 std::vector<ObjectId>& results) const -> void
{
    if (!node.bounds.intersects(region))
    {
        return;
    }

    for (const auto& entry : node.entries)
    {
        if (entry.bounds.intersects(region))
        {
            results.push_back(entry.id);
        }
    }

    if (!node.is_leaf)
    {
        for (const auto& child : node.children)
        {
            if (child)
            {
                query_region_impl(*child, region, results);
            }
        }
    }
}

auto Quadtree::remove_from(Node& node, ObjectId obj_id) -> bool
{
    // Check entries in this node.
    auto iter = std::find_if(node.entries.begin(),
                             node.entries.end(),
                             [obj_id](const Entry& entry) { return entry.id == obj_id; });
    if (iter != node.entries.end())
    {
        node.entries.erase(iter);
        return true;
    }

    // Recurse into children.
    if (!node.is_leaf)
    {
        for (auto& child : node.children)
        {
            if (child && remove_from(*child, obj_id))
            {
                return true;
            }
        }
    }

    return false;
}

// --- Batch 10 (#55-57) ---

auto Quadtree::is_empty() const -> bool
{
    return total_entries_ == 0;
}

auto Quadtree::entry_count_in(const AABB& region) const -> size_t
{
    return query_region(region).size();
}

auto Quadtree::depth() const -> int
{
    return max_depth_;
}

} // namespace markamp::canvas
