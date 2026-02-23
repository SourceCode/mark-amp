#include "SkeletonLoader.h"

#include "../../core/ThemeEngine.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

namespace markamp::ui::animation
{

wxBEGIN_EVENT_TABLE(SkeletonLoader, ThemeAwareWindow) EVT_PAINT(SkeletonLoader::on_paint)
    EVT_SIZE(SkeletonLoader::on_size) wxEND_EVENT_TABLE()

        SkeletonLoader::SkeletonLoader(wxWindow* parent,
                                       core::ThemeEngine& theme_engine,
                                       wxWindowID id,
                                       const wxPoint& pos,
                                       const wxSize& size)
    : ThemeAwareWindow(parent, theme_engine, id, pos, size, wxBORDER_NONE)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

SkeletonLoader::~SkeletonLoader()
{
    stop_animation();
}

void SkeletonLoader::add_block(const wxRect& rect, int border_radius)
{
    blocks_.push_back({rect, border_radius});
    Refresh();
}

void SkeletonLoader::clear_blocks()
{
    blocks_.clear();
    Refresh();
}

void SkeletonLoader::start_animation()
{
    stop_animation();

    AnimationConfig config;
    config.duration = std::chrono::milliseconds(800);
    config.easing_type = EasingType::EaseInOutQuad;
    config.repeat_count = -1;   // Infinite loop
    config.auto_reverse = true; // Pulse back and forth

    timeline_.animate<float>(0.2f,
                             0.8f,
                             config,
                             [this](const float& val)
                             {
                                 pulse_progress_ = val;
                                 Refresh();
                             });
}

void SkeletonLoader::stop_animation()
{
    timeline_.stop_all();
    pulse_progress_ = 0.3f;
    Refresh();
}

void SkeletonLoader::on_paint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);

    auto bg_color =
        theme_engine().resolve_token("editor.background").value_or(wxColour(30, 30, 30));
    dc.SetBackground(wxBrush(bg_color));
    dc.Clear();

    std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));
    if (!gc)
        return;

    auto skeleton_color =
        theme_engine().resolve_token("widget.shadow").value_or(wxColour(100, 100, 100));
    int alpha = static_cast<int>(255.0f * pulse_progress_);
    wxColour draw_color(skeleton_color.Red(),
                        skeleton_color.Green(),
                        skeleton_color.Blue(),
                        static_cast<unsigned char>(alpha));

    gc->SetBrush(wxBrush(draw_color));
    gc->SetPen(*wxTRANSPARENT_PEN);

    for (const auto& block : blocks_)
    {
        if (block.border_radius > 0)
        {
            gc->DrawRoundedRectangle(block.rect.GetX(),
                                     block.rect.GetY(),
                                     block.rect.GetWidth(),
                                     block.rect.GetHeight(),
                                     block.border_radius);
        }
        else
        {
            gc->DrawRectangle(block.rect.GetX(),
                              block.rect.GetY(),
                              block.rect.GetWidth(),
                              block.rect.GetHeight());
        }
    }
}

void SkeletonLoader::on_size(wxSizeEvent& event)
{
    event.Skip();
    Refresh();
}

void SkeletonLoader::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    Refresh();
}

} // namespace markamp::ui::animation
