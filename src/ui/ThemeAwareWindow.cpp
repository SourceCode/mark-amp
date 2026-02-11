#include "ThemeAwareWindow.h"

#include "core/Events.h"

#include <wx/dc.h>

namespace markamp::ui
{

ThemeAwareWindow::ThemeAwareWindow(wxWindow* parent,
                                   core::ThemeEngine& theme_engine,
                                   wxWindowID id,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long style)
    : wxPanel(parent, id, pos, size, style)
    , theme_engine_(theme_engine)
{
    // Subscribe to theme changes via ThemeEngine's subscription helper
    theme_subscription_ = theme_engine_.subscribe_theme_change(
        [this](const std::string& /*theme_id*/)
        {
            OnThemeChanged(theme_engine_.current_theme());
            Refresh();
        });

    // Apply initial theme
    SetBackgroundColour(theme_engine_.color(core::ThemeColorToken::BgApp));
    SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMain));
}

ThemeAwareWindow::~ThemeAwareWindow() = default;

void ThemeAwareWindow::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    // Default: update background and foreground
    SetBackgroundColour(theme_engine_.color(core::ThemeColorToken::BgApp));
    SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMain));
}

auto ThemeAwareWindow::theme() const -> const core::Theme&
{
    return theme_engine_.current_theme();
}

auto ThemeAwareWindow::theme_engine() const -> core::ThemeEngine&
{
    return theme_engine_;
}

void ThemeAwareWindow::FillBackground(wxDC& dc, core::ThemeColorToken token)
{
    dc.SetBrush(theme_engine_.brush(token));
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(GetClientSize());
}

} // namespace markamp::ui
