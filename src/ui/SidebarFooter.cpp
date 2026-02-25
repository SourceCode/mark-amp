#include "ui/SidebarFooter.h"

#include "ui/DesignSystemContext.h"
#include "ui/TypographyScale.h"
#include "ui/accessibility/AccessibilityController.h"

#include <wx/dcbuffer.h>

namespace markamp::ui
{

wxBEGIN_EVENT_TABLE(SidebarFooter, ThemeAwareWindow) EVT_PAINT(SidebarFooter::OnPaint)
    EVT_SIZE(SidebarFooter::OnSize) EVT_SET_FOCUS(SidebarFooter::OnSetFocus)
        EVT_KILL_FOCUS(SidebarFooter::OnKillFocus) wxEND_EVENT_TABLE()

            SidebarFooter::SidebarFooter(wxWindow* parent,
                                         DesignSystemContext& ds,
                                         core::EventBus& /*event_bus*/)
    : ThemeAwareWindow(parent,
                       ds.theme,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxDefaultSize,
                       wxBORDER_NONE | wxFULL_REPAINT_ON_RESIZE)
    , ds_(ds)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    UpdateMetrics();
}

void SidebarFooter::set_text(const std::string& text)
{
    if (text_ != text)
    {
        text_ = text;
        accessibility::AccessibilityController::get().announce(text_, false);
        Refresh();
    }
}

void SidebarFooter::UpdateMetrics()
{
    footer_height_ = 24; // Hardcoded default, can pull from ds if needed
    SetMinSize(wxSize(-1, footer_height_));
    SetMaxSize(wxSize(-1, footer_height_));
}

void SidebarFooter::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    UpdateMetrics();
    Refresh();
}

void SidebarFooter::OnSize(wxSizeEvent& event)
{
    event.Skip();
}

void SidebarFooter::OnSetFocus(wxFocusEvent& event)
{
    accessibility::AccessibilityController::get().announce_focus(text_, "Status", "");
    Refresh();
    event.Skip();
}

void SidebarFooter::OnKillFocus(wxFocusEvent& event)
{
    Refresh();
    event.Skip();
}

void SidebarFooter::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);

    auto& current_theme = ds_.theme;

    dc.SetBackground(wxBrush(current_theme.color(core::ThemeColorToken::SidebarBg)));
    dc.Clear();

    // Top border separating footer from content
    dc.SetPen(wxPen(current_theme.color(core::ThemeColorToken::BorderDark)));
    wxSize size = GetClientSize();
    dc.DrawLine(0, 0, size.x, 0);

    // Text - Small, muted
    dc.SetFont(ds_.typography.font(TypeSlot::kCaption));
    dc.SetTextForeground(current_theme.color(core::ThemeColorToken::TextMuted));

    wxSize extent = dc.GetTextExtent(text_);

    int left_padding = 10;
    dc.DrawText(text_, left_padding, (footer_height_ - extent.y) / 2);

    if (HasFocus())
    {
        dc.SetPen(wxPen(current_theme.color(core::ThemeColorToken::FocusRingColor)));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(0, 0, size.x, footer_height_);
    }
}

} // namespace markamp::ui
