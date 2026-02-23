#include "ProgressBar.h"

#include "../../core/ThemeEngine.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

#include <algorithm>

namespace markamp::ui::animation
{

wxBEGIN_EVENT_TABLE(ProgressBar, ThemeAwareWindow) EVT_PAINT(ProgressBar::on_paint)
    EVT_SIZE(ProgressBar::on_size) wxEND_EVENT_TABLE()

        ProgressBar::ProgressBar(wxWindow* parent,
                                 core::ThemeEngine& theme_engine,
                                 wxWindowID id,
                                 const wxPoint& pos,
                                 const wxSize& size)
    : ThemeAwareWindow(parent, theme_engine, id, pos, size, wxBORDER_NONE)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

ProgressBar::~ProgressBar()
{
    timeline_.stop_all();
}

void ProgressBar::set_progress(float progress)
{
    if (progress < 0.0f)
    {
        set_indeterminate(true);
        return;
    }

    set_indeterminate(false);
    target_progress_ = std::clamp(progress, 0.0f, 1.0f);

    timeline_.stop_all();

    AnimationConfig config;
    config.duration = std::chrono::milliseconds(250);
    config.easing_type = EasingType::EaseOutQuad;

    timeline_.animate<float>(current_progress_,
                             target_progress_,
                             config,
                             [this](const float& val)
                             {
                                 current_progress_ = val;
                                 Refresh();
                             });
}

void ProgressBar::set_indeterminate(bool indeterminate)
{
    if (is_indeterminate_ == indeterminate)
        return;

    is_indeterminate_ = indeterminate;
    timeline_.stop_all();

    if (is_indeterminate_)
    {
        AnimationConfig config;
        config.duration = std::chrono::milliseconds(1500);
        config.easing_type = EasingType::Linear;
        config.repeat_count = -1; // infinite loop

        timeline_.animate<float>(0.0f,
                                 1.0f,
                                 config,
                                 [this](const float& val)
                                 {
                                     indeterminate_offset_ = val;
                                     Refresh();
                                 });
    }
    else
    {
        Refresh();
    }
}

void ProgressBar::on_paint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);

    auto bg_color =
        theme_engine().resolve_token("progressBar.background").value_or(wxColour(40, 40, 40));
    dc.SetBackground(wxBrush(bg_color));
    dc.Clear();

    std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));
    if (!gc)
        return;

    auto fill_color =
        theme_engine().resolve_token("progressBar.fill").value_or(wxColour(0, 122, 204));
    gc->SetBrush(wxBrush(fill_color));
    gc->SetPen(*wxTRANSPARENT_PEN);

    wxSize size = GetClientSize();

    if (is_indeterminate_)
    {
        // Draw a marching bar spanning 30% of width
        float bar_width = static_cast<float>(size.GetWidth()) * 0.3f;
        float start_x =
            (static_cast<float>(size.GetWidth()) + bar_width) * indeterminate_offset_ - bar_width;
        gc->DrawRectangle(static_cast<wxDouble>(start_x),
                          0.0,
                          static_cast<wxDouble>(bar_width),
                          static_cast<wxDouble>(size.GetHeight()));
    }
    else
    {
        float width = static_cast<float>(size.GetWidth()) * current_progress_;
        gc->DrawRectangle(
            0.0, 0.0, static_cast<wxDouble>(width), static_cast<wxDouble>(size.GetHeight()));
    }
}

void ProgressBar::on_size(wxSizeEvent& event)
{
    event.Skip();
    Refresh();
}

void ProgressBar::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    Refresh();
}

} // namespace markamp::ui::animation
