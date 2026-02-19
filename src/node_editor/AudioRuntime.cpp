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

    result.success = true;
    result.nodes_evaluated = static_cast<int>(plan.step_count());
    return result;
}

auto AudioRuntime::validate(const NodeGraph& graph) const -> bool
{
    return graph.mode() == GraphMode::Audio;
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
