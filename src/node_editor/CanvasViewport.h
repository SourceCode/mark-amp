#pragma once

// V11 Phase 22: Canvas Viewport Grid And Zoom Model For Nodes
// Pure-math viewport with pan/zoom, coordinate transforms, grid snapping, and visible-rect queries.
// No wxWidgets dependency — fully testable headlessly.

#include "NodeEditorTypes.h"

#include <algorithm>

namespace markamp::node_editor
{

/// Configurable constraints for the viewport zoom range and grid parameters.
struct ViewportConfig
{
    float min_zoom{0.1F};
    float max_zoom{10.0F};
    float default_zoom{1.0F};
    float grid_major_spacing{100.0F};
    float grid_minor_divisions{4.0F};
    float snap_threshold{8.0F}; // Distance in pixels to snap to grid
    float zoom_speed{0.1F};     // Scroll wheel zoom multiplier
};

/// Canvas viewport: manages pan offset, zoom level, and coordinate transforms.
class CanvasViewport
{
public:
    CanvasViewport();
    explicit CanvasViewport(ViewportConfig config);

    // --- Accessors ---
    [[nodiscard]] auto pan() const noexcept -> Vec2;
    [[nodiscard]] auto zoom() const noexcept -> float;
    [[nodiscard]] auto config() const noexcept -> const ViewportConfig&;

    // --- Viewport size (screen dimensions) ---
    void set_viewport_size(float screen_width, float screen_height);
    [[nodiscard]] auto viewport_size() const noexcept -> Vec2;

    // --- Pan controls ---
    void pan_by(Vec2 delta_screen);
    void set_pan(Vec2 world_offset);

    // --- Zoom controls ---
    void set_zoom(float level);
    void zoom_at(Vec2 screen_point, float delta);

    // --- Coordinate transforms ---
    [[nodiscard]] auto screen_to_world(Vec2 screen_pos) const noexcept -> Vec2;
    [[nodiscard]] auto world_to_screen(Vec2 world_pos) const noexcept -> Vec2;
    [[nodiscard]] auto screen_to_world_rect(Rect screen_rect) const noexcept -> Rect;
    [[nodiscard]] auto world_to_screen_rect(Rect world_rect) const noexcept -> Rect;

    // --- Visible area ---
    [[nodiscard]] auto visible_world_rect() const noexcept -> Rect;

    // --- Zoom-to-fit ---
    void zoom_to_fit(Rect world_bounds, float padding = 20.0F);
    void reset();

    // --- Grid snapping ---
    [[nodiscard]] auto snap_to_grid(Vec2 world_pos) const noexcept -> Vec2;
    [[nodiscard]] auto grid_major_spacing_screen() const noexcept -> float;
    [[nodiscard]] auto grid_minor_spacing_screen() const noexcept -> float;

private:
    ViewportConfig config_;
    Vec2 pan_offset_;
    float zoom_level_{1.0F};
    Vec2 viewport_size_;
};

} // namespace markamp::node_editor
