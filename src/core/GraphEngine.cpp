/// @file GraphEngine.cpp
/// @brief V4 Phase 07 – Graph View Data Model and Force-Directed Layout Engine.

#include "core/GraphEngine.h"

#include "core/BacklinkIndex.h"
#include "core/Events.h"
#include "core/TagService.h"
#include "core/VaultService.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <regex>

namespace markamp::core
{

// ============================================================================
// GraphData methods
// ============================================================================

auto GraphData::node_count() const -> int
{
    return static_cast<int>(nodes.size());
}

auto GraphData::edge_count() const -> int
{
    return static_cast<int>(edges.size());
}

auto GraphData::find_node(const std::string& id) const -> const GraphNode*
{
    for (const auto& node : nodes)
    {
        if (node.id == id)
        {
            return &node;
        }
    }
    return nullptr;
}

auto GraphData::find_node_index(const std::string& id) const -> std::optional<int>
{
    for (int idx = 0; idx < static_cast<int>(nodes.size()); ++idx)
    {
        if (nodes[static_cast<size_t>(idx)].id == id)
        {
            return idx;
        }
    }
    return std::nullopt;
}

// ============================================================================
// GraphEngine constructor
// ============================================================================

GraphEngine::GraphEngine(EventBus& event_bus,
                         VaultService& vault_service,
                         BacklinkIndex& backlink_index,
                         TagService& tag_service)
    : event_bus_(event_bus)
    , vault_service_(vault_service)
    , backlink_index_(backlink_index)
    , tag_service_(tag_service)
{
    vault_changed_sub_ = event_bus_.subscribe<events::VaultOpenedEvent>(
        [this](const events::VaultOpenedEvent&) { invalidate(); });
}

// ============================================================================
// Graph Building
// ============================================================================

auto GraphEngine::build_graph(const GraphLayoutConfig& config) -> GraphData
{
    const std::lock_guard<std::mutex> lock(graph_mutex_);
    cancel_requested_ = false;

    GraphData graph;
    graph.nodes = collect_nodes(config);
    graph.edges = collect_edges(graph.nodes);

    // Compute node link counts from edges
    for (const auto& edge : graph.edges)
    {
        if (edge.source_index >= 0 && edge.source_index < static_cast<int>(graph.nodes.size()))
        {
            graph.nodes[static_cast<size_t>(edge.source_index)].link_count++;
        }
        if (edge.target_index >= 0 && edge.target_index < static_cast<int>(graph.nodes.size()))
        {
            graph.nodes[static_cast<size_t>(edge.target_index)].link_count++;
        }
    }

    // Mark orphans
    for (auto& node : graph.nodes)
    {
        node.is_orphan = (node.link_count == 0);
    }

    // Apply min_links filter (post link-count computation)
    if (config.min_links > 0)
    {
        std::vector<GraphNode> filtered_nodes;
        for (const auto& node : graph.nodes)
        {
            if (node.link_count >= config.min_links)
            {
                filtered_nodes.push_back(node);
            }
        }
        graph.nodes = std::move(filtered_nodes);
        // Rebuild edges with new indices
        graph.edges = collect_edges(graph.nodes);
    }

    // Filter orphans
    if (!config.show_orphans)
    {
        std::vector<GraphNode> non_orphan;
        for (const auto& node : graph.nodes)
        {
            if (!node.is_orphan)
            {
                non_orphan.push_back(node);
            }
        }
        graph.nodes = std::move(non_orphan);
        graph.edges = collect_edges(graph.nodes);
    }

    compute_node_sizes(graph, config);
    initialize_positions(graph);
    run_layout(graph, config);
    detect_clusters(graph);
    compute_bounds(graph);

    auto event = events::GraphComputedEvent{};
    event.node_count = graph.node_count();
    event.edge_count = graph.edge_count();
    event.cluster_count = graph.cluster_count;
    event_bus_.publish(event);

    return graph;
}

auto GraphEngine::build_local_graph(const std::string& center_id,
                                    int depth,
                                    const GraphLayoutConfig& config) -> GraphData
{
    const std::lock_guard<std::mutex> lock(graph_mutex_);
    cancel_requested_ = false;

    // BFS to find neighborhood
    auto neighborhood = bfs_neighbors(center_id, depth);
    neighborhood.insert(center_id);

    // Collect nodes only for the neighborhood
    auto all_nodes = collect_nodes(config);
    GraphData graph;

    for (const auto& node : all_nodes)
    {
        if (neighborhood.contains(node.id))
        {
            graph.nodes.push_back(node);
        }
    }

    graph.edges = collect_edges(graph.nodes);

    // Compute link counts
    for (const auto& edge : graph.edges)
    {
        if (edge.source_index >= 0 && edge.source_index < static_cast<int>(graph.nodes.size()))
        {
            graph.nodes[static_cast<size_t>(edge.source_index)].link_count++;
        }
        if (edge.target_index >= 0 && edge.target_index < static_cast<int>(graph.nodes.size()))
        {
            graph.nodes[static_cast<size_t>(edge.target_index)].link_count++;
        }
    }

    compute_node_sizes(graph, config);
    initialize_positions(graph);

    // Pin the center node at origin
    auto center_idx = graph.find_node_index(center_id);
    if (center_idx.has_value())
    {
        graph.nodes[static_cast<size_t>(*center_idx)].x = 0.0;
        graph.nodes[static_cast<size_t>(*center_idx)].y = 0.0;
    }

    run_layout(graph, config);

    // Pin center again after layout
    if (center_idx.has_value())
    {
        graph.nodes[static_cast<size_t>(*center_idx)].x = 0.0;
        graph.nodes[static_cast<size_t>(*center_idx)].y = 0.0;
    }

    detect_clusters(graph);
    compute_bounds(graph);

    return graph;
}

// ============================================================================
// Force-Directed Layout
// ============================================================================

auto GraphEngine::run_layout(GraphData& graph, const GraphLayoutConfig& config) -> int
{
    if (graph.nodes.size() <= 1)
    {
        return 0;
    }

    int iteration = 0;
    for (; iteration < config.max_iterations; ++iteration)
    {
        if (cancel_requested_.load())
        {
            break;
        }

        const double energy = step_layout(graph, config);

        if (energy < config.convergence_threshold)
        {
            break;
        }
    }

    return iteration;
}

auto GraphEngine::step_layout(GraphData& graph, const GraphLayoutConfig& config) -> double
{
    apply_repulsion(graph, config);
    apply_attraction(graph, config);
    apply_centering(graph);
    apply_damping(graph, config.damping);
    clamp_velocity(graph, config.max_velocity);
    update_positions(graph);

    return total_kinetic_energy(graph);
}

auto GraphEngine::apply_repulsion(GraphData& graph, const GraphLayoutConfig& config) -> void
{
    const auto count = graph.nodes.size();
    for (size_t idx_a = 0; idx_a < count; ++idx_a)
    {
        for (size_t idx_b = idx_a + 1; idx_b < count; ++idx_b)
        {
            double dx = graph.nodes[idx_b].x - graph.nodes[idx_a].x;
            double dy = graph.nodes[idx_b].y - graph.nodes[idx_a].y;
            double dist_sq = dx * dx + dy * dy;

            // Prevent division by zero / extreme forces
            const double kMinDistSq = config.min_distance * config.min_distance;
            if (dist_sq < kMinDistSq)
            {
                dist_sq = kMinDistSq;
            }

            const double dist = std::sqrt(dist_sq);

            // Coulomb's law: F = -strength / distance^2
            const double force = config.repulsion_strength / dist_sq;
            const double fx = force * (dx / dist);
            const double fy = force * (dy / dist);

            graph.nodes[idx_a].vx -= fx;
            graph.nodes[idx_a].vy -= fy;
            graph.nodes[idx_b].vx += fx;
            graph.nodes[idx_b].vy += fy;
        }
    }
}

auto GraphEngine::apply_attraction(GraphData& graph, const GraphLayoutConfig& config) -> void
{
    for (const auto& edge : graph.edges)
    {
        if (edge.source_index < 0 || edge.source_index >= static_cast<int>(graph.nodes.size()) ||
            edge.target_index < 0 || edge.target_index >= static_cast<int>(graph.nodes.size()))
        {
            continue;
        }

        auto& source = graph.nodes[static_cast<size_t>(edge.source_index)];
        auto& target = graph.nodes[static_cast<size_t>(edge.target_index)];

        const double dx = target.x - source.x;
        const double dy = target.y - source.y;
        const double dist = std::sqrt(dx * dx + dy * dy);

        if (dist < 1e-6)
        {
            continue;
        }

        // Hooke's law: F = strength * distance
        const double force = config.attraction_strength * dist * edge.strength;
        const double fx = force * (dx / dist);
        const double fy = force * (dy / dist);

        source.vx += fx;
        source.vy += fy;
        target.vx -= fx;
        target.vy -= fy;
    }
}

auto GraphEngine::apply_centering(GraphData& graph) -> void
{
    if (graph.nodes.empty())
    {
        return;
    }

    double cx = 0.0;
    double cy = 0.0;
    for (const auto& node : graph.nodes)
    {
        cx += node.x;
        cy += node.y;
    }
    cx /= static_cast<double>(graph.nodes.size());
    cy /= static_cast<double>(graph.nodes.size());

    // Gentle centering force
    constexpr double kCenteringStrength = 0.005;
    for (auto& node : graph.nodes)
    {
        node.vx -= (node.x - cx) * kCenteringStrength;
        node.vy -= (node.y - cy) * kCenteringStrength;
    }
}

auto GraphEngine::apply_damping(GraphData& graph, double damping) -> void
{
    for (auto& node : graph.nodes)
    {
        node.vx *= damping;
        node.vy *= damping;
    }
}

auto GraphEngine::clamp_velocity(GraphData& graph, double max_vel) -> void
{
    for (auto& node : graph.nodes)
    {
        const double speed = std::sqrt(node.vx * node.vx + node.vy * node.vy);
        if (speed > max_vel)
        {
            const double scale = max_vel / speed;
            node.vx *= scale;
            node.vy *= scale;
        }
    }
}

auto GraphEngine::update_positions(GraphData& graph) -> void
{
    for (auto& node : graph.nodes)
    {
        node.x += node.vx;
        node.y += node.vy;
    }
}

auto GraphEngine::total_kinetic_energy(const GraphData& graph) const -> double
{
    double energy = 0.0;
    for (const auto& node : graph.nodes)
    {
        energy += node.vx * node.vx + node.vy * node.vy;
    }
    return energy;
}

// ============================================================================
// Cluster Detection (Union-Find)
// ============================================================================

auto GraphEngine::detect_clusters(GraphData& graph) -> int
{
    const int count = static_cast<int>(graph.nodes.size());
    if (count == 0)
    {
        graph.cluster_count = 0;
        return 0;
    }

    // Union-Find with path compression and union by rank
    std::vector<int> parent(static_cast<size_t>(count));
    std::vector<int> rank(static_cast<size_t>(count), 0);
    std::iota(parent.begin(), parent.end(), 0);

    // Find with path compression
    std::function<int(int)> find = [&](int node_idx) -> int
    {
        if (parent[static_cast<size_t>(node_idx)] != node_idx)
        {
            parent[static_cast<size_t>(node_idx)] = find(parent[static_cast<size_t>(node_idx)]);
        }
        return parent[static_cast<size_t>(node_idx)];
    };

    // Union by rank
    auto unite = [&](int idx_a, int idx_b)
    {
        const int root_a = find(idx_a);
        const int root_b = find(idx_b);
        if (root_a == root_b)
        {
            return;
        }
        if (rank[static_cast<size_t>(root_a)] < rank[static_cast<size_t>(root_b)])
        {
            parent[static_cast<size_t>(root_a)] = root_b;
        }
        else if (rank[static_cast<size_t>(root_a)] > rank[static_cast<size_t>(root_b)])
        {
            parent[static_cast<size_t>(root_b)] = root_a;
        }
        else
        {
            parent[static_cast<size_t>(root_b)] = root_a;
            rank[static_cast<size_t>(root_a)]++;
        }
    };

    // Union all edges
    for (const auto& edge : graph.edges)
    {
        if (edge.source_index >= 0 && edge.source_index < count && edge.target_index >= 0 &&
            edge.target_index < count)
        {
            unite(edge.source_index, edge.target_index);
        }
    }

    // Assign cluster IDs
    std::unordered_map<int, int> root_to_cluster;
    int next_cluster = 0;
    for (int idx = 0; idx < count; ++idx)
    {
        const int root = find(idx);
        if (!root_to_cluster.contains(root))
        {
            root_to_cluster[root] = next_cluster++;
        }
        graph.nodes[static_cast<size_t>(idx)].cluster_id = root_to_cluster[root];
    }

    graph.cluster_count = next_cluster;
    return next_cluster;
}

// ============================================================================
// Graph Construction Helpers
// ============================================================================

auto GraphEngine::collect_nodes(const GraphLayoutConfig& config) -> std::vector<GraphNode>
{
    std::vector<GraphNode> nodes;

    // Daily note pattern: YYYY-MM-DD
    static const std::regex kDailyNotePattern(R"(\d{4}-\d{2}-\d{2})");

    auto documents = vault_service_.list_documents();
    for (const auto& entry : documents)
    {
        GraphNode node;
        node.id = entry.document_id;
        node.label = entry.title;
        node.notebook_id = entry.notebook_id;
        node.type = GraphNode::NodeType::Document;

        // Check if daily note by title pattern
        node.is_daily_note = std::regex_match(entry.title, kDailyNotePattern);

        // Filter daily notes if disabled
        if (!config.show_daily_notes && node.is_daily_note)
        {
            continue;
        }

        nodes.push_back(std::move(node));
    }

    // Optionally add tag nodes
    if (config.show_tags)
    {
        auto tag_stats = tag_service_.get_all_tags();
        for (const auto& stat : tag_stats)
        {
            GraphNode tag_node;
            tag_node.id = "tag:" + stat.tag;
            tag_node.label = "#" + stat.tag;
            tag_node.type = GraphNode::NodeType::Tag;
            tag_node.link_count = stat.count;
            nodes.push_back(std::move(tag_node));
        }
    }

    return nodes;
}

auto GraphEngine::collect_edges(const std::vector<GraphNode>& nodes) -> std::vector<GraphEdge>
{
    std::vector<GraphEdge> edges;

    // Build id-to-index map
    std::unordered_map<std::string, int> id_to_index;
    for (int idx = 0; idx < static_cast<int>(nodes.size()); ++idx)
    {
        id_to_index[nodes[static_cast<size_t>(idx)].id] = idx;
    }

    // Collect wikilink edges from BacklinkIndex
    for (const auto& node : nodes)
    {
        if (node.type != GraphNode::NodeType::Document)
        {
            continue;
        }

        auto outgoing = backlink_index_.get_outgoing_links(node.id);
        for (const auto& backlink : outgoing)
        {
            auto target_it = id_to_index.find(backlink.target_document_id);
            if (target_it != id_to_index.end())
            {
                auto source_it = id_to_index.find(node.id);
                if (source_it != id_to_index.end())
                {
                    // Avoid duplicate edges
                    bool exists = false;
                    for (const auto& existing : edges)
                    {
                        if (existing.source_index == source_it->second &&
                            existing.target_index == target_it->second)
                        {
                            exists = true;
                            break;
                        }
                    }
                    if (!exists)
                    {
                        GraphEdge edge;
                        edge.source_index = source_it->second;
                        edge.target_index = target_it->second;
                        edge.type = GraphEdge::EdgeType::WikiLink;
                        edges.push_back(edge);
                    }
                }
            }
        }
    }

    // Add tag edges if tag nodes are present
    for (int idx = 0; idx < static_cast<int>(nodes.size()); ++idx)
    {
        if (nodes[static_cast<size_t>(idx)].type == GraphNode::NodeType::Tag)
        {
            // Tag node id is "tag:<tag_name>"
            const std::string tag_name =
                nodes[static_cast<size_t>(idx)].id.substr(4); // Skip "tag:"
            auto doc_ids = tag_service_.get_documents_for_tag(tag_name);
            for (const auto& doc_id : doc_ids)
            {
                auto doc_it = id_to_index.find(doc_id);
                if (doc_it != id_to_index.end())
                {
                    GraphEdge edge;
                    edge.source_index = doc_it->second;
                    edge.target_index = idx;
                    edge.type = GraphEdge::EdgeType::Tag;
                    edge.strength = 0.5; // Weaker than wikilinks
                    edges.push_back(edge);
                }
            }
        }
    }

    return edges;
}

auto GraphEngine::compute_node_sizes(GraphData& graph, const GraphLayoutConfig& config) -> void
{
    if (!config.scale_by_links || graph.nodes.empty())
    {
        return;
    }

    // Find max link count
    int max_links = 0;
    for (const auto& node : graph.nodes)
    {
        max_links = std::max(max_links, node.link_count);
    }

    if (max_links == 0)
    {
        return;
    }

    // Logarithmic scaling: radius = min + (max - min) * log(1 + count) / log(1 + max_count)
    const double range = config.node_size_max - config.node_size_min;
    const double log_max = std::log(1.0 + static_cast<double>(max_links));

    for (auto& node : graph.nodes)
    {
        const double log_count = std::log(1.0 + static_cast<double>(node.link_count));
        node.radius = config.node_size_min + range * (log_count / log_max);
    }
}

auto GraphEngine::initialize_positions(GraphData& graph) -> void
{
    // Deterministic seed based on node count for reproducibility in tests
    std::mt19937 rng(static_cast<unsigned int>(graph.nodes.size() * 42));
    std::uniform_real_distribution<double> dist(-200.0, 200.0);

    for (auto& node : graph.nodes)
    {
        node.x = dist(rng);
        node.y = dist(rng);
        node.vx = 0.0;
        node.vy = 0.0;
    }
}

auto GraphEngine::compute_bounds(GraphData& graph) -> void
{
    if (graph.nodes.empty())
    {
        graph.bounds_x = 0.0;
        graph.bounds_y = 0.0;
        graph.bounds_width = 1000.0;
        graph.bounds_height = 1000.0;
        return;
    }

    double min_x = graph.nodes[0].x;
    double min_y = graph.nodes[0].y;
    double max_x = graph.nodes[0].x;
    double max_y = graph.nodes[0].y;

    for (const auto& node : graph.nodes)
    {
        min_x = std::min(min_x, node.x - node.radius);
        min_y = std::min(min_y, node.y - node.radius);
        max_x = std::max(max_x, node.x + node.radius);
        max_y = std::max(max_y, node.y + node.radius);
    }

    constexpr double kPadding = 50.0;
    graph.bounds_x = min_x - kPadding;
    graph.bounds_y = min_y - kPadding;
    graph.bounds_width = (max_x - min_x) + 2.0 * kPadding;
    graph.bounds_height = (max_y - min_y) + 2.0 * kPadding;
}

// ============================================================================
// BFS for Local Graph
// ============================================================================

auto GraphEngine::bfs_neighbors(const std::string& center_id, int depth) const
    -> std::set<std::string>
{
    std::set<std::string> visited;
    std::vector<std::string> current_level = {center_id};
    visited.insert(center_id);

    for (int level = 0; level < depth; ++level)
    {
        std::vector<std::string> next_level;
        for (const auto& doc_id : current_level)
        {
            auto connected = backlink_index_.get_connected_documents(doc_id);
            for (const auto& neighbor_id : connected)
            {
                if (!visited.contains(neighbor_id))
                {
                    visited.insert(neighbor_id);
                    next_level.push_back(neighbor_id);
                }
            }
        }
        current_level = std::move(next_level);
    }

    return visited;
}

// ============================================================================
// Cancel / Invalidate
// ============================================================================

auto GraphEngine::cancel_layout() -> void
{
    cancel_requested_ = true;
}

auto GraphEngine::invalidate() -> void
{
    cancel_requested_ = true;
    event_bus_.publish(events::GraphInvalidatedEvent{});
}

} // namespace markamp::core
