#include "SelectionRenderer.h"

#include <wx/graphics.h>
#include <wx/wx.h>

#include <cmath>

namespace markamp::canvas
{

auto SelectionRenderer::render_selection_outlines(wxGraphicsContext& ctx,
                                                  const std::vector<AABB>& selected_bounds,
                                                  const ViewportTransform& viewport) -> void
{
    ctx.SetPen(ctx.CreatePen(wxGraphicsPenInfo(wxColour(50, 130, 255, 200)).Width(2)));
    ctx.SetBrush(wxNullBrush);

    for (const auto& world_box : selected_bounds)
    {
        const auto screen = viewport.world_to_screen(world_box);
        ctx.DrawRectangle(screen.min_x, screen.min_y, screen.width(), screen.height());
    }
}

auto SelectionRenderer::render_selection_handles(wxGraphicsContext& ctx,
                                                 const AABB& combined_bounds,
                                                 const ViewportTransform& viewport) -> void
{
    const auto screen = viewport.world_to_screen(combined_bounds);
    const double half_h = kHandleSize / 2.0;

    ctx.SetPen(ctx.CreatePen(wxGraphicsPenInfo(wxColour(50, 130, 255, 255)).Width(1)));
    ctx.SetBrush(ctx.CreateBrush(wxBrush(wxColour(255, 255, 255, 255))));

    // 8 resize handle positions.
    const double positions[][2] = {
        {screen.min_x, screen.min_y},                        // TopLeft
        {(screen.min_x + screen.max_x) / 2.0, screen.min_y}, // TopCenter
        {screen.max_x, screen.min_y},                        // TopRight
        {screen.min_x, (screen.min_y + screen.max_y) / 2.0}, // MiddleLeft
        {screen.max_x, (screen.min_y + screen.max_y) / 2.0}, // MiddleRight
        {screen.min_x, screen.max_y},                        // BottomLeft
        {(screen.min_x + screen.max_x) / 2.0, screen.max_y}, // BottomCenter
        {screen.max_x, screen.max_y},                        // BottomRight
    };

    for (const auto& pos : positions)
    {
        ctx.DrawRectangle(pos[0] - half_h, pos[1] - half_h, kHandleSize, kHandleSize);
    }

    // Rotation handle: centered above the top edge.
    const double rot_x = (screen.min_x + screen.max_x) / 2.0;
    const double rot_y = screen.min_y - 20.0;
    ctx.DrawEllipse(rot_x - half_h, rot_y - half_h, kHandleSize, kHandleSize);

    // Line from top center to rotation handle.
    ctx.StrokeLine((screen.min_x + screen.max_x) / 2.0, screen.min_y, rot_x, rot_y);
}

auto SelectionRenderer::render_marquee(wxGraphicsContext& ctx,
                                       const AABB& marquee_bounds,
                                       const ViewportTransform& viewport) -> void
{
    const auto screen = viewport.world_to_screen(marquee_bounds);

    ctx.SetPen(ctx.CreatePen(
        wxGraphicsPenInfo(wxColour(50, 130, 255, 180)).Width(1).Style(wxPENSTYLE_SHORT_DASH)));
    ctx.SetBrush(ctx.CreateBrush(wxBrush(wxColour(50, 130, 255, 30))));
    ctx.DrawRectangle(screen.min_x, screen.min_y, screen.width(), screen.height());
}

auto SelectionRenderer::render_rotation_indicator(wxGraphicsContext& ctx,
                                                  const AABB& combined_bounds,
                                                  double current_angle_radians,
                                                  const ViewportTransform& viewport) -> void
{
    const auto screen = viewport.world_to_screen(combined_bounds);
    const double center_x = (screen.min_x + screen.max_x) / 2.0;
    const double center_y = (screen.min_y + screen.max_y) / 2.0;
    const double radius = std::min(screen.width(), screen.height()) / 2.0 + 15.0;

    ctx.SetPen(ctx.CreatePen(wxGraphicsPenInfo(wxColour(255, 100, 50, 180)).Width(2)));

    // Draw arc from 0 to current angle.
    const double end_x = center_x + radius * std::cos(current_angle_radians);
    const double end_y = center_y + radius * std::sin(current_angle_radians);

    ctx.StrokeLine(center_x + radius, center_y, end_x, end_y);
}

} // namespace markamp::canvas
