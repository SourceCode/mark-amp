#pragma once

/**
 * @file ConnectorRouteEngine.h
 * @brief Phase 44 Task 2-4: Connector routing modes and label attachment.
 *
 * Route modes (straight/orthogonal/curved), bend point editing,
 * inline label attachment with offset, route recomputation.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Routing mode for connectors.
enum class RouteMode : uint8_t
{
    kStraight,
    kOrthogonal,
    kCurved,
};

/// A waypoint / bend point in a route.
struct RoutePoint
{
    double pos_x{0.0};
    double pos_y{0.0};
    bool is_user_defined{false}; ///< Manual vs auto-generated
};

/// A label attached to a connector.
struct ConnectorLabel
{
    std::string text;
    double offset{0.5};         ///< 0.0=source, 1.0=target, 0.5=midpoint
    double lateral_offset{0.0}; ///< Perpendicular offset from line
};

/**
 * @brief Computes and manages connector routes with bend editing.
 */
class ConnectorRouteEngine
{
public:
    ConnectorRouteEngine() = default;

    // ── Route mode ─────────────────────────────────────────────────

    /// Set the routing mode.
    void set_mode(RouteMode mode);

    /// Get the routing mode.
    [[nodiscard]] auto mode() const -> RouteMode;

    /// Get mode as string.
    [[nodiscard]] static auto mode_name(RouteMode mode) -> std::string;

    // ── Route computation ──────────────────────────────────────────

    /// Compute route between two points.
    void compute_route(double src_x, double src_y, double dst_x, double dst_y);

    /// Get the computed route points.
    [[nodiscard]] auto route_points() const -> const std::vector<RoutePoint>&;

    /// Get route point count.
    [[nodiscard]] auto point_count() const -> int;

    // ── Bend editing ───────────────────────────────────────────────

    /// Add a user-defined bend point at an index.
    void add_bend(int index, double pos_x, double pos_y);

    /// Remove a bend point at an index.
    void remove_bend(int index);

    /// Move a bend point.
    void move_bend(int index, double pos_x, double pos_y);

    /// Get user-defined bend count.
    [[nodiscard]] auto bend_count() const -> int;

    // ── Labels ─────────────────────────────────────────────────────

    /// Set the connector label.
    void set_label(const ConnectorLabel& label);

    /// Get the label.
    [[nodiscard]] auto label() const -> const ConnectorLabel&;

    /// Check if a label is set.
    [[nodiscard]] auto has_label() const -> bool;

    /// Compute label position on the route.
    struct LabelPosition
    {
        double pos_x{0.0};
        double pos_y{0.0};
    };

    [[nodiscard]] auto compute_label_position() const -> LabelPosition;

private:
    RouteMode mode_{RouteMode::kStraight};
    std::vector<RoutePoint> points_;
    ConnectorLabel label_;
    bool has_label_{false};
};

} // namespace markamp::canvas
