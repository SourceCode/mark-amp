#include "ui/SidebarFooter.h"

#include "ui/DesignSystemContext.h"
#include "ui/TypographyScale.h"

#include <wx/dcbuffer.h>

namespace markamp::ui
{

wxBEGIN_EVENT_TABLE(SidebarFooter, ThemeAwareWindow) EVT_PAINT(SidebarFooter::OnPaint)
    EVT_SIZE(SidebarFooter::OnSize) wxEND_EVENT_TABLE()

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
}

} // namespace markamp::ui
