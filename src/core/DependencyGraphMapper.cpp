/// @file DependencyGraphMapper.cpp
/// @brief V24 P01-T03: Implementation of the dependency graph mapper.
#include "DependencyGraphMapper.h"

#include <algorithm>
#include <queue>
#include <sstream>

namespace markamp::core
{

void DependencyGraphMapper::add_node(const std::string& task_id)
{
    if (adjacency_.find(task_id) == adjacency_.end()) {
        adjacency_[task_id] = {};
    }
    if (reverse_adj_.find(task_id) == reverse_adj_.end()) {
        reverse_adj_[task_id] = {};
    }
}

auto DependencyGraphMapper::add_dependency(const std::string& from_task,
                                            const std::string& to_task) -> bool
{
    if (from_task.empty() || to_task.empty() || from_task == to_task) return false;
    if (would_create_cycle(from_task, to_task)) return false;

    add_node(from_task);
    add_node(to_task);

    adjacency_[from_task].push_back(to_task);
    reverse_adj_[to_task].push_back(from_task);
    ++edge_count_;
    return true;
}

auto DependencyGraphMapper::would_create_cycle(const std::string& from_task,
                                                const std::string& to_task) const -> bool
{
    // Adding from_task -> to_task means "from depends on to".
    // A cycle exists if there's already a path from from_task to to_task via reverse edges
    // (i.e., to_task already transitively depends on from_task).
    return has_path(to_task, from_task);
}

void DependencyGraphMapper::dfs_path(const std::string& node, const std::string& target,
                                      std::unordered_set<std::string>& visited,
                                      bool& found) const
{
    if (found) return;
    if (node == target) { found = true; return; }
    if (visited.count(node)) return;
    visited.insert(node);

    auto it = adjacency_.find(node);
    if (it == adjacency_.end()) return;

    for (const auto& neighbor : it->second) {
        dfs_path(neighbor, target, visited, found);
        if (found) return;
    }
}

auto DependencyGraphMapper::has_path(const std::string& from, const std::string& to) const -> bool
{
    std::unordered_set<std::string> visited;
    bool found = false;
    dfs_path(from, to, visited, found);
    return found;
}

auto DependencyGraphMapper::topological_sort() const -> std::vector<std::string>
{
    // Kahn's algorithm
    std::unordered_map<std::string, int> in_degree;
    for (const auto& [node, _] : adjacency_) {
        if (in_degree.find(node) == in_degree.end()) in_degree[node] = 0;
    }
    for (const auto& [node, deps] : adjacency_) {
        for (const auto& dep : deps) {
            ++in_degree[dep];
        }
    }

    std::queue<std::string> queue;
    for (const auto& [node, degree] : in_degree) {
        if (degree == 0) queue.push(node);
    }

    std::vector<std::string> result;
    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop();
        result.push_back(current);

        auto it = adjacency_.find(current);
        if (it != adjacency_.end()) {
            for (const auto& dep : it->second) {
                --in_degree[dep];
                if (in_degree[dep] == 0) queue.push(dep);
            }
        }
    }

    return result;
}

auto DependencyGraphMapper::critical_path() const -> std::vector<std::string>
{
    auto sorted = topological_sort();
    if (sorted.empty()) return {};

    // Longest path via DP on topological order
    std::unordered_map<std::string, int> dist;
    std::unordered_map<std::string, std::string> prev;
    for (const auto& node : sorted) dist[node] = 0;

    for (const auto& node : sorted) {
        auto it = adjacency_.find(node);
        if (it == adjacency_.end()) continue;
        for (const auto& dep : it->second) {
            if (dist[node] + 1 > dist[dep]) {
                dist[dep] = dist[node] + 1;
                prev[dep] = node;
            }
        }
    }

    // Find the node with max distance
    std::string end_node;
    int max_dist = -1;
    for (const auto& [node, d] : dist) {
        if (d > max_dist) {
            max_dist = d;
            end_node = node;
        }
    }

    // Trace back
    std::vector<std::string> path;
    std::string current = end_node;
    while (!current.empty()) {
        path.push_back(current);
        auto it = prev.find(current);
        current = it != prev.end() ? it->second : "";
    }
    std::reverse(path.begin(), path.end());
    return path;
}

auto DependencyGraphMapper::parallel_groups() const -> std::vector<std::vector<std::string>>
{
    // Group by topological depth (BFS layers)
    std::unordered_map<std::string, int> in_degree;
    for (const auto& [node, _] : adjacency_) {
        if (in_degree.find(node) == in_degree.end()) in_degree[node] = 0;
    }
    for (const auto& [node, deps] : adjacency_) {
        for (const auto& dep : deps) {
            ++in_degree[dep];
        }
    }

    std::queue<std::string> queue;
    for (const auto& [node, degree] : in_degree) {
        if (degree == 0) queue.push(node);
    }

    std::vector<std::vector<std::string>> groups;
    while (!queue.empty()) {
        auto layer_size = queue.size();
        std::vector<std::string> group;
        for (size_t i = 0; i < layer_size; ++i) {
            auto current = queue.front();
            queue.pop();
            group.push_back(current);

            auto it = adjacency_.find(current);
            if (it != adjacency_.end()) {
                for (const auto& dep : it->second) {
                    --in_degree[dep];
                    if (in_degree[dep] == 0) queue.push(dep);
                }
            }
        }
        if (!group.empty()) groups.push_back(std::move(group));
    }
    return groups;
}

auto DependencyGraphMapper::analyze() const -> GraphAnalysis
{
    GraphAnalysis result;
    result.total_nodes = node_count();
    result.total_edges = edge_count_;
    result.topological_order = topological_sort();
    result.has_cycles = static_cast<int>(result.topological_order.size()) != result.total_nodes;
    if (!result.has_cycles) {
        result.critical_path = critical_path();
        result.parallel_groups = parallel_groups();
    }
    return result;
}

auto DependencyGraphMapper::get_dependencies(const std::string& task_id) const
    -> std::vector<std::string>
{
    auto it = adjacency_.find(task_id);
    return it != adjacency_.end() ? it->second : std::vector<std::string>{};
}

auto DependencyGraphMapper::get_dependents(const std::string& task_id) const
    -> std::vector<std::string>
{
    auto it = reverse_adj_.find(task_id);
    return it != reverse_adj_.end() ? it->second : std::vector<std::string>{};
}

auto DependencyGraphMapper::export_mermaid() const -> std::string
{
    std::ostringstream ss;
    ss << "graph TD\n";
    for (const auto& [node, deps] : adjacency_) {
        if (deps.empty()) {
            ss << "    " << node << "\n";
        }
        for (const auto& dep : deps) {
            ss << "    " << node << " --> " << dep << "\n";
        }
    }
    return ss.str();
}

void DependencyGraphMapper::clear()
{
    adjacency_.clear();
    reverse_adj_.clear();
    edge_count_ = 0;
}

} // namespace markamp::core
