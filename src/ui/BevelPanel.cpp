#include "BevelPanel.h"

#include <wx/dcbuffer.h>

namespace markamp::ui
{

BevelPanel::BevelPanel(
    wxWindow* parent, wxWindowID id, Style style, const wxPoint& pos, const wxSize& size)
    : wxPanel(parent, id, pos, size, wxFULL_REPAINT_ON_RESIZE | wxTRANSPARENT_WINDOW)
    , style_(style)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &BevelPanel::OnPaint, this);
}

void BevelPanel::set_style(Style style)
{
    style_ = style;
    Refresh();
}

auto BevelPanel::get_style() const -> Style
{
    return style_;
}

void BevelPanel::set_shadow_colour(const wxColour& colour)
{
    shadow_colour_ = colour;
    Refresh();
}

void BevelPanel::set_highlight_colour(const wxColour& colour)
{
    highlight_colour_ = colour;
    Refresh();
}

void BevelPanel::OnPaint(wxPaintEvent& /*event*/)
{
    wxPaintDC dc(this);
    auto sz = GetClientSize();

    // Transparent overlay — do NOT fill background.
    // Only draw bevel border lines so the editor underneath remains visible.
    if (style_ != Style::Flat)
    {
        drawBevel(dc, sz);
    }
}

void BevelPanel::drawBevel(wxDC& dc, const wxSize& size)
{
    const int w = size.GetWidth();
    const int h = size.GetHeight();

    if (w < 4 || h < 4)
    {
        return; // too small for bevel
    }

    wxColour topLeft;
    wxColour bottomRight;

    if (style_ == Style::Sunken)
    {
        topLeft = shadow_colour_;
        bottomRight = highlight_colour_;
    }
    else // Raised
    {
        topLeft = highlight_colour_;
        bottomRight = shadow_colour_;
    }

    // Draw the outer bevel lines (2px border)
    // Top edge
    dc.SetPen(wxPen(topLeft, 1));
    dc.DrawLine(0, 0, w, 0);
    dc.DrawLine(0, 1, w, 1);

    // Left edge
    dc.DrawLine(0, 0, 0, h);
    dc.DrawLine(1, 0, 1, h);

    // Bottom edge
    dc.SetPen(wxPen(bottomRight, 1));
    dc.DrawLine(0, h - 1, w, h - 1);
    dc.DrawLine(0, h - 2, w, h - 2);

    // Right edge
    dc.DrawLine(w - 1, 0, w - 1, h);
    dc.DrawLine(w - 2, 0, w - 2, h);
}

} // namespace markamp::ui
