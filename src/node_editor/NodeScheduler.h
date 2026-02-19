#pragma once

// V11 Phase 12: Node Execution Scheduler & Dependencies
// Drives evaluation step-by-step with dirty propagation and cancellation.

#include "node_editor/ExecutionPlan.h"
#include "node_editor/IDomainRuntime.h"
#include "node_editor/NodeEditorTypes.h"
#include "node_editor/NodeGraph.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::node_editor
{

// ============================================================================
// Dirty Tracker
// ============================================================================

/// Tracks per-node dirty state and propagates invalidation downstream.
class DirtyTracker
{
public:
    DirtyTracker() = default;

    /// Mark a node as dirty. Does NOT propagate — call propagate() separately.
    void mark_dirty(NodeId node_id);

    /// Mark all nodes as dirty.
    void mark_all_dirty(const std::vector<NodeId>& node_ids);

    /// Mark a node as clean.
    void mark_clean(NodeId node_id);

    /// Check if a node is dirty.
    [[nodiscard]] auto is_dirty(NodeId node_id) const -> bool;

    /// Get all dirty node IDs.
    [[nodiscard]] auto dirty_nodes() const -> std::vector<NodeId>;

    /// Get the count of dirty nodes.
    [[nodiscard]] auto dirty_count() const -> std::size_t;

    /// Propagate dirty state downstream using the execution plan's dependency info.
    /// If node A is dirty and B depends on A, B becomes dirty too.
    void propagate(const ExecutionPlan& plan);

    /// Clear all dirty flags.
    void clear();

private:
    std::unordered_set<NodeId> dirty_set_;
};

// ============================================================================
// Scheduler Result
// ============================================================================

/// Outcome of a scheduler run.
struct SchedulerResult
{
    bool success{false};
    bool cancelled{false};
    int steps_executed{0};
    int steps_skipped{0}; // Clean nodes that didn't need re-evaluation
    std::string error_message;
};

// ============================================================================
// Node Scheduler
// ============================================================================

/// Drives graph execution step-by-step using an ExecutionPlan and IDomainRuntime.
///
/// The scheduler respects dirty flags: only dirty nodes are re-evaluated.
/// Clean nodes are skipped. Cancellation is checked between steps.
class NodeScheduler
{
public:
    NodeScheduler() = default;

    /// Execute the plan using the given runtime.
    /// Only dirty nodes are re-evaluated; clean nodes are skipped.
    auto schedule(const NodeGraph& graph,
                  const ExecutionPlan& plan,
                  IDomainRuntime& runtime,
                  DirtyTracker& tracker,
                  CancellationToken& token) -> SchedulerResult;

    /// Execute the plan fully (all nodes dirty).
    auto schedule_full(const NodeGraph& graph,
                       const ExecutionPlan& plan,
                       IDomainRuntime& runtime,
                       CancellationToken& token) -> SchedulerResult;
};

} // namespace markamp::node_editor
