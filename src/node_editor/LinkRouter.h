#pragma once

// V11 Phase 25: Link Creation Reroute And Cable Routing System
// Computes visual paths between socket positions for link rendering.
// No wxWidgets dependency.

#include "NodeEditorTypes.h"

#include <vector>

namespace markamp::node_editor
{

/// Cable routing mode for link rendering.
enum class RoutingMode : uint8_t
{
    Bezier,
    Straight,
    Orthogonal
};

/// A computed link path: a sequence of control points for rendering.
struct LinkPath
{
    Vec2 start;
    Vec2 end;
    std::vector<Vec2> control_points;
    RoutingMode mode{RoutingMode::Bezier};
};

/// Computes visual cable paths between socket positions.
class LinkRouter
{
public:
    LinkRouter();
    explicit LinkRouter(RoutingMode default_mode);

    /// Compute path between two socket positions.
    [[nodiscard]] auto compute_path(Vec2 from, Vec2 to_pos) const -> LinkPath;

    /// Compute path with explicit routing mode override.
    [[nodiscard]] auto compute_path(Vec2 from, Vec2 to_pos, RoutingMode mode) const -> LinkPath;

    /// Insert a reroute midpoint into an existing path.
    [[nodiscard]] static auto insert_reroute(const LinkPath& path, Vec2 reroute_pos) -> LinkPath;

    /// Get/set default routing mode.
    [[nodiscard]] auto mode() const noexcept -> RoutingMode;
    void set_mode(RoutingMode routing_mode);

    /// Curvature factor for Bézier routing (0.0 = straight, 1.0 = max curve).
    [[nodiscard]] auto curvature() const noexcept -> float;
    void set_curvature(float curvature_factor);

private:
    RoutingMode mode_{RoutingMode::Bezier};
    float curvature_{0.5F};

    [[nodiscard]] auto compute_bezier(Vec2 from, Vec2 to_pos) const -> LinkPath;
    [[nodiscard]] static auto compute_straight(Vec2 from, Vec2 to_pos) -> LinkPath;
    [[nodiscard]] static auto compute_orthogonal(Vec2 from, Vec2 to_pos) -> LinkPath;
};

} // namespace markamp::node_editor
