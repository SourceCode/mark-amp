#include "TableRenderer.h"

#include "canvas/TableObject.h"

#include <wx/brush.h>
#include <wx/dcgraph.h>
#include <wx/graphics.h>
#include <wx/pen.h>

namespace markamp::canvas
{

auto TableRenderer::object_type() const -> CanvasObjectType
{
    return CanvasObjectType::Table;
}

auto TableRenderer::render(wxGraphicsContext& graphics_ctx,
                           const CanvasObject& obj,
                           const ViewportTransform& viewport,
                           bool /*selected*/) -> void
{
    const auto& table = static_cast<const TableObject&>(obj);
    const auto bounds = table.world_bounds();

    const auto screen_min = viewport.world_to_screen(Point2D{bounds.min_x, bounds.min_y});
    const auto screen_max = viewport.world_to_screen(Point2D{bounds.max_x, bounds.max_y});
    const double screen_w = screen_max.x - screen_min.x;
    const double screen_h = screen_max.y - screen_min.y;

    if (screen_w < 2.0 || screen_h < 2.0)
    {
        return; // Too small to render.
    }

    const auto zoom = viewport.zoom();
    const double hdr_h = table.header_height() * zoom;
    const double row_h = table.row_height() * zoom;

    // ── Header background ─────────────────────────────────────
    const auto& hdr_bg = table.header_bg_color();
    graphics_ctx.SetBrush(
        graphics_ctx.CreateBrush(wxBrush(wxColour(hdr_bg.r, hdr_bg.g, hdr_bg.b, hdr_bg.a))));
    graphics_ctx.SetPen(wxNullPen);
    graphics_ctx.DrawRectangle(screen_min.x, screen_min.y, screen_w, hdr_h);

    // ── Body rows ─────────────────────────────────────────────
    const auto& alt_color = table.alternate_row_color();
    const wxColour alt_wx(alt_color.r, alt_color.g, alt_color.b, alt_color.a);
    const wxColour white_wx(255, 255, 255, 255);

    for (size_t ri = 0; ri < table.row_count(); ++ri)
    {
        const double row_y = screen_min.y + hdr_h + static_cast<double>(ri) * row_h;
        const wxColour& row_bg = (ri % 2 == 1) ? alt_wx : white_wx;
        graphics_ctx.SetBrush(graphics_ctx.CreateBrush(wxBrush(row_bg)));
        graphics_ctx.DrawRectangle(screen_min.x, row_y, screen_w, row_h);
    }

    // ── Border ────────────────────────────────────────────────
    const auto& border = table.border_color();
    const wxColour border_wx(border.r, border.g, border.b, border.a);
    graphics_ctx.SetPen(graphics_ctx.CreatePen(wxPen(border_wx, 1)));
    graphics_ctx.SetBrush(wxNullBrush);
    graphics_ctx.DrawRectangle(screen_min.x, screen_min.y, screen_w, screen_h);

    // ── Column dividers ───────────────────────────────────────
    double col_x = screen_min.x;
    for (size_t ci = 0; ci + 1 < table.column_count(); ++ci)
    {
        col_x += table.column(ci).width * zoom;
        graphics_ctx.StrokeLine(col_x, screen_min.y, col_x, screen_min.y + screen_h);
    }

    // ── Row dividers ──────────────────────────────────────────
    for (size_t ri = 0; ri <= table.row_count(); ++ri)
    {
        const double row_y = screen_min.y + hdr_h + static_cast<double>(ri) * row_h;
        graphics_ctx.StrokeLine(screen_min.x, row_y, screen_min.x + screen_w, row_y);
    }

    // ── Header text ───────────────────────────────────────────
    const double font_size = 11.0 * zoom;
    graphics_ctx.SetFont(
        graphics_ctx.CreateFont(font_size, "sans-serif", wxFONTFLAG_BOLD, wxColour(30, 30, 30)));

    col_x = screen_min.x;
    for (size_t ci = 0; ci < table.column_count(); ++ci)
    {
        const double col_width = table.column(ci).width * zoom;
        graphics_ctx.DrawText(
            table.column(ci).header, col_x + 4.0 * zoom, screen_min.y + 4.0 * zoom);
        col_x += col_width;
    }

    // ── Cell text ─────────────────────────────────────────────
    graphics_ctx.SetFont(
        graphics_ctx.CreateFont(font_size, "sans-serif", wxFONTFLAG_DEFAULT, wxColour(51, 51, 51)));

    for (size_t ri = 0; ri < table.row_count(); ++ri)
    {
        col_x = screen_min.x;
        const double row_y = screen_min.y + hdr_h + static_cast<double>(ri) * row_h;
        for (size_t ci = 0; ci < table.column_count(); ++ci)
        {
            const auto& cell = table.get_cell(ri, ci);
            if (!cell.text.empty())
            {
                graphics_ctx.DrawText(cell.text, col_x + 4.0 * zoom, row_y + 4.0 * zoom);
            }
            col_x += table.column(ci).width * zoom;
        }
    }
}

} // namespace markamp::canvas
