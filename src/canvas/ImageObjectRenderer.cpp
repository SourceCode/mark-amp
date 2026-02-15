#include "ImageObjectRenderer.h"

#include "canvas/ImageObject.h"

#include <wx/brush.h>
#include <wx/dcgraph.h>
#include <wx/graphics.h>
#include <wx/pen.h>

namespace markamp::canvas
{

auto ImageObjectRenderer::object_type() const -> CanvasObjectType
{
    return CanvasObjectType::Image;
}

auto ImageObjectRenderer::render(wxGraphicsContext& gc,
                                 const CanvasObject& obj,
                                 const ViewportTransform& viewport,
                                 bool selected) -> void
{
    const auto& img = static_cast<const ImageObject&>(obj);
    const auto bounds = img.world_bounds();

    const auto screen_min = viewport.world_to_screen(Point2D{bounds.min_x, bounds.min_y});
    const auto screen_max = viewport.world_to_screen(Point2D{bounds.max_x, bounds.max_y});
    const double screen_w = screen_max.x - screen_min.x;
    const double screen_h = screen_max.y - screen_min.y;

    // Placeholder rendering: gray rectangle with "Image" label.
    // Real implementation would load wxBitmap from img.file_path().
    gc.SetBrush(wxBrush(wxColour(230, 230, 230, 255)));
    gc.SetPen(wxPen(wxColour(180, 180, 180, 255), 1));
    gc.DrawRectangle(screen_min.x, screen_min.y, screen_w, screen_h);

    // Draw diagonal cross to indicate image placeholder.
    gc.SetPen(wxPen(wxColour(200, 200, 200, 200), 1, wxPENSTYLE_SHORT_DASH));
    gc.StrokeLine(screen_min.x, screen_min.y, screen_max.x, screen_max.y);
    gc.StrokeLine(screen_max.x, screen_min.y, screen_min.x, screen_max.y);

    // Label.
    gc.SetFont(gc.CreateFont(10.0, "sans-serif", wxFONTFLAG_DEFAULT, wxColour(120, 120, 120)));

    const auto& file_name = img.file_path().empty() ? std::string("Image") : img.file_path();
    gc.DrawText(file_name, screen_min.x + 4, screen_min.y + 4);

    // Selection outline.
    if (selected)
    {
        gc.SetPen(wxPen(wxColour(0, 120, 215, 200), 2));
        gc.SetBrush(*wxTRANSPARENT_BRUSH);
        gc.DrawRectangle(screen_min.x - 1, screen_min.y - 1, screen_w + 2, screen_h + 2);
    }
}

} // namespace markamp::canvas
