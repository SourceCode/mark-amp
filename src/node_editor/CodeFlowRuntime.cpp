// V11 Phase 15: Domain CodeFlow Runtime MVP

#include "node_editor/CodeFlowRuntime.h"

#include "node_editor/ExecutionPlan.h"
#include "node_editor/NodeGraph.h"

namespace markamp::node_editor
{

auto CodeFlowRuntime::domain_id() const -> std::string
{
    return "codeflow";
}

auto CodeFlowRuntime::graph_mode() const -> GraphMode
{
    return GraphMode::CodeFlow;
}

auto CodeFlowRuntime::capabilities() const -> DomainCapabilities
{
    return DomainCapabilities{
        .supports_async = true,
        .supports_preview = false,
        .supports_caching = true,
        .supports_streaming = true,
        .supports_debug = true,
        .supports_cancellation = true,
    };
}

auto CodeFlowRuntime::execute(const NodeGraph& graph,
                              const ExecutionPlan& plan,
                              CancellationToken& token) -> ExecutionResult
{
    ExecutionResult result;

    if (token.is_cancelled())
    {
        result.success = false;
        result.error_message = "Cancelled before execution";
        return result;
    }

    if (!plan.is_valid())
    {
        result.success = false;
        result.error_message = "Invalid execution plan";
        return result;
    }

    result.success = true;
    result.nodes_evaluated = static_cast<int>(plan.step_count());
    return result;
}

auto CodeFlowRuntime::validate(const NodeGraph& graph) const -> bool
{
    return graph.mode() == GraphMode::CodeFlow;
}

auto CodeFlowRuntime::display_name() const -> std::string
{
    return "CodeFlow";
}

auto CodeFlowRuntime::supported_node_types() const -> std::vector<std::string>
{
    return {"if_branch", "for_each", "map", "reduce", "function_call", "variable"};
}

} // namespace markamp::node_editor
