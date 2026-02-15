/// @file LocalGraphEngine.cpp
/// @brief V4 Phase 36 – Local Backlink Graph Engine implementation.

#include "core/LocalGraphEngine.h"

#include "core/EventBus.h"
#include "core/Events.h"

#include <algorithm>
#include <queue>
#include <unordered_set>

namespace markamp::core
{

// ============================================================================
// Constructor
// ============================================================================

LocalGraphEngine::LocalGraphEngine(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ============================================================================
// Load data
// ============================================================================

auto LocalGraphEngine::load_notes(const std::vector<NoteInfo>& notes,
                                  const std::vector<NoteLink>& links) -> void
{
    notes_ = notes;
    outgoing_.clear();
    incoming_.clear();
    link_labels_.clear();

    for (const auto& link : links)
    {
        outgoing_[link.source_id].push_back(link.target_id);
        incoming_[link.target_id].push_back(link.source_id);
        const std::string edge_key = link.source_id + "->" + link.target_id;
        link_labels_[edge_key] = link.label;
    }
}

// ============================================================================
// Build local graph (BFS)
// ============================================================================

auto LocalGraphEngine::build_local_graph(const std::string& note_id, int depth) -> LocalGraphData
{
    const int max_depth = (depth >= 0) ? depth : config_.max_depth;

    LocalGraphData result;
    result.focus_id = note_id;

    // BFS from focus node.
    std::unordered_map<std::string, int> visited; // note_id -> depth
    std::queue<std::pair<std::string, int>> bfs_queue;

    visited[note_id] = 0;
    bfs_queue.push({note_id, 0});

    while (!bfs_queue.empty() && static_cast<int>(visited.size()) < config_.max_nodes)
    {
        auto [current_id, current_depth] = bfs_queue.front();
        bfs_queue.pop();

        if (current_depth < max_depth)
        {
            // Expand outgoing.
            auto out_it = outgoing_.find(current_id);
            if (out_it != outgoing_.end())
            {
                for (const auto& neighbor_id : out_it->second)
                {
                    if (visited.find(neighbor_id) == visited.end() &&
                        static_cast<int>(visited.size()) < config_.max_nodes)
                    {
                        visited[neighbor_id] = current_depth + 1;
                        bfs_queue.push({neighbor_id, current_depth + 1});
                    }
                }
            }

            // Expand incoming.
            auto inc_it = incoming_.find(current_id);
            if (inc_it != incoming_.end())
            {
                for (const auto& neighbor_id : inc_it->second)
                {
                    if (visited.find(neighbor_id) == visited.end() &&
                        static_cast<int>(visited.size()) < config_.max_nodes)
                    {
                        visited[neighbor_id] = current_depth + 1;
                        bfs_queue.push({neighbor_id, current_depth + 1});
                    }
                }
            }
        }
    }

    // Build node list.
    std::unordered_set<std::string> visited_set;
    for (const auto& [vid, vd] : visited)
    {
        visited_set.insert(vid);
    }

    for (const auto& note : notes_)
    {
        if (visited_set.find(note.note_id) != visited_set.end())
        {
            LocalGraphNode node;
            node.note_id = note.note_id;
            node.title = note.title;
            node.tags = note.tags;
            node.depth = visited[note.note_id];
            node.is_focus = (note.note_id == note_id);

            int links = 0;
            auto out_it = outgoing_.find(note.note_id);
            if (out_it != outgoing_.end())
            {
                links += static_cast<int>(out_it->second.size());
            }
            auto inc_it = incoming_.find(note.note_id);
            if (inc_it != incoming_.end())
            {
                links += static_cast<int>(inc_it->second.size());
            }
            node.link_count = links;

            result.nodes.push_back(std::move(node));
        }
    }

    // Build edge list (only edges between visited nodes).
    for (const auto& [vid, _depth] : visited)
    {
        auto out_it = outgoing_.find(vid);
        if (out_it != outgoing_.end())
        {
            for (const auto& target : out_it->second)
            {
                if (visited_set.find(target) != visited_set.end())
                {
                    LocalGraphEdge edge;
                    edge.source_id = vid;
                    edge.target_id = target;
                    const std::string edge_key = vid + "->" + target;
                    auto label_it = link_labels_.find(edge_key);
                    if (label_it != link_labels_.end())
                    {
                        edge.label = label_it->second;
                    }
                    result.edges.push_back(std::move(edge));
                }
            }
        }
    }

    events::LocalGraphBuiltEvent event;
    event.focus_id = note_id;
    event.node_count = static_cast<int>(result.nodes.size());
    event.edge_count = static_cast<int>(result.edges.size());
    event_bus_.publish(event);

    return result;
}

// ============================================================================
// Configuration
// ============================================================================

auto LocalGraphEngine::set_config(const LocalGraphConfig& config) -> void
{
    config_ = config;
}

// ============================================================================
// Neighbors
// ============================================================================

auto LocalGraphEngine::get_neighbors(const std::string& note_id) const -> std::vector<std::string>
{
    std::unordered_set<std::string> neighbors;

    auto out_it = outgoing_.find(note_id);
    if (out_it != outgoing_.end())
    {
        for (const auto& target : out_it->second)
        {
            neighbors.insert(target);
        }
    }

    auto inc_it = incoming_.find(note_id);
    if (inc_it != incoming_.end())
    {
        for (const auto& source : inc_it->second)
        {
            neighbors.insert(source);
        }
    }

    return {neighbors.begin(), neighbors.end()};
}

// ============================================================================
// Clusters (connected components)
// ============================================================================

auto LocalGraphEngine::get_clusters(const LocalGraphData& graph) const
    -> std::vector<std::vector<std::string>>
{
    // Build adjacency for the subgraph.
    std::unordered_map<std::string, std::vector<std::string>> adj;
    std::unordered_set<std::string> all_ids;

    for (const auto& node : graph.nodes)
    {
        all_ids.insert(node.note_id);
        adj[node.note_id]; // Ensure entry exists.
    }

    for (const auto& edge : graph.edges)
    {
        if (all_ids.count(edge.source_id) > 0 && all_ids.count(edge.target_id) > 0)
        {
            adj[edge.source_id].push_back(edge.target_id);
            adj[edge.target_id].push_back(edge.source_id);
        }
    }

    // BFS to find connected components.
    std::unordered_set<std::string> visited;
    std::vector<std::vector<std::string>> clusters;

    for (const auto& node_id : all_ids)
    {
        if (visited.count(node_id) > 0)
        {
            continue;
        }

        std::vector<std::string> component;
        std::queue<std::string> component_queue;
        component_queue.push(node_id);
        visited.insert(node_id);

        while (!component_queue.empty())
        {
            auto current = component_queue.front();
            component_queue.pop();
            component.push_back(current);

            for (const auto& neighbor : adj[current])
            {
                if (visited.count(neighbor) == 0)
                {
                    visited.insert(neighbor);
                    component_queue.push(neighbor);
                }
            }
        }

        std::sort(component.begin(), component.end());
        clusters.push_back(std::move(component));
    }

    return clusters;
}

// ============================================================================
// Filter by tag
// ============================================================================

auto LocalGraphEngine::filter_by_tag(const LocalGraphData& graph, const std::string& tag) const
    -> LocalGraphData
{
    LocalGraphData result;
    result.focus_id = graph.focus_id;

    std::unordered_set<std::string> kept_ids;
    for (const auto& node : graph.nodes)
    {
        for (const auto& node_tag : node.tags)
        {
            if (node_tag == tag)
            {
                result.nodes.push_back(node);
                kept_ids.insert(node.note_id);
                break;
            }
        }
    }

    for (const auto& edge : graph.edges)
    {
        if (kept_ids.count(edge.source_id) > 0 && kept_ids.count(edge.target_id) > 0)
        {
            result.edges.push_back(edge);
        }
    }

    events::LocalGraphFilteredEvent event;
    event.tag = tag;
    event.remaining_nodes = static_cast<int>(result.nodes.size());
    event_bus_.publish(event);

    return result;
}

// ============================================================================
// Shortest path (BFS)
// ============================================================================

auto LocalGraphEngine::highlight_path(const std::string& from, const std::string& to) const
    -> std::vector<std::string>
{
    if (from == to)
    {
        return {from};
    }

    std::unordered_map<std::string, std::string> parent;
    std::queue<std::string> path_queue;

    parent[from] = "";
    path_queue.push(from);

    while (!path_queue.empty())
    {
        auto current = path_queue.front();
        path_queue.pop();

        if (current == to)
        {
            // Reconstruct path.
            std::vector<std::string> path;
            std::string trace = to;
            while (!trace.empty())
            {
                path.push_back(trace);
                trace = parent[trace];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        // Expand outgoing.
        auto out_it = outgoing_.find(current);
        if (out_it != outgoing_.end())
        {
            for (const auto& neighbor : out_it->second)
            {
                if (parent.find(neighbor) == parent.end())
                {
                    parent[neighbor] = current;
                    path_queue.push(neighbor);
                }
            }
        }

        // Expand incoming (treat graph as undirected for path finding).
        auto inc_it = incoming_.find(current);
        if (inc_it != incoming_.end())
        {
            for (const auto& neighbor : inc_it->second)
            {
                if (parent.find(neighbor) == parent.end())
                {
                    parent[neighbor] = current;
                    path_queue.push(neighbor);
                }
            }
        }
    }

    return {}; // No path found.
}

// ============================================================================
// Accessors
// ============================================================================

auto LocalGraphEngine::node_count() const -> int
{
    return static_cast<int>(notes_.size());
}

auto LocalGraphEngine::edge_count() const -> int
{
    int count = 0;
    for (const auto& [_id, targets] : outgoing_)
    {
        count += static_cast<int>(targets.size());
    }
    return count;
}

} // namespace markamp::core
