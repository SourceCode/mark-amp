#include "CanvasRenderer.h"

#include <wx/graphics.h>
#include <wx/wx.h>

#include <algorithm>
#include <chrono>

namespace markamp::canvas
{

auto CanvasRenderer::register_renderer(std::unique_ptr<IObjectRenderer> renderer) -> void
{
    const auto type_key = static_cast<uint8_t>(renderer->object_type());
    renderers_[type_key] = std::move(renderer);
}

auto CanvasRenderer::has_renderer(CanvasObjectType type) const -> bool
{
    return renderers_.contains(static_cast<uint8_t>(type));
}

auto CanvasRenderer::render_frame(wxGraphicsContext& gc,
                                  const std::vector<CanvasObject*>& objects,
                                  const ViewportTransform& viewport,
                                  const std::vector<ObjectId>& selected_ids) -> RenderStats
{
    const auto start = std::chrono::high_resolution_clock::now();
    RenderStats stats;

    render_background(gc, viewport);
    render_grid(gc, viewport);

    const auto visible = viewport.visible_region();

    for (const auto* obj : objects)
    {
        if (!obj || !obj->is_visible())
        {
            continue;
        }

        // Viewport culling: skip objects entirely outside the visible region.
        const auto world_box = obj->world_bounds();
        if (!world_box.intersects(visible))
        {
            ++stats.objects_culled;
            continue;
        }

        const bool selected =
            std::find(selected_ids.begin(), selected_ids.end(), obj->id()) != selected_ids.end();

        const auto type_key = static_cast<uint8_t>(obj->type());
        const auto it = renderers_.find(type_key);
        if (it != renderers_.end())
        {
            it->second->render(gc, *obj, viewport, selected);
        }
        else
        {
            render_wireframe(gc, *obj, viewport);
        }

        ++stats.objects_rendered;
    }

    const auto end = std::chrono::high_resolution_clock::now();
    stats.frame_time_ms = std::chrono::duration<double, std::milli>(end - start).count();

    return stats;
}

auto CanvasRenderer::render_background(wxGraphicsContext& gc, const ViewportTransform& viewport)
    -> void
{
    const auto& bg = grid_settings_.background;
    gc.SetBrush(gc.CreateBrush(wxBrush(wxColour(bg.r, bg.g, bg.b, bg.a))));
    gc.DrawRectangle(0, 0, viewport.screen_width(), viewport.screen_height());
}

auto CanvasRenderer::render_grid(wxGraphicsContext& gc, const ViewportTransform& viewport) -> void
{
    if (!grid_settings_.visible)
    {
        return;
    }

    const double zoom = viewport.zoom();
    const double spacing = grid_settings_.spacing;

    // Adaptive: skip grid when lines would be too close together.
    double effective_spacing = spacing;
    while (effective_spacing * zoom < 10.0)
    {
        effective_spacing *= grid_settings_.major_every;
    }

    const auto visible = viewport.visible_region();
    const auto& col = grid_settings_.color;

    if (grid_settings_.style == GridStyle::kDots)
    {
        gc.SetBrush(gc.CreateBrush(wxBrush(wxColour(col.r, col.g, col.b, col.a))));
        gc.SetPen(*wxTRANSPARENT_PEN);

        const double dot_radius = std::max(1.0, 1.5 * zoom);
        const double start_x = std::floor(visible.min_x / effective_spacing) * effective_spacing;
        const double start_y = std::floor(visible.min_y / effective_spacing) * effective_spacing;

        for (double world_x = start_x; world_x <= visible.max_x; world_x += effective_spacing)
        {
            for (double world_y = start_y; world_y <= visible.max_y; world_y += effective_spacing)
            {
                const auto screen_pt = viewport.world_to_screen(Point2D{world_x, world_y});
                gc.DrawEllipse(screen_pt.x - dot_radius,
                               screen_pt.y - dot_radius,
                               dot_radius * 2,
                               dot_radius * 2);
            }
        }
    }
    else
    {
        // Draw grid lines within the visible region.
        gc.SetPen(gc.CreatePen(wxGraphicsPenInfo(wxColour(col.r, col.g, col.b, col.a)).Width(1)));

        const double start_x = std::floor(visible.min_x / effective_spacing) * effective_spacing;
        const double start_y = std::floor(visible.min_y / effective_spacing) * effective_spacing;

        for (double world_x = start_x; world_x <= visible.max_x; world_x += effective_spacing)
        {
            const auto screen_pt = viewport.world_to_screen(Point2D{world_x, 0.0});
            gc.StrokeLine(screen_pt.x, 0, screen_pt.x, viewport.screen_height());
        }

        for (double world_y = start_y; world_y <= visible.max_y; world_y += effective_spacing)
        {
            const auto screen_pt = viewport.world_to_screen(Point2D{0.0, world_y});
            gc.StrokeLine(0, screen_pt.y, viewport.screen_width(), screen_pt.y);
        }
    }
}

auto CanvasRenderer::grid_settings() const -> const GridSettings&
{
    return grid_settings_;
}

auto CanvasRenderer::set_grid_settings(const GridSettings& settings) -> void
{
    grid_settings_ = settings;
}

auto CanvasRenderer::render_wireframe(wxGraphicsContext& gc,
                                      const CanvasObject& obj,
                                      const ViewportTransform& viewport) -> void
{
    const auto world_box = obj.world_bounds();
    const auto screen_box = viewport.world_to_screen(world_box);

    gc.SetPen(gc.CreatePen(wxGraphicsPenInfo(wxColour(128, 128, 128, 200)).Width(1)));
    gc.SetBrush(gc.CreateBrush(wxBrush(wxColour(200, 200, 200, 50))));
    gc.DrawRectangle(screen_box.min_x, screen_box.min_y, screen_box.width(), screen_box.height());
}

// --- Minimap (#26) ---

auto CanvasRenderer::render_minimap(wxGraphicsContext& gc,
                                    const std::vector<CanvasObject*>& objects,
                                    const ViewportTransform& viewport,
                                    double minimap_width,
                                    double minimap_height) -> void
{
    // Compute world extent of all objects.
    AABB world_extent{0.0, 0.0, 0.0, 0.0};
    bool first = true;
    for (const auto* obj : objects)
    {
        if (obj == nullptr || !obj->is_visible())
        {
            continue;
        }
        if (first)
        {
            world_extent = obj->world_bounds();
            first = false;
        }
        else
        {
            world_extent = world_extent.merged(obj->world_bounds());
        }
    }
    if (first)
    {
        return; // no objects
    }

    // Draw minimap background in bottom-right corner.
    const double margin = 10.0;
    const double map_x = viewport.screen_width() - minimap_width - margin;
    const double map_y = viewport.screen_height() - minimap_height - margin;

    const auto& bg = minimap_settings_.background;
    const auto& border = minimap_settings_.border;
    gc.SetBrush(gc.CreateBrush(wxBrush(wxColour(bg.r, bg.g, bg.b, bg.a))));
    gc.SetPen(
        gc.CreatePen(wxGraphicsPenInfo(wxColour(border.r, border.g, border.b, border.a)).Width(1)));

    // Rounded rect for extra polish
    gc.DrawRoundedRectangle(map_x, map_y, minimap_width, minimap_height, 6.0);

    // Scale factor: world -> minimap.
    const double scale_x = minimap_width / world_extent.width();
    const double scale_y = minimap_height / world_extent.height();
    const double scale = std::min(scale_x, scale_y) * 0.9;

    // Set a clipping region to ensure objects don't draw outside the rounded rect
    gc.Clip(map_x, map_y, minimap_width, minimap_height);

    // Draw each object as a small rect.
    const auto& obj_col = minimap_settings_.object_rect;
    gc.SetBrush(gc.CreateBrush(wxBrush(wxColour(obj_col.r, obj_col.g, obj_col.b, obj_col.a))));
    gc.SetPen(*wxTRANSPARENT_PEN);
    for (const auto* obj : objects)
    {
        if (obj == nullptr || !obj->is_visible())
        {
            continue;
        }
        const auto wb = obj->world_bounds();
        const double rx = map_x + (wb.min_x - world_extent.min_x) * scale;
        const double ry = map_y + (wb.min_y - world_extent.min_y) * scale;
        const double rw = std::max(2.0, wb.width() * scale);
        const double rh = std::max(2.0, wb.height() * scale);
        gc.DrawRoundedRectangle(rx, ry, rw, rh, 1.0);
    }

    // Draw visible region indicator.
    const auto vis = viewport.visible_region();
    const auto& v_col = minimap_settings_.viewport_rect;
    gc.SetBrush(gc.CreateBrush(
        wxBrush(wxColour(v_col.r, v_col.g, v_col.b, static_cast<uint8_t>(v_col.a * 0.15)))));
    gc.SetPen(
        gc.CreatePen(wxGraphicsPenInfo(wxColour(v_col.r, v_col.g, v_col.b, v_col.a)).Width(2)));
    const double vx = map_x + (vis.min_x - world_extent.min_x) * scale;
    const double vy = map_y + (vis.min_y - world_extent.min_y) * scale;
    const double vw = vis.width() * scale;
    const double vh = vis.height() * scale;
    gc.DrawRoundedRectangle(vx, vy, vw, vh, 2.0);

    gc.ResetClip();
}

auto CanvasRenderer::minimap_settings() const -> const MinimapSettings&
{
    return minimap_settings_;
}

auto CanvasRenderer::set_minimap_settings(const MinimapSettings& settings) -> void
{
    minimap_settings_ = settings;
}

// --- Batch 3 (#17-18) ---

auto CanvasRenderer::background_color() const -> const CanvasColor&
{
    return background_color_;
}

auto CanvasRenderer::set_background_color(const CanvasColor& color) -> void
{
    background_color_ = color;
}

auto CanvasRenderer::debug_wireframes() const -> bool
{
    return debug_wireframes_;
}

auto CanvasRenderer::set_debug_wireframes(bool enabled) -> void
{
    debug_wireframes_ = enabled;
}

} // namespace markamp::canvas
