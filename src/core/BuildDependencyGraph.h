#pragma once

/// @file BuildDependencyGraph.h
/// @brief Phase 38 Task 26 — Build dependency graph model from CMake/Ninja DOT output.

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

/// A node in the build dependency graph.
struct BuildGraphNode
{
    std::string name;
    std::string type;                    ///< "target", "source", "header", "library"
    std::vector<std::string> depends_on; ///< Direct dependencies
};

/// Represents a build dependency graph.
class BuildDependencyGraph
{
public:
    BuildDependencyGraph() = default;

    /// Parse a DOT (graphviz) file.
    void parse_dot(const std::string& dot_content);

    /// Add a node to the graph.
    void add_node(const std::string& name, const std::string& type = "target");

    /// Add a dependency edge: from depends on to.
    void add_edge(const std::string& from, const std::string& to);

    /// Get all nodes.
    [[nodiscard]] auto nodes() const -> std::vector<BuildGraphNode>;

    /// Get direct dependencies of a node.
    [[nodiscard]] auto dependencies_of(const std::string& name) const -> std::vector<std::string>;

    /// Get all transitive dependencies of a node.
    [[nodiscard]] auto transitive_dependencies(const std::string& name) const
        -> std::vector<std::string>;

    /// Get reverse dependencies (who depends on this node).
    [[nodiscard]] auto dependents_of(const std::string& name) const -> std::vector<std::string>;

    /// Perform topological sort.
    [[nodiscard]] auto topological_sort() const -> std::vector<std::string>;

    /// Total node count.
    [[nodiscard]] auto node_count() const -> std::size_t;

    /// Total edge count.
    [[nodiscard]] auto edge_count() const -> std::size_t;

    /// Check if graph has cycles.
    [[nodiscard]] auto has_cycles() const -> bool;

    /// Clear the graph.
    void clear();

private:
    std::unordered_map<std::string, BuildGraphNode> nodes_;
    std::size_t edge_count_{0};
};

} // namespace markamp::core
