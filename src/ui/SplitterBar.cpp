#include "SplitterBar.h"

#include "LayoutManager.h"

#include <wx/dcbuffer.h>

#include <algorithm>

namespace markamp::ui
{

SplitterBar::SplitterBar(wxWindow* parent,
                         DesignSystemContext& ds_ctx,
                         LayoutManager* layout_manager)
    : ThemeAwareWindow(parent,
                       ds_ctx.theme,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxSize(ds_ctx.metrics.splitter_hit_width(), -1),
                       wxNO_BORDER)
    , ds_(ds_ctx)
    , layout_manager_(layout_manager)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    const int kHitWidth = ds_.metrics.splitter_hit_width();
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
    wxAutoBufferedPaintDC paint_dc(this);
    auto client_sz = GetClientSize();
    const int kBarWidth = client_sz.GetWidth();
    const int kBarHeight = client_sz.GetHeight();

    // Background — BgApp
    paint_dc.SetBrush(ds_.theme.brush(core::ThemeColorToken::BgApp));
    paint_dc.SetPen(*wxTRANSPARENT_PEN);
    paint_dc.DrawRectangle(client_sz);

    // 8B + 8C: Soft center line — blend between BorderLight and AccentPrimary
    const int kCenterX = kBarWidth / 2;

    if (hover_alpha_ > 0.01F)
    {
        // Draw accent line at hover_alpha_ opacity
        auto accent = ds_.theme.color(core::ThemeColorToken::AccentPrimary);
        auto alpha_val =
            static_cast<unsigned char>(std::clamp(static_cast<int>(hover_alpha_ * 180.0F), 0, 255));

        const int kLineWidth = ds_.metrics.splitter_visual_width();
        paint_dc.SetPen(
            wxPen(wxColour(accent.Red(), accent.Green(), accent.Blue(), alpha_val), kLineWidth));
        paint_dc.DrawLine(kCenterX, 0, kCenterX, kBarHeight);
    }
    else
    {
        // Default: subtle BorderLight at 30% alpha
        auto border_col = ds_.theme.color(core::ThemeColorToken::BorderLight);

        const int kLineWidth = ds_.metrics.splitter_visual_width();
        paint_dc.SetPen(wxPen(wxColour(border_col.Red(), border_col.Green(), border_col.Blue(), 77),
                              kLineWidth));
        paint_dc.DrawLine(kCenterX, 0, kCenterX, kBarHeight);
    }
}

void SplitterBar::OnMouseEnter(wxMouseEvent& /*event*/)
{
    SetCursor(wxCursor(wxCURSOR_SIZEWE));
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

void SplitterBar::OnMouseLeave(wxMouseEvent& /*event*/)
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

void SplitterBar::UpdateLayoutMetrics()
{
    const int hit_width = ds_.metrics.splitter_hit_width();
    SetMinSize(wxSize(hit_width, -1));
    SetMaxSize(wxSize(hit_width, -1));
    Refresh();
}

} // namespace markamp::ui
