#include "TerminalTabBar.h"

#include "core/Logger.h"

#include <wx/dcbuffer.h>

#include <algorithm>

namespace markamp::ui
{

TerminalTabBar::TerminalTabBar(wxWindow* parent,
                               core::ThemeEngine& theme_engine,
                               core::EventBus& event_bus)
    : ThemeAwareWindow(parent, theme_engine, wxID_ANY, wxDefaultPosition, wxSize(-1, kHeight))
{
    (void)event_bus; // Reserved for future event subscriptions
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(-1, kHeight));
    SetMaxSize(wxSize(-1, kHeight));

    Bind(wxEVT_PAINT, &TerminalTabBar::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &TerminalTabBar::OnMouseDown, this);
    Bind(wxEVT_MOTION, &TerminalTabBar::OnMouseMove, this);
}

void TerminalTabBar::AddTab(int terminal_id, const std::string& name, const std::string& shell_type)
{
    TerminalTab tab;
    tab.terminal_id = terminal_id;
    tab.name = name;
    tab.shell_type = shell_type;
    tabs_.push_back(std::move(tab));
    RecalculateTabRects();
    Refresh();
}

void TerminalTabBar::RemoveTab(int terminal_id)
{
    std::erase_if(tabs_,
                  [terminal_id](const TerminalTab& tab_item)
                  { return tab_item.terminal_id == terminal_id; });
    RecalculateTabRects();
    Refresh();
}

void TerminalTabBar::SetActiveTab(int terminal_id)
{
    for (auto& tab_item : tabs_)
    {
        tab_item.is_active = (tab_item.terminal_id == terminal_id);
    }
    Refresh();
}

void TerminalTabBar::RenameTab(int terminal_id, const std::string& new_name)
{
    for (auto& tab_item : tabs_)
    {
        if (tab_item.terminal_id == terminal_id)
        {
            tab_item.name = new_name;
            RecalculateTabRects();
            Refresh();
            return;
        }
    }
}

auto TerminalTabBar::tab_count() const -> int
{
    return static_cast<int>(tabs_.size());
}

void TerminalTabBar::set_on_tab_selected(TabSelectedCallback callback)
{
    on_tab_selected_ = std::move(callback);
}

void TerminalTabBar::set_on_new_terminal(NewTerminalCallback callback)
{
    on_new_terminal_ = std::move(callback);
}

void TerminalTabBar::set_on_close_terminal(CloseTerminalCallback callback)
{
    on_close_terminal_ = std::move(callback);
}

void TerminalTabBar::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);
    Refresh();
}

void TerminalTabBar::OnPaint(wxPaintEvent& /*event*/)
{
    wxBufferedPaintDC device_context(this);
    const auto& app_theme = this->theme();

    // Background
    device_context.SetBackground(wxBrush(app_theme.colors.bg_panel.to_wx_colour()));
    device_context.Clear();

    const wxFont tab_font(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    device_context.SetFont(tab_font);

    for (const auto& tab_item : tabs_)
    {
        // Tab background
        if (tab_item.is_active)
        {
            device_context.SetBrush(wxBrush(app_theme.colors.editor_bg.to_wx_colour()));
            device_context.SetPen(*wxTRANSPARENT_PEN);
            device_context.DrawRectangle(tab_item.rect);
        }
        else if (hovered_tab_ == tab_item.terminal_id)
        {
            wxColour hover_bg = app_theme.colors.bg_panel.to_wx_colour().ChangeLightness(110);
            device_context.SetBrush(wxBrush(hover_bg));
            device_context.SetPen(*wxTRANSPARENT_PEN);
            device_context.DrawRectangle(tab_item.rect);
        }

        // Tab text
        device_context.SetTextForeground(tab_item.is_active
                                             ? app_theme.colors.text_main.to_wx_colour()
                                             : app_theme.colors.text_muted.to_wx_colour());
        device_context.DrawText(tab_item.name,
                                tab_item.rect.x + 8,
                                tab_item.rect.y + (kHeight - device_context.GetCharHeight()) / 2);

        // Close button "×"
        device_context.SetTextForeground(app_theme.colors.text_muted.to_wx_colour());
        device_context.DrawText(
            wxString::FromUTF8("\xc3\x97"), tab_item.close_rect.x + 2, tab_item.close_rect.y + 2);
    }

    // "+" button
    device_context.SetTextForeground(app_theme.colors.text_muted.to_wx_colour());
    device_context.DrawText("+",
                            plus_button_rect_.x + 4,
                            plus_button_rect_.y + (kHeight - device_context.GetCharHeight()) / 2);
}

void TerminalTabBar::OnMouseDown(wxMouseEvent& event)
{
    const wxPoint pos = event.GetPosition();

    // Check "+" button
    if (plus_button_rect_.Contains(pos))
    {
        if (on_new_terminal_)
        {
            on_new_terminal_();
        }
        return;
    }

    // Check tabs
    for (const auto& tab_item : tabs_)
    {
        if (tab_item.close_rect.Contains(pos))
        {
            if (on_close_terminal_)
            {
                on_close_terminal_(tab_item.terminal_id);
            }
            return;
        }

        if (tab_item.rect.Contains(pos))
        {
            if (on_tab_selected_)
            {
                on_tab_selected_(tab_item.terminal_id);
            }
            return;
        }
    }
}

void TerminalTabBar::OnMouseMove(wxMouseEvent& event)
{
    const wxPoint pos = event.GetPosition();
    int new_hovered = -1;

    for (const auto& tab_item : tabs_)
    {
        if (tab_item.rect.Contains(pos))
        {
            new_hovered = tab_item.terminal_id;
            break;
        }
    }

    if (new_hovered != hovered_tab_)
    {
        hovered_tab_ = new_hovered;
        Refresh();
    }
}

void TerminalTabBar::RecalculateTabRects()
{
    constexpr int kTabWidth = 150;
    constexpr int kCloseSize = 14;
    constexpr int kPlusWidth = 24;

    int x_offset = 0;
    for (auto& tab_item : tabs_)
    {
        tab_item.rect = wxRect(x_offset, 0, kTabWidth, kHeight);
        tab_item.close_rect = wxRect(x_offset + kTabWidth - kCloseSize - 6,
                                     (kHeight - kCloseSize) / 2,
                                     kCloseSize,
                                     kCloseSize);
        x_offset += kTabWidth;
    }

    plus_button_rect_ = wxRect(x_offset, 0, kPlusWidth, kHeight);
}

} // namespace markamp::ui
