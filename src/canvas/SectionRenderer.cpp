#include "SectionRenderer.h"

#include "canvas/SectionObject.h"

#include <wx/brush.h>
#include <wx/dcgraph.h>
#include <wx/graphics.h>
#include <wx/pen.h>

namespace markamp::canvas
{

auto SectionRenderer::object_type() const -> CanvasObjectType
{
    return CanvasObjectType::Section;
}

auto SectionRenderer::render(wxGraphicsContext& gc,
                             const CanvasObject& obj,
                             const ViewportTransform& viewport,
                             bool selected) -> void
{
    const auto& section = static_cast<const SectionObject&>(obj);
    const auto bounds = section.world_bounds();

    const auto screen_min = viewport.world_to_screen(Point2D{bounds.min_x, bounds.min_y});
    const auto screen_max = viewport.world_to_screen(Point2D{bounds.max_x, bounds.max_y});
    const double screen_w = screen_max.x - screen_min.x;
    const double screen_h = screen_max.y - screen_min.y;

    constexpr double kHeaderHeight = 28.0;
    constexpr double kCornerRadius = 6.0;

    const auto tint = section.tint_color();

    if (!section.is_collapsed())
    {
        // Full section: tinted background with rounded corners, no hard border.
        gc.SetBrush(wxBrush(wxColour(tint.r, tint.g, tint.b, tint.a)));
        gc.SetPen(*wxTRANSPARENT_PEN);
        gc.DrawRoundedRectangle(screen_min.x, screen_min.y, screen_w, screen_h, kCornerRadius);
    }

    // Header bar: darker tint at the top.
    gc.SetBrush(wxBrush(
        wxColour(tint.r, tint.g, tint.b, static_cast<unsigned char>(std::min(255, tint.a + 80)))));
    gc.SetPen(*wxTRANSPARENT_PEN);
    gc.DrawRoundedRectangle(screen_min.x, screen_min.y, screen_w, kHeaderHeight, kCornerRadius);

    // Title text.
    if (!section.title().empty())
    {
        gc.SetFont(gc.CreateFont(11.0, "sans-serif", wxFONTFLAG_BOLD, wxColour(60, 60, 60)));
        gc.DrawText(section.title(), screen_min.x + 10, screen_min.y + 6);
    }

    // Selection highlight.
    if (selected)
    {
        gc.SetPen(wxPen(wxColour(0, 120, 215, 200), 2, wxPENSTYLE_SHORT_DASH));
        gc.SetBrush(*wxTRANSPARENT_BRUSH);
        gc.DrawRoundedRectangle(
            screen_min.x - 2, screen_min.y - 2, screen_w + 4, screen_h + 4, kCornerRadius);
    }
}

} // namespace markamp::canvas
