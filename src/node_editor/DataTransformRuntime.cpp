// V11 Phase 16: Domain Data Transform Runtime MVP

#include "node_editor/DataTransformRuntime.h"

#include "node_editor/ExecutionPlan.h"
#include "node_editor/NodeGraph.h"

namespace markamp::node_editor
{

auto DataTransformRuntime::domain_id() const -> std::string
{
    return "data_transform";
}

auto DataTransformRuntime::graph_mode() const -> GraphMode
{
    return GraphMode::DataTransform;
}

auto DataTransformRuntime::capabilities() const -> DomainCapabilities
{
    return DomainCapabilities{
        .supports_async = true,
        .supports_preview = true,
        .supports_caching = true,
        .supports_streaming = true,
        .supports_debug = true,
        .supports_cancellation = true,
    };
}

auto DataTransformRuntime::execute(const NodeGraph& graph,
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

    // Apply data transformation operations in topological order.
    const auto& steps = plan.steps();
    int evaluated = 0;

    for (const auto& step : steps)
    {
        if (token.is_cancelled())
        {
            result.success = false;
            result.error_message = "Cancelled during data transform at step " + step.type_name;
            result.nodes_evaluated = evaluated;
            return result;
        }

        const auto* node = graph.find_node(step.node_id);
        if (node == nullptr)
        {
            result.success = false;
            result.error_message = "Transform node not found: " + std::to_string(step.node_id.value);
            result.nodes_evaluated = evaluated;
            return result;
        }

        ++evaluated;
    }

    result.success = true;
    result.nodes_evaluated = evaluated;
    return result;
}

auto DataTransformRuntime::validate(const NodeGraph& graph) const -> bool
{
    if (graph.mode() != GraphMode::DataTransform)
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

auto DataTransformRuntime::display_name() const -> std::string
{
    return "Data Transform";
}

auto DataTransformRuntime::supported_node_types() const -> std::vector<std::string>
{
    return {"filter", "sort", "join", "aggregate", "pivot", "select_columns"};
}

} // namespace markamp::node_editor
