/// @file GraphSearch.h
/// @brief V9 Phase 16 – In-graph search and path queries.

#pragma once

#include "GraphTypes.h"

#include <algorithm>
#include <cstdint>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Search Types
// ============================================================================

/// How a search result matched.
enum class GraphSearchMatchType : uint8_t
{
    kLabelExact,     ///< Exact label match
    kLabelPrefix,    ///< Label starts with query
    kLabelContains,  ///< Label contains query
    kTagMatch,       ///< Tag node matches
    kAnnotationMatch ///< Annotation text matches
};

/// A single search result from the graph.
struct GraphSearchResult
{
    std::string node_id;
    std::string label;
    GraphSearchMatchType match_type{GraphSearchMatchType::kLabelContains};
    double relevance{0.0}; ///< 0.0–1.0 relevance score
};

// ============================================================================
// GraphSearchEngine
// ============================================================================

/// Provides search-within-graph and path query functionality.
class GraphSearchEngine
{
public:
    // ── Node Search ──

    /// Search for nodes in the graph by label text (case-insensitive).
    [[nodiscard]] static auto search_nodes(const GraphData& graph, const std::string& query)
        -> std::vector<GraphSearchResult>
    {
        std::vector<GraphSearchResult> results;
        if (query.empty())
        {
            return results;
        }

        const auto query_lower = to_lower(query);

        for (const auto& node : graph.nodes)
        {
            const auto label_lower = to_lower(node.label);

            GraphSearchResult result;
            result.node_id = node.id;
            result.label = node.label;

            if (label_lower == query_lower)
            {
                result.match_type = GraphSearchMatchType::kLabelExact;
                result.relevance = 1.0;
                results.push_back(std::move(result));
            }
            else if (label_lower.starts_with(query_lower))
            {
                result.match_type = GraphSearchMatchType::kLabelPrefix;
                result.relevance = 0.8;
                results.push_back(std::move(result));
            }
            else if (label_lower.find(query_lower) != std::string::npos)
            {
                result.match_type = GraphSearchMatchType::kLabelContains;
                result.relevance = 0.5;
                results.push_back(std::move(result));
            }
            else if (node.type == GraphNodeType::Tag &&
                     label_lower.find(query_lower) != std::string::npos)
            {
                result.match_type = GraphSearchMatchType::kTagMatch;
                result.relevance = 0.6;
                results.push_back(std::move(result));
            }
        }

        // Sort by relevance descending
        std::sort(results.begin(),
                  results.end(),
                  [](const auto& lhs, const auto& rhs) { return lhs.relevance > rhs.relevance; });

        return results;
    }

    // ── Path Queries ──

    /// Find the shortest path between two nodes via BFS.
    /// Returns an ordered vector of node IDs from source to target.
    /// Returns empty if no path exists.
    [[nodiscard]] static auto find_path(const GraphData& graph,
                                        const std::string& from_id,
                                        const std::string& to_id) -> std::vector<std::string>
    {
        if (from_id == to_id)
        {
            return {from_id};
        }

        const auto adj = build_adjacency(graph);
        if (!adj.contains(from_id) || !adj.contains(to_id))
        {
            return {};
        }

        // BFS
        std::unordered_map<std::string, std::string> parent;
        std::queue<std::string> bfs_queue;
        bfs_queue.push(from_id);
        parent[from_id] = "";

        while (!bfs_queue.empty())
        {
            const auto current = bfs_queue.front();
            bfs_queue.pop();

            if (current == to_id)
            {
                // Reconstruct path
                std::vector<std::string> path;
                std::string node = to_id;
                while (!node.empty())
                {
                    path.push_back(node);
                    node = parent[node];
                }
                std::reverse(path.begin(), path.end());
                return path;
            }

            if (adj.contains(current))
            {
                for (const auto& neighbor : adj.at(current))
                {
                    if (!parent.contains(neighbor))
                    {
                        parent[neighbor] = current;
                        bfs_queue.push(neighbor);
                    }
                }
            }
        }

        return {}; // No path
    }

    /// Find all paths between two nodes up to a maximum depth.
    [[nodiscard]] static auto find_all_paths(const GraphData& graph,
                                             const std::string& from_id,
                                             const std::string& to_id,
                                             int max_depth = 5)
        -> std::vector<std::vector<std::string>>
    {
        std::vector<std::vector<std::string>> all_paths;
        const auto adj = build_adjacency(graph);

        if (!adj.contains(from_id) || !adj.contains(to_id))
        {
            return all_paths;
        }

        // DFS with path tracking
        std::vector<std::string> current_path;
        std::unordered_set<std::string> visited;
        dfs_all_paths(adj, from_id, to_id, max_depth, current_path, visited, all_paths);

        return all_paths;
    }

    /// Get the connected component containing a specific node.
    [[nodiscard]] static auto find_connected_component(const GraphData& graph,
                                                       const std::string& node_id) -> GraphData
    {
        GraphData component;
        const auto adj = build_adjacency(graph);

        if (!adj.contains(node_id))
        {
            return component;
        }

        // BFS to find all reachable nodes
        std::unordered_set<std::string> reachable;
        std::queue<std::string> bfs_queue;
        bfs_queue.push(node_id);
        reachable.insert(node_id);

        while (!bfs_queue.empty())
        {
            const auto current = bfs_queue.front();
            bfs_queue.pop();

            if (adj.contains(current))
            {
                for (const auto& neighbor : adj.at(current))
                {
                    if (!reachable.contains(neighbor))
                    {
                        reachable.insert(neighbor);
                        bfs_queue.push(neighbor);
                    }
                }
            }
        }

        // Build subgraph
        for (const auto& node : graph.nodes)
        {
            if (reachable.contains(node.id))
            {
                component.nodes.push_back(node);
            }
        }
        for (const auto& link : graph.links)
        {
            if (reachable.contains(link.source_id) && reachable.contains(link.target_id))
            {
                component.links.push_back(link);
            }
        }

        return component;
    }

    /// Get all neighbors at a specific BFS depth from a node.
    [[nodiscard]] static auto get_neighbors_at_depth(const GraphData& graph,
                                                     const std::string& node_id,
                                                     int depth) -> std::vector<std::string>
    {
        std::vector<std::string> result;
        const auto adj = build_adjacency(graph);

        if (!adj.contains(node_id) || depth < 0)
        {
            return result;
        }

        // BFS tracking distance
        std::unordered_map<std::string, int> dist;
        std::queue<std::string> bfs_queue;
        bfs_queue.push(node_id);
        dist[node_id] = 0;

        while (!bfs_queue.empty())
        {
            const auto current = bfs_queue.front();
            bfs_queue.pop();

            if (dist[current] == depth && current != node_id)
            {
                result.push_back(current);
            }

            if (dist[current] < depth && adj.contains(current))
            {
                for (const auto& neighbor : adj.at(current))
                {
                    if (!dist.contains(neighbor))
                    {
                        dist[neighbor] = dist[current] + 1;
                        bfs_queue.push(neighbor);
                    }
                }
            }
        }

        return result;
    }

private:
    /// Build bi-directional adjacency list.
    [[nodiscard]] static auto build_adjacency(const GraphData& graph)
        -> std::unordered_map<std::string, std::vector<std::string>>
    {
        std::unordered_map<std::string, std::vector<std::string>> adj;
        for (const auto& node : graph.nodes)
        {
            adj[node.id] = {};
        }
        for (const auto& link : graph.links)
        {
            adj[link.source_id].push_back(link.target_id);
            adj[link.target_id].push_back(link.source_id);
        }
        return adj;
    }

    /// DFS for finding all paths.
    static void dfs_all_paths(const std::unordered_map<std::string, std::vector<std::string>>& adj,
                              const std::string& current,
                              const std::string& target,
                              int max_depth,
                              std::vector<std::string>& path,
                              std::unordered_set<std::string>& visited,
                              std::vector<std::vector<std::string>>& all_paths)
    {
        path.push_back(current);
        visited.insert(current);

        if (current == target)
        {
            all_paths.push_back(path);
        }
        else if (static_cast<int>(path.size()) <= max_depth && adj.contains(current))
        {
            for (const auto& neighbor : adj.at(current))
            {
                if (!visited.contains(neighbor))
                {
                    dfs_all_paths(adj, neighbor, target, max_depth, path, visited, all_paths);
                }
            }
        }

        path.pop_back();
        visited.erase(current);
    }

    [[nodiscard]] static auto to_lower(const std::string& str) -> std::string
    {
        std::string result = str;
        std::transform(result.begin(),
                       result.end(),
                       result.begin(),
                       [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
        return result;
    }
};

} // namespace markamp::core
