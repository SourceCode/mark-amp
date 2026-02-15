#include "StickyNoteRenderer.h"

#include "canvas/StickyNote.h"

#include <wx/brush.h>
#include <wx/graphics.h>
#include <wx/pen.h>

namespace markamp::canvas
{

auto StickyNoteRenderer::object_type() const -> CanvasObjectType
{
    return CanvasObjectType::StickyNote;
}

auto StickyNoteRenderer::render(wxGraphicsContext& graphics_ctx,
                                const CanvasObject& obj,
                                const ViewportTransform& viewport,
                                bool /*selected*/) -> void
{
    const auto* note = dynamic_cast<const StickyNote*>(&obj);
    if (note == nullptr)
    {
        return;
    }

    const auto pos = viewport.world_to_screen(obj.position());
    const double zoom = viewport.zoom();
    const double render_w = note->width() * zoom;
    const double render_h = note->height() * zoom;
    const double corner_radius = 6.0 * zoom;
    const double shadow_offset = 3.0 * zoom;

    // Shadow.
    graphics_ctx.SetBrush(wxBrush(wxColour(0, 0, 0, 30)));
    graphics_ctx.SetPen(*wxTRANSPARENT_PEN);
    auto shadow_path = graphics_ctx.CreatePath();
    shadow_path.AddRoundedRectangle(
        pos.x + shadow_offset, pos.y + shadow_offset, render_w, render_h, corner_radius);
    graphics_ctx.FillPath(shadow_path);

    // Note body.
    const auto fill_color = sticky_color_to_rgba(note->note_color());
    graphics_ctx.SetBrush(
        wxBrush(wxColour(fill_color.r, fill_color.g, fill_color.b, fill_color.a)));
    graphics_ctx.SetPen(wxPen(wxColour(static_cast<unsigned char>(fill_color.r * 0.85),
                                       static_cast<unsigned char>(fill_color.g * 0.85),
                                       static_cast<unsigned char>(fill_color.b * 0.85)),
                              1));
    auto body_path = graphics_ctx.CreatePath();
    body_path.AddRoundedRectangle(pos.x, pos.y, render_w, render_h, corner_radius);
    graphics_ctx.DrawPath(body_path);

    // Text.
    if (!note->text().empty())
    {
        const double padding = 10.0 * zoom;
        const double font_size = note->font_size() * zoom;
        const auto font = graphics_ctx.CreateFont(
            font_size, "sans-serif", wxFONTFLAG_DEFAULT, wxColour(51, 51, 51));
        graphics_ctx.SetFont(font);
        graphics_ctx.DrawText(note->text(), pos.x + padding, pos.y + padding);
    }
}

} // namespace markamp::canvas
