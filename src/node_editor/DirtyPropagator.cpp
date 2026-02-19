#include "DirtyPropagator.h"

#include <queue>

namespace markamp::node_editor
{

// ---------------------------------------------------------------------------
// Dirty marking with downstream propagation
// ---------------------------------------------------------------------------

void DirtyPropagator::mark_dirty(NodeId node_id, const NodeGraph& graph)
{
    mark_dirty_local(node_id);
    propagate_downstream(node_id, graph);
}

void DirtyPropagator::mark_dirty_local(NodeId node_id)
{
    dirty_.insert(node_id);
    versions_[node_id]++;
}

auto DirtyPropagator::is_dirty(NodeId node_id) const -> bool
{
    return dirty_.contains(node_id);
}

auto DirtyPropagator::dirty_nodes() const -> std::vector<NodeId>
{
    return {dirty_.begin(), dirty_.end()};
}

auto DirtyPropagator::dirty_count() const -> std::size_t
{
    return dirty_.size();
}

// ---------------------------------------------------------------------------
// Clearing
// ---------------------------------------------------------------------------

void DirtyPropagator::clear_dirty(NodeId node_id)
{
    dirty_.erase(node_id);
}

void DirtyPropagator::clear_all()
{
    dirty_.clear();
}

// ---------------------------------------------------------------------------
// Cache versioning
// ---------------------------------------------------------------------------

auto DirtyPropagator::cache_version(NodeId node_id) const -> uint64_t
{
    const auto found = versions_.find(node_id);
    if (found == versions_.end())
    {
        return 0;
    }
    return found->second;
}

auto DirtyPropagator::is_cache_valid(NodeId node_id, uint64_t cached_version) const -> bool
{
    return cache_version(node_id) == cached_version;
}

// ---------------------------------------------------------------------------
// Dependency queries
// ---------------------------------------------------------------------------

auto DirtyPropagator::downstream_of(NodeId node_id, const NodeGraph& graph) const
    -> std::vector<NodeId>
{
    std::vector<NodeId> result;
    std::unordered_set<NodeId> visited;
    std::queue<NodeId> queue;

    queue.push(node_id);
    visited.insert(node_id);

    while (!queue.empty())
    {
        const auto current = queue.front();
        queue.pop();

        // Find all links originating from this node's output sockets.
        const auto* node = graph.find_node(current);
        if (node == nullptr)
        {
            continue;
        }

        const auto all_links = graph.all_link_ids();
        for (const auto& output_socket_id : node->outputs)
        {
            for (const auto& link_id : all_links)
            {
                const auto* link = graph.find_link(link_id);
                if (link == nullptr || link->source != output_socket_id)
                {
                    continue;
                }

                // Find the node that owns the target socket.
                const auto* target_socket = graph.find_socket(link->target);
                if (target_socket == nullptr)
                {
                    continue;
                }

                const auto target_node = target_socket->owner_node;
                if (!visited.contains(target_node))
                {
                    visited.insert(target_node);
                    result.push_back(target_node);
                    queue.push(target_node);
                }
            }
        }
    }

    return result;
}

// ---------------------------------------------------------------------------
// Private: propagation
// ---------------------------------------------------------------------------

void DirtyPropagator::propagate_downstream(NodeId node_id, const NodeGraph& graph)
{
    const auto downstream = downstream_of(node_id, graph);
    for (const auto& dep_id : downstream)
    {
        mark_dirty_local(dep_id);
    }
}

} // namespace markamp::node_editor
