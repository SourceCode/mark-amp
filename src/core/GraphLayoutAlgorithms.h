/// @file GraphLayoutAlgorithms.h
/// @brief V9 Phase 16 – Additional graph layout algorithms with separate position storage.

#pragma once

#include "GraphTypes.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <queue>
#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Layout Types
// ============================================================================

/// Position data for a graph node (GraphNode itself has no x/y).
struct NodePosition
{
    std::string node_id;
    double pos_x{0.0};
    double pos_y{0.0};
};

/// Full result of a layout computation.
struct LayoutResult
{
    std::vector<NodePosition> positions;

    /// Find position by node ID (or nullptr).
    [[nodiscard]] auto find(const std::string& node_id) const -> const NodePosition*
    {
        for (const auto& pos : positions)
        {
            if (pos.node_id == node_id)
            {
                return &pos;
            }
        }
        return nullptr;
    }
};

/// Available layout algorithms.
enum class LayoutAlgorithm : uint8_t
{
    kForceDirected, ///< Physics-based (handled by GraphEngine)
    kCircular,      ///< Nodes on a circle
    kGrid,          ///< Nodes in a grid
    kRadial,        ///< Concentric rings from center node
    kHierarchical,  ///< Top-down tree layout
    kRandom         ///< Random placement
};

/// Parameters for layout algorithms.
struct LayoutParams
{
    double center_x{0.0};
    double center_y{0.0};
    double radius{200.0};        ///< For circular/radial layouts
    double spacing{80.0};        ///< For grid/hierarchical layouts
    int columns{0};              ///< For grid layout (0 = auto)
    double level_spacing{100.0}; ///< Vertical spacing for hierarchical
    double node_spacing{60.0};   ///< Horizontal spacing for hierarchical
    double width{800.0};         ///< For random layout bounds
    double height{600.0};        ///< For random layout bounds
    std::string center_id;       ///< For radial/hierarchical root node
    unsigned int seed{42};       ///< For random layout reproducibility
};

// ============================================================================
// CircularLayout
// ============================================================================

/// Places nodes evenly on a circle.
class CircularLayout
{
public:
    [[nodiscard]] static auto
    apply(const GraphData& graph, double center_x, double center_y, double radius_val)
        -> LayoutResult
    {
        LayoutResult result;
        if (graph.nodes.empty())
        {
            return result;
        }

        const int count = static_cast<int>(graph.nodes.size());
        const double angle_step = 2.0 * M_PI / static_cast<double>(count);

        for (int i = 0; i < count; ++i)
        {
            const double angle = static_cast<double>(i) * angle_step;
            NodePosition pos;
            pos.node_id = graph.nodes[static_cast<size_t>(i)].id;
            pos.pos_x = center_x + radius_val * std::cos(angle);
            pos.pos_y = center_y + radius_val * std::sin(angle);
            result.positions.push_back(pos);
        }
        return result;
    }
};

// ============================================================================
// GridLayout
// ============================================================================

/// Places nodes in a regular grid.
class GridLayout
{
public:
    [[nodiscard]] static auto apply(const GraphData& graph, int cols, double spacing)
        -> LayoutResult
    {
        LayoutResult result;
        if (graph.nodes.empty())
        {
            return result;
        }

        const int count = static_cast<int>(graph.nodes.size());
        const int effective_cols =
            (cols > 0) ? cols : static_cast<int>(std::ceil(std::sqrt(count)));

        for (int i = 0; i < count; ++i)
        {
            const int row = i / effective_cols;
            const int col = i % effective_cols;
            NodePosition pos;
            pos.node_id = graph.nodes[static_cast<size_t>(i)].id;
            pos.pos_x = static_cast<double>(col) * spacing;
            pos.pos_y = static_cast<double>(row) * spacing;
            result.positions.push_back(pos);
        }
        return result;
    }
};

// ============================================================================
// RadialLayout
// ============================================================================

/// Places nodes in concentric rings based on BFS distance from a center node.
class RadialLayout
{
public:
    [[nodiscard]] static auto
    apply(const GraphData& graph, const std::string& center_id, double ring_spacing) -> LayoutResult
    {
        LayoutResult result;
        if (graph.nodes.empty())
        {
            return result;
        }

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

        // BFS from center
        std::unordered_map<std::string, int> depth;
        std::queue<std::string> bfs_queue;

        const std::string& root =
            (!center_id.empty() && adj.contains(center_id)) ? center_id : graph.nodes[0].id;

        bfs_queue.push(root);
        depth[root] = 0;

        while (!bfs_queue.empty())
        {
            const auto current = bfs_queue.front();
            bfs_queue.pop();
            for (const auto& neighbor : adj[current])
            {
                if (!depth.contains(neighbor))
                {
                    depth[neighbor] = depth[current] + 1;
                    bfs_queue.push(neighbor);
                }
            }
        }

        // Group by ring
        std::unordered_map<int, std::vector<std::string>> rings;
        for (const auto& node : graph.nodes)
        {
            const int dist = depth.contains(node.id) ? depth[node.id] : 999;
            rings[dist].push_back(node.id);
        }

        // Place nodes on concentric rings
        for (const auto& [ring_depth, node_ids] : rings)
        {
            const double ring_radius = static_cast<double>(ring_depth) * ring_spacing;
            if (ring_depth == 0)
            {
                for (const auto& nid : node_ids)
                {
                    NodePosition pos;
                    pos.node_id = nid;
                    pos.pos_x = 0.0;
                    pos.pos_y = 0.0;
                    result.positions.push_back(pos);
                }
                continue;
            }

            const double angle_step = 2.0 * M_PI / static_cast<double>(node_ids.size());
            for (size_t j = 0; j < node_ids.size(); ++j)
            {
                const double angle = static_cast<double>(j) * angle_step;
                NodePosition pos;
                pos.node_id = node_ids[j];
                pos.pos_x = ring_radius * std::cos(angle);
                pos.pos_y = ring_radius * std::sin(angle);
                result.positions.push_back(pos);
            }
        }
        return result;
    }
};

// ============================================================================
// HierarchicalLayout
// ============================================================================

/// Top-down tree layout using BFS levels.
class HierarchicalLayout
{
public:
    [[nodiscard]] static auto apply(const GraphData& graph,
                                    const std::string& root_id,
                                    double level_spacing,
                                    double node_spacing) -> LayoutResult
    {
        LayoutResult result;
        if (graph.nodes.empty())
        {
            return result;
        }

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

        // BFS from root
        const std::string& root =
            (!root_id.empty() && adj.contains(root_id)) ? root_id : graph.nodes[0].id;

        std::unordered_map<std::string, int> level;
        std::queue<std::string> bfs_queue;
        bfs_queue.push(root);
        level[root] = 0;

        while (!bfs_queue.empty())
        {
            const auto current = bfs_queue.front();
            bfs_queue.pop();
            for (const auto& neighbor : adj[current])
            {
                if (!level.contains(neighbor))
                {
                    level[neighbor] = level[current] + 1;
                    bfs_queue.push(neighbor);
                }
            }
        }

        // Group by level
        std::unordered_map<int, std::vector<std::string>> levels;
        for (const auto& node : graph.nodes)
        {
            const int lvl = level.contains(node.id) ? level[node.id] : 999;
            levels[lvl].push_back(node.id);
        }

        // Position: y = level * level_spacing, x = evenly spaced within level
        for (const auto& [lvl, node_ids] : levels)
        {
            const double y_pos = static_cast<double>(lvl) * level_spacing;
            const double total_width =
                static_cast<double>(static_cast<int>(node_ids.size()) - 1) * node_spacing;
            const double start_x = -total_width / 2.0;

            for (size_t j = 0; j < node_ids.size(); ++j)
            {
                NodePosition pos;
                pos.node_id = node_ids[j];
                pos.pos_x = start_x + static_cast<double>(j) * node_spacing;
                pos.pos_y = y_pos;
                result.positions.push_back(pos);
            }
        }
        return result;
    }
};

// ============================================================================
// RandomLayout
// ============================================================================

/// Random placement within bounds.
class RandomLayout
{
public:
    [[nodiscard]] static auto
    apply(const GraphData& graph, double width, double height, unsigned int seed = 42)
        -> LayoutResult
    {
        LayoutResult result;
        std::mt19937 rng(seed);
        std::uniform_real_distribution<double> dist_x(-width / 2.0, width / 2.0);
        std::uniform_real_distribution<double> dist_y(-height / 2.0, height / 2.0);

        for (const auto& node : graph.nodes)
        {
            NodePosition pos;
            pos.node_id = node.id;
            pos.pos_x = dist_x(rng);
            pos.pos_y = dist_y(rng);
            result.positions.push_back(pos);
        }
        return result;
    }
};

// ============================================================================
// Layout Factory
// ============================================================================

/// Apply a layout algorithm to graph data using a params struct.
[[nodiscard]] inline auto apply_layout(const GraphData& graph,
                                       LayoutAlgorithm algorithm,
                                       const LayoutParams& params) -> LayoutResult
{
    switch (algorithm)
    {
        case LayoutAlgorithm::kCircular:
            return CircularLayout::apply(graph, params.center_x, params.center_y, params.radius);
        case LayoutAlgorithm::kGrid:
            return GridLayout::apply(graph, params.columns, params.spacing);
        case LayoutAlgorithm::kRadial:
            return RadialLayout::apply(graph, params.center_id, params.radius);
        case LayoutAlgorithm::kHierarchical:
            return HierarchicalLayout::apply(
                graph, params.center_id, params.level_spacing, params.node_spacing);
        case LayoutAlgorithm::kRandom:
            return RandomLayout::apply(graph, params.width, params.height, params.seed);
        case LayoutAlgorithm::kForceDirected:
            // Handled by GraphEngine; return empty positions
            return {};
    }
    return {};
}

} // namespace markamp::core
