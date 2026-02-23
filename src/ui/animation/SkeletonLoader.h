#pragma once

#include "../../core/ThemeEngine.h"
#include "../ThemeAwareWindow.h" // Assuming ThemeAwareWindow is up one directory
#include "AnimationTimeline.h"

#include <vector>

namespace markamp::ui::animation
{

/// A reusable component that renders pulsing placeholder blocks while content is loading.
class SkeletonLoader : public ThemeAwareWindow
{
public:
    SkeletonLoader(wxWindow* parent,
                   core::ThemeEngine& theme_engine,
                   wxWindowID id = wxID_ANY,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize);
    ~SkeletonLoader() override;

    void add_block(const wxRect& rect, int border_radius = 4);
    void clear_blocks();

    void start_animation();
    void stop_animation();

protected:
    void on_paint(wxPaintEvent& event);
    void on_size(wxSizeEvent& event);
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    struct Block
    {
        wxRect rect;
        int border_radius;
    };

    std::vector<Block> blocks_;
    AnimationTimeline timeline_;
    float pulse_progress_{0.3f};

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui::animation
