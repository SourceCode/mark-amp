#include "ThemedScrollbar.h"

#include <wx/dcbuffer.h>

namespace markamp::ui
{

ThemedScrollbar::ThemedScrollbar(wxWindow* parent,
                                 core::ThemeEngine& theme_engine,
                                 wxWindow* scroll_target)
    : ThemeAwareWindow(
          parent, theme_engine, wxID_ANY, wxDefaultPosition, wxSize(kWidth, -1), wxNO_BORDER)
    , scroll_target_(scroll_target)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(kWidth, kWidth));
    SetMaxSize(wxSize(kWidth, -1));

    Bind(wxEVT_PAINT, &ThemedScrollbar::OnPaint, this);
    Bind(wxEVT_ENTER_WINDOW, &ThemedScrollbar::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &ThemedScrollbar::OnMouseLeave, this);
    Bind(wxEVT_LEFT_DOWN, &ThemedScrollbar::OnMouseDown, this);
    Bind(wxEVT_MOTION, &ThemedScrollbar::OnMouseMove, this);
    Bind(wxEVT_LEFT_UP, &ThemedScrollbar::OnMouseUp, this);

    // R20 Fix 37: Auto-hide timer
    Bind(wxEVT_TIMER, &ThemedScrollbar::OnAutoHideTimer, this, auto_hide_timer_.GetId());
    auto_hide_timer_.Start(kAutoHideDelayMs);
}

void ThemedScrollbar::UpdateScrollPosition(int position, int visible_range, int total_range)
{
    position_ = position;
    visible_range_ = visible_range;
    total_range_ = total_range;
    // R20 Fix 37: Show thumb instantly when scroll position updates
    thumb_opacity_ = 1.0F;
    ResetAutoHideTimer();
    Refresh();
}

auto ThemedScrollbar::scroll_position() const -> int
{
    return position_;
}

void ThemedScrollbar::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);
    Refresh();
}

// --- Drawing ---

void ThemedScrollbar::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);
    auto size = GetClientSize();

    // Track
    dc.SetBrush(theme_engine().brush(core::ThemeColorToken::ScrollbarTrack));
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(size);

    if (total_range_ <= visible_range_ || total_range_ == 0)
    {
        return; // No scrollbar needed
    }

    // Thumb
    auto token = is_hovering_ || is_dragging_ ? core::ThemeColorToken::ScrollbarHover
                                              : core::ThemeColorToken::ScrollbarThumb;
    auto thumb_color = theme_engine().color(token);
    // R20 Fix 37: Apply opacity for auto-hide
    wxColour faded_thumb(thumb_color.Red(),
                         thumb_color.Green(),
                         thumb_color.Blue(),
                         static_cast<unsigned char>(thumb_opacity_ * 255.0F));
    dc.SetBrush(wxBrush(faded_thumb));
    dc.SetPen(*wxTRANSPARENT_PEN);

    auto tr = thumb_rect();
    dc.DrawRoundedRectangle(tr, 4.0);

    // R20 Fix 38: Scroll position tick marks at regular intervals
    if (total_range_ > visible_range_ && total_range_ > 0)
    {
        constexpr int kTickInterval = 50; // tick every 50 content units
        auto tick_color = theme_engine().color(core::ThemeColorToken::BorderLight);
        wxColour faded_tick(tick_color.Red(),
                            tick_color.Green(),
                            tick_color.Blue(),
                            static_cast<unsigned char>(thumb_opacity_ * 60.0F));
        dc.SetPen(wxPen(faded_tick, 1));

        for (int content_pos = kTickInterval; content_pos < total_range_;
             content_pos += kTickInterval)
        {
            int pixel_y = content_to_pixel(content_pos);
            dc.DrawLine(size.GetWidth() - 3, pixel_y, size.GetWidth(), pixel_y);
        }
    }
}

auto ThemedScrollbar::thumb_rect() const -> wxRect
{
    auto size = GetClientSize();
    if (total_range_ <= 0)
    {
        return {};
    }

    double ratio = static_cast<double>(visible_range_) / static_cast<double>(total_range_);
    int thumb_h = std::max(kThumbMinHeight, static_cast<int>(size.GetHeight() * ratio));
    int track_space = size.GetHeight() - thumb_h;
    int scrollable = total_range_ - visible_range_;
    int thumb_y = scrollable > 0 ? static_cast<int>(track_space * (static_cast<double>(position_) /
                                                                   static_cast<double>(scrollable)))
                                 : 0;

    return {0, thumb_y, size.GetWidth(), thumb_h};
}

auto ThemedScrollbar::content_to_pixel(int content_pos) const -> int
{
    auto size = GetClientSize();
    double ratio = static_cast<double>(visible_range_) / static_cast<double>(total_range_);
    int thumb_h = std::max(kThumbMinHeight, static_cast<int>(size.GetHeight() * ratio));
    int track_space = size.GetHeight() - thumb_h;
    int scrollable = total_range_ - visible_range_;
    return scrollable > 0 ? static_cast<int>(track_space * (static_cast<double>(content_pos) /
                                                            static_cast<double>(scrollable)))
                          : 0;
}

auto ThemedScrollbar::pixel_to_content(int pixel_y) const -> int
{
    auto size = GetClientSize();
    double ratio = static_cast<double>(visible_range_) / static_cast<double>(total_range_);
    int thumb_h = std::max(kThumbMinHeight, static_cast<int>(size.GetHeight() * ratio));
    int track_space = size.GetHeight() - thumb_h;
    int scrollable = total_range_ - visible_range_;
    if (track_space <= 0)
    {
        return 0;
    }
    return static_cast<int>(scrollable *
                            (static_cast<double>(pixel_y) / static_cast<double>(track_space)));
}

// --- Mouse interaction ---

void ThemedScrollbar::OnMouseEnter(wxMouseEvent& /*event*/)
{
    is_hovering_ = true;
    // R20 Fix 37: Show thumb on hover
    thumb_opacity_ = 1.0F;
    ResetAutoHideTimer();
    Refresh();
}

void ThemedScrollbar::OnMouseLeave(wxMouseEvent& /*event*/)
{
    is_hovering_ = false;
    if (!is_dragging_)
    {
        Refresh();
    }
}

void ThemedScrollbar::OnMouseDown(wxMouseEvent& event)
{
    auto tr = thumb_rect();
    if (tr.Contains(event.GetPosition()))
    {
        is_dragging_ = true;
        drag_start_y_ = event.GetY();
        drag_start_position_ = position_;
        CaptureMouse();
    }
}

void ThemedScrollbar::OnMouseMove(wxMouseEvent& event)
{
    if (is_dragging_)
    {
        int delta_y = event.GetY() - drag_start_y_;
        int new_pos = drag_start_position_ + pixel_to_content(delta_y);
        int max_pos = total_range_ - visible_range_;
        new_pos = std::clamp(new_pos, 0, max_pos);
        position_ = new_pos;
        Refresh();

        // Notify scroll target
        if (scroll_target_ != nullptr)
        {
            wxScrollEvent scroll_event(wxEVT_SCROLLWIN_THUMBTRACK);
            scroll_event.SetPosition(position_);
            scroll_target_->GetEventHandler()->ProcessEvent(scroll_event);
        }
    }
}

void ThemedScrollbar::OnMouseUp(wxMouseEvent& /*event*/)
{
    if (is_dragging_)
    {
        is_dragging_ = false;
        if (HasCapture())
        {
            ReleaseMouse();
        }
        Refresh();
    }
}

// R20 Fix 37: Auto-hide timer handler
void ThemedScrollbar::OnAutoHideTimer(wxTimerEvent& /*event*/)
{
    if (!is_hovering_ && !is_dragging_)
    {
        thumb_opacity_ -= 0.08F;
        if (thumb_opacity_ <= 0.0F)
        {
            thumb_opacity_ = 0.0F;
            auto_hide_timer_.Stop();
        }
        Refresh();
    }
}

void ThemedScrollbar::ResetAutoHideTimer()
{
    if (auto_hide_timer_.IsRunning())
    {
        auto_hide_timer_.Stop();
    }
    auto_hide_timer_.Start(kAutoHideDelayMs);
}

} // namespace markamp::ui
