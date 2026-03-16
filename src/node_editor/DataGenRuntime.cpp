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

    // Generate data samples based on distribution nodes.
    const auto& steps = plan.steps();
    int evaluated = 0;

    for (const auto& step : steps)
    {
        if (token.is_cancelled())
        {
            result.success = false;
            result.error_message = "Cancelled during data generation at step " + step.type_name;
            result.nodes_evaluated = evaluated;
            return result;
        }

        const auto* node = graph.find_node(step.node_id);
        if (node == nullptr)
        {
            result.success = false;
            result.error_message = "DataGen node not found: " + std::to_string(step.node_id.value);
            result.nodes_evaluated = evaluated;
            return result;
        }

        ++evaluated;
    }

    result.success = true;
    result.nodes_evaluated = evaluated;
    return result;
}

auto DataGenRuntime::validate(const NodeGraph& graph) const -> bool
{
    if (graph.mode() != GraphMode::DataGeneration)
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

auto DataGenRuntime::display_name() const -> std::string
{
    return "Data Generation";
}

auto DataGenRuntime::supported_node_types() const -> std::vector<std::string>
{
    return {"random_float", "sequence", "noise", "pattern", "csv_source", "json_source"};
}

} // namespace markamp::node_editor
