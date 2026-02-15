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

    // Draw grid lines within the visible region.
    const auto& col = grid_settings_.color;
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

} // namespace markamp::canvas
