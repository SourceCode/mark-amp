// ============================================================================
// File: src/canvas/ConnectorRoutingService.h
// Phase 12: Canvas Advanced Objects — intelligent connector routing
// ============================================================================
#pragma once

#include "canvas/Board.h"
#include "canvas/CanvasTypes.h"
#include "canvas/ConnectorData.h"

#include <cstdint>
#include <vector>

namespace markamp::canvas
{

/// Configuration for connector routing behavior.
struct RoutingConfig
{
    double min_segment_length{20.0};   ///< Minimum length for orthogonal segments
    double obstacle_padding{10.0};     ///< Padding around obstacles
    double anchor_snap_distance{15.0}; ///< Snap distance for anchor points
    bool avoid_objects{true};          ///< Whether to route around objects
    bool prefer_straight{true};        ///< Prefer straighter paths when possible
    int max_waypoints{20};             ///< Maximum waypoints for a single route
};

/// Result of a routing computation.
struct RoutingResult
{
    bool success{false};
    std::vector<Point2D> waypoints; ///< Computed waypoints for the route
    AnchorPosition start_anchor{AnchorPosition::kAuto};
    AnchorPosition end_anchor{AnchorPosition::kAuto};
    double total_length{0.0};
};

/// Anchor suggestion for a connector endpoint.
struct AnchorSuggestion
{
    AnchorPosition anchor;
    Point2D position;
    double score{0.0}; ///< Higher = better match (0.0–1.0)
};

/// Service for intelligent connector routing with obstacle avoidance.
///
/// Provides:
///   - Orthogonal routing with obstacle avoidance
///   - Automatic anchor point selection
///   - Batch re-routing when board changes
///   - Snap-to-anchor for interactive dragging
class ConnectorRoutingService
{
public:
    explicit ConnectorRoutingService(const Board& board);
    ConnectorRoutingService(const Board& board, RoutingConfig config);

    // ── Single Connector Routing ──────────────────────────────────

    /// Compute an optimal route for a connector.
    [[nodiscard]] auto route_connector(const ConnectorObject& connector) const -> RoutingResult;

    /// Compute a route between two specific points.
    [[nodiscard]] auto route_between(const Point2D& start_point,
                                     const Point2D& end_point,
                                     ConnectorRouting mode) const -> RoutingResult;

    // ── Batch Routing ─────────────────────────────────────────────

    /// Re-route all connectors on the board.
    [[nodiscard]] auto reroute_all() const -> std::vector<std::pair<ObjectId, RoutingResult>>;

    /// Re-route connectors attached to a specific object (e.g. after move).
    [[nodiscard]] auto reroute_attached_to(ObjectId obj_id) const
        -> std::vector<std::pair<ObjectId, RoutingResult>>;

    // ── Anchor Selection ──────────────────────────────────────────

    /// Determine the best anchor point on a target object for a connector
    /// coming from a given direction.
    [[nodiscard]] auto auto_anchor(ObjectId target_id, const Point2D& from_point) const
        -> AnchorSuggestion;

    /// Get all anchor suggestions for a target object from a given direction,
    /// sorted by score (best first).
    [[nodiscard]] auto anchor_suggestions(ObjectId target_id, const Point2D& from_point) const
        -> std::vector<AnchorSuggestion>;

    /// Snap a point to the nearest anchor on any nearby object.
    [[nodiscard]] auto snap_to_anchor(const Point2D& point) const
        -> std::optional<AnchorSuggestion>;

    // ── Configuration ─────────────────────────────────────────────

    [[nodiscard]] auto config() const -> const RoutingConfig&;
    auto set_config(const RoutingConfig& config) -> void;

private:
    const Board& board_;
    RoutingConfig config_;

    /// Collect obstacle AABBs from the board, excluding specific objects.
    [[nodiscard]] auto collect_obstacles(const std::vector<ObjectId>& exclude) const
        -> std::vector<AABB>;

    /// Compute the anchor point position on an object's bounds.
    [[nodiscard]] static auto anchor_position(const AABB& bounds, AnchorPosition anchor) -> Point2D;

    /// Route orthogonally between two points avoiding obstacles.
    [[nodiscard]] auto route_orthogonal(const Point2D& start_point,
                                        const Point2D& end_point,
                                        const std::vector<AABB>& obstacles) const
        -> std::vector<Point2D>;

    /// Route with a cubic Bézier curve between two points.
    [[nodiscard]] static auto route_curved(const Point2D& start_point, const Point2D& end_point)
        -> std::vector<Point2D>;

    /// Score an anchor position based on direction and clearance.
    [[nodiscard]] auto score_anchor(const AABB& bounds,
                                    AnchorPosition anchor,
                                    const Point2D& from_point) const -> double;
};

} // namespace markamp::canvas
