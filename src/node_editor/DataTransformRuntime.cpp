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

    result.success = true;
    result.nodes_evaluated = static_cast<int>(plan.step_count());
    return result;
}

auto DataTransformRuntime::validate(const NodeGraph& graph) const -> bool
{
    return graph.mode() == GraphMode::DataTransform;
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
