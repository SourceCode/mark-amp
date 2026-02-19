#include "NodeLayout.h"

namespace markamp::node_editor
{

NodeLayout::NodeLayout()
    : config_()
{
}

NodeLayout::NodeLayout(LayoutConfig config)
    : config_(config)
{
}

auto NodeLayout::compute_node_layout(const NodeGraph& graph, NodeId node_id) const
    -> NodeLayoutResult
{
    NodeLayoutResult result;
    result.node_id = node_id;

    const GraphNode* node = graph.find_node(node_id);
    if (node == nullptr)
    {
        return result;
    }

    const float kNodeX = node->position.x;
    const float kNodeY = node->position.y;
    const float kWidth = (node->dimensions.x > 0.0F) ? node->dimensions.x : config_.node_width;

    // Determine collapsed state
    result.collapsed = has_flag(node->flags, NodeFlags::Collapsed);

    // Header rect
    result.header_rect = {kNodeX, kNodeY, kWidth, config_.header_height};

    if (result.collapsed)
    {
        // Collapsed: header only, no sockets shown
        result.body_rect = {kNodeX, kNodeY + config_.header_height, kWidth, 0.0F};
        result.total_bounds = {kNodeX, kNodeY, kWidth, config_.collapsed_height};
        return result;
    }

    // Compute socket layout
    const std::size_t kInputCount = node->inputs.size();
    const std::size_t kOutputCount = node->outputs.size();
    const std::size_t kMaxSockets = (kInputCount > kOutputCount) ? kInputCount : kOutputCount;

    const float kBodyHeight = (kMaxSockets > 0)
                                  ? config_.body_padding_top +
                                        static_cast<float>(kMaxSockets) * config_.socket_spacing +
                                        config_.body_padding_bottom
                                  : config_.min_body_height;

    const float kBodyStartY = kNodeY + config_.header_height;
    result.body_rect = {kNodeX, kBodyStartY, kWidth, kBodyHeight};

    // Input socket positions (left edge)
    for (std::size_t idx = 0; idx < kInputCount; ++idx)
    {
        const GraphSocket* socket = graph.find_socket(node->inputs[idx]);
        if (socket == nullptr)
        {
            continue;
        }

        SocketLayoutEntry entry;
        entry.socket_id = socket->id;
        entry.direction = SocketDirection::Input;
        entry.data_type = socket->data_type;
        entry.radius = config_.socket_radius;
        entry.position = {kNodeX + config_.socket_offset_x,
                          kBodyStartY + config_.body_padding_top +
                              (static_cast<float>(idx) + 0.5F) * config_.socket_spacing};
        result.sockets.push_back(entry);
    }

    // Output socket positions (right edge)
    for (std::size_t idx = 0; idx < kOutputCount; ++idx)
    {
        const GraphSocket* socket = graph.find_socket(node->outputs[idx]);
        if (socket == nullptr)
        {
            continue;
        }

        SocketLayoutEntry entry;
        entry.socket_id = socket->id;
        entry.direction = SocketDirection::Output;
        entry.data_type = socket->data_type;
        entry.radius = config_.socket_radius;
        entry.position = {kNodeX + kWidth - config_.socket_offset_x,
                          kBodyStartY + config_.body_padding_top +
                              (static_cast<float>(idx) + 0.5F) * config_.socket_spacing};
        result.sockets.push_back(entry);
    }

    // Determine interaction state from flags
    if (has_flag(node->flags, NodeFlags::Error))
    {
        result.interaction_state = NodeInteractionState::Error;
    }
    else if (has_flag(node->flags, NodeFlags::Muted))
    {
        result.interaction_state = NodeInteractionState::Muted;
    }
    else if (has_flag(node->flags, NodeFlags::Active))
    {
        result.interaction_state = NodeInteractionState::Active;
    }

    const float kTotalHeight = config_.header_height + kBodyHeight;
    result.total_bounds = {kNodeX, kNodeY, kWidth, kTotalHeight};

    return result;
}

auto NodeLayout::compute_all_layouts(const NodeGraph& graph) const -> std::vector<NodeLayoutResult>
{
    const auto kNodeIds = graph.all_node_ids();
    std::vector<NodeLayoutResult> results;
    results.reserve(kNodeIds.size());

    for (const auto& node_id : kNodeIds)
    {
        results.push_back(compute_node_layout(graph, node_id));
    }
    return results;
}

auto NodeLayout::compute_graph_bounds(const NodeGraph& graph) const -> Rect
{
    const auto kAllLayouts = compute_all_layouts(graph);
    if (kAllLayouts.empty())
    {
        return {};
    }

    Rect bounds = kAllLayouts[0].total_bounds;
    for (std::size_t idx = 1; idx < kAllLayouts.size(); ++idx)
    {
        bounds = bounds.union_with(kAllLayouts[idx].total_bounds);
    }
    return bounds;
}

auto NodeLayout::config() const noexcept -> const LayoutConfig&
{
    return config_;
}

} // namespace markamp::node_editor
