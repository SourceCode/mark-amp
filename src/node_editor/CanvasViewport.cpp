#include "CanvasViewport.h"

#include <cmath>

namespace markamp::node_editor
{

CanvasViewport::CanvasViewport()
    : config_()
    , pan_offset_{0.0F, 0.0F}
    , zoom_level_(config_.default_zoom)
    , viewport_size_{800.0F, 600.0F}
{
}

CanvasViewport::CanvasViewport(ViewportConfig config)
    : config_(config)
    , pan_offset_{0.0F, 0.0F}
    , zoom_level_(config.default_zoom)
    , viewport_size_{800.0F, 600.0F}
{
}

auto CanvasViewport::pan() const noexcept -> Vec2
{
    return pan_offset_;
}

auto CanvasViewport::zoom() const noexcept -> float
{
    return zoom_level_;
}

auto CanvasViewport::config() const noexcept -> const ViewportConfig&
{
    return config_;
}

void CanvasViewport::set_viewport_size(float screen_width, float screen_height)
{
    viewport_size_ = {screen_width, screen_height};
}

auto CanvasViewport::viewport_size() const noexcept -> Vec2
{
    return viewport_size_;
}

void CanvasViewport::pan_by(Vec2 delta_screen)
{
    pan_offset_.x += delta_screen.x / zoom_level_;
    pan_offset_.y += delta_screen.y / zoom_level_;
}

void CanvasViewport::set_pan(Vec2 world_offset)
{
    pan_offset_ = world_offset;
}

void CanvasViewport::set_zoom(float level)
{
    zoom_level_ = std::clamp(level, config_.min_zoom, config_.max_zoom);
}

void CanvasViewport::zoom_at(Vec2 screen_point, float delta)
{
    // Zoom centered on screen_point: world position under cursor stays fixed.
    const Vec2 kWorldBefore = screen_to_world(screen_point);

    const float kNewZoom = std::clamp(
        zoom_level_ * (1.0F + delta * config_.zoom_speed), config_.min_zoom, config_.max_zoom);
    zoom_level_ = kNewZoom;

    const Vec2 kWorldAfter = screen_to_world(screen_point);
    pan_offset_.x += (kWorldAfter.x - kWorldBefore.x);
    pan_offset_.y += (kWorldAfter.y - kWorldBefore.y);
}

auto CanvasViewport::screen_to_world(Vec2 screen_pos) const noexcept -> Vec2
{
    return {screen_pos.x / zoom_level_ - pan_offset_.x, screen_pos.y / zoom_level_ - pan_offset_.y};
}

auto CanvasViewport::world_to_screen(Vec2 world_pos) const noexcept -> Vec2
{
    return {(world_pos.x + pan_offset_.x) * zoom_level_,
            (world_pos.y + pan_offset_.y) * zoom_level_};
}

auto CanvasViewport::screen_to_world_rect(Rect screen_rect) const noexcept -> Rect
{
    const Vec2 kTopLeft = screen_to_world({screen_rect.x, screen_rect.y});
    const Vec2 kBottomRight = screen_to_world({screen_rect.right(), screen_rect.bottom()});
    return {kTopLeft.x, kTopLeft.y, kBottomRight.x - kTopLeft.x, kBottomRight.y - kTopLeft.y};
}

auto CanvasViewport::world_to_screen_rect(Rect world_rect) const noexcept -> Rect
{
    const Vec2 kTopLeft = world_to_screen({world_rect.x, world_rect.y});
    const Vec2 kBottomRight = world_to_screen({world_rect.right(), world_rect.bottom()});
    return {kTopLeft.x, kTopLeft.y, kBottomRight.x - kTopLeft.x, kBottomRight.y - kTopLeft.y};
}

auto CanvasViewport::visible_world_rect() const noexcept -> Rect
{
    const Vec2 kTopLeft = screen_to_world({0.0F, 0.0F});
    const Vec2 kBottomRight = screen_to_world(viewport_size_);
    return {kTopLeft.x, kTopLeft.y, kBottomRight.x - kTopLeft.x, kBottomRight.y - kTopLeft.y};
}

void CanvasViewport::zoom_to_fit(Rect world_bounds, float padding)
{
    if (world_bounds.width <= 0.0F || world_bounds.height <= 0.0F)
    {
        return;
    }

    const float kPaddedWidth = world_bounds.width + padding * 2.0F;
    const float kPaddedHeight = world_bounds.height + padding * 2.0F;

    const float kZoomX = viewport_size_.x / kPaddedWidth;
    const float kZoomY = viewport_size_.y / kPaddedHeight;
    zoom_level_ = std::clamp(std::min(kZoomX, kZoomY), config_.min_zoom, config_.max_zoom);

    // Center the bounds in the viewport
    const Vec2 kBoundsCenter = world_bounds.center();
    pan_offset_.x = (viewport_size_.x / (2.0F * zoom_level_)) - kBoundsCenter.x;
    pan_offset_.y = (viewport_size_.y / (2.0F * zoom_level_)) - kBoundsCenter.y;
}

void CanvasViewport::reset()
{
    pan_offset_ = {0.0F, 0.0F};
    zoom_level_ = config_.default_zoom;
}

auto CanvasViewport::snap_to_grid(Vec2 world_pos) const noexcept -> Vec2
{
    const float kMinorSpacing = config_.grid_major_spacing / config_.grid_minor_divisions;
    return {std::round(world_pos.x / kMinorSpacing) * kMinorSpacing,
            std::round(world_pos.y / kMinorSpacing) * kMinorSpacing};
}

auto CanvasViewport::grid_major_spacing_screen() const noexcept -> float
{
    return config_.grid_major_spacing * zoom_level_;
}

auto CanvasViewport::grid_minor_spacing_screen() const noexcept -> float
{
    return (config_.grid_major_spacing / config_.grid_minor_divisions) * zoom_level_;
}

} // namespace markamp::node_editor
