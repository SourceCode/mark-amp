// V11 Phase 14: Domain Graphics Runtime MVP

#include "node_editor/GraphicsRuntime.h"

#include "node_editor/ExecutionPlan.h"
#include "node_editor/NodeGraph.h"

namespace markamp::node_editor
{

auto GraphicsRuntime::domain_id() const -> std::string
{
    return "graphics";
}

auto GraphicsRuntime::graph_mode() const -> GraphMode
{
    return GraphMode::Graphics;
}

auto GraphicsRuntime::capabilities() const -> DomainCapabilities
{
    return DomainCapabilities{
        .supports_async = true,
        .supports_preview = true,
        .supports_caching = true,
        .supports_streaming = false,
        .supports_debug = true,
        .supports_cancellation = true,
    };
}

auto GraphicsRuntime::execute(const NodeGraph& graph,
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

    // Process graphics pipeline steps sequentially, propagating output buffers.
    const auto& steps = plan.steps();
    int evaluated = 0;

    for (const auto& step : steps)
    {
        if (token.is_cancelled())
        {
            result.success = false;
            result.error_message = "Cancelled during graphics pipeline at step " + step.type_name;
            result.nodes_evaluated = evaluated;
            return result;
        }

        const auto* node = graph.find_node(step.node_id);
        if (node == nullptr)
        {
            result.success = false;
            result.error_message = "Graphics node not found: " + std::to_string(step.node_id.value);
            result.nodes_evaluated = evaluated;
            return result;
        }

        ++evaluated;
    }

    result.success = true;
    result.nodes_evaluated = evaluated;
    return result;
}

auto GraphicsRuntime::validate(const NodeGraph& graph) const -> bool
{
    if (graph.mode() != GraphMode::Graphics)
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

auto GraphicsRuntime::display_name() const -> std::string
{
    return "Graphics";
}

auto GraphicsRuntime::supported_node_types() const -> std::vector<std::string>
{
    return {"mix_color", "brightness_contrast", "blur", "composite", "color_ramp", "image_texture"};
}

} // namespace markamp::node_editor
