#include "FrameRenderer.h"

#include "canvas/FrameObject.h"

#include <wx/brush.h>
#include <wx/dcgraph.h>
#include <wx/graphics.h>
#include <wx/pen.h>

namespace markamp::canvas
{

auto FrameRenderer::object_type() const -> CanvasObjectType
{
    return CanvasObjectType::Frame;
}

auto FrameRenderer::render(wxGraphicsContext& gc,
                           const CanvasObject& obj,
                           const ViewportTransform& viewport,
                           bool selected) -> void
{
    const auto& frame = static_cast<const FrameObject&>(obj);
    const auto bounds = frame.world_bounds();

    const auto screen_min = viewport.world_to_screen(Point2D{bounds.min_x, bounds.min_y});
    const auto screen_max = viewport.world_to_screen(Point2D{bounds.max_x, bounds.max_y});
    const double screen_w = screen_max.x - screen_min.x;
    const double screen_h = screen_max.y - screen_min.y;

    // Background fill.
    const auto bg = frame.background_color();
    gc.SetBrush(wxBrush(wxColour(bg.r, bg.g, bg.b, bg.a)));

    const auto border = frame.border_color();
    gc.SetPen(wxPen(wxColour(border.r, border.g, border.b, border.a), 2));
    gc.DrawRectangle(screen_min.x, screen_min.y, screen_w, screen_h);

    // Title bar (rendered above the frame rect).
    if (frame.show_title() && !frame.title().empty())
    {
        constexpr double kTitleBarHeight = 24.0;
        const double title_y = screen_min.y - kTitleBarHeight;

        // Title background.
        gc.SetBrush(wxBrush(wxColour(border.r, border.g, border.b, 200)));
        gc.SetPen(*wxTRANSPARENT_PEN);
        gc.DrawRectangle(screen_min.x, title_y, screen_w, kTitleBarHeight);

        // Title text.
        gc.SetFont(gc.CreateFont(12.0, "sans-serif", wxFONTFLAG_BOLD, wxColour(255, 255, 255)));
        gc.DrawText(frame.title(), screen_min.x + 8, title_y + 4);
    }

    // Selection highlight.
    if (selected)
    {
        gc.SetPen(wxPen(wxColour(0, 120, 215, 200), 2, wxPENSTYLE_SHORT_DASH));
        gc.SetBrush(*wxTRANSPARENT_BRUSH);
        gc.DrawRectangle(screen_min.x - 2, screen_min.y - 2, screen_w + 4, screen_h + 4);
    }
}

} // namespace markamp::canvas
