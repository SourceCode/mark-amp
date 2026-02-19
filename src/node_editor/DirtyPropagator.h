#pragma once

#include "NodeEditorTypes.h"
#include "NodeGraph.h"

#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::node_editor
{

// ---------------------------------------------------------------------------
// DirtyPropagator — tracks dirty state and propagates downstream
// ---------------------------------------------------------------------------

class DirtyPropagator
{
public:
    DirtyPropagator() = default;

    // --- Dirty marking ----------------------------------------------------

    /// Mark a node as dirty and propagate to all downstream dependents.
    void mark_dirty(NodeId node_id, const NodeGraph& graph);

    /// Mark a single node dirty without propagation.
    void mark_dirty_local(NodeId node_id);

    /// Check if a node is dirty.
    [[nodiscard]] auto is_dirty(NodeId node_id) const -> bool;

    /// Get all dirty node IDs.
    [[nodiscard]] auto dirty_nodes() const -> std::vector<NodeId>;

    /// Count of dirty nodes.
    [[nodiscard]] auto dirty_count() const -> std::size_t;

    // --- Clearing ---------------------------------------------------------

    /// Clear dirty flag for a single node (e.g. after recompute).
    void clear_dirty(NodeId node_id);

    /// Clear all dirty flags.
    void clear_all();

    // --- Cache versioning -------------------------------------------------

    /// Get the cache version for a node (increments each time it becomes dirty).
    [[nodiscard]] auto cache_version(NodeId node_id) const -> uint64_t;

    /// Check if a cached result is still valid.
    [[nodiscard]] auto is_cache_valid(NodeId node_id, uint64_t cached_version) const -> bool;

    // --- Dependency queries -----------------------------------------------

    /// Get all nodes downstream of a given node (transitive).
    [[nodiscard]] auto downstream_of(NodeId node_id, const NodeGraph& graph) const
        -> std::vector<NodeId>;

private:
    std::unordered_set<NodeId> dirty_;
    std::unordered_map<NodeId, uint64_t> versions_;

    void propagate_downstream(NodeId node_id, const NodeGraph& graph);
};

} // namespace markamp::node_editor
