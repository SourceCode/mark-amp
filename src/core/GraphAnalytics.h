/// @file GraphAnalytics.h
/// @brief V9 Phase 16 – Graph analytics, tag graph, and dashboard.

#pragma once

#include "GraphTypes.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <numeric>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Graph Statistics
// ============================================================================

/// Aggregate statistics computed from a graph.
struct GraphStatistics
{
    int node_count{0};
    int edge_count{0};
    double density{0.0};    ///< edge_count / max_possible_edges
    double avg_degree{0.0}; ///< Average node degree
    int max_degree{0};      ///< Maximum node degree
    int connected_components{0};
    int isolated_count{0}; ///< Nodes with zero connections
};

// ============================================================================
// Centrality
// ============================================================================

/// Centrality metrics for a single node.
struct NodeCentrality
{
    std::string node_id;
    double degree_centrality{0.0};      ///< Fraction of nodes connected to this one
    double closeness_centrality{0.0};   ///< Inverse mean distance to all reachable nodes
    double betweenness_centrality{0.0}; ///< Fraction of shortest paths through this node
};

// ============================================================================
// Tag Graph
// ============================================================================

/// A node in the tag co-occurrence graph.
struct TagNode
{
    std::string tag;
    int document_count{0};
    std::vector<std::string> related_tags;
};

/// An edge in the tag co-occurrence graph.
struct TagEdge
{
    std::string source_tag;
    std::string target_tag;
    int co_occurrence_count{0}; ///< Number of documents sharing both tags
};

/// Graph of tag relationships (co-occurrence).
struct TagGraphData
{
    std::vector<TagNode> nodes;
    std::vector<TagEdge> edges;

    [[nodiscard]] auto node_count() const -> int
    {
        return static_cast<int>(nodes.size());
    }

    [[nodiscard]] auto edge_count() const -> int
    {
        return static_cast<int>(edges.size());
    }
};

// ============================================================================
// Dashboard
// ============================================================================

/// Aggregated health/status dashboard for the knowledge graph.
struct GraphDashboard
{
    GraphStatistics statistics;
    std::vector<std::pair<std::string, int>> top_connected; ///< Top N nodes by degree
    std::vector<std::string> recent_orphans;                ///< Orphan node IDs
    std::vector<std::pair<std::string, int>> tag_cloud;     ///< Tags with document counts
    double health_score{0.0};                               ///< 0.0 (unhealthy) – 1.0 (healthy)
};

// ============================================================================
// GraphAnalytics
// ============================================================================

/// Computes analytics over GraphData: statistics, centrality, tag graph, dashboard.
class GraphAnalytics
{
public:
    // ── Statistics ──

    /// Compute aggregate statistics for the graph.
    [[nodiscard]] static auto compute_statistics(const GraphData& graph) -> GraphStatistics
    {
        GraphStatistics stats;
        stats.node_count = static_cast<int>(graph.nodes.size());
        stats.edge_count = static_cast<int>(graph.links.size());

        if (stats.node_count == 0)
        {
            return stats;
        }

        // Build degree map
        std::unordered_map<std::string, int> degree_map;
        for (const auto& node : graph.nodes)
        {
            degree_map[node.id] = 0;
        }
        for (const auto& link : graph.links)
        {
            ++degree_map[link.source_id];
            ++degree_map[link.target_id];
        }

        // Compute degree metrics
        int total_degree = 0;
        for (const auto& [node_id, deg] : degree_map)
        {
            total_degree += deg;
            if (deg > stats.max_degree)
            {
                stats.max_degree = deg;
            }
            if (deg == 0)
            {
                ++stats.isolated_count;
            }
        }
        stats.avg_degree =
            static_cast<double>(total_degree) / static_cast<double>(stats.node_count);

        // Density: E / (N*(N-1)/2) for undirected
        const double max_edges =
            static_cast<double>(stats.node_count) * static_cast<double>(stats.node_count - 1) / 2.0;
        stats.density = (max_edges > 0.0) ? static_cast<double>(stats.edge_count) / max_edges : 0.0;

        // Connected components via BFS
        stats.connected_components = count_components(graph, degree_map);

        return stats;
    }

    // ── Centrality ──

    /// Compute degree, closeness, and betweenness centrality for all nodes.
    [[nodiscard]] static auto compute_centrality(const GraphData& graph)
        -> std::vector<NodeCentrality>
    {
        std::vector<NodeCentrality> result;
        if (graph.nodes.empty())
        {
            return result;
        }
        result.reserve(graph.nodes.size());

        // Build adjacency
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

        const int total_nodes = static_cast<int>(graph.nodes.size());
        const double denom = (total_nodes > 1) ? static_cast<double>(total_nodes - 1) : 1.0;

        // Betweenness accumulator
        std::unordered_map<std::string, double> betweenness;
        for (const auto& node : graph.nodes)
        {
            betweenness[node.id] = 0.0;
        }

        // Brandes' algorithm for betweenness + distances for closeness
        std::unordered_map<std::string, std::unordered_map<std::string, int>> all_distances;

        for (const auto& source_node : graph.nodes)
        {
            const auto& src = source_node.id;

            // BFS from source
            std::queue<std::string> bfs_queue;
            std::vector<std::string> bfs_order;
            std::unordered_map<std::string, int> dist;
            std::unordered_map<std::string, double> sigma; // # shortest paths
            std::unordered_map<std::string, std::vector<std::string>> predecessors;

            for (const auto& node : graph.nodes)
            {
                dist[node.id] = -1;
                sigma[node.id] = 0.0;
            }
            dist[src] = 0;
            sigma[src] = 1.0;
            bfs_queue.push(src);

            while (!bfs_queue.empty())
            {
                const auto current = bfs_queue.front();
                bfs_queue.pop();
                bfs_order.push_back(current);

                for (const auto& neighbor : adj[current])
                {
                    if (dist[neighbor] < 0)
                    {
                        dist[neighbor] = dist[current] + 1;
                        bfs_queue.push(neighbor);
                    }
                    if (dist[neighbor] == dist[current] + 1)
                    {
                        sigma[neighbor] += sigma[current];
                        predecessors[neighbor].push_back(current);
                    }
                }
            }

            all_distances[src] = dist;

            // Accumulate betweenness
            std::unordered_map<std::string, double> delta;
            for (const auto& node : graph.nodes)
            {
                delta[node.id] = 0.0;
            }

            // Reverse BFS order
            for (auto it = bfs_order.rbegin(); it != bfs_order.rend(); ++it)
            {
                const auto& w_node = *it;
                for (const auto& pred : predecessors[w_node])
                {
                    delta[pred] += (sigma[pred] / sigma[w_node]) * (1.0 + delta[w_node]);
                }
                if (w_node != src)
                {
                    betweenness[w_node] += delta[w_node];
                }
            }
        }

        // Build result
        for (const auto& node : graph.nodes)
        {
            NodeCentrality nc;
            nc.node_id = node.id;

            // Degree centrality
            nc.degree_centrality = static_cast<double>(adj[node.id].size()) / denom;

            // Closeness centrality
            const auto& distances = all_distances[node.id];
            double total_dist = 0.0;
            int reachable = 0;
            for (const auto& [target, d] : distances)
            {
                if (d > 0)
                {
                    total_dist += static_cast<double>(d);
                    ++reachable;
                }
            }
            nc.closeness_centrality =
                (reachable > 0) ? static_cast<double>(reachable) / total_dist : 0.0;

            // Betweenness centrality (normalized)
            const double bt_denom = (total_nodes > 2)
                                        ? static_cast<double>((total_nodes - 1) * (total_nodes - 2))
                                        : 1.0;
            nc.betweenness_centrality = betweenness[node.id] / bt_denom;

            result.push_back(std::move(nc));
        }

        return result;
    }

    // ── Tag Graph ──

    /// Build a tag co-occurrence graph from the knowledge graph.
    /// Nodes sharing a tag create edges between those tags weighted by co-occurrence.
    [[nodiscard]] static auto build_tag_graph(const GraphData& graph) -> TagGraphData
    {
        TagGraphData tag_graph;

        // Collect tags per node — use the icon field as tag storage (comma-separated)
        // In practice, tags come from the graph nodes' metadata.
        // For header-only testing, we treat nodes of type Tag as tag nodes.
        std::unordered_map<std::string, std::vector<std::string>> doc_tags; // doc_id → tags
        std::unordered_map<std::string, int> tag_doc_count;                 // tag → # docs

        // Identify tag nodes and their connections
        for (const auto& link : graph.links)
        {
            const auto* source = graph.find_node(link.source_id);
            const auto* target = graph.find_node(link.target_id);

            if (source != nullptr && target != nullptr)
            {
                if (target->type == GraphNodeType::Tag)
                {
                    doc_tags[source->id].push_back(target->label);
                }
                if (source->type == GraphNodeType::Tag)
                {
                    doc_tags[target->id].push_back(source->label);
                }
            }
        }

        // Count documents per tag
        for (const auto& [doc_id, tags] : doc_tags)
        {
            for (const auto& tag : tags)
            {
                ++tag_doc_count[tag];
            }
        }

        // Compute co-occurrence
        std::map<std::pair<std::string, std::string>, int> cooccurrence;
        for (const auto& [doc_id, tags] : doc_tags)
        {
            for (size_t i = 0; i < tags.size(); ++i)
            {
                for (size_t j = i + 1; j < tags.size(); ++j)
                {
                    auto key = (tags[i] < tags[j]) ? std::make_pair(tags[i], tags[j])
                                                   : std::make_pair(tags[j], tags[i]);
                    ++cooccurrence[key];
                }
            }
        }

        // Build tag nodes
        std::unordered_map<std::string, std::unordered_set<std::string>> related;
        for (const auto& [pair, count] : cooccurrence)
        {
            related[pair.first].insert(pair.second);
            related[pair.second].insert(pair.first);
        }

        for (const auto& [tag, doc_count] : tag_doc_count)
        {
            TagNode tn;
            tn.tag = tag;
            tn.document_count = doc_count;
            if (related.contains(tag))
            {
                tn.related_tags.assign(related[tag].begin(), related[tag].end());
                std::sort(tn.related_tags.begin(), tn.related_tags.end());
            }
            tag_graph.nodes.push_back(std::move(tn));
        }

        // Build tag edges
        for (const auto& [pair, count] : cooccurrence)
        {
            TagEdge te;
            te.source_tag = pair.first;
            te.target_tag = pair.second;
            te.co_occurrence_count = count;
            tag_graph.edges.push_back(std::move(te));
        }

        return tag_graph;
    }

    // ── Dashboard ──

    /// Compute aggregated dashboard data for the knowledge graph.
    [[nodiscard]] static auto compute_dashboard(const GraphData& graph, int top_n = 10)
        -> GraphDashboard
    {
        GraphDashboard dashboard;
        dashboard.statistics = compute_statistics(graph);

        // Top connected nodes
        std::unordered_map<std::string, int> degree_map;
        for (const auto& node : graph.nodes)
        {
            degree_map[node.id] = 0;
        }
        for (const auto& link : graph.links)
        {
            ++degree_map[link.source_id];
            ++degree_map[link.target_id];
        }

        std::vector<std::pair<std::string, int>> sorted_nodes(degree_map.begin(), degree_map.end());
        std::sort(sorted_nodes.begin(),
                  sorted_nodes.end(),
                  [](const auto& lhs, const auto& rhs) { return lhs.second > rhs.second; });

        const int limit = std::min(top_n, static_cast<int>(sorted_nodes.size()));
        dashboard.top_connected.assign(sorted_nodes.begin(), sorted_nodes.begin() + limit);

        // Orphans
        for (const auto& [node_id, deg] : degree_map)
        {
            if (deg == 0)
            {
                dashboard.recent_orphans.push_back(node_id);
            }
        }

        // Tag cloud
        const auto tag_graph = build_tag_graph(graph);
        dashboard.tag_cloud.reserve(tag_graph.nodes.size());
        for (const auto& tn : tag_graph.nodes)
        {
            dashboard.tag_cloud.emplace_back(tn.tag, tn.document_count);
        }
        std::sort(dashboard.tag_cloud.begin(),
                  dashboard.tag_cloud.end(),
                  [](const auto& lhs, const auto& rhs) { return lhs.second > rhs.second; });

        // Health score
        const double orphan_ratio = (dashboard.statistics.node_count > 0)
                                        ? static_cast<double>(dashboard.statistics.isolated_count) /
                                              static_cast<double>(dashboard.statistics.node_count)
                                        : 0.0;
        const double component_ratio =
            (dashboard.statistics.node_count > 0)
                ? static_cast<double>(dashboard.statistics.connected_components) /
                      static_cast<double>(dashboard.statistics.node_count)
                : 0.0;
        dashboard.health_score =
            std::clamp(1.0 - (orphan_ratio * 0.5 + (1.0 - dashboard.statistics.density) * 0.3 +
                              component_ratio * 0.2),
                       0.0,
                       1.0);

        return dashboard;
    }

private:
    /// Count connected components via BFS.
    [[nodiscard]] static auto
    count_components(const GraphData& graph,
                     const std::unordered_map<std::string, int>& /*degree_map*/) -> int
    {
        std::unordered_set<std::string> visited;
        int components = 0;

        // Build adjacency
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

        for (const auto& node : graph.nodes)
        {
            if (visited.contains(node.id))
            {
                continue;
            }
            ++components;

            // BFS
            std::queue<std::string> bfs_queue;
            bfs_queue.push(node.id);
            visited.insert(node.id);

            while (!bfs_queue.empty())
            {
                const auto current = bfs_queue.front();
                bfs_queue.pop();

                for (const auto& neighbor : adj[current])
                {
                    if (!visited.contains(neighbor))
                    {
                        visited.insert(neighbor);
                        bfs_queue.push(neighbor);
                    }
                }
            }
        }

        return components;
    }
};

} // namespace markamp::core
