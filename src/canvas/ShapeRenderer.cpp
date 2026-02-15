#include "ShapeRenderer.h"

#include "canvas/ShapeData.h"

#include <wx/brush.h>
#include <wx/graphics.h>
#include <wx/pen.h>

#include <cmath>

namespace markamp::canvas
{

auto ShapeRenderer::object_type() const -> CanvasObjectType
{
    return CanvasObjectType::Shape;
}

auto ShapeRenderer::render(wxGraphicsContext& gc,
                           const CanvasObject& obj,
                           const ViewportTransform& viewport,
                           bool /*selected*/) -> void
{
    const auto* shape = dynamic_cast<const ShapeObject*>(&obj);
    if (shape == nullptr)
    {
        return;
    }

    const auto pos = viewport.world_to_screen(obj.position());
    const double zoom = viewport.zoom();
    const double render_w = shape->width() * zoom;
    const double render_h = shape->height() * zoom;
    const auto& style = shape->style();

    // Set pen/brush.
    const auto& sc = style.stroke_color;
    const auto& fc = style.fill_color;

    wxPenStyle wx_pen_style = wxPENSTYLE_SOLID;
    if (style.dash_pattern == DashPattern::kDashed)
    {
        wx_pen_style = wxPENSTYLE_SHORT_DASH;
    }
    else if (style.dash_pattern == DashPattern::kDotted)
    {
        wx_pen_style = wxPENSTYLE_DOT;
    }

    gc.SetPen(wxPen(wxColour(sc.r, sc.g, sc.b, sc.a),
                    static_cast<int>(style.stroke_width * zoom),
                    wx_pen_style));
    gc.SetBrush(wxBrush(wxColour(fc.r, fc.g, fc.b, fc.a)));

    switch (shape->shape_type())
    {
        case ShapeType::kRectangle:
            render_rectangle(gc, pos.x, pos.y, render_w, render_h, 0.0);
            break;
        case ShapeType::kRoundedRectangle:
            render_rectangle(gc, pos.x, pos.y, render_w, render_h, style.corner_radius * zoom);
            break;
        case ShapeType::kEllipse:
            render_ellipse(gc, pos.x, pos.y, render_w, render_h);
            break;
        case ShapeType::kDiamond:
            render_diamond(gc, pos.x, pos.y, render_w, render_h);
            break;
        case ShapeType::kTriangle:
            render_triangle(gc, pos.x, pos.y, render_w, render_h);
            break;
        case ShapeType::kHexagon:
            render_hexagon(gc, pos.x, pos.y, render_w, render_h);
            break;
        case ShapeType::kStar:
            render_star(gc, pos.x, pos.y, render_w, render_h);
            break;
        default:
            // Arrow, Callout, Cylinder, Parallelogram, Trapezoid — fallback to rectangle.
            render_rectangle(gc, pos.x, pos.y, render_w, render_h, 0.0);
            break;
    }

    // Embedded text.
    if (!shape->text().empty())
    {
        const auto& ts = shape->text_style();
        const double font_size = ts.font_size * zoom;
        const auto font = gc.CreateFont(
            font_size,
            ts.font_family,
            wxFONTFLAG_DEFAULT,
            wxColour(ts.text_color.r, ts.text_color.g, ts.text_color.b, ts.text_color.a));
        gc.SetFont(font);

        double text_w = 0;
        double text_h = 0;
        gc.GetTextExtent(shape->text(), &text_w, &text_h);
        gc.DrawText(
            shape->text(), pos.x + (render_w - text_w) / 2.0, pos.y + (render_h - text_h) / 2.0);
    }
}

auto ShapeRenderer::render_rectangle(
    wxGraphicsContext& gc, double x, double y, double w, double h, double corner_radius) -> void
{
    if (corner_radius > 0.0)
    {
        auto path = gc.CreatePath();
        path.AddRoundedRectangle(x, y, w, h, corner_radius);
        gc.DrawPath(path);
    }
    else
    {
        gc.DrawRectangle(x, y, w, h);
    }
}

auto ShapeRenderer::render_ellipse(wxGraphicsContext& gc, double x, double y, double w, double h)
    -> void
{
    gc.DrawEllipse(x, y, w, h);
}

auto ShapeRenderer::render_diamond(wxGraphicsContext& gc, double x, double y, double w, double h)
    -> void
{
    auto path = gc.CreatePath();
    path.MoveToPoint(x + w / 2.0, y);
    path.AddLineToPoint(x + w, y + h / 2.0);
    path.AddLineToPoint(x + w / 2.0, y + h);
    path.AddLineToPoint(x, y + h / 2.0);
    path.CloseSubpath();
    gc.DrawPath(path);
}

auto ShapeRenderer::render_triangle(wxGraphicsContext& gc, double x, double y, double w, double h)
    -> void
{
    auto path = gc.CreatePath();
    path.MoveToPoint(x + w / 2.0, y);
    path.AddLineToPoint(x + w, y + h);
    path.AddLineToPoint(x, y + h);
    path.CloseSubpath();
    gc.DrawPath(path);
}

auto ShapeRenderer::render_hexagon(wxGraphicsContext& gc, double x, double y, double w, double h)
    -> void
{
    const double quarter_w = w / 4.0;
    auto path = gc.CreatePath();
    path.MoveToPoint(x + quarter_w, y);
    path.AddLineToPoint(x + w - quarter_w, y);
    path.AddLineToPoint(x + w, y + h / 2.0);
    path.AddLineToPoint(x + w - quarter_w, y + h);
    path.AddLineToPoint(x + quarter_w, y + h);
    path.AddLineToPoint(x, y + h / 2.0);
    path.CloseSubpath();
    gc.DrawPath(path);
}

auto ShapeRenderer::render_star(wxGraphicsContext& gc, double x, double y, double w, double h)
    -> void
{
    constexpr int kPoints = 5;
    const double cx = x + w / 2.0;
    const double cy = y + h / 2.0;
    const double outer_rx = w / 2.0;
    const double outer_ry = h / 2.0;
    const double inner_rx = outer_rx * 0.4;
    const double inner_ry = outer_ry * 0.4;
    constexpr double kPi = 3.14159265358979323846;

    auto path = gc.CreatePath();

    for (int pt_idx = 0; pt_idx < kPoints * 2; ++pt_idx)
    {
        const double angle = (static_cast<double>(pt_idx) * kPi / kPoints) - kPi / 2.0;
        const double radius_x = (pt_idx % 2 == 0) ? outer_rx : inner_rx;
        const double radius_y = (pt_idx % 2 == 0) ? outer_ry : inner_ry;
        const double px = cx + radius_x * std::cos(angle);
        const double py = cy + radius_y * std::sin(angle);

        if (pt_idx == 0)
        {
            path.MoveToPoint(px, py);
        }
        else
        {
            path.AddLineToPoint(px, py);
        }
    }

    path.CloseSubpath();
    gc.DrawPath(path);
}

} // namespace markamp::canvas
