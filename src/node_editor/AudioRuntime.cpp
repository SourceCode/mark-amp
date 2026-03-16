// V11 Phase 18: Domain Audio Generation Runtime MVP

#include "node_editor/AudioRuntime.h"

#include "node_editor/ExecutionPlan.h"
#include "node_editor/NodeGraph.h"

namespace markamp::node_editor
{

auto AudioRuntime::domain_id() const -> std::string
{
    return "audio";
}

auto AudioRuntime::graph_mode() const -> GraphMode
{
    return GraphMode::Audio;
}

auto AudioRuntime::capabilities() const -> DomainCapabilities
{
    return DomainCapabilities{
        .supports_async = true,
        .supports_preview = true,
        .supports_caching = false,
        .supports_streaming = true,
        .supports_debug = true,
        .supports_cancellation = true,
    };
}

auto AudioRuntime::execute(const NodeGraph& graph,
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

    // Execute each step in topological order.
    const auto& steps = plan.steps();
    int evaluated = 0;

    for (const auto& step : steps)
    {
        if (token.is_cancelled())
        {
            result.success = false;
            result.error_message = "Cancelled during execution at step " + step.type_name;
            result.nodes_evaluated = evaluated;
            return result;
        }

        // Verify this audio node type is supported.
        const auto* node = graph.find_node(step.node_id);
        if (node == nullptr)
        {
            result.success = false;
            result.error_message = "Node not found: " + std::to_string(step.node_id.value);
            result.nodes_evaluated = evaluated;
            return result;
        }

        // Simulate processing this audio node (oscillator, filter, mix, etc.).
        ++evaluated;
    }

    result.success = true;
    result.nodes_evaluated = evaluated;
    return result;
}

auto AudioRuntime::validate(const NodeGraph& graph) const -> bool
{
    if (graph.mode() != GraphMode::Audio)
    {
        return false;
    }

    // Verify all nodes in the graph are supported audio node types.
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

auto AudioRuntime::display_name() const -> std::string
{
    return "Audio";
}

auto AudioRuntime::supported_node_types() const -> std::vector<std::string>
{
    return {"oscillator", "envelope", "filter", "mix", "output", "delay", "reverb"};
}

} // namespace markamp::node_editor
