#include "TextBoxRenderer.h"

#include "canvas/TextBox.h"

#include <wx/brush.h>
#include <wx/graphics.h>
#include <wx/pen.h>

namespace markamp::canvas
{

auto TextBoxRenderer::object_type() const -> CanvasObjectType
{
    return CanvasObjectType::TextBox;
}

auto TextBoxRenderer::render(wxGraphicsContext& gc,
                             const CanvasObject& obj,
                             const ViewportTransform& viewport,
                             bool /*selected*/) -> void
{
    const auto* text_box = dynamic_cast<const TextBox*>(&obj);
    if (text_box == nullptr)
    {
        return;
    }

    const auto pos = viewport.world_to_screen(obj.position());
    const double zoom = viewport.zoom();
    const double render_w = text_box->width() * zoom;
    const double render_h = text_box->height() * zoom;

    // Optional fill.
    if (text_box->has_fill())
    {
        const auto fc = text_box->fill_color();
        gc.SetBrush(wxBrush(wxColour(fc.r, fc.g, fc.b, fc.a)));
        gc.SetPen(*wxTRANSPARENT_PEN);
        gc.DrawRectangle(pos.x, pos.y, render_w, render_h);
    }

    // Optional border.
    if (text_box->has_border())
    {
        const auto bc = text_box->border_color();
        gc.SetPen(wxPen(wxColour(bc.r, bc.g, bc.b, bc.a), 1));
        gc.SetBrush(*wxTRANSPARENT_BRUSH);
        gc.DrawRectangle(pos.x, pos.y, render_w, render_h);
    }

    // Text.
    if (!text_box->text().empty())
    {
        const auto& style = text_box->style();
        const double font_size = style.font_size * zoom;
        const double padding = 8.0 * zoom;

        int font_flags = wxFONTFLAG_DEFAULT;
        if (style.bold)
        {
            font_flags |= wxFONTFLAG_BOLD;
        }
        if (style.italic)
        {
            font_flags |= wxFONTFLAG_ITALIC;
        }
        if (style.underline)
        {
            font_flags |= wxFONTFLAG_UNDERLINED;
        }

        const auto font = gc.CreateFont(
            font_size,
            style.font_family,
            font_flags,
            wxColour(
                style.text_color.r, style.text_color.g, style.text_color.b, style.text_color.a));
        gc.SetFont(font);
        gc.DrawText(text_box->text(), pos.x + padding, pos.y + padding);
    }
}

} // namespace markamp::canvas
