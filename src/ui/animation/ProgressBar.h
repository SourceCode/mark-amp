#pragma once

#include "../../core/ThemeEngine.h"
#include "../ThemeAwareWindow.h"
#include "AnimationTimeline.h"

namespace markamp::ui::animation
{

/// A reusable animated progress bar component for indeterminate and determinate states.
class ProgressBar : public ThemeAwareWindow
{
public:
    ProgressBar(wxWindow* parent,
                core::ThemeEngine& theme_engine,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize);
    ~ProgressBar() override;

    void set_progress(float progress); // 0.0 to 1.0. If < 0, becomes indeterminate
    void set_indeterminate(bool indeterminate);

protected:
    void on_paint(wxPaintEvent& event);
    void on_size(wxSizeEvent& event);
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    float target_progress_{0.0f};
    float current_progress_{0.0f};
    bool is_indeterminate_{false};
    float indeterminate_offset_{0.0f};

    AnimationTimeline timeline_;
    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui::animation
