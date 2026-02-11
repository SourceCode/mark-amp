#pragma once

#include "core/EventBus.h"
#include "core/Theme.h"
#include "core/ThemeEngine.h"

#include <wx/panel.h>

namespace markamp::ui
{

/// Base class for UI panels that respond to theme changes.
/// Subscribes to ThemeChangedEvent via RAII and provides convenience accessors.
class ThemeAwareWindow : public wxPanel
{
public:
    ThemeAwareWindow(wxWindow* parent,
                     core::ThemeEngine& theme_engine,
                     wxWindowID id = wxID_ANY,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxTAB_TRAVERSAL);

    ~ThemeAwareWindow() override;

    // Non-copyable, non-movable (destructor unsubscribes)
    ThemeAwareWindow(const ThemeAwareWindow&) = delete;
    auto operator=(const ThemeAwareWindow&) -> ThemeAwareWindow& = delete;
    ThemeAwareWindow(ThemeAwareWindow&&) = delete;
    auto operator=(ThemeAwareWindow&&) -> ThemeAwareWindow& = delete;

protected:
    /// Called when the theme changes. Subclasses override to update appearance.
    virtual void OnThemeChanged(const core::Theme& new_theme);

    /// Convenience: current theme.
    [[nodiscard]] auto theme() const -> const core::Theme&;

    /// Convenience: theme engine reference.
    [[nodiscard]] auto theme_engine() const -> core::ThemeEngine&;

    /// Fill the DC background with a theme color token.
    void FillBackground(wxDC& dc, core::ThemeColorToken token);

private:
    core::ThemeEngine& theme_engine_;
    core::Subscription theme_subscription_;
};

} // namespace markamp::ui
