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

    // --- Navigation helpers (#17–22) ---
    /// Zoom to fit a selection AABB with padding.
    auto zoom_to_fit_selection(const AABB& selection, double padding = 60.0) -> void;
    /// Set zoom from a percentage (e.g. 150 = 150%).
    auto zoom_to_percent(double percent) -> void;
    /// Step zoom in by kZoomStep.
    auto zoom_in() -> void;
    /// Step zoom out by kZoomStep.
    auto zoom_out() -> void;
    /// Pan so that the given world point is centered on screen.
    auto center_on_point(const Point2D& world_pt) -> void;
    /// Returns true if a world point falls within the visible region.
    [[nodiscard]] auto is_point_visible(const Point2D& world_pt) const -> bool;
    /// Returns zoom as a percentage for display.
    [[nodiscard]] auto zoom_percent() const -> double;

    static constexpr double kZoomStep = 0.1;

    // ── Batch 3 (#13-16) ──────────────────────────────────────────

    /// Center and zoom to fit a single object's bounds with padding.
    auto zoom_to_object(const AABB& obj_bounds, double padding = 80.0) -> void;

    /// Reset zoom to exactly 1.0 (100%) while keeping pan unchanged.
    auto zoom_to_100() -> void;

    /// Return the world-space point at the center of the screen.
    [[nodiscard]] auto screen_center_world() const -> Point2D;

    /// Constrain pan so the view doesn't drift far from content.
    auto clamp_pan(const AABB& content_bounds, double margin = 500.0) -> void;

    /// Whether the viewport is zoomed in (zoom > 1.0).
    [[nodiscard]] auto is_zoomed_in() const noexcept -> bool
    {
        return zoom_ > 1.0;
    }

    /// Whether the viewport is zoomed out (zoom < 1.0).
    [[nodiscard]] auto is_zoomed_out() const noexcept -> bool
    {
        return zoom_ < 1.0;
    }

    /// Whether the viewport is at default zoom (1.0).
    [[nodiscard]] auto is_default_zoom() const noexcept -> bool
    {
        return zoom_ == 1.0;
    }

    /// Total screen area in pixels².
    [[nodiscard]] auto screen_area() const noexcept -> double
    {
        return screen_width_ * screen_height_;
    }

    /// Total zoom range (max - min).
    [[nodiscard]] static constexpr auto zoom_range() noexcept -> double
    {
        return kMaxZoom - kMinZoom;
    }

    // ── Batch 4 (#31-40) ──────────────────────────────────────────

    /// (#31) Whether an AABB intersects the visible region.
    [[nodiscard]] auto is_region_visible(const AABB& region) const -> bool
    {
        return visible_region().intersects(region);
    }

    /// (#32) Shortcut to zoom to 200%.
    auto zoom_to_200() -> void
    {
        zoom_to_percent(200.0);
    }

    /// (#33) Shortcut to zoom to 50%.
    auto zoom_to_50() -> void
    {
        zoom_to_percent(50.0);
    }

    /// (#34) Screen size as a Size2D.
    [[nodiscard]] auto screen_size() const noexcept -> Size2D
    {
        return {screen_width_, screen_height_};
    }

    /// (#35) Area of the visible region in world units².
    [[nodiscard]] auto visible_area() const -> double
    {
        return visible_region().area();
    }

    /// (#36) Formatted zoom label string like "150%".
    [[nodiscard]] auto zoom_level_label() const -> std::string
    {
        return std::to_string(static_cast<int>(zoom_ * 100.0)) + "%";
    }

    /// (#37) Whether the zoom is at the minimum allowed.
    [[nodiscard]] auto is_at_min_zoom() const noexcept -> bool
    {
        return zoom_ <= kMinZoom;
    }

    /// (#38) Whether the zoom is at the maximum allowed.
    [[nodiscard]] auto is_at_max_zoom() const noexcept -> bool
    {
        return zoom_ >= kMaxZoom;
    }

    /// (#39) Individual pan X coordinate.
    [[nodiscard]] auto pan_x() const noexcept -> double
    {
        return pan_.x;
    }

    /// (#39) Individual pan Y coordinate.
    [[nodiscard]] auto pan_y() const noexcept -> double
    {
        return pan_.y;
    }

    /// (#40) Reset pan to origin.
    auto reset_pan() -> void
    {
        pan_ = {0.0, 0.0};
    }

private:
    double zoom_{1.0};
    Point2D pan_{0.0, 0.0};
    double screen_width_{800.0};
    double screen_height_{600.0};
};

} // namespace markamp::canvas
