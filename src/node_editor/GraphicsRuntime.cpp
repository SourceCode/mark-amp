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

    // Stub: simulate successful execution of all steps
    result.success = true;
    result.nodes_evaluated = static_cast<int>(plan.step_count());
    return result;
}

auto GraphicsRuntime::validate(const NodeGraph& graph) const -> bool
{
    return graph.mode() == GraphMode::Graphics;
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
