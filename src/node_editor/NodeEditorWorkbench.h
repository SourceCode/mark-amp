#pragma once

// V11 Phase 21: Node Editor Workbench Host And Panel Integration
// Workbench view-model: manages the active graph session, viewport, layout, and command routing.
// No wxWidgets dependency — fully testable headlessly.

#include "CanvasViewport.h"
#include "NodeEditorContext.h"
#include "NodeEditorTypes.h"
#include "NodeGraph.h"
#include "NodeLayout.h"

#include <memory>
#include <string>
#include <vector>

namespace markamp::node_editor
{

/// Workbench panel mode (what region is focused).
enum class WorkbenchPanel : uint8_t
{
    Canvas,
    Inspector,
    Search,
    Preview
};

/// Workbench view-model: the central controller for a node editor session.
/// Owns the active graph, viewport, layout engine, and orchestrates commands.
class NodeEditorWorkbench
{
public:
    NodeEditorWorkbench();

    // --- Graph lifecycle ---
    void open_graph(std::unique_ptr<NodeGraph> graph);
    void close_graph();
    [[nodiscard]] auto has_graph() const noexcept -> bool;
    [[nodiscard]] auto graph() const -> const NodeGraph*;
    auto graph_mut() -> NodeGraph*;

    // --- Panel state ---
    void activate();
    void deactivate();
    [[nodiscard]] auto is_active() const noexcept -> bool;

    void set_active_panel(WorkbenchPanel panel);
    [[nodiscard]] auto active_panel() const noexcept -> WorkbenchPanel;

    // --- Viewport ---
    [[nodiscard]] auto viewport() const noexcept -> const CanvasViewport&;
    auto viewport_mut() noexcept -> CanvasViewport&;

    // --- Layout ---
    [[nodiscard]] auto layout() const noexcept -> const NodeLayout&;

    // --- Context ---
    [[nodiscard]] auto context() const noexcept -> const NodeEditorContext&;
    auto context_mut() noexcept -> NodeEditorContext&;

    // --- Status bar ---
    [[nodiscard]] auto status_text() const -> std::string;
    [[nodiscard]] auto mode_text() const -> std::string;

    // --- Command routing ---
    /// Execute a named command. Returns true if handled.
    auto execute_command(const std::string& command_id) -> bool;

    /// List available command IDs for the current state.
    [[nodiscard]] auto available_commands() const -> std::vector<std::string>;

private:
    std::unique_ptr<NodeGraph> graph_;
    CanvasViewport viewport_;
    NodeLayout layout_;
    NodeEditorContext context_;
    WorkbenchPanel active_panel_{WorkbenchPanel::Canvas};
    bool active_{false};
};

} // namespace markamp::node_editor
