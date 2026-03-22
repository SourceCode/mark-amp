/// @file DependencyGraphMapper.h
/// @brief V24 P01-T03: Dependency graph for task ordering and critical-path analysis.
///
/// Topological sort of task dependencies, critical-path extraction,
/// parallelization group identification, and Mermaid diagram export.
#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

/// Dependency relationship between two tasks.
struct TaskDependency
{
    std::string from_task_id;  ///< The task that depends
    std::string to_task_id;    ///< The task that must be completed first

    [[nodiscard]] auto is_valid() const noexcept -> bool
    {
        return !from_task_id.empty() && !to_task_id.empty() && from_task_id != to_task_id;
    }
};

/// Graph analysis result.
struct GraphAnalysis
{
    std::vector<std::string> topological_order;     ///< Tasks in dependency order
    std::vector<std::string> critical_path;         ///< Longest dependency chain
    std::vector<std::vector<std::string>> parallel_groups; ///< Groups that can run in parallel
    int total_nodes{0};
    int total_edges{0};
    bool has_cycles{false};

    [[nodiscard]] auto is_valid() const noexcept -> bool
    {
        return !has_cycles && total_nodes > 0;
    }

    [[nodiscard]] auto max_parallelism() const noexcept -> int
    {
        int max_size = 0;
        for (const auto& group : parallel_groups) {
            int size = static_cast<int>(group.size());
            if (size > max_size) max_size = size;
        }
        return max_size;
    }

    [[nodiscard]] auto critical_path_length() const noexcept -> int
    {
        return static_cast<int>(critical_path.size());
    }
};

/// Directed acyclic graph of task dependencies with analysis capabilities.
class DependencyGraphMapper
{
public:
    DependencyGraphMapper() = default;

    /// Add a node (task) to the graph.
    void add_node(const std::string& task_id);

    /// Add a dependency edge. Returns false if it would create a cycle.
    [[nodiscard]] auto add_dependency(const std::string& from_task, const std::string& to_task)
        -> bool;

    /// Check if adding a dependency would create a cycle.
    [[nodiscard]] auto would_create_cycle(const std::string& from_task,
                                           const std::string& to_task) const -> bool;

    /// Get the topological sort of all tasks.
    [[nodiscard]] auto topological_sort() const -> std::vector<std::string>;

    /// Get the critical path (longest dependency chain).
    [[nodiscard]] auto critical_path() const -> std::vector<std::string>;

    /// Get groups of tasks that can be executed in parallel.
    [[nodiscard]] auto parallel_groups() const -> std::vector<std::vector<std::string>>;

    /// Full analysis.
    [[nodiscard]] auto analyze() const -> GraphAnalysis;

    /// Get direct dependencies of a task.
    [[nodiscard]] auto get_dependencies(const std::string& task_id) const
        -> std::vector<std::string>;

    /// Get direct dependents of a task (tasks that depend on it).
    [[nodiscard]] auto get_dependents(const std::string& task_id) const
        -> std::vector<std::string>;

    /// Node count.
    [[nodiscard]] auto node_count() const noexcept -> int
    {
        return static_cast<int>(adjacency_.size());
    }

    /// Edge count.
    [[nodiscard]] auto edge_count() const noexcept -> int { return edge_count_; }

    /// Export as Mermaid diagram.
    [[nodiscard]] auto export_mermaid() const -> std::string;

    /// Clear the graph.
    void clear();

private:
    std::unordered_map<std::string, std::vector<std::string>> adjacency_;   ///< from -> [to]
    std::unordered_map<std::string, std::vector<std::string>> reverse_adj_; ///< to -> [from]
    int edge_count_{0};

    [[nodiscard]] auto has_path(const std::string& from, const std::string& to) const -> bool;
    void dfs_path(const std::string& node, const std::string& target,
                  std::unordered_set<std::string>& visited, bool& found) const;
};

} // namespace markamp::core
