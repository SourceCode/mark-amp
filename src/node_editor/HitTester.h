#pragma once

// V11 Phase 24: Socket Rendering Link Points And Hit Testing
// Spatial hit-test engine for determining what canvas element is under the cursor.
// No wxWidgets dependency.

#include "NodeEditorTypes.h"
#include "NodeGraph.h"
#include "NodeLayout.h"

#include <vector>

namespace markamp::node_editor
{

/// What type of element was hit.
enum class HitTarget : uint8_t
{
    Nothing,
    NodeHeader,
    NodeBody,
    InputSocket,
    OutputSocket,
    Link,
    Frame,
    ReroutePoint
};

/// Result of a hit-test query.
struct HitResult
{
    HitTarget target{HitTarget::Nothing};
    NodeId node_id;
    SocketId socket_id;
    LinkId link_id;
    FrameId frame_id;

    [[nodiscard]] auto hit() const noexcept -> bool
    {
        return target != HitTarget::Nothing;
    }
};

/// Configuration for hit-test radii.
struct HitTestConfig
{
    float socket_radius{8.0F}; // Extra margin around socket center
    float link_distance{6.0F}; // Distance threshold for link hit
    float frame_border{4.0F};  // Border width for frame edge hit
};

/// Spatial query engine for canvas elements.
class HitTester
{
public:
    HitTester();
    explicit HitTester(HitTestConfig config);

    /// Test what element is at the given world-space position.
    /// Priority: sockets > node header > node body > links > frames > nothing.
    [[nodiscard]] auto test(Vec2 world_pos,
                            const NodeGraph& graph,
                            const std::vector<NodeLayoutResult>& layouts) const -> HitResult;

    /// Test only against nodes (no links/frames). Faster for common case.
    [[nodiscard]] auto test_nodes_only(Vec2 world_pos,
                                       const std::vector<NodeLayoutResult>& layouts) const
        -> HitResult;

    /// Find all nodes whose bounds intersect the given rect (for box selection).
    [[nodiscard]] auto nodes_in_rect(Rect world_rect,
                                     const std::vector<NodeLayoutResult>& layouts) const
        -> std::vector<NodeId>;

    [[nodiscard]] auto config() const noexcept -> const HitTestConfig&;

private:
    HitTestConfig config_;

    [[nodiscard]] auto test_sockets(Vec2 world_pos, const NodeLayoutResult& layout) const
        -> HitResult;
    [[nodiscard]] auto test_node(Vec2 world_pos, const NodeLayoutResult& layout) const -> HitResult;
};

} // namespace markamp::node_editor
