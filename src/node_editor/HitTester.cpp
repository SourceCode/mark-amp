#include "HitTester.h"

#include <cmath>

namespace markamp::node_editor
{

HitTester::HitTester()
    : config_()
{
}

HitTester::HitTester(HitTestConfig config)
    : config_(config)
{
}

auto HitTester::test_sockets(Vec2 world_pos, const NodeLayoutResult& layout) const -> HitResult
{
    for (const auto& socket_entry : layout.sockets)
    {
        const float kDeltaX = world_pos.x - socket_entry.position.x;
        const float kDeltaY = world_pos.y - socket_entry.position.y;
        const float kDistSq = kDeltaX * kDeltaX + kDeltaY * kDeltaY;
        const float kRadius = config_.socket_radius;

        if (kDistSq <= kRadius * kRadius)
        {
            HitResult result;
            result.node_id = layout.node_id;
            result.socket_id = socket_entry.socket_id;
            result.target = (socket_entry.direction == SocketDirection::Input)
                                ? HitTarget::InputSocket
                                : HitTarget::OutputSocket;
            return result;
        }
    }
    return {};
}

auto HitTester::test_node(Vec2 world_pos, const NodeLayoutResult& layout) const -> HitResult
{
    if (!layout.total_bounds.contains(world_pos))
    {
        return {};
    }

    HitResult result;
    result.node_id = layout.node_id;

    if (layout.header_rect.contains(world_pos))
    {
        result.target = HitTarget::NodeHeader;
    }
    else
    {
        result.target = HitTarget::NodeBody;
    }
    return result;
}

auto HitTester::test(Vec2 world_pos,
                     const NodeGraph& /*graph*/,
                     const std::vector<NodeLayoutResult>& layouts) const -> HitResult
{
    // Priority 1: Sockets (check all nodes first for sockets)
    for (const auto& layout : layouts)
    {
        HitResult socket_hit = test_sockets(world_pos, layout);
        if (socket_hit.hit())
        {
            return socket_hit;
        }
    }

    // Priority 2: Node header/body
    for (const auto& layout : layouts)
    {
        HitResult node_hit = test_node(world_pos, layout);
        if (node_hit.hit())
        {
            return node_hit;
        }
    }

    // Priority 3: Links (simple distance check — placeholder for Bézier distance)
    // Full link hit-testing requires the LinkRouter path data; deferred.

    // Priority 4: Frames
    // Frame hit-testing is handled by NodeAnnotation layer; deferred.

    return {};
}

auto HitTester::test_nodes_only(Vec2 world_pos, const std::vector<NodeLayoutResult>& layouts) const
    -> HitResult
{
    // Sockets first
    for (const auto& layout : layouts)
    {
        HitResult socket_hit = test_sockets(world_pos, layout);
        if (socket_hit.hit())
        {
            return socket_hit;
        }
    }

    // Then node body/header
    for (const auto& layout : layouts)
    {
        HitResult node_hit = test_node(world_pos, layout);
        if (node_hit.hit())
        {
            return node_hit;
        }
    }

    return {};
}

auto HitTester::nodes_in_rect(Rect world_rect, const std::vector<NodeLayoutResult>& layouts) const
    -> std::vector<NodeId>
{
    std::vector<NodeId> results;
    for (const auto& layout : layouts)
    {
        if (world_rect.intersects(layout.total_bounds))
        {
            results.push_back(layout.node_id);
        }
    }
    return results;
}

auto HitTester::config() const noexcept -> const HitTestConfig&
{
    return config_;
}

} // namespace markamp::node_editor
