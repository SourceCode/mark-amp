// V11 Phase 20: Runtime Sandboxing, Resource Quotas & Safety

#include "node_editor/RuntimeSandbox.h"

#include "node_editor/ExecutionPlan.h"
#include "node_editor/NodeGraph.h"

namespace markamp::node_editor
{

RuntimeSandbox::RuntimeSandbox(ResourceQuota quota)
    : quota_(quota)
{
}

auto RuntimeSandbox::execute(IDomainRuntime& runtime,
                             const NodeGraph& graph,
                             const ExecutionPlan& plan,
                             CancellationToken& token) -> SandboxResult
{
    SandboxResult result;

    // Check cancellation first
    if (token.is_cancelled())
    {
        result.violation = SandboxViolation::Cancelled;
        result.violation_message = "Execution cancelled before start";
        result.execution_result.success = false;
        result.execution_result.error_message = result.violation_message;
        return result;
    }

    // Pre-validate resource constraints
    const auto kViolation = pre_validate(graph);
    if (kViolation != SandboxViolation::None)
    {
        result.violation = kViolation;
        result.violation_message = std::string("Resource limit exceeded: ") +
                                   std::string(sandbox_violation_name(kViolation));
        result.execution_result.success = false;
        result.execution_result.error_message = result.violation_message;
        return result;
    }

    // Check work unit limit against step count
    if (static_cast<int>(plan.step_count()) > quota_.max_work_units)
    {
        result.violation = SandboxViolation::WorkUnitsExceeded;
        result.violation_message = "Work unit limit exceeded";
        result.execution_result.success = false;
        result.execution_result.error_message = result.violation_message;
        return result;
    }

    // Delegate to the actual runtime
    result.execution_result = runtime.execute(graph, plan, token);

    // Check for post-execution cancellation
    if (token.is_cancelled() && !result.execution_result.success)
    {
        result.violation = SandboxViolation::Cancelled;
        result.violation_message = "Execution cancelled during execution";
    }

    return result;
}

auto RuntimeSandbox::quota() const -> const ResourceQuota&
{
    return quota_;
}

void RuntimeSandbox::set_quota(ResourceQuota new_quota)
{
    quota_ = new_quota;
}

auto RuntimeSandbox::pre_validate(const NodeGraph& graph) const -> SandboxViolation
{
    if (static_cast<int>(graph.node_count()) > quota_.max_node_count)
    {
        return SandboxViolation::NodeCountExceeded;
    }
    return SandboxViolation::None;
}

} // namespace markamp::node_editor
