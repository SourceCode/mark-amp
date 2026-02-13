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

    // R18 Fix 40: Focus ring for keyboard accessibility
    Bind(wxEVT_SET_FOCUS,
         [this](wxFocusEvent& evt)
         {
             has_focus_ = true;
             Refresh();
             evt.Skip();
         });
    Bind(wxEVT_KILL_FOCUS,
         [this](wxFocusEvent& evt)
         {
             has_focus_ = false;
             Refresh();
             evt.Skip();
         });
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

void ThemeAwareWindow::DrawFocusRing(wxDC& dc)
{
    if (!has_focus_)
    {
        return;
    }
    auto accent = theme_engine_.color(core::ThemeColorToken::AccentPrimary);
    dc.SetPen(wxPen(accent, 2));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    auto client_size = GetClientSize();
    dc.DrawRectangle(1, 1, client_size.GetWidth() - 2, client_size.GetHeight() - 2);
}

} // namespace markamp::ui
