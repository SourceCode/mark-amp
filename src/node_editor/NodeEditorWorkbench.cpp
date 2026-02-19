#include "NodeEditorWorkbench.h"

#include <utility>

namespace markamp::node_editor
{

NodeEditorWorkbench::NodeEditorWorkbench()
    : graph_(nullptr)
    , viewport_()
    , layout_()
    , context_()
{
}

void NodeEditorWorkbench::open_graph(std::unique_ptr<NodeGraph> graph)
{
    graph_ = std::move(graph);
    if (graph_ != nullptr)
    {
        context_.set_active_graph(graph_.get());
        // Fit viewport to graph content
        const Rect kBounds = layout_.compute_graph_bounds(*graph_);
        if (kBounds.width > 0.0F && kBounds.height > 0.0F)
        {
            viewport_.zoom_to_fit(kBounds);
        }
    }
}

void NodeEditorWorkbench::close_graph()
{
    context_.set_active_graph(nullptr);
    graph_.reset();
    viewport_.reset();
}

auto NodeEditorWorkbench::has_graph() const noexcept -> bool
{
    return graph_ != nullptr;
}

auto NodeEditorWorkbench::graph() const -> const NodeGraph*
{
    return graph_.get();
}

auto NodeEditorWorkbench::graph_mut() -> NodeGraph*
{
    return graph_.get();
}

void NodeEditorWorkbench::activate()
{
    active_ = true;
}

void NodeEditorWorkbench::deactivate()
{
    active_ = false;
}

auto NodeEditorWorkbench::is_active() const noexcept -> bool
{
    return active_;
}

void NodeEditorWorkbench::set_active_panel(WorkbenchPanel panel)
{
    active_panel_ = panel;
}

auto NodeEditorWorkbench::active_panel() const noexcept -> WorkbenchPanel
{
    return active_panel_;
}

auto NodeEditorWorkbench::viewport() const noexcept -> const CanvasViewport&
{
    return viewport_;
}

auto NodeEditorWorkbench::viewport_mut() noexcept -> CanvasViewport&
{
    return viewport_;
}

auto NodeEditorWorkbench::layout() const noexcept -> const NodeLayout&
{
    return layout_;
}

auto NodeEditorWorkbench::context() const noexcept -> const NodeEditorContext&
{
    return context_;
}

auto NodeEditorWorkbench::context_mut() noexcept -> NodeEditorContext&
{
    return context_;
}

auto NodeEditorWorkbench::status_text() const -> std::string
{
    if (!has_graph())
    {
        return "No graph open";
    }
    const auto kNodeCount = graph_->node_count();
    const auto kLinkCount = graph_->link_count();
    return std::to_string(kNodeCount) + " nodes, " + std::to_string(kLinkCount) + " links";
}

auto NodeEditorWorkbench::mode_text() const -> std::string
{
    if (!has_graph())
    {
        return "Idle";
    }
    return std::string(graph_mode_name(graph_->mode()));
}

auto NodeEditorWorkbench::execute_command(const std::string& command_id) -> bool
{
    if (command_id == "node_editor.zoom_reset")
    {
        viewport_.reset();
        return true;
    }
    if (command_id == "node_editor.zoom_to_fit" && has_graph())
    {
        const Rect kBounds = layout_.compute_graph_bounds(*graph_);
        viewport_.zoom_to_fit(kBounds);
        return true;
    }
    if (command_id == "node_editor.close_graph")
    {
        close_graph();
        return true;
    }
    return false;
}

auto NodeEditorWorkbench::available_commands() const -> std::vector<std::string>
{
    std::vector<std::string> commands;
    commands.emplace_back("node_editor.zoom_reset");
    if (has_graph())
    {
        commands.emplace_back("node_editor.zoom_to_fit");
        commands.emplace_back("node_editor.close_graph");
    }
    return commands;
}

} // namespace markamp::node_editor
