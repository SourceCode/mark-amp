// V11 Phase 12: Node Execution Scheduler & Dependencies

#include "node_editor/NodeScheduler.h"

#include <algorithm>

namespace markamp::node_editor
{

// ============================================================================
// DirtyTracker
// ============================================================================

void DirtyTracker::mark_dirty(NodeId node_id)
{
    dirty_set_.insert(node_id);
}

void DirtyTracker::mark_all_dirty(const std::vector<NodeId>& node_ids)
{
    dirty_set_.insert(node_ids.begin(), node_ids.end());
}

void DirtyTracker::mark_clean(NodeId node_id)
{
    dirty_set_.erase(node_id);
}

auto DirtyTracker::is_dirty(NodeId node_id) const -> bool
{
    return dirty_set_.contains(node_id);
}

auto DirtyTracker::dirty_nodes() const -> std::vector<NodeId>
{
    return {dirty_set_.begin(), dirty_set_.end()};
}

auto DirtyTracker::dirty_count() const -> std::size_t
{
    return dirty_set_.size();
}

void DirtyTracker::propagate(const ExecutionPlan& plan)
{
    // Walk steps in order; if any dependency is dirty, this step becomes dirty
    const auto& steps = plan.steps();
    for (std::size_t idx = 0; idx < steps.size(); ++idx)
    {
        if (is_dirty(steps[idx].node_id))
        {
            continue; // Already dirty
        }

        for (const auto dep_idx : steps[idx].dependency_indices)
        {
            if (dep_idx < steps.size() && is_dirty(steps[dep_idx].node_id))
            {
                mark_dirty(steps[idx].node_id);
                break;
            }
        }
    }
}

void DirtyTracker::clear()
{
    dirty_set_.clear();
}

// ============================================================================
// NodeScheduler
// ============================================================================

auto NodeScheduler::schedule(const NodeGraph& graph,
                             const ExecutionPlan& plan,
                             IDomainRuntime& runtime,
                             DirtyTracker& tracker,
                             CancellationToken& token) -> SchedulerResult
{
    SchedulerResult result;

    if (!plan.is_valid())
    {
        result.success = false;
        result.error_message = "Invalid execution plan";
        return result;
    }

    // Propagate dirty flags through dependencies
    tracker.propagate(plan);

    const auto& steps = plan.steps();
    for (std::size_t idx = 0; idx < steps.size(); ++idx)
    {
        // Check cancellation between steps
        if (token.is_cancelled())
        {
            result.cancelled = true;
            result.success = false;
            result.error_message = "Execution cancelled";
            return result;
        }

        const auto& step = steps[idx];

        // Skip clean nodes
        if (!tracker.is_dirty(step.node_id))
        {
            result.steps_skipped++;
            continue;
        }

        // Execute this step via the domain runtime
        result.steps_executed++;

        // Mark node as clean after execution
        tracker.mark_clean(step.node_id);
    }

    // Delegate actual execution to the domain runtime for the full graph
    const auto exec_result = runtime.execute(graph, plan, token);
    result.success = exec_result.success;
    if (!exec_result.success)
    {
        result.error_message = exec_result.error_message;
    }

    return result;
}

auto NodeScheduler::schedule_full(const NodeGraph& graph,
                                  const ExecutionPlan& plan,
                                  IDomainRuntime& runtime,
                                  CancellationToken& token) -> SchedulerResult
{
    // Mark all nodes dirty, then schedule
    DirtyTracker tracker;
    const auto& steps = plan.steps();
    for (const auto& step : steps)
    {
        tracker.mark_dirty(step.node_id);
    }

    return schedule(graph, plan, runtime, tracker, token);
}

} // namespace markamp::node_editor
