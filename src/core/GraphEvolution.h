/// @file GraphEvolution.h
/// @brief V9 Phase 16 – Graph snapshot, evolution tracking, and temporal comparison.

#pragma once

#include "GraphAnalytics.h"
#include "GraphTypes.h"

#include <chrono>
#include <deque>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Snapshot & Diff Types
// ============================================================================

/// A point-in-time snapshot of graph structure.
struct GraphSnapshot
{
    std::chrono::system_clock::time_point timestamp;
    int node_count{0};
    int edge_count{0};
    GraphStatistics statistics;
    std::set<std::string> node_ids;
    std::vector<std::pair<std::string, std::string>> edge_pairs;
};

/// Difference between two graph snapshots.
struct GraphDiff
{
    std::vector<std::string> nodes_added;
    std::vector<std::string> nodes_removed;
    std::vector<std::pair<std::string, std::string>> edges_added;
    std::vector<std::pair<std::string, std::string>> edges_removed;
    int node_count_delta{0};
    int edge_count_delta{0};
    double density_delta{0.0};
};

/// Comparison result with a human-readable summary.
struct GraphComparisonResult
{
    GraphDiff diff;
    double growth_rate{0.0}; ///< Node growth rate (newer - older) / older
    std::string summary;     ///< Human-readable comparison text
};

// ============================================================================
// GraphEvolutionTracker
// ============================================================================

/// Tracks graph evolution over time via snapshots.
class GraphEvolutionTracker
{
public:
    static constexpr int kMaxSnapshots = 50;

    // ── Snapshot Management ──

    /// Take a snapshot of the current graph state.
    auto take_snapshot(const GraphData& graph) -> GraphSnapshot
    {
        GraphSnapshot snapshot;
        snapshot.timestamp = std::chrono::system_clock::now();
        snapshot.node_count = static_cast<int>(graph.nodes.size());
        snapshot.edge_count = static_cast<int>(graph.links.size());
        snapshot.statistics = GraphAnalytics::compute_statistics(graph);

        for (const auto& node : graph.nodes)
        {
            snapshot.node_ids.insert(node.id);
        }
        for (const auto& link : graph.links)
        {
            snapshot.edge_pairs.emplace_back(link.source_id, link.target_id);
        }

        snapshots_.push_back(snapshot);

        // Enforce capacity
        while (static_cast<int>(snapshots_.size()) > kMaxSnapshots)
        {
            snapshots_.pop_front();
        }

        return snapshot;
    }

    /// Get all stored snapshots.
    [[nodiscard]] auto get_snapshots() const -> std::vector<GraphSnapshot>
    {
        return {snapshots_.begin(), snapshots_.end()};
    }

    /// Get the number of stored snapshots.
    [[nodiscard]] auto get_snapshot_count() const -> int
    {
        return static_cast<int>(snapshots_.size());
    }

    /// Get the most recent snapshot (or nullopt if none).
    [[nodiscard]] auto latest_snapshot() const -> const GraphSnapshot*
    {
        if (snapshots_.empty())
        {
            return nullptr;
        }
        return &snapshots_.back();
    }

    /// Clear all snapshots.
    void clear()
    {
        snapshots_.clear();
    }

    // ── Comparison ──

    /// Compare two snapshots and produce a diff.
    [[nodiscard]] static auto compare_snapshots(const GraphSnapshot& older,
                                                const GraphSnapshot& newer) -> GraphDiff
    {
        GraphDiff diff;

        // Nodes added
        for (const auto& id : newer.node_ids)
        {
            if (!older.node_ids.contains(id))
            {
                diff.nodes_added.push_back(id);
            }
        }

        // Nodes removed
        for (const auto& id : older.node_ids)
        {
            if (!newer.node_ids.contains(id))
            {
                diff.nodes_removed.push_back(id);
            }
        }

        // Edges: use set for O(1) lookup
        std::set<std::pair<std::string, std::string>> old_edges(older.edge_pairs.begin(),
                                                                older.edge_pairs.end());
        std::set<std::pair<std::string, std::string>> new_edges(newer.edge_pairs.begin(),
                                                                newer.edge_pairs.end());

        for (const auto& edge : new_edges)
        {
            if (!old_edges.contains(edge))
            {
                diff.edges_added.push_back(edge);
            }
        }
        for (const auto& edge : old_edges)
        {
            if (!new_edges.contains(edge))
            {
                diff.edges_removed.push_back(edge);
            }
        }

        diff.node_count_delta = newer.node_count - older.node_count;
        diff.edge_count_delta = newer.edge_count - older.edge_count;
        diff.density_delta = newer.statistics.density - older.statistics.density;

        return diff;
    }

    /// Generate a full comparison result with summary text.
    [[nodiscard]] static auto generate_comparison_summary(const GraphSnapshot& older,
                                                          const GraphSnapshot& newer)
        -> GraphComparisonResult
    {
        GraphComparisonResult result;
        result.diff = compare_snapshots(older, newer);

        // Growth rate
        result.growth_rate = (older.node_count > 0)
                                 ? static_cast<double>(result.diff.node_count_delta) /
                                       static_cast<double>(older.node_count)
                                 : 0.0;

        // Build summary
        std::ostringstream oss;
        oss << "Added " << result.diff.nodes_added.size() << " nodes, removed "
            << result.diff.nodes_removed.size() << ". ";

        if (result.diff.density_delta > 0)
        {
            oss << "Density increased by " << format_percent(result.diff.density_delta) << "%. ";
        }
        else if (result.diff.density_delta < 0)
        {
            oss << "Density decreased by " << format_percent(-result.diff.density_delta) << "%. ";
        }

        if (!result.diff.edges_added.empty())
        {
            oss << "New connections: ";
            const int limit = std::min(3, static_cast<int>(result.diff.edges_added.size()));
            for (int i = 0; i < limit; ++i)
            {
                if (i > 0)
                {
                    oss << ", ";
                }
                oss << result.diff.edges_added[static_cast<size_t>(i)].first << " → "
                    << result.diff.edges_added[static_cast<size_t>(i)].second;
            }
            if (static_cast<int>(result.diff.edges_added.size()) > limit)
            {
                oss << " (+" << (result.diff.edges_added.size() - static_cast<size_t>(limit))
                    << " more)";
            }
            oss << ".";
        }

        result.summary = oss.str();
        return result;
    }

    // ── Persistence ──

    /// Serialize a snapshot to a string.
    [[nodiscard]] static auto serialize_snapshot(const GraphSnapshot& snapshot) -> std::string
    {
        std::ostringstream oss;
        const auto ts = std::chrono::system_clock::to_time_t(snapshot.timestamp);
        oss << "SNAPSHOT\n"
            << "timestamp=" << ts << "\n"
            << "node_count=" << snapshot.node_count << "\n"
            << "edge_count=" << snapshot.edge_count << "\n"
            << "density=" << snapshot.statistics.density << "\n"
            << "connected_components=" << snapshot.statistics.connected_components << "\n";

        oss << "nodes=";
        bool first = true;
        for (const auto& id : snapshot.node_ids)
        {
            if (!first)
            {
                oss << ",";
            }
            oss << id;
            first = false;
        }
        oss << "\n";

        oss << "edges=";
        first = true;
        for (const auto& [src, tgt] : snapshot.edge_pairs)
        {
            if (!first)
            {
                oss << ",";
            }
            oss << src << "->" << tgt;
            first = false;
        }
        oss << "\n---\n";

        return oss.str();
    }

    /// Deserialize a snapshot from a string.
    [[nodiscard]] static auto deserialize_snapshot(const std::string& data) -> GraphSnapshot
    {
        GraphSnapshot snapshot;
        std::istringstream iss(data);
        std::string line;

        while (std::getline(iss, line))
        {
            if (line == "SNAPSHOT" || line == "---")
            {
                continue;
            }

            const auto eq_pos = line.find('=');
            if (eq_pos == std::string::npos)
            {
                continue;
            }
            const auto key = line.substr(0, eq_pos);
            const auto value = line.substr(eq_pos + 1);

            if (key == "timestamp")
            {
                snapshot.timestamp = std::chrono::system_clock::from_time_t(std::stol(value));
            }
            else if (key == "node_count")
            {
                snapshot.node_count = std::stoi(value);
            }
            else if (key == "edge_count")
            {
                snapshot.edge_count = std::stoi(value);
            }
            else if (key == "density")
            {
                snapshot.statistics.density = std::stod(value);
            }
            else if (key == "connected_components")
            {
                snapshot.statistics.connected_components = std::stoi(value);
            }
            else if (key == "nodes" && !value.empty())
            {
                parse_csv_list(value, snapshot.node_ids);
            }
            else if (key == "edges" && !value.empty())
            {
                parse_edge_list(value, snapshot.edge_pairs);
            }
        }

        return snapshot;
    }

private:
    std::deque<GraphSnapshot> snapshots_;

    [[nodiscard]] static auto format_percent(double value) -> std::string
    {
        std::ostringstream oss;
        oss.precision(1);
        oss << std::fixed << (value * 100.0);
        return oss.str();
    }

    static void parse_csv_list(const std::string& csv, std::set<std::string>& output)
    {
        std::istringstream iss(csv);
        std::string token;
        while (std::getline(iss, token, ','))
        {
            if (!token.empty())
            {
                output.insert(token);
            }
        }
    }

    static void parse_edge_list(const std::string& edges,
                                std::vector<std::pair<std::string, std::string>>& output)
    {
        std::istringstream iss(edges);
        std::string token;
        while (std::getline(iss, token, ','))
        {
            const auto arrow = token.find("->");
            if (arrow != std::string::npos)
            {
                output.emplace_back(token.substr(0, arrow), token.substr(arrow + 2));
            }
        }
    }
};

} // namespace markamp::core
