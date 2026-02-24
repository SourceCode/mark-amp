#include "ResizeHandle.h"

#include "../LayoutMetrics.h"

#include <wx/dcbuffer.h>

#include <algorithm>

namespace markamp::ui::layout
{

ResizeHandle::ResizeHandle(wxWindow* parent,
                           DesignSystemContext& ds_ctx,
                           ResizeOrientation orientation,
                           std::function<void(int delta)> on_drag)
    : ThemeAwareWindow(
          parent, ds_ctx.theme, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER)
    , ds_(ds_ctx)
    , orientation_(orientation)
    , on_drag_(std::move(on_drag))
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    UpdateLayoutMetrics();

    Bind(wxEVT_PAINT, &ResizeHandle::OnPaint, this);
    Bind(wxEVT_ENTER_WINDOW, &ResizeHandle::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &ResizeHandle::OnMouseLeave, this);
    Bind(wxEVT_LEFT_DOWN, &ResizeHandle::OnMouseDown, this);
    Bind(wxEVT_MOTION, &ResizeHandle::OnMouseMove, this);
    Bind(wxEVT_LEFT_UP, &ResizeHandle::OnMouseUp, this);
}

void ResizeHandle::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);
    Refresh();
}

void ResizeHandle::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC paint_dc(this);
    auto client_sz = GetClientSize();
    const int kBarWidth = client_sz.GetWidth();
    const int kBarHeight = client_sz.GetHeight();

    // Background — BgApp gives seamless blending
    paint_dc.SetBrush(ds_.theme.brush(core::ThemeColorToken::BgApp));
    paint_dc.SetPen(*wxTRANSPARENT_PEN);
    paint_dc.DrawRectangle(client_sz);

    // Soft center line — blend between BorderLight and AccentPrimary
    const int kLinePos =
        (orientation_ == ResizeOrientation::kVertical) ? (kBarWidth / 2) : (kBarHeight / 2);

    // Width of the actual visual separator
    const int kLineWidth = ds_.metrics.splitter_visual_width();

    wxColour draw_color;
    if (hover_alpha_ > 0.01F)
    {
        // Draw accent line at hover_alpha_ opacity
        auto accent = ds_.theme.color(core::ThemeColorToken::AccentPrimary);
        auto alpha_val =
            static_cast<unsigned char>(std::clamp(static_cast<int>(hover_alpha_ * 180.0F), 0, 255));
        draw_color = wxColour(accent.Red(), accent.Green(), accent.Blue(), alpha_val);
    }
    else
    {
        // Default: subtle BorderLight at 30% alpha
        auto border_col = ds_.theme.color(core::ThemeColorToken::BorderLight);
        draw_color = wxColour(border_col.Red(), border_col.Green(), border_col.Blue(), 77);
    }

    paint_dc.SetPen(wxPen(draw_color, kLineWidth));

    if (orientation_ == ResizeOrientation::kVertical)
    {
        paint_dc.DrawLine(kLinePos, 0, kLinePos, kBarHeight);
    }
    else
    {
        paint_dc.DrawLine(0, kLinePos, kBarWidth, kLinePos);
    }
}

void ResizeHandle::OnMouseEnter(wxMouseEvent& /*event*/)
{
    if (orientation_ == ResizeOrientation::kVertical)
        SetCursor(wxCursor(wxCURSOR_SIZEWE));
    else
        SetCursor(wxCursor(wxCURSOR_SIZENS));

    is_hovered_ = true;

    animation::AnimationConfig cfg;
    cfg.duration = std::chrono::milliseconds(150);
    cfg.easing_type = animation::EasingType::EaseOutCubic;
    transition_manager_.register_transition("hover", cfg);
    transition_manager_.start<float>("hover",
                                     hover_alpha_,
                                     1.0F,
                                     [this](float a)
                                     {
                                         hover_alpha_ = a;
                                         Refresh();
                                     });
}

void ResizeHandle::OnMouseLeave(wxMouseEvent& /*event*/)
{
    is_hovered_ = false;
    if (!is_dragging_)
    {
        SetCursor(wxNullCursor);

        animation::AnimationConfig cfg;
        cfg.duration = std::chrono::milliseconds(200);
        cfg.easing_type = animation::EasingType::EaseInCubic;
        transition_manager_.register_transition("hover", cfg);
        transition_manager_.start<float>("hover",
                                         hover_alpha_,
                                         0.0F,
                                         [this](float a)
                                         {
                                             hover_alpha_ = a;
                                             Refresh();
                                         });
    }
}

void ResizeHandle::OnMouseDown(wxMouseEvent& event)
{
    is_dragging_ = true;
    drag_start_pos_ = (orientation_ == ResizeOrientation::kVertical)
                          ? ClientToScreen(event.GetPosition()).x
                          : ClientToScreen(event.GetPosition()).y;

    CaptureMouse();
}

void ResizeHandle::OnMouseMove(wxMouseEvent& event)
{
    if (is_dragging_ && on_drag_)
    {
        int current_pos = (orientation_ == ResizeOrientation::kVertical)
                              ? ClientToScreen(event.GetPosition()).x
                              : ClientToScreen(event.GetPosition()).y;

        int delta = current_pos - drag_start_pos_;
        if (delta != 0)
        {
            on_drag_(delta);
            // Update drag origin for incremental relative changes
            drag_start_pos_ = current_pos;
        }
    }
}

void ResizeHandle::OnMouseUp(wxMouseEvent& /*event*/)
{
    if (is_dragging_)
    {
        is_dragging_ = false;
        if (HasCapture())
        {
            ReleaseMouse();
        }

        if (!is_hovered_)
        {
            SetCursor(wxNullCursor);

            animation::AnimationConfig cfg;
            cfg.duration = std::chrono::milliseconds(200);
            cfg.easing_type = animation::EasingType::EaseInCubic;
            transition_manager_.register_transition("hover", cfg);
            transition_manager_.start<float>("hover",
                                             hover_alpha_,
                                             0.0F,
                                             [this](float a)
                                             {
                                                 hover_alpha_ = a;
                                                 Refresh();
                                             });
        }
    }
}

void ResizeHandle::UpdateLayoutMetrics()
{
    const int hit_width = ds_.metrics.splitter_hit_width();

    if (orientation_ == ResizeOrientation::kVertical)
    {
        SetMinSize(wxSize(hit_width, -1));
        SetMaxSize(wxSize(hit_width, -1));
    }
    else
    {
        SetMinSize(wxSize(-1, hit_width));
        SetMaxSize(wxSize(-1, hit_width));
    }
    Refresh();
}

} // namespace markamp::ui::layout
