#pragma once

/// @file GraphEngine.h
/// @brief V4 Phase 07 – Graph View Data Model and Force-Directed Layout Engine.

#include "EventBus.h"

#include <atomic>
#include <cstdint>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class VaultService;
class BacklinkIndex;
class TagService;

// ============================================================================
// Graph Node
// ============================================================================

struct GraphNode
{
    std::string id;
    std::string label;
    std::string notebook_id;
    double x{0.0};
    double y{0.0};
    double vx{0.0}; // velocity x
    double vy{0.0}; // velocity y
    double radius{6.0};
    int link_count{0}; // total incoming + outgoing
    bool is_daily_note{false};
    bool is_orphan{false};
    int cluster_id{-1}; // -1 = unassigned

    enum class NodeType : uint8_t
    {
        Document,
        Tag,
        Heading
    };
    NodeType type{NodeType::Document};
};

// ============================================================================
// Graph Edge
// ============================================================================

struct GraphEdge
{
    int source_index{0}; // Index into nodes vector
    int target_index{0};
    double strength{1.0};

    enum class EdgeType : uint8_t
    {
        WikiLink,
        Embed,
        Tag
    };
    EdgeType type{EdgeType::WikiLink};
};

// ============================================================================
// Layout Configuration
// ============================================================================

struct GraphLayoutConfig
{
    // Force simulation parameters
    double repulsion_strength{-400.0};
    double attraction_strength{0.01};
    double damping{0.9};
    double min_distance{20.0};
    double max_velocity{50.0};
    double convergence_threshold{0.01};
    int max_iterations{500};

    // Node sizing
    double node_size_min{4.0};
    double node_size_max{28.0};
    bool scale_by_links{true};

    // Filtering
    int min_links{0};
    bool show_orphans{true};
    bool show_daily_notes{true};
    bool show_tags{false};
    std::set<std::string> highlight_tags;
};

// ============================================================================
// Graph Data (output)
// ============================================================================

struct GraphData
{
    std::vector<GraphNode> nodes;
    std::vector<GraphEdge> edges;
    double bounds_x{0.0};
    double bounds_y{0.0};
    double bounds_width{1000.0};
    double bounds_height{1000.0};
    int cluster_count{0};

    [[nodiscard]] auto node_count() const -> int;
    [[nodiscard]] auto edge_count() const -> int;
    [[nodiscard]] auto find_node(const std::string& id) const -> const GraphNode*;
    [[nodiscard]] auto find_node_index(const std::string& id) const -> std::optional<int>;
};

// ============================================================================
// Graph Engine
// ============================================================================

/// Computes graph data from the vault and runs force-directed layout.
class GraphEngine
{
public:
    GraphEngine(EventBus& event_bus,
                VaultService& vault_service,
                BacklinkIndex& backlink_index,
                TagService& tag_service);

    /// Build graph data from the current vault state.
    [[nodiscard]] auto build_graph(const GraphLayoutConfig& config) -> GraphData;

    /// Build a local subgraph centered on a document with BFS to depth.
    [[nodiscard]] auto build_local_graph(const std::string& center_id,
                                         int depth,
                                         const GraphLayoutConfig& config) -> GraphData;

    /// Run the force-directed layout simulation on existing graph data.
    /// Modifies node positions in-place. Returns iteration count.
    auto run_layout(GraphData& graph, const GraphLayoutConfig& config) -> int;

    /// Run a single layout iteration (for animated layout).
    /// Returns the total kinetic energy (for convergence check).
    [[nodiscard]] auto step_layout(GraphData& graph, const GraphLayoutConfig& config) -> double;

    /// Detect clusters using connected component analysis (Union-Find).
    auto detect_clusters(GraphData& graph) -> int;

    /// Cancel a running layout simulation.
    auto cancel_layout() -> void;

    /// Invalidate cached graph data.
    auto invalidate() -> void;

private:
    EventBus& event_bus_;
    VaultService& vault_service_;
    BacklinkIndex& backlink_index_;
    TagService& tag_service_;

    std::atomic<bool> cancel_requested_{false};
    mutable std::mutex graph_mutex_;

    // Force calculation helpers
    auto apply_repulsion(GraphData& graph, const GraphLayoutConfig& config) -> void;
    auto apply_attraction(GraphData& graph, const GraphLayoutConfig& config) -> void;
    auto apply_centering(GraphData& graph) -> void;
    auto apply_damping(GraphData& graph, double damping) -> void;
    auto clamp_velocity(GraphData& graph, double max_vel) -> void;
    auto update_positions(GraphData& graph) -> void;
    [[nodiscard]] auto total_kinetic_energy(const GraphData& graph) const -> double;

    // Graph construction
    auto collect_nodes(const GraphLayoutConfig& config) -> std::vector<GraphNode>;
    auto collect_edges(const std::vector<GraphNode>& nodes) -> std::vector<GraphEdge>;
    auto compute_node_sizes(GraphData& graph, const GraphLayoutConfig& config) -> void;
    auto initialize_positions(GraphData& graph) -> void; // Random initial placement
    auto compute_bounds(GraphData& graph) -> void;

    // Local graph
    [[nodiscard]] auto bfs_neighbors(const std::string& center_id, int depth) const
        -> std::set<std::string>;

    Subscription vault_changed_sub_;
};

} // namespace markamp::core
