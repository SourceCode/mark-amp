// V11 Phase 17: Domain Data Generation Runtime MVP

#include "node_editor/DataGenRuntime.h"

#include "node_editor/ExecutionPlan.h"
#include "node_editor/NodeGraph.h"

namespace markamp::node_editor
{

auto DataGenRuntime::domain_id() const -> std::string
{
    return "data_gen";
}

auto DataGenRuntime::graph_mode() const -> GraphMode
{
    return GraphMode::DataGeneration;
}

auto DataGenRuntime::capabilities() const -> DomainCapabilities
{
    return DomainCapabilities{
        .supports_async = false,
        .supports_preview = true,
        .supports_caching = true,
        .supports_streaming = false,
        .supports_debug = true,
        .supports_cancellation = true,
    };
}

auto DataGenRuntime::execute(const NodeGraph& graph,
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

auto DataGenRuntime::validate(const NodeGraph& graph) const -> bool
{
    return graph.mode() == GraphMode::DataGeneration;
}

auto DataGenRuntime::display_name() const -> std::string
{
    return "Data Generation";
}

auto DataGenRuntime::supported_node_types() const -> std::vector<std::string>
{
    return {"random_float", "sequence", "noise", "pattern", "csv_source", "json_source"};
}

} // namespace markamp::node_editor
