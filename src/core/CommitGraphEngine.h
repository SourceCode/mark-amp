/// @file CommitGraphEngine.h
/// @brief Phase 25: Version Control Integration — Commit graph visualization.
/// Builds a DAG from GitService commit history, assigns columns using lane
/// allocation, and supports filtering, searching, and ASCII rendering.

#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

class GitService;

// ============================================================================
// Data structures
// ============================================================================

/// A node in the commit graph representing a single commit.
struct GraphNode
{
    std::string commit_hash;
    std::string short_hash;
    std::string message;
    std::string author;
    std::chrono::system_clock::time_point timestamp;
    int column{0};                          ///< Horizontal lane in the graph.
    std::vector<std::string> parent_hashes; ///< Parent commit hashes.
    std::vector<std::string> child_hashes;  ///< Child commit hashes (populated during layout).
    bool is_merge{false};                   ///< True if more than one parent.
    bool is_branch_head{false};             ///< True if this is a branch tip.
};

/// An edge connecting two graph nodes.
struct GraphEdge
{
    std::string from_hash;
    std::string to_hash;
    int from_column{0};
    int to_column{0};

    enum class EdgeType : uint8_t
    {
        kNormal,
        kMerge,
        kBranch
    };
    EdgeType edge_type{EdgeType::kNormal};
};

/// The complete commit graph.
struct CommitGraph
{
    std::vector<GraphNode> nodes;
    std::vector<GraphEdge> edges;
    int max_columns{0}; ///< Maximum lane width.
    int total_commits{0};
};

// ============================================================================
// CommitGraphEngine
// ============================================================================

/// Builds and lays out a commit DAG from GitService history.
class CommitGraphEngine
{
public:
    explicit CommitGraphEngine(GitService& git_service);

    /// Build a graph from the full commit log.
    [[nodiscard]] auto build_graph() -> CommitGraph;

    /// Build a graph limited to N most recent commits.
    [[nodiscard]] auto build_graph(int max_commits) -> CommitGraph;

    /// Layout the graph by assigning column positions.
    auto layout_graph(CommitGraph& graph) -> void;

    /// Filter graph to only show commits by a specific author.
    [[nodiscard]] auto filter_by_author(const CommitGraph& graph, const std::string& author) const
        -> CommitGraph;

    /// Filter graph to only show commits touching a specific path.
    [[nodiscard]] auto filter_by_path(const CommitGraph& graph, const std::string& path) const
        -> CommitGraph;

    /// Search commits by message substring (case-insensitive).
    [[nodiscard]] auto search_commits(const CommitGraph& graph, const std::string& query) const
        -> CommitGraph;

    /// Render the graph as ASCII art.
    [[nodiscard]] auto render_ascii(const CommitGraph& graph) const -> std::string;

private:
    GitService& git_service_;

    /// Assign lanes to nodes using a simple algorithm.
    auto assign_lanes(CommitGraph& graph) -> void;
};

} // namespace markamp::core
