#pragma once

#include "core/ThemeEngine.h"

#include <wx/panel.h>

namespace markamp::ui::accessibility
{

/// A visually hidden button that becomes visible when receiving keyboard focus.
/// Used for allowing screen reader and keyboard-only users to bypass navigation
/// and jump directly to the main content area.
class SkipToContentButton : public wxPanel
{
public:
    SkipToContentButton(wxWindow* parent,
                        core::ThemeEngine& theme_engine,
                        wxWindowID id = wxID_ANY);
    ~SkipToContentButton() override = default;

private:
    void OnPaint(wxPaintEvent& event);
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnLeftDown(wxMouseEvent& event);

    void Activate();

    core::ThemeEngine& theme_engine_;
    bool has_focus_{false};
};

} // namespace markamp::ui::accessibility
