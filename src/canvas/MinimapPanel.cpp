#include "MinimapPanel.h"

#include <algorithm>

namespace markamp::canvas
{

// ── Data setters ───────────────────────────────────────────────────────

auto MinimapPanel::set_content_bounds(const AABB& bounds) -> void
{
    content_bounds_ = bounds;
}

auto MinimapPanel::set_viewport(const AABB& visible_region) -> void
{
    viewport_region_ = visible_region;
}

auto MinimapPanel::set_entries(const std::vector<MinimapEntry>& entries) -> void
{
    entries_ = entries;
}

// ── Coordinate mapping ─────────────────────────────────────────────────

auto MinimapPanel::compute_scale() const -> double
{
    const double content_w = content_bounds_.max_x - content_bounds_.min_x;
    const double content_h = content_bounds_.max_y - content_bounds_.min_y;

    if (content_w <= 0.0 || content_h <= 0.0)
    {
        return 1.0;
    }

    const double usable_w = panel_width_ * (1.0 - 2.0 * kMarginFraction);
    const double usable_h = panel_height_ * (1.0 - 2.0 * kMarginFraction);

    return std::min(usable_w / content_w, usable_h / content_h);
}

auto MinimapPanel::world_to_minimap(const Point2D& world) const -> Point2D
{
    const double scale = compute_scale();
    const double margin_x = panel_width_ * kMarginFraction;
    const double margin_y = panel_height_ * kMarginFraction;

    return {margin_x + (world.x - content_bounds_.min_x) * scale,
            margin_y + (world.y - content_bounds_.min_y) * scale};
}

auto MinimapPanel::minimap_to_world(const Point2D& minimap) const -> Point2D
{
    const double scale = compute_scale();
    const double margin_x = panel_width_ * kMarginFraction;
    const double margin_y = panel_height_ * kMarginFraction;

    if (scale <= 0.0)
    {
        return {0.0, 0.0};
    }

    return {content_bounds_.min_x + (minimap.x - margin_x) / scale,
            content_bounds_.min_y + (minimap.y - margin_y) / scale};
}

// ── Interaction ────────────────────────────────────────────────────────

auto MinimapPanel::handle_click(double map_x, double map_y) -> bool
{
    if (!visible_)
    {
        return false;
    }
    if (map_x < 0 || map_x > panel_width_ || map_y < 0 || map_y > panel_height_)
    {
        return false;
    }

    auto world_point = minimap_to_world({map_x, map_y});
    if (on_navigate_)
    {
        on_navigate_(world_point);
    }
    return true;
}

auto MinimapPanel::handle_drag(double map_x, double map_y) -> bool
{
    if (!visible_)
    {
        return false;
    }
    dragging_ = true;
    auto world_point = minimap_to_world({map_x, map_y});
    if (on_navigate_)
    {
        on_navigate_(world_point);
    }
    return true;
}

// ── Navigation callback ────────────────────────────────────────────────

auto MinimapPanel::set_on_navigate(OnNavigateCallback callback) -> void
{
    on_navigate_ = std::move(callback);
}

// ── Panel geometry ─────────────────────────────────────────────────────

auto MinimapPanel::set_panel_size(double width, double height) -> void
{
    panel_width_ = width;
    panel_height_ = height;
}

auto MinimapPanel::panel_width() const -> double
{
    return panel_width_;
}
auto MinimapPanel::panel_height() const -> double
{
    return panel_height_;
}

// ── Visibility ─────────────────────────────────────────────────────────

auto MinimapPanel::is_visible() const -> bool
{
    return visible_;
}
auto MinimapPanel::set_visible(bool visible) -> void
{
    visible_ = visible;
}

// ── Accessors ──────────────────────────────────────────────────────────

auto MinimapPanel::content_bounds() const -> const AABB&
{
    return content_bounds_;
}
auto MinimapPanel::viewport_region() const -> const AABB&
{
    return viewport_region_;
}
auto MinimapPanel::entries() const -> const std::vector<MinimapEntry>&
{
    return entries_;
}

// ── Batch 7 (#41-42) ──────────────────────────────────────────────

auto MinimapPanel::zoom_factor() const -> double
{
    return zoom_factor_;
}

auto MinimapPanel::set_zoom_factor(double factor) -> void
{
    zoom_factor_ = std::max(0.1, std::min(factor, 10.0));
}

auto MinimapPanel::highlight_objects(const std::vector<ObjectId>& ids) -> void
{
    highlighted_ids_ = ids;
}

auto MinimapPanel::highlighted_ids() const -> const std::vector<ObjectId>&
{
    return highlighted_ids_;
}

} // namespace markamp::canvas
