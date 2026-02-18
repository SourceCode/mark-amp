/// @file CellDependencyTracker.cpp
/// @brief V8 Phase 15 – Cell dependency analysis implementation.

#include "core/CellDependencyTracker.h"

#include "core/Events.h"

#include <algorithm>
#include <queue>
#include <regex>
#include <stack>

namespace markamp::core
{

// ============================================================================
// Constructor
// ============================================================================

CellDependencyTracker::CellDependencyTracker(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ============================================================================
// Cell analysis
// ============================================================================

auto CellDependencyTracker::analyze_cell(const std::string& cell_id, const std::string& source)
    -> void
{
    CellDependency dep;
    dep.cell_id = cell_id;
    dep.defines = extract_defines(source);
    dep.references = extract_references(source);

    // Remove self-references (a variable defined in the same cell is not a dependency).
    for (const auto& defined : dep.defines)
    {
        dep.references.erase(defined);
    }

    deps_[cell_id] = std::move(dep);
    graph_dirty_ = true;

    // Publish event.
    events::CellDependencyChangedEvent evt;
    evt.cell_id = cell_id;
    evt.stale_count = 0; // Will be computed when graph is rebuilt.
    event_bus_.publish(evt);
}

auto CellDependencyTracker::remove_cell(const std::string& cell_id) -> void
{
    deps_.erase(cell_id);
    dependents_.erase(cell_id);
    graph_dirty_ = true;
}

// ============================================================================
// Graph construction
// ============================================================================

auto CellDependencyTracker::build_dependency_graph() -> void
{
    // Clear existing graph.
    dependents_.clear();
    var_to_cell_.clear();

    // Build variable -> cell mapping.
    for (const auto& [cell_id, dep] : deps_)
    {
        for (const auto& var : dep.defines)
        {
            var_to_cell_[var] = cell_id;
        }
    }

    // Build dependency edges.
    for (const auto& [cell_id, dep] : deps_)
    {
        for (const auto& ref : dep.references)
        {
            auto iter = var_to_cell_.find(ref);
            if (iter != var_to_cell_.end() && iter->second != cell_id)
            {
                // cell_id depends on iter->second.
                dependents_[iter->second].insert(cell_id);
            }
        }
    }

    // Rebuild topological order.
    topo_order_ = topological_sort();
    graph_dirty_ = false;
}

// ============================================================================
// Query methods
// ============================================================================

auto CellDependencyTracker::stale_cells(const std::string& modified_cell_id) const
    -> std::vector<std::string>
{
    std::vector<std::string> result;
    std::unordered_set<std::string> visited;
    std::queue<std::string> work_queue;

    // Start BFS from the modified cell's dependents.
    auto iter = dependents_.find(modified_cell_id);
    if (iter != dependents_.end())
    {
        for (const auto& dep : iter->second)
        {
            work_queue.push(dep);
        }
    }

    while (!work_queue.empty())
    {
        auto current = work_queue.front();
        work_queue.pop();

        if (visited.contains(current))
        {
            continue;
        }
        visited.insert(current);
        result.push_back(current);

        auto dep_iter = dependents_.find(current);
        if (dep_iter != dependents_.end())
        {
            for (const auto& next : dep_iter->second)
            {
                if (!visited.contains(next))
                {
                    work_queue.push(next);
                }
            }
        }
    }

    return result;
}

auto CellDependencyTracker::execution_order() const -> std::vector<std::string>
{
    if (graph_dirty_)
    {
        return topological_sort();
    }
    return topo_order_;
}

auto CellDependencyTracker::has_circular_dependency() const -> bool
{
    return detect_cycle();
}

auto CellDependencyTracker::defined_in(const std::string& variable) const
    -> std::optional<std::string>
{
    auto iter = var_to_cell_.find(variable);
    if (iter != var_to_cell_.end())
    {
        return iter->second;
    }
    return std::nullopt;
}

auto CellDependencyTracker::all_defined_variables() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    for (const auto& [var, cell_id] : var_to_cell_)
    {
        result.push_back(var);
    }
    std::sort(result.begin(), result.end());
    return result;
}

auto CellDependencyTracker::get_cell_dependency(const std::string& cell_id) const
    -> std::optional<CellDependency>
{
    auto iter = deps_.find(cell_id);
    if (iter != deps_.end())
    {
        return iter->second;
    }
    return std::nullopt;
}

auto CellDependencyTracker::tracked_cell_count() const -> int
{
    return static_cast<int>(deps_.size());
}

auto CellDependencyTracker::clear() -> void
{
    deps_.clear();
    dependents_.clear();
    var_to_cell_.clear();
    topo_order_.clear();
    graph_dirty_ = true;
}

// ============================================================================
// Static extraction helpers
// ============================================================================

auto CellDependencyTracker::extract_defines(const std::string& source)
    -> std::unordered_set<std::string>
{
    std::unordered_set<std::string> defines;

    // Match simple Python-style assignment: identifier = ...
    // Patterns: "x = ...", "x, y = ...", "x += ...", etc.
    const std::regex assign_regex(R"(^([a-zA-Z_][a-zA-Z0-9_]*)\s*[+\-*/%&|^]?=(?!=))",
                                  std::regex::multiline);

    auto begin = std::sregex_iterator(source.begin(), source.end(), assign_regex);
    auto end = std::sregex_iterator();

    for (auto iter = begin; iter != end; ++iter)
    {
        defines.insert((*iter)[1].str());
    }

    // Also match "def func_name(" and "class ClassName:" as definitions.
    const std::regex def_regex(R"(^(?:def|class)\s+([a-zA-Z_][a-zA-Z0-9_]*))",
                               std::regex::multiline);

    begin = std::sregex_iterator(source.begin(), source.end(), def_regex);
    for (auto iter = begin; iter != end; ++iter)
    {
        defines.insert((*iter)[1].str());
    }

    return defines;
}

auto CellDependencyTracker::extract_references(const std::string& source)
    -> std::unordered_set<std::string>
{
    std::unordered_set<std::string> references;

    // Match all identifiers, then filter out keywords.
    const std::regex ident_regex(R"([a-zA-Z_][a-zA-Z0-9_]*)");
    static const std::unordered_set<std::string> kKeywords = {
        "and",    "as",    "assert", "async",  "await",  "break",   "class",    "continue",
        "def",    "del",   "elif",   "else",   "except", "finally", "for",      "from",
        "global", "if",    "import", "in",     "is",     "lambda",  "nonlocal", "not",
        "or",     "pass",  "raise",  "return", "try",    "while",   "with",     "yield",
        "True",   "False", "None",   "print",  "len",    "range",   "int",      "str",
        "float",  "list",  "dict",   "set",    "tuple",  "type",    "self"};

    auto begin = std::sregex_iterator(source.begin(), source.end(), ident_regex);
    auto end_iter = std::sregex_iterator();

    for (auto iter = begin; iter != end_iter; ++iter)
    {
        const std::string ident = iter->str();
        if (!kKeywords.contains(ident))
        {
            references.insert(ident);
        }
    }

    return references;
}

// ============================================================================
// Topological sort (Kahn's algorithm)
// ============================================================================

auto CellDependencyTracker::topological_sort() const -> std::vector<std::string>
{
    // Compute in-degrees.
    std::unordered_map<std::string, int> in_degree;
    for (const auto& [cell_id, dep] : deps_)
    {
        if (!in_degree.contains(cell_id))
        {
            in_degree[cell_id] = 0;
        }
    }

    for (const auto& [source, targets] : dependents_)
    {
        for (const auto& target : targets)
        {
            in_degree[target]++;
        }
    }

    // Start with nodes that have no incoming edges.
    std::queue<std::string> ready;
    for (const auto& [cell_id, degree] : in_degree)
    {
        if (degree == 0)
        {
            ready.push(cell_id);
        }
    }

    std::vector<std::string> order;
    while (!ready.empty())
    {
        auto current = ready.front();
        ready.pop();
        order.push_back(current);

        auto iter = dependents_.find(current);
        if (iter != dependents_.end())
        {
            for (const auto& next : iter->second)
            {
                in_degree[next]--;
                if (in_degree[next] == 0)
                {
                    ready.push(next);
                }
            }
        }
    }

    return order;
}

// ============================================================================
// Cycle detection (DFS)
// ============================================================================

auto CellDependencyTracker::detect_cycle() const -> bool
{
    enum class State
    {
        kWhite,
        kGray,
        kBlack
    };
    std::unordered_map<std::string, State> color;

    for (const auto& [cell_id, dep] : deps_)
    {
        color[cell_id] = State::kWhite;
    }

    // DFS from each unvisited node.
    for (const auto& [cell_id, dep] : deps_)
    {
        if (color[cell_id] == State::kWhite)
        {
            std::stack<std::pair<std::string, bool>> dfs_stack;
            dfs_stack.emplace(cell_id, false);

            while (!dfs_stack.empty())
            {
                auto [node, processed] = dfs_stack.top();
                dfs_stack.pop();

                if (processed)
                {
                    color[node] = State::kBlack;
                    continue;
                }

                if (color[node] == State::kGray)
                {
                    return true; // Cycle detected.
                }

                color[node] = State::kGray;
                dfs_stack.emplace(node, true); // Post-processing marker.

                auto iter = dependents_.find(node);
                if (iter != dependents_.end())
                {
                    for (const auto& next : iter->second)
                    {
                        if (color[next] == State::kGray)
                        {
                            return true;
                        }
                        if (color[next] == State::kWhite)
                        {
                            dfs_stack.emplace(next, false);
                        }
                    }
                }
            }
        }
    }

    return false;
}

} // namespace markamp::core
