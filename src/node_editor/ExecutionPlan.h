#pragma once

// V11 Phase 09: Evaluation IR & Execution Planning
// Deterministic execution ordering with dependency resolution and memoization hints.

#include "node_editor/EvaluationIR.h"
#include "node_editor/NodeEditorTypes.h"

#include <string>
#include <unordered_set>
#include <vector>

namespace markamp::node_editor
{

// ============================================================================
// Execution Step
// ============================================================================

/// A single step in the execution plan.
struct ExecutionStep
{
    NodeId node_id;
    std::string type_name;

    /// Indices into the plan's step list for nodes that must execute before this one.
    std::vector<std::size_t> dependency_indices;

    /// Cache key for memoization (derived from node type + input socket IDs).
    std::string cache_key;

    /// Whether this step has upstream dependencies.
    [[nodiscard]] auto has_dependencies() const -> bool
    {
        return !dependency_indices.empty();
    }
};

// ============================================================================
// Execution Plan
// ============================================================================

/// Ordered sequence of execution steps built from an EvaluationIR.
///
/// Steps are in topological (dependency) order — each step's dependencies
/// are guaranteed to appear earlier in the list.
class ExecutionPlan
{
public:
    ExecutionPlan() = default;

    /// Factory: build an execution plan from an EvaluationIR.
    [[nodiscard]] static auto build(const EvaluationIR& evaluation_ir) -> ExecutionPlan;

    // -- Accessors --
    [[nodiscard]] auto steps() const -> const std::vector<ExecutionStep>&;
    [[nodiscard]] auto step_count() const -> std::size_t;
    [[nodiscard]] auto is_valid() const -> bool;

    /// Find the step index for a given NodeId. Returns -1 if not found.
    [[nodiscard]] auto find_step_index(NodeId node_id) const -> int;

    /// Get the set of root nodes (no dependencies).
    [[nodiscard]] auto root_steps() const -> std::vector<std::size_t>;

    /// Get the set of leaf nodes (no downstream dependents).
    [[nodiscard]] auto leaf_steps() const -> std::vector<std::size_t>;

private:
    std::vector<ExecutionStep> steps_;
    bool valid_{false};
};

} // namespace markamp::node_editor
