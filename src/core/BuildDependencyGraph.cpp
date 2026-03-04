/// @file BuildDependencyGraph.cpp
/// @brief Phase 38 Task 26 — Build dependency graph implementation.

#include "core/BuildDependencyGraph.h"

#include <algorithm>
#include <queue>
#include <regex>
#include <sstream>
#include <stack>

namespace markamp::core
{

void BuildDependencyGraph::parse_dot(const std::string& dot_content)
{
    std::istringstream stream(dot_content);
    std::string line;

    // Match edge lines: "node_a" -> "node_b" or node_a -> node_b
    std::regex edge_re("\\s*\"?([^\"]+?)\"?\\s*->\\s*\"?([^\"]+?)\"?\\s*(?:\\[.*\\])?\\s*;?\\s*");

    while (std::getline(stream, line))
    {
        std::smatch match;
        if (std::regex_match(line, match, edge_re))
        {
            auto from = match[1].str();
            auto target_node = match[2].str();
            add_node(from);
            add_node(target_node);
            add_edge(from, target_node);
        }
    }
}

void BuildDependencyGraph::add_node(const std::string& name, const std::string& type)
{
    if (nodes_.find(name) == nodes_.end())
    {
        nodes_[name] = BuildGraphNode{name, type, {}};
    }
}

void BuildDependencyGraph::add_edge(const std::string& from, const std::string& to)
{
    add_node(from);
    add_node(to);

    auto& deps = nodes_[from].depends_on;
    if (std::find(deps.begin(), deps.end(), to) == deps.end())
    {
        deps.push_back(to);
        ++edge_count_;
    }
}

auto BuildDependencyGraph::nodes() const -> std::vector<BuildGraphNode>
{
    std::vector<BuildGraphNode> result;
    result.reserve(nodes_.size());
    for (const auto& [unused_key, node] : nodes_)
    {
        result.push_back(node);
    }
    return result;
}

auto BuildDependencyGraph::dependencies_of(const std::string& name) const
    -> std::vector<std::string>
{
    auto it = nodes_.find(name);
    if (it != nodes_.end())
        return it->second.depends_on;
    return {};
}

auto BuildDependencyGraph::transitive_dependencies(const std::string& name) const
    -> std::vector<std::string>
{
    std::vector<std::string> result;
    std::unordered_set<std::string> visited;
    std::queue<std::string> queue;

    auto it = nodes_.find(name);
    if (it == nodes_.end())
        return result;

    for (const auto& dep : it->second.depends_on)
        queue.push(dep);

    while (!queue.empty())
    {
        auto current = queue.front();
        queue.pop();
        if (visited.count(current) > 0)
            continue;
        visited.insert(current);
        result.push_back(current);

        auto node_it = nodes_.find(current);
        if (node_it != nodes_.end())
        {
            for (const auto& dep : node_it->second.depends_on)
            {
                if (visited.count(dep) == 0)
                    queue.push(dep);
            }
        }
    }

    return result;
}

auto BuildDependencyGraph::dependents_of(const std::string& name) const -> std::vector<std::string>
{
    std::vector<std::string> result;
    for (const auto& [node_name, node] : nodes_)
    {
        for (const auto& dep : node.depends_on)
        {
            if (dep == name)
            {
                result.push_back(node_name);
                break;
            }
        }
    }
    return result;
}

auto BuildDependencyGraph::topological_sort() const -> std::vector<std::string>
{
    // Kahn's algorithm
    std::unordered_map<std::string, int> in_degree;
    for (const auto& [name, unused_node] : nodes_)
        in_degree[name] = 0;

    for (const auto& [unused_name, node] : nodes_)
    {
        for (const auto& dep : node.depends_on)
            ++in_degree[dep];
    }

    std::queue<std::string> queue;
    for (const auto& [name, degree] : in_degree)
    {
        if (degree == 0)
            queue.push(name);
    }

    std::vector<std::string> result;
    while (!queue.empty())
    {
        auto current = queue.front();
        queue.pop();
        result.push_back(current);

        auto it = nodes_.find(current);
        if (it != nodes_.end())
        {
            for (const auto& dep : it->second.depends_on)
            {
                if (--in_degree[dep] == 0)
                    queue.push(dep);
            }
        }
    }

    return result;
}

auto BuildDependencyGraph::node_count() const -> std::size_t
{
    return nodes_.size();
}

auto BuildDependencyGraph::edge_count() const -> std::size_t
{
    return edge_count_;
}

auto BuildDependencyGraph::has_cycles() const -> bool
{
    auto sorted = topological_sort();
    return sorted.size() != nodes_.size();
}

void BuildDependencyGraph::clear()
{
    nodes_.clear();
    edge_count_ = 0;
}

} // namespace markamp::core
