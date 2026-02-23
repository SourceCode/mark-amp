#include "SkipToContentButton.h"

#include "ui/FocusManager.h"

#include <wx/dcbuffer.h>

namespace markamp::ui::accessibility
{

SkipToContentButton::SkipToContentButton(wxWindow* parent,
                                         core::ThemeEngine& theme_engine,
                                         wxWindowID id)
    : wxPanel(parent, id, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS | wxBORDER_NONE)
    , theme_engine_(theme_engine)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    Bind(wxEVT_PAINT, &SkipToContentButton::OnPaint, this);
    Bind(wxEVT_SET_FOCUS, &SkipToContentButton::OnSetFocus, this);
    Bind(wxEVT_KILL_FOCUS, &SkipToContentButton::OnKillFocus, this);
    Bind(wxEVT_KEY_DOWN, &SkipToContentButton::OnKeyDown, this);
    Bind(wxEVT_LEFT_DOWN, &SkipToContentButton::OnLeftDown, this);

    SetMinSize(wxSize(1, 1));
}

void SkipToContentButton::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);

    if (!has_focus_)
    {
        // Draw nothing to keep it transparent when not focused
        dc.Clear();
        return;
    }

    auto rect = GetClientRect();

    // Draw background
    dc.SetBrush(wxBrush(theme_engine_.color(core::ThemeColorToken::BgPanel)));
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(rect);

    // Draw text
    dc.SetFont(GetFont());
    dc.SetTextForeground(theme_engine_.color(core::ThemeColorToken::TextMain));
    wxString text = "Skip to Content";

    wxSize textSize = dc.GetTextExtent(text);
    int x = (rect.GetWidth() - textSize.GetWidth()) / 2;
    int y = (rect.GetHeight() - textSize.GetHeight()) / 2;
    dc.DrawText(text, x, y);

    // Draw focus ring indicator
    dc.SetPen(wxPen(theme_engine_.color(core::ThemeColorToken::AccentPrimary), 2));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(rect);
}

void SkipToContentButton::OnSetFocus(wxFocusEvent& event)
{
    has_focus_ = true;
    Refresh();
    event.Skip();
}

void SkipToContentButton::OnKillFocus(wxFocusEvent& event)
{
    has_focus_ = false;
    Refresh();
    event.Skip();
}

void SkipToContentButton::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_SPACE)
    {
        Activate();
    }
    else
    {
        event.Skip();
    }
}

void SkipToContentButton::OnLeftDown(wxMouseEvent& /*event*/)
{
    Activate();
}

void SkipToContentButton::Activate()
{
    // Notify global focus manager to move focus directly to the editor zone
    FocusManager::get().set_focus(FocusZoneId::kEditorArea, -1);
}

} // namespace markamp::ui::accessibility
