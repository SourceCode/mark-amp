#include "FreehandPathRenderer.h"

#include "canvas/FreehandPath.h"

#include <wx/brush.h>
#include <wx/dcgraph.h>
#include <wx/graphics.h>
#include <wx/pen.h>

namespace markamp::canvas
{

auto FreehandPathRenderer::object_type() const -> CanvasObjectType
{
    return CanvasObjectType::FreehandPath;
}

auto FreehandPathRenderer::render(wxGraphicsContext& gc,
                                  const CanvasObject& obj,
                                  const ViewportTransform& viewport,
                                  bool selected) -> void
{
    const auto& path_obj = static_cast<const FreehandPath&>(obj);
    const auto& pts = path_obj.points();

    if (pts.size() < 2)
    {
        return;
    }

    // Set up stroke.
    const auto stroke_col = path_obj.stroke_color();
    const double stroke_w = path_obj.stroke_width() * viewport.zoom();

    gc.SetPen(wxPen(wxColour(stroke_col.r, stroke_col.g, stroke_col.b, stroke_col.a),
                    static_cast<int>(std::max(1.0, stroke_w)),
                    wxPENSTYLE_SOLID));
    gc.SetBrush(*wxTRANSPARENT_BRUSH);

    // Build graphics path with all points transformed to screen space.
    auto gfx_path = gc.CreatePath();

    const auto screen_start = viewport.world_to_screen(pts[0]);
    gfx_path.MoveToPoint(screen_start.x, screen_start.y);

    for (size_t idx = 1; idx < pts.size(); ++idx)
    {
        const auto screen_pt = viewport.world_to_screen(pts[idx]);
        gfx_path.AddLineToPoint(screen_pt.x, screen_pt.y);
    }

    gc.StrokePath(gfx_path);

    // Selection highlight: thicker semi-transparent overlay stroke.
    if (selected)
    {
        gc.SetPen(wxPen(wxColour(0, 120, 215, 120),
                        static_cast<int>(std::max(3.0, stroke_w + 4.0)),
                        wxPENSTYLE_SOLID));
        gc.StrokePath(gfx_path);
    }
}

} // namespace markamp::canvas
