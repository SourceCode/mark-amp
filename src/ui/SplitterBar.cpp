#include "SplitterBar.h"

#include "LayoutManager.h"

#include <wx/dcbuffer.h>

#include <algorithm>

namespace markamp::ui
{

SplitterBar::SplitterBar(wxWindow* parent,
                         core::ThemeEngine& theme_engine,
                         LayoutManager* layout_manager)
    : ThemeAwareWindow(
          parent, theme_engine, wxID_ANY, wxDefaultPosition, wxSize(kHitWidth, -1), wxNO_BORDER)
    , layout_manager_(layout_manager)
    , hover_timer_(this, kHoverTimerId)
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
    Bind(wxEVT_TIMER, &SplitterBar::OnHoverTimer, this, kHoverTimerId);
}

void SplitterBar::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);
    Refresh();
}

void SplitterBar::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC paint_dc(this);
    auto client_sz = GetClientSize();
    const int bar_width = client_sz.GetWidth();
    const int bar_height = client_sz.GetHeight();

    // Background — BgApp
    paint_dc.SetBrush(theme_engine().brush(core::ThemeColorToken::BgApp));
    paint_dc.SetPen(*wxTRANSPARENT_PEN);
    paint_dc.DrawRectangle(client_sz);

    // 8B + 8C: Soft center line — blend between BorderLight and AccentPrimary
    const int center_x = bar_width / 2;

    if (hover_alpha_ > 0.01F)
    {
        // Draw accent line at hover_alpha_ opacity
        auto accent = theme_engine().color(core::ThemeColorToken::AccentPrimary);
        auto alpha_val =
            static_cast<unsigned char>(std::clamp(static_cast<int>(hover_alpha_ * 180.0F), 0, 255));
        paint_dc.SetPen(wxPen(wxColour(accent.Red(), accent.Green(), accent.Blue(), alpha_val), 2));
        paint_dc.DrawLine(center_x, 0, center_x, bar_height);
    }
    else
    {
        // Default: subtle BorderLight at 30% alpha
        auto border_col = theme_engine().color(core::ThemeColorToken::BorderLight);
        paint_dc.SetPen(
            wxPen(wxColour(border_col.Red(), border_col.Green(), border_col.Blue(), 77), 1));
        paint_dc.DrawLine(center_x, 0, center_x, bar_height);
    }
}

void SplitterBar::OnMouseEnter(wxMouseEvent& /*event*/)
{
    SetCursor(wxCursor(wxCURSOR_SIZEWE));
    is_hovered_ = true;
    if (!hover_timer_.IsRunning())
    {
        hover_timer_.Start(16); // ~60fps
    }
}

void SplitterBar::OnMouseLeave(wxMouseEvent& /*event*/)
{
    if (!is_dragging_)
    {
        SetCursor(wxNullCursor);
        is_hovered_ = false;
        if (!hover_timer_.IsRunning())
        {
            hover_timer_.Start(16);
        }
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

void SplitterBar::OnHoverTimer(wxTimerEvent& /*event*/)
{
    if (is_hovered_ || is_dragging_)
    {
        hover_alpha_ = std::min(hover_alpha_ + kHoverFadeStep, 1.0F);
    }
    else
    {
        hover_alpha_ = std::max(hover_alpha_ - kHoverFadeStep, 0.0F);
    }

    Refresh();

    // Stop timer when animation settles
    if ((!is_hovered_ && !is_dragging_ && hover_alpha_ <= 0.0F) ||
        ((is_hovered_ || is_dragging_) && hover_alpha_ >= 1.0F))
    {
        hover_timer_.Stop();
    }
}

} // namespace markamp::ui
