// ============================================================================
// File: src/canvas/ConnectorRoutingService.cpp
// Phase 12: Canvas Advanced Objects — intelligent connector routing
// ============================================================================
#include "canvas/ConnectorRoutingService.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>

namespace markamp::canvas
{

ConnectorRoutingService::ConnectorRoutingService(const Board& board)
    : board_(board)
{
}

ConnectorRoutingService::ConnectorRoutingService(const Board& board, RoutingConfig config)
    : board_(board)
    , config_(config)
{
}

// ── Single Connector Routing ──────────────────────────────────────

auto ConnectorRoutingService::route_connector(const ConnectorObject& connector) const
    -> RoutingResult
{
    const auto start_pt = connector.resolve_start(board_);
    const auto end_pt = connector.resolve_end(board_);

    auto result = route_between(start_pt, end_pt, connector.routing());

    // If endpoints are attached, optimize anchors
    if (connector.start_endpoint().is_attached())
    {
        const auto suggestion = auto_anchor(connector.start_endpoint().object_id, end_pt);
        result.start_anchor = suggestion.anchor;
    }
    if (connector.end_endpoint().is_attached())
    {
        const auto suggestion = auto_anchor(connector.end_endpoint().object_id, start_pt);
        result.end_anchor = suggestion.anchor;
    }

    return result;
}

auto ConnectorRoutingService::route_between(const Point2D& start_point,
                                            const Point2D& end_point,
                                            ConnectorRouting mode) const -> RoutingResult
{
    RoutingResult result;
    result.success = true;

    switch (mode)
    {
        case ConnectorRouting::kStraight:
            // No waypoints needed for straight lines
            result.waypoints.clear();
            break;

        case ConnectorRouting::kOrthogonal:
        {
            const auto obstacles = collect_obstacles({});
            result.waypoints = route_orthogonal(start_point, end_point, obstacles);
            break;
        }

        case ConnectorRouting::kCurved:
            result.waypoints = route_curved(start_point, end_point);
            break;
    }

    // Compute total length
    result.total_length = 0.0;
    Point2D prev = start_point;
    for (const auto& waypoint : result.waypoints)
    {
        const double segment_dx = waypoint.x - prev.x;
        const double segment_dy = waypoint.y - prev.y;
        result.total_length += std::sqrt(segment_dx * segment_dx + segment_dy * segment_dy);
        prev = waypoint;
    }
    const double final_dx = end_point.x - prev.x;
    const double final_dy = end_point.y - prev.y;
    result.total_length += std::sqrt(final_dx * final_dx + final_dy * final_dy);

    return result;
}

// ── Batch Routing ─────────────────────────────────────────────────

auto ConnectorRoutingService::reroute_all() const -> std::vector<std::pair<ObjectId, RoutingResult>>
{
    std::vector<std::pair<ObjectId, RoutingResult>> results;

    for (const auto& obj_ptr : board_.objects())
    {
        const auto& obj = *obj_ptr;
        if (obj.type() == CanvasObjectType::Connector)
        {
            const auto& connector = static_cast<const ConnectorObject&>(obj);
            auto route = route_connector(connector);
            results.emplace_back(obj.id(), std::move(route));
        }
    }

    return results;
}

auto ConnectorRoutingService::reroute_attached_to(ObjectId obj_id) const
    -> std::vector<std::pair<ObjectId, RoutingResult>>
{
    std::vector<std::pair<ObjectId, RoutingResult>> results;

    for (const auto& obj_ptr : board_.objects())
    {
        const auto& obj = *obj_ptr;
        if (obj.type() == CanvasObjectType::Connector)
        {
            const auto& connector = static_cast<const ConnectorObject&>(obj);
            if (connector.is_attached_to(obj_id))
            {
                auto route = route_connector(connector);
                results.emplace_back(obj.id(), std::move(route));
            }
        }
    }

    return results;
}

// ── Anchor Selection ──────────────────────────────────────────────

auto ConnectorRoutingService::auto_anchor(ObjectId target_id, const Point2D& from_point) const
    -> AnchorSuggestion
{
    const auto suggestions = anchor_suggestions(target_id, from_point);
    if (suggestions.empty())
    {
        return {AnchorPosition::kAuto, {0.0, 0.0}, 0.0};
    }
    return suggestions.front(); // Best score
}

auto ConnectorRoutingService::anchor_suggestions(ObjectId target_id,
                                                 const Point2D& from_point) const
    -> std::vector<AnchorSuggestion>
{
    const auto* obj = board_.get_object(target_id);
    if (obj == nullptr)
    {
        return {};
    }

    const auto bounds = obj->world_bounds();

    std::vector<AnchorSuggestion> suggestions;
    const std::array<AnchorPosition, 4> kAnchors = {AnchorPosition::kTop,
                                                    AnchorPosition::kBottom,
                                                    AnchorPosition::kLeft,
                                                    AnchorPosition::kRight};

    for (const auto anchor : kAnchors)
    {
        const auto pos = anchor_position(bounds, anchor);
        const double anchor_score = score_anchor(bounds, anchor, from_point);
        suggestions.push_back({anchor, pos, anchor_score});
    }

    // Sort by score descending (best first)
    std::ranges::sort(suggestions,
                      [](const AnchorSuggestion& lhs, const AnchorSuggestion& rhs)
                      { return lhs.score > rhs.score; });

    return suggestions;
}

auto ConnectorRoutingService::snap_to_anchor(const Point2D& point) const
    -> std::optional<AnchorSuggestion>
{
    std::optional<AnchorSuggestion> best;
    double best_distance = config_.anchor_snap_distance;

    for (const auto& obj_ptr : board_.objects())
    {
        const auto& obj = *obj_ptr;
        if (obj.type() == CanvasObjectType::Connector)
        {
            continue; // Skip connectors themselves
        }

        const auto bounds = obj.world_bounds();
        const std::array<AnchorPosition, 4> kSnapAnchors = {AnchorPosition::kTop,
                                                            AnchorPosition::kBottom,
                                                            AnchorPosition::kLeft,
                                                            AnchorPosition::kRight};

        for (const auto anchor : kSnapAnchors)
        {
            const auto pos = anchor_position(bounds, anchor);
            const double snap_dx = pos.x - point.x;
            const double snap_dy = pos.y - point.y;
            const double dist = std::sqrt(snap_dx * snap_dx + snap_dy * snap_dy);

            if (dist < best_distance)
            {
                best_distance = dist;
                best = AnchorSuggestion{anchor, pos, 1.0 - (dist / config_.anchor_snap_distance)};
            }
        }
    }

    return best;
}

// ── Configuration ─────────────────────────────────────────────────

auto ConnectorRoutingService::config() const -> const RoutingConfig&
{
    return config_;
}

auto ConnectorRoutingService::set_config(const RoutingConfig& config) -> void
{
    config_ = config;
}

// ── Private Helpers ───────────────────────────────────────────────

auto ConnectorRoutingService::collect_obstacles(const std::vector<ObjectId>& exclude) const
    -> std::vector<AABB>
{
    std::vector<AABB> obstacles;

    for (const auto& obj_ptr : board_.objects())
    {
        const auto& obj = *obj_ptr;
        // Skip connectors and excluded objects
        if (obj.type() == CanvasObjectType::Connector)
        {
            continue;
        }
        if (std::ranges::find(exclude, obj.id()) != exclude.end())
        {
            continue;
        }

        auto bounds = obj.world_bounds();
        // Inflate by obstacle padding
        bounds = bounds.expanded(config_.obstacle_padding);
        obstacles.push_back(bounds);
    }

    return obstacles;
}

auto ConnectorRoutingService::anchor_position(const AABB& bounds, AnchorPosition anchor) -> Point2D
{
    const auto ctr = bounds.center();
    const double hw = bounds.width() / 2.0;
    const double hh = bounds.height() / 2.0;

    switch (anchor)
    {
        case AnchorPosition::kTop:
            return {ctr.x, ctr.y - hh};
        case AnchorPosition::kBottom:
            return {ctr.x, ctr.y + hh};
        case AnchorPosition::kLeft:
            return {ctr.x - hw, ctr.y};
        case AnchorPosition::kRight:
            return {ctr.x + hw, ctr.y};
        case AnchorPosition::kCenter:
        case AnchorPosition::kAuto:
            return ctr;
    }
    return ctr;
}

auto ConnectorRoutingService::route_orthogonal(const Point2D& start_point,
                                               const Point2D& end_point,
                                               const std::vector<AABB>& /*obstacles*/) const
    -> std::vector<Point2D>
{
    // Simple L-shaped or Z-shaped orthogonal routing
    std::vector<Point2D> waypoints;

    const double route_dx = end_point.x - start_point.x;
    const double route_dy = end_point.y - start_point.y;

    if (std::abs(route_dx) < config_.min_segment_length ||
        std::abs(route_dy) < config_.min_segment_length)
    {
        // Nearly aligned — single bend (L-shape)
        waypoints.push_back({end_point.x, start_point.y});
    }
    else
    {
        // Z-shape: midpoint horizontal, then vertical
        const double mid_x = (start_point.x + end_point.x) / 2.0;
        waypoints.push_back({mid_x, start_point.y});
        waypoints.push_back({mid_x, end_point.y});
    }

    return waypoints;
}

auto ConnectorRoutingService::route_curved(const Point2D& start_point, const Point2D& end_point)
    -> std::vector<Point2D>
{
    // Generate control points for a cubic Bézier curve
    const double route_dx = end_point.x - start_point.x;
    const double route_dy = end_point.y - start_point.y;
    constexpr double kControlPointRatio = 0.4;

    std::vector<Point2D> control_points;
    // Control point 1: extend horizontally from start
    control_points.push_back({start_point.x + route_dx * kControlPointRatio, start_point.y});
    // Control point 2: extend horizontally from end (backwards)
    control_points.push_back(
        {end_point.x - route_dx * kControlPointRatio, end_point.y - route_dy * kControlPointRatio});

    return control_points;
}

auto ConnectorRoutingService::score_anchor(const AABB& bounds,
                                           AnchorPosition anchor,
                                           const Point2D& from_point) const -> double
{
    const auto anchor_pos = anchor_position(bounds, anchor);
    const auto ctr = bounds.center();

    // Compute direction from center to from_point
    const double center_dx = from_point.x - ctr.x;
    const double center_dy = from_point.y - ctr.y;

    // Compute direction from center to anchor
    const double anchor_dx = anchor_pos.x - ctr.x;
    const double anchor_dy = anchor_pos.y - ctr.y;

    // Dot product → how aligned the anchor is with the incoming direction
    const double center_len = std::sqrt(center_dx * center_dx + center_dy * center_dy);
    const double anchor_len = std::sqrt(anchor_dx * anchor_dx + anchor_dy * anchor_dy);

    if (center_len < 1e-6 || anchor_len < 1e-6)
    {
        return 0.5;
    }

    const double dot_product = (center_dx / center_len) * (anchor_dx / anchor_len) +
                               (center_dy / center_len) * (anchor_dy / anchor_len);

    // Normalize to 0.0–1.0 (dot product is -1 to 1)
    return (dot_product + 1.0) / 2.0;
}

} // namespace markamp::canvas
