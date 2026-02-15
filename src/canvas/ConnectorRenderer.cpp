#include "ConnectorRenderer.h"

#include "canvas/ConnectorData.h"

#include <wx/brush.h>
#include <wx/graphics.h>
#include <wx/pen.h>

#include <cmath>

namespace markamp::canvas
{

auto ConnectorRenderer::object_type() const -> CanvasObjectType
{
    return CanvasObjectType::Connector;
}

auto ConnectorRenderer::render(wxGraphicsContext& gc,
                               const CanvasObject& obj,
                               const ViewportTransform& viewport,
                               bool /*selected*/) -> void
{
    const auto* connector = dynamic_cast<const ConnectorObject*>(&obj);
    if (connector == nullptr)
    {
        return;
    }

    const double zoom = viewport.zoom();
    const auto& lc = connector->line_color();

    // Line style.
    wxPenStyle wx_pen_style = wxPENSTYLE_SOLID;
    if (connector->line_style() == ConnectorLineStyle::kDashed)
    {
        wx_pen_style = wxPENSTYLE_SHORT_DASH;
    }
    else if (connector->line_style() == ConnectorLineStyle::kDotted)
    {
        wx_pen_style = wxPENSTYLE_DOT;
    }

    gc.SetPen(wxPen(wxColour(lc.r, lc.g, lc.b, lc.a),
                    static_cast<int>(connector->line_width() * zoom),
                    wx_pen_style));

    // For free connectors (no Board context), use offset points directly.
    const auto screen_start = viewport.world_to_screen(connector->start_endpoint().offset);
    const auto screen_end = viewport.world_to_screen(connector->end_endpoint().offset);

    // Build polyline through waypoints.
    auto path = gc.CreatePath();
    path.MoveToPoint(screen_start.x, screen_start.y);

    for (const auto& wp : connector->waypoints())
    {
        const auto screen_wp = viewport.world_to_screen(wp);
        path.AddLineToPoint(screen_wp.x, screen_wp.y);
    }

    path.AddLineToPoint(screen_end.x, screen_end.y);
    gc.StrokePath(path);

    // Arrowheads.
    const double arrow_size = 10.0 * zoom;

    if (connector->end_arrow() != ArrowheadStyle::kNone)
    {
        const auto& last_wp = connector->waypoints().empty()
                                  ? screen_start
                                  : viewport.world_to_screen(connector->waypoints().back());
        render_arrowhead(gc, screen_end, last_wp, arrow_size);
    }

    if (connector->start_arrow() != ArrowheadStyle::kNone)
    {
        const auto& first_wp = connector->waypoints().empty()
                                   ? screen_end
                                   : viewport.world_to_screen(connector->waypoints().front());
        render_arrowhead(gc, screen_start, first_wp, arrow_size);
    }

    // Optional label at midpoint.
    if (!connector->label().empty())
    {
        const double mid_x = (screen_start.x + screen_end.x) / 2.0;
        const double mid_y = (screen_start.y + screen_end.y) / 2.0;

        const double font_size = 12.0 * zoom;
        const auto font =
            gc.CreateFont(font_size, "sans-serif", wxFONTFLAG_DEFAULT, wxColour(51, 51, 51));
        gc.SetFont(font);
        gc.DrawText(connector->label(), mid_x, mid_y - font_size);
    }
}

auto ConnectorRenderer::render_arrowhead(wxGraphicsContext& gc,
                                         const Point2D& tip,
                                         const Point2D& from,
                                         double size) -> void
{
    const double dx = tip.x - from.x;
    const double dy = tip.y - from.y;
    const double len = std::sqrt(dx * dx + dy * dy);

    if (len < 0.001)
    {
        return;
    }

    const double ux = dx / len;
    const double uy = dy / len;
    constexpr double kHalfAngle = 0.4; // ~23 degrees

    const double left_x = tip.x - size * (ux * std::cos(kHalfAngle) - uy * std::sin(kHalfAngle));
    const double left_y = tip.y - size * (uy * std::cos(kHalfAngle) + ux * std::sin(kHalfAngle));
    const double right_x = tip.x - size * (ux * std::cos(kHalfAngle) + uy * std::sin(kHalfAngle));
    const double right_y = tip.y - size * (uy * std::cos(kHalfAngle) - ux * std::sin(kHalfAngle));

    auto arrow_path = gc.CreatePath();
    arrow_path.MoveToPoint(tip.x, tip.y);
    arrow_path.AddLineToPoint(left_x, left_y);
    arrow_path.AddLineToPoint(right_x, right_y);
    arrow_path.CloseSubpath();
    gc.DrawPath(arrow_path);
}

} // namespace markamp::canvas
