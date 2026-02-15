#include "ViewportTransform.h"

#include <algorithm>
#include <cmath>

namespace markamp::canvas
{

auto ViewportTransform::set_screen_size(double width, double height) -> void
{
    screen_width_ = width;
    screen_height_ = height;
}

auto ViewportTransform::zoom() const -> double
{
    return zoom_;
}

auto ViewportTransform::set_zoom(double z) -> void
{
    zoom_ = std::clamp(z, kMinZoom, kMaxZoom);
}

auto ViewportTransform::zoom_at(double screen_x, double screen_y, double new_zoom) -> void
{
    // Adjust pan so the point under the cursor stays fixed.
    new_zoom = std::clamp(new_zoom, kMinZoom, kMaxZoom);

    // World point under cursor before zoom change.
    const Point2D world_before = screen_to_world(Point2D{screen_x, screen_y});

    zoom_ = new_zoom;

    // World point under cursor after zoom change (with old pan).
    const Point2D world_after = screen_to_world(Point2D{screen_x, screen_y});

    // Adjust pan to keep the world point fixed.
    pan_.x -= (world_after.x - world_before.x);
    pan_.y -= (world_after.y - world_before.y);
}

auto ViewportTransform::pan() const -> Point2D
{
    return pan_;
}

auto ViewportTransform::set_pan(const Point2D& p) -> void
{
    pan_ = p;
}

auto ViewportTransform::pan_by(double dx, double dy) -> void
{
    pan_.x += dx;
    pan_.y += dy;
}

auto ViewportTransform::world_to_screen(const Point2D& world) const -> Point2D
{
    return {(world.x - pan_.x) * zoom_, (world.y - pan_.y) * zoom_};
}

auto ViewportTransform::world_to_screen(const AABB& world) const -> AABB
{
    const auto top_left = world_to_screen(Point2D{world.min_x, world.min_y});
    const auto bottom_right = world_to_screen(Point2D{world.max_x, world.max_y});
    return {top_left.x, top_left.y, bottom_right.x, bottom_right.y};
}

auto ViewportTransform::screen_to_world(const Point2D& screen) const -> Point2D
{
    return {screen.x / zoom_ + pan_.x, screen.y / zoom_ + pan_.y};
}

auto ViewportTransform::screen_to_world(const AABB& screen) const -> AABB
{
    const auto top_left = screen_to_world(Point2D{screen.min_x, screen.min_y});
    const auto bottom_right = screen_to_world(Point2D{screen.max_x, screen.max_y});
    return {top_left.x, top_left.y, bottom_right.x, bottom_right.y};
}

auto ViewportTransform::visible_region() const -> AABB
{
    return screen_to_world(AABB{0.0, 0.0, screen_width_, screen_height_});
}

auto ViewportTransform::screen_width() const -> double
{
    return screen_width_;
}

auto ViewportTransform::screen_height() const -> double
{
    return screen_height_;
}

auto ViewportTransform::fit_to_bounds(const AABB& world_bounds, double padding) -> void
{
    if (!world_bounds.is_valid())
    {
        return;
    }

    const double content_width = world_bounds.width();
    const double content_height = world_bounds.height();

    if (content_width <= 0.0 || content_height <= 0.0)
    {
        return;
    }

    // Compute zoom to fit content + padding into screen.
    const double available_width = screen_width_ - 2.0 * padding;
    const double available_height = screen_height_ - 2.0 * padding;

    if (available_width <= 0.0 || available_height <= 0.0)
    {
        return;
    }

    const double zoom_x = available_width / content_width;
    const double zoom_y = available_height / content_height;

    zoom_ = std::clamp(std::min(zoom_x, zoom_y), kMinZoom, kMaxZoom);

    // Center the content in the viewport.
    const auto content_center = world_bounds.center();
    pan_.x = content_center.x - (screen_width_ / zoom_) / 2.0;
    pan_.y = content_center.y - (screen_height_ / zoom_) / 2.0;
}

} // namespace markamp::canvas
