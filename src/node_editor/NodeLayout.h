#pragma once

// V11 Phase 23: Node Block Rendering Theme Tokens And States
// Calculates node bounding rects and socket positions from graph data.
// Pure geometry — no rendering dependency.

#include "NodeEditorTypes.h"
#include "NodeGraph.h"

#include <vector>

namespace markamp::node_editor
{

/// Visual interaction state for a node.
enum class NodeInteractionState : uint8_t
{
    Normal,
    Hovered,
    Selected,
    Active,
    Error,
    Muted,
    Disabled
};

/// Computed position for a single socket.
struct SocketLayoutEntry
{
    SocketId socket_id;
    Vec2 position;      // Center of socket circle in world space
    float radius{6.0F}; // Hit-test radius
    SocketDirection direction;
    SocketDataType data_type;
};

/// Complete layout result for a single node.
struct NodeLayoutResult
{
    NodeId node_id;
    Rect total_bounds; // Entire node bounding rect
    Rect header_rect;  // Title bar area
    Rect body_rect;    // Socket area
    std::vector<SocketLayoutEntry> sockets;
    NodeInteractionState interaction_state{NodeInteractionState::Normal};
    bool collapsed{false};
};

/// Layout configuration constants.
struct LayoutConfig
{
    float node_width{180.0F};
    float header_height{28.0F};
    float socket_spacing{22.0F};
    float socket_radius{6.0F};
    float socket_offset_x{0.0F}; // Inset from node edge
    float body_padding_top{6.0F};
    float body_padding_bottom{8.0F};
    float collapsed_height{28.0F}; // Header-only height when collapsed
    float min_body_height{20.0F};
};

/// Computes node layout geometry from graph data.
class NodeLayout
{
public:
    NodeLayout();
    explicit NodeLayout(LayoutConfig config);

    /// Compute layout for a single node.
    [[nodiscard]] auto compute_node_layout(const NodeGraph& graph, NodeId node_id) const
        -> NodeLayoutResult;

    /// Compute layout for all nodes in the graph.
    [[nodiscard]] auto compute_all_layouts(const NodeGraph& graph) const
        -> std::vector<NodeLayoutResult>;

    /// Get the bounding rect enclosing all nodes in the graph.
    [[nodiscard]] auto compute_graph_bounds(const NodeGraph& graph) const -> Rect;

    /// Access layout config.
    [[nodiscard]] auto config() const noexcept -> const LayoutConfig&;

private:
    LayoutConfig config_;
};

} // namespace markamp::node_editor
