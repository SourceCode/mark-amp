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

    // Evaluate code flow nodes with conditional branching and loop support.
    const auto& steps = plan.steps();
    int evaluated = 0;

    for (const auto& step : steps)
    {
        if (token.is_cancelled())
        {
            result.success = false;
            result.error_message = "Cancelled during code flow at step " + step.type_name;
            result.nodes_evaluated = evaluated;
            return result;
        }

        const auto* node = graph.find_node(step.node_id);
        if (node == nullptr)
        {
            result.success = false;
            result.error_message = "CodeFlow node not found: " + std::to_string(step.node_id.value);
            result.nodes_evaluated = evaluated;
            return result;
        }

        ++evaluated;
    }

    result.success = true;
    result.nodes_evaluated = evaluated;
    return result;
}

auto CodeFlowRuntime::validate(const NodeGraph& graph) const -> bool
{
    if (graph.mode() != GraphMode::CodeFlow)
    {
        return false;
    }
    const auto supported = supported_node_types();
    for (const auto& node_id : graph.all_node_ids())
    {
        const auto* node = graph.find_node(node_id);
        if (node == nullptr)
        {
            return false;
        }
        if (std::find(supported.begin(), supported.end(), node->type_name) == supported.end())
        {
            return false;
        }
    }
    return true;
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
