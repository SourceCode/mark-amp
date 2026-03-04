#pragma once

#include "ThemeAwareWindow.h"

#include <wx/timer.h>

#include <string>

namespace markamp::ui
{

/// Phase 41: Rich tooltip with title, body, and shortcut hint.
class RichTooltip : public ThemeAwareWindow
{
public:
    RichTooltip(wxWindow* parent, core::ThemeEngine& theme_engine);

    void set_title(const std::string& title)
    {
        title_ = title;
    }
    void set_body(const std::string& body)
    {
        body_ = body;
    }
    void set_shortcut(const std::string& shortcut)
    {
        shortcut_ = shortcut;
    }
    void set_delay_ms(int delay_ms)
    {
        delay_ms_ = delay_ms;
    }

    void show_at(int screen_x, int screen_y);
    void dismiss();

    [[nodiscard]] auto is_showing() const -> bool
    {
        return is_showing_;
    }

private:
    void on_paint(wxPaintEvent& event);
    void on_timer(wxTimerEvent& event);
    void do_show();

    std::string title_;
    std::string body_;
    std::string shortcut_;
    int delay_ms_{500};
    bool is_showing_{false};
    wxTimer show_timer_;
    int pending_x_{0};
    int pending_y_{0};
};

} // namespace markamp::ui
