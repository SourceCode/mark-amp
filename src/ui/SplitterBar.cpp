#include "SplitterBar.h"

#include "LayoutManager.h"

#include <wx/dcbuffer.h>

namespace markamp::ui
{

SplitterBar::SplitterBar(wxWindow* parent,
                         core::ThemeEngine& theme_engine,
                         LayoutManager* layout_manager)
    : ThemeAwareWindow(
          parent, theme_engine, wxID_ANY, wxDefaultPosition, wxSize(kHitWidth, -1), wxNO_BORDER)
    , layout_manager_(layout_manager)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(kHitWidth, -1));
    SetMaxSize(wxSize(kHitWidth, -1));

    Bind(wxEVT_PAINT, &SplitterBar::OnPaint, this);
    Bind(wxEVT_ENTER_WINDOW, &SplitterBar::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &SplitterBar::OnMouseLeave, this);
    Bind(wxEVT_LEFT_DOWN, &SplitterBar::OnMouseDown, this);
    Bind(wxEVT_MOTION, &SplitterBar::OnMouseMove, this);
    Bind(wxEVT_LEFT_UP, &SplitterBar::OnMouseUp, this);
}

void SplitterBar::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);
    Refresh();
}

void SplitterBar::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);
    auto sz = GetClientSize();

    // Background — transparent / BgApp
    dc.SetBrush(theme_engine().brush(core::ThemeColorToken::BgApp));
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(sz);

    // Center line — 1px border_dark
    int cx = sz.GetWidth() / 2;
    dc.SetPen(theme_engine().pen(core::ThemeColorToken::BorderDark, 1));
    dc.DrawLine(cx, 0, cx, sz.GetHeight());
}

void SplitterBar::OnMouseEnter(wxMouseEvent& /*event*/)
{
    SetCursor(wxCursor(wxCURSOR_SIZEWE));
}

void SplitterBar::OnMouseLeave(wxMouseEvent& /*event*/)
{
    if (!is_dragging_)
    {
        SetCursor(wxNullCursor);
    }
}

void SplitterBar::OnMouseDown(wxMouseEvent& event)
{
    is_dragging_ = true;
    drag_start_x_ = ClientToScreen(event.GetPosition()).x;
    drag_start_width_ = layout_manager_->sidebar_width();
    CaptureMouse();
}

void SplitterBar::OnMouseMove(wxMouseEvent& event)
{
    if (is_dragging_)
    {
        int delta = ClientToScreen(event.GetPosition()).x - drag_start_x_;
        int new_width = drag_start_width_ + delta;
        layout_manager_->set_sidebar_width(new_width);
    }
}

void SplitterBar::OnMouseUp(wxMouseEvent& /*event*/)
{
    if (is_dragging_)
    {
        is_dragging_ = false;
        if (HasCapture())
        {
            ReleaseMouse();
        }
    }
}

} // namespace markamp::ui
