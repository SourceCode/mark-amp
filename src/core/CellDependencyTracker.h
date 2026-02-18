/// @file CellDependencyTracker.h
/// @brief V8 Phase 15 – Cell dependency analysis for reactive execution ordering.

#pragma once

#include "core/EventBus.h"

#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Cell dependency data
// ============================================================================

/// Variables defined and referenced by a single cell.
struct CellDependency
{
    std::string cell_id;
    std::unordered_set<std::string> defines;    ///< Variables assigned in this cell
    std::unordered_set<std::string> references; ///< Variables used in this cell
};

// ============================================================================
// CellDependencyTracker
// ============================================================================

class CellDependencyTracker
{
public:
    explicit CellDependencyTracker(EventBus& event_bus);

    /// Analyze a cell's source code and extract variable definitions and references.
    auto analyze_cell(const std::string& cell_id, const std::string& source) -> void;

    /// Remove a cell from the dependency graph.
    auto remove_cell(const std::string& cell_id) -> void;

    /// Build the full dependency graph from all analyzed cells.
    auto build_dependency_graph() -> void;

    /// Get all cells that depend (directly or transitively) on a modified cell.
    [[nodiscard]] auto stale_cells(const std::string& modified_cell_id) const
        -> std::vector<std::string>;

    /// Get a topologically sorted execution order for all analyzed cells.
    [[nodiscard]] auto execution_order() const -> std::vector<std::string>;

    /// Check if the dependency graph contains circular dependencies.
    [[nodiscard]] auto has_circular_dependency() const -> bool;

    /// Find the cell that defines a given variable.
    [[nodiscard]] auto defined_in(const std::string& variable) const -> std::optional<std::string>;

    /// Get all variables defined across all cells.
    [[nodiscard]] auto all_defined_variables() const -> std::vector<std::string>;

    /// Get the dependency info for a specific cell.
    [[nodiscard]] auto get_cell_dependency(const std::string& cell_id) const
        -> std::optional<CellDependency>;

    /// Get the total number of tracked cells.
    [[nodiscard]] auto tracked_cell_count() const -> int;

    /// Clear all analyzed data.
    auto clear() -> void;

private:
    EventBus& event_bus_;

    /// Per-cell dependency data, keyed by cell_id.
    std::unordered_map<std::string, CellDependency> deps_;

    /// Graph edges: cell_id -> set of cell_ids that depend on it.
    std::unordered_map<std::string, std::unordered_set<std::string>> dependents_;

    /// Variable -> defining cell_id.
    std::unordered_map<std::string, std::string> var_to_cell_;

    /// Cached topological order (rebuilt when graph changes).
    std::vector<std::string> topo_order_;
    bool graph_dirty_{true};

    /// Parse simple assignment patterns from source code.
    [[nodiscard]] static auto extract_defines(const std::string& source)
        -> std::unordered_set<std::string>;

    /// Parse variable references from source code.
    [[nodiscard]] static auto extract_references(const std::string& source)
        -> std::unordered_set<std::string>;

    /// Topological sort using Kahn's algorithm.
    [[nodiscard]] auto topological_sort() const -> std::vector<std::string>;

    /// Detect cycles using DFS.
    [[nodiscard]] auto detect_cycle() const -> bool;
};

} // namespace markamp::core
