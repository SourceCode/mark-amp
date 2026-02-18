/// @file CommitGraphEngine.cpp
/// @brief Phase 25: Version Control Integration — Commit graph implementation.

#include "core/CommitGraphEngine.h"

#include "core/GitService.h"

#include <algorithm>
#include <format>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace markamp::core
{

CommitGraphEngine::CommitGraphEngine(GitService& git_service)
    : git_service_(git_service)
{
}

auto CommitGraphEngine::build_graph() -> CommitGraph
{
    return build_graph(0); // unlimited
}

auto CommitGraphEngine::build_graph(int max_commits) -> CommitGraph
{
    CommitGraph graph;

    auto commits = git_service_.log();

    if (max_commits > 0 && static_cast<int>(commits.size()) > max_commits)
    {
        commits.resize(static_cast<size_t>(max_commits));
    }

    // Build graph nodes from commits.
    // GitCommit has: hash, short_hash, author, email, message, timestamp, changed_files
    // Since GitCommit doesn't carry parent_hashes, we infer a linear chain:
    // each commit's parent is the next one in chronological order (log is newest-first).

    for (size_t idx = 0; idx < commits.size(); ++idx)
    {
        const auto& commit = commits[idx];

        GraphNode node;
        node.commit_hash = commit.hash;
        node.short_hash = commit.short_hash;
        node.message = commit.message;
        node.author = commit.author;
        node.timestamp = commit.timestamp;
        node.is_merge = false;

        // Link to parent (the next commit in the log is the parent).
        if (idx + 1 < commits.size())
        {
            node.parent_hashes.push_back(commits[idx + 1].hash);
        }

        graph.nodes.push_back(std::move(node));
    }

    // Populate child hashes from parent relationships.
    std::unordered_map<std::string, std::vector<std::string>> children_map;
    for (const auto& node : graph.nodes)
    {
        for (const auto& parent_hash : node.parent_hashes)
        {
            children_map[parent_hash].push_back(node.commit_hash);
        }
    }
    for (auto& node : graph.nodes)
    {
        auto children_iter = children_map.find(node.commit_hash);
        if (children_iter != children_map.end())
        {
            node.child_hashes = children_iter->second;
        }
    }

    // Mark branch heads: the first commit from each branch is a branch head.
    // Since GitBranch doesn't carry head_hash, we mark the first node if
    // the branch is current.
    auto branches = git_service_.branches();
    for (const auto& branch : branches)
    {
        if (branch.is_current && !graph.nodes.empty())
        {
            graph.nodes.front().is_branch_head = true;
        }
    }

    graph.total_commits = static_cast<int>(graph.nodes.size());

    // Layout.
    layout_graph(graph);

    return graph;
}

auto CommitGraphEngine::layout_graph(CommitGraph& graph) -> void
{
    assign_lanes(graph);

    // Build edges from parent relationships.
    graph.edges.clear();
    for (const auto& node : graph.nodes)
    {
        for (const auto& parent_hash : node.parent_hashes)
        {
            GraphEdge edge;
            edge.from_hash = node.commit_hash;
            edge.to_hash = parent_hash;
            edge.from_column = node.column;

            // Find parent's column.
            auto parent_iter = std::ranges::find_if(graph.nodes,
                                                    [&](const GraphNode& gnode)
                                                    { return gnode.commit_hash == parent_hash; });

            if (parent_iter != graph.nodes.end())
            {
                edge.to_column = parent_iter->column;
            }

            // Determine edge type.
            if (node.is_merge && &parent_hash != &node.parent_hashes.front())
            {
                edge.edge_type = GraphEdge::EdgeType::kMerge;
            }
            else if (edge.from_column != edge.to_column)
            {
                edge.edge_type = GraphEdge::EdgeType::kBranch;
            }

            graph.edges.push_back(std::move(edge));
        }
    }
}

auto CommitGraphEngine::filter_by_author(const CommitGraph& graph, const std::string& author) const
    -> CommitGraph
{
    CommitGraph result;
    for (const auto& node : graph.nodes)
    {
        if (node.author == author)
        {
            result.nodes.push_back(node);
        }
    }
    result.total_commits = static_cast<int>(result.nodes.size());
    result.max_columns = graph.max_columns;
    return result;
}

auto CommitGraphEngine::filter_by_path(const CommitGraph& graph, const std::string& path) const
    -> CommitGraph
{
    // In the stubbed implementation, we check if any commit message
    // references the path.  A real implementation would walk diff trees.
    CommitGraph result;
    for (const auto& node : graph.nodes)
    {
        if (node.message.find(path) != std::string::npos)
        {
            result.nodes.push_back(node);
        }
    }
    result.total_commits = static_cast<int>(result.nodes.size());
    result.max_columns = graph.max_columns;
    return result;
}

auto CommitGraphEngine::search_commits(const CommitGraph& graph, const std::string& query) const
    -> CommitGraph
{
    // Case-insensitive substring search on message.
    std::string lower_query = query;
    std::ranges::transform(lower_query,
                           lower_query.begin(),
                           [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

    CommitGraph result;
    for (const auto& node : graph.nodes)
    {
        std::string lower_msg = node.message;
        std::ranges::transform(lower_msg,
                               lower_msg.begin(),
                               [](unsigned char chr)
                               { return static_cast<char>(std::tolower(chr)); });

        if (lower_msg.find(lower_query) != std::string::npos)
        {
            result.nodes.push_back(node);
        }
    }
    result.total_commits = static_cast<int>(result.nodes.size());
    result.max_columns = graph.max_columns;
    return result;
}

auto CommitGraphEngine::render_ascii(const CommitGraph& graph) const -> std::string
{
    std::ostringstream output;

    for (const auto& node : graph.nodes)
    {
        // Build lane prefix.
        for (int col = 0; col < graph.max_columns; ++col)
        {
            if (col == node.column)
            {
                output << "* ";
            }
            else if (col < node.column)
            {
                output << "| ";
            }
        }

        // Commit info.
        output << node.short_hash << " " << node.message;

        if (node.is_merge)
        {
            output << " (merge)";
        }
        if (node.is_branch_head)
        {
            output << " [HEAD]";
        }

        output << "\n";
    }

    return output.str();
}

// ── Private helpers ──

auto CommitGraphEngine::assign_lanes(CommitGraph& graph) -> void
{
    // Simple lane allocation: each node gets lane 0 unless it's
    // part of a merge, in which case the secondary parent's children
    // get offset lanes.
    int max_col = 0;

    // Track active lanes: hash -> column.
    std::unordered_map<std::string, int> lane_map;
    int next_lane = 0;

    for (auto& node : graph.nodes)
    {
        // First time seeing this commit? Assign a lane.
        auto lane_iter = lane_map.find(node.commit_hash);
        if (lane_iter != lane_map.end())
        {
            node.column = lane_iter->second;
        }
        else
        {
            node.column = next_lane++;
        }

        max_col = std::max(max_col, node.column);

        // Assign parent lanes.
        for (size_t idx = 0; idx < node.parent_hashes.size(); ++idx)
        {
            const auto& parent = node.parent_hashes[idx];
            if (!lane_map.contains(parent))
            {
                if (idx == 0)
                {
                    lane_map[parent] = node.column; // First parent inherits lane.
                }
                else
                {
                    lane_map[parent] = next_lane++; // Merge parents get new lanes.
                    max_col = std::max(max_col, lane_map[parent]);
                }
            }
        }
    }

    graph.max_columns = max_col + 1;
}

} // namespace markamp::core
