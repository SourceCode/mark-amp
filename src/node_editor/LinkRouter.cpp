#include "LinkRouter.h"

#include <cmath>

namespace markamp::node_editor
{

LinkRouter::LinkRouter()
    : mode_(RoutingMode::Bezier)
    , curvature_(0.5F)
{
}

LinkRouter::LinkRouter(RoutingMode default_mode)
    : mode_(default_mode)
    , curvature_(0.5F)
{
}

auto LinkRouter::compute_path(Vec2 from, Vec2 to_pos) const -> LinkPath
{
    return compute_path(from, to_pos, mode_);
}

auto LinkRouter::compute_path(Vec2 from, Vec2 to_pos, RoutingMode mode) const -> LinkPath
{
    switch (mode)
    {
        case RoutingMode::Bezier:
            return compute_bezier(from, to_pos);
        case RoutingMode::Straight:
            return compute_straight(from, to_pos);
        case RoutingMode::Orthogonal:
            return compute_orthogonal(from, to_pos);
    }
    return compute_bezier(from, to_pos);
}

auto LinkRouter::compute_bezier(Vec2 from, Vec2 to_pos) const -> LinkPath
{
    LinkPath path;
    path.start = from;
    path.end = to_pos;
    path.mode = RoutingMode::Bezier;

    // Compute horizontal offset for Bézier control points.
    // The offset scales with distance for natural-looking curves.
    const float kDeltaX = std::abs(to_pos.x - from.x);
    const float kOffset = (kDeltaX * 0.5F + 50.0F) * curvature_;

    // Control point 1: right of source
    path.control_points.push_back({from.x + kOffset, from.y});
    // Control point 2: left of target
    path.control_points.push_back({to_pos.x - kOffset, to_pos.y});

    return path;
}

auto LinkRouter::compute_straight(Vec2 from, Vec2 to_pos) -> LinkPath
{
    LinkPath path;
    path.start = from;
    path.end = to_pos;
    path.mode = RoutingMode::Straight;
    // No control points needed for straight lines
    return path;
}

auto LinkRouter::compute_orthogonal(Vec2 from, Vec2 to_pos) -> LinkPath
{
    LinkPath path;
    path.start = from;
    path.end = to_pos;
    path.mode = RoutingMode::Orthogonal;

    // Orthogonal routing: horizontal out, vertical connector, horizontal in
    const float kMidX = (from.x + to_pos.x) * 0.5F;
    path.control_points.push_back({kMidX, from.y});
    path.control_points.push_back({kMidX, to_pos.y});

    return path;
}

auto LinkRouter::insert_reroute(const LinkPath& path, Vec2 reroute_pos) -> LinkPath
{
    // Create two sub-paths joined at the reroute point.
    // For simplicity, we produce a straight-segment path through the reroute.
    LinkPath result;
    result.start = path.start;
    result.end = path.end;
    result.mode = path.mode;
    result.control_points.push_back(reroute_pos);
    return result;
}

auto LinkRouter::mode() const noexcept -> RoutingMode
{
    return mode_;
}

void LinkRouter::set_mode(RoutingMode routing_mode)
{
    mode_ = routing_mode;
}

auto LinkRouter::curvature() const noexcept -> float
{
    return curvature_;
}

void LinkRouter::set_curvature(float curvature_factor)
{
    curvature_ = curvature_factor;
}

} // namespace markamp::node_editor
