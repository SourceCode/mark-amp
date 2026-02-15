#pragma once

#include "CanvasTypes.h"

namespace markamp::canvas
{

/// Manages the mapping between world coordinates and screen coordinates.
/// Tracks zoom level, pan offset, and screen dimensions.
class ViewportTransform
{
public:
    ViewportTransform() = default;

    /// Set the screen (panel) size in pixels.
    auto set_screen_size(double width, double height) -> void;

    /// Current zoom level (1.0 = 100%).
    [[nodiscard]] auto zoom() const -> double;
    auto set_zoom(double z) -> void;

    /// Zoom centered on a screen point.
    auto zoom_at(double screen_x, double screen_y, double new_zoom) -> void;

    /// Pan offset in world coordinates.
    [[nodiscard]] auto pan() const -> Point2D;
    auto set_pan(const Point2D& p) -> void;
    auto pan_by(double dx, double dy) -> void;

    /// Convert world coords to screen coords.
    [[nodiscard]] auto world_to_screen(const Point2D& world) const -> Point2D;
    [[nodiscard]] auto world_to_screen(const AABB& world) const -> AABB;

    /// Convert screen coords to world coords.
    [[nodiscard]] auto screen_to_world(const Point2D& screen) const -> Point2D;
    [[nodiscard]] auto screen_to_world(const AABB& screen) const -> AABB;

    /// Visible world-space region (the viewport).
    [[nodiscard]] auto visible_region() const -> AABB;

    /// Screen dimensions.
    [[nodiscard]] auto screen_width() const -> double;
    [[nodiscard]] auto screen_height() const -> double;

    /// Zoom constraints.
    static constexpr double kMinZoom = 0.05; // 5%
    static constexpr double kMaxZoom = 20.0; // 2000%

    /// Fit a world-space AABB into the current screen, adjusting zoom and pan.
    auto fit_to_bounds(const AABB& world_bounds, double padding = 50.0) -> void;

private:
    double zoom_{1.0};
    Point2D pan_{0.0, 0.0};
    double screen_width_{800.0};
    double screen_height_{600.0};
};

} // namespace markamp::canvas
