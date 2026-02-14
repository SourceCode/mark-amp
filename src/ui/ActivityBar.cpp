#include "ActivityBar.h"

#include "core/Logger.h"

#include <wx/dcbuffer.h>

namespace markamp::ui
{

ActivityBar::ActivityBar(wxWindow* parent,
                         core::ThemeEngine& theme_engine,
                         core::EventBus& event_bus)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(kBarWidth, -1))
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
{
    SetMinSize(wxSize(kBarWidth, -1));
    SetMaxSize(wxSize(kBarWidth, -1));
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    CreateItems();
    ApplyTheme();

    Bind(wxEVT_PAINT, &ActivityBar::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &ActivityBar::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &ActivityBar::OnMouseUp, this);         // R20 Fix 18
    Bind(wxEVT_LEFT_DCLICK, &ActivityBar::OnDoubleClick, this); // R20 Fix 16
    Bind(wxEVT_MOTION, &ActivityBar::OnMouseMove, this);
    Bind(wxEVT_LEAVE_WINDOW, &ActivityBar::OnMouseLeave, this);

    theme_sub_ = event_bus_.subscribe<core::events::ThemeChangedEvent>(
        [this](const core::events::ThemeChangedEvent& /*evt*/) { ApplyTheme(); });
}

void ActivityBar::CreateItems()
{
    items_ = {
        {core::events::ActivityBarItem::FileExplorer, "Explorer", "\xF0\x9F\x93\x81", {}},
        {core::events::ActivityBarItem::Search, "Search", "\xF0\x9F\x94\x8D", {}},
        {core::events::ActivityBarItem::Settings, "Settings", "\xE2\x9A\x99", {}},
        {core::events::ActivityBarItem::Themes, "Themes", "\xF0\x9F\x8E\xA8", {}},
    };
}

void ActivityBar::SetActiveItem(core::events::ActivityBarItem item)
{
    active_item_ = item;
    Refresh();
}

auto ActivityBar::GetActiveItem() const -> core::events::ActivityBarItem
{
    return active_item_;
}

void ActivityBar::ApplyTheme()
{
    const auto& theme = theme_engine_.current_theme();
    SetBackgroundColour(theme.colors.bg_panel.to_wx_colour());
    Refresh();
}

void ActivityBar::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC paint_dc(this);
    const auto& theme = theme_engine_.current_theme();
    const auto& clr = theme.colors;

    // Background
    paint_dc.SetBackground(wxBrush(clr.bg_panel.to_wx_colour()));
    paint_dc.Clear();

    auto size = GetClientSize();
    int item_y = kIconPadding;

    for (int idx = 0; idx < static_cast<int>(items_.size()); ++idx)
    {
        auto& item = items_[static_cast<std::size_t>(idx)];
        item.bounds = wxRect(0, item_y, kBarWidth, kBarWidth);

        bool is_active = (item.item_id == active_item_);
        bool is_hover = (idx == hover_index_);

        // R17 Fix 30: Active item background highlight — subtle accent tint
        if (is_active)
        {
            auto active_bg = clr.accent_primary.with_alpha(0.12F);
            paint_dc.SetBrush(wxBrush(active_bg.to_wx_colour()));
            paint_dc.SetPen(*wxTRANSPARENT_PEN);
            paint_dc.DrawRectangle(0, item_y, kBarWidth, kBarWidth);
        }

        // Active indicator (left border bar)
        if (is_active)
        {
            paint_dc.SetBrush(wxBrush(clr.accent_primary.to_wx_colour()));
            paint_dc.SetPen(*wxTRANSPARENT_PEN);
            paint_dc.DrawRectangle(0, item_y, 3, kBarWidth);
        }

        // Hover background
        if (is_hover && !is_active)
        {
            auto hover = clr.bg_panel.lighten(0.1F);
            paint_dc.SetBrush(wxBrush(hover.to_wx_colour()));
            paint_dc.SetPen(*wxTRANSPARENT_PEN);
            paint_dc.DrawRectangle(0, item_y, kBarWidth, kBarWidth);
        }

        // Icon text (emoji fallback)
        // R20 Fix 18: press offset — shift icon 1px when pressed
        int press_offset_x = 0;
        int press_offset_y = 0;
        if (idx == pressed_index_)
        {
            press_offset_x = 1;
            press_offset_y = 1;
        }

        if (is_active)
        {
            paint_dc.SetTextForeground(clr.editor_fg.to_wx_colour());
        }
        else
        {
            // Dimmed for inactive — blend fg towards bg
            auto dimmed = clr.editor_fg.blend(clr.bg_panel, 0.5F);
            paint_dc.SetTextForeground(dimmed.to_wx_colour());
        }

        auto font = GetFont();
        font.SetPointSize(16);
        paint_dc.SetFont(font);

        auto text_extent = paint_dc.GetTextExtent(item.icon_char);
        int text_x = (kBarWidth - text_extent.GetWidth()) / 2 + press_offset_x;
        int text_y = item_y + (kBarWidth - text_extent.GetHeight()) / 2 + press_offset_y;
        paint_dc.DrawText(item.icon_char, text_x, text_y);

        // R18 Fix 25: Badge count indicator
        if (item.badge_count > 0)
        {
            auto badge_bg = clr.accent_primary.to_wx_colour();
            paint_dc.SetBrush(wxBrush(badge_bg));
            paint_dc.SetPen(*wxTRANSPARENT_PEN);

            int badge_x = kBarWidth - 16;
            int badge_y_pos = item_y + 4;
            int badge_r = 8;
            paint_dc.DrawCircle(badge_x, badge_y_pos + badge_r, badge_r);

            paint_dc.SetTextForeground(*wxWHITE);
            auto badge_font = GetFont();
            badge_font.SetPointSize(8);
            paint_dc.SetFont(badge_font);
            auto badge_text = wxString::Format("%d", item.badge_count);
            auto badge_extent = paint_dc.GetTextExtent(badge_text);
            paint_dc.DrawText(badge_text,
                              badge_x - badge_extent.GetWidth() / 2,
                              badge_y_pos + badge_r - badge_extent.GetHeight() / 2);

            // Restore font
            paint_dc.SetFont(font);
        }

        item_y += kBarWidth;
    }

    // R17 Fix 29: Bottom border separator below last item
    if (!items_.empty())
    {
        auto border_light = clr.border_light.to_wx_colour();
        paint_dc.SetPen(wxPen(border_light));
        paint_dc.DrawLine(4, item_y, kBarWidth - 4, item_y);
    }

    // R20 Fix 20: Separator above the bottom-most item (last item)
    if (items_.size() > 1)
    {
        const auto& last_item = items_.back();
        int sep_y = last_item.bounds.GetY() - 2;
        auto sep_col = clr.border_light.to_wx_colour();
        paint_dc.SetPen(wxPen(sep_col));
        paint_dc.DrawLine(8, sep_y, kBarWidth - 8, sep_y);
    }

    // R20 Fix 17: Drag handle dots — 3 small dots centered in bar
    {
        int drag_y = size.GetHeight() - 40;
        auto dot_col = clr.text_muted.to_wx_colour();
        paint_dc.SetBrush(wxBrush(dot_col));
        paint_dc.SetPen(*wxTRANSPARENT_PEN);
        int dot_x = kBarWidth / 2;
        for (int dot_idx = 0; dot_idx < 3; ++dot_idx)
        {
            paint_dc.DrawCircle(dot_x, drag_y + dot_idx * 6, 2);
        }
    }

    // R20 Fix 19: Themed tooltip pill for hovered item
    if (hover_index_ >= 0 && hover_index_ < static_cast<int>(items_.size()))
    {
        const auto& hov_item = items_[static_cast<std::size_t>(hover_index_)];
        auto pill_bg = clr.bg_header.to_wx_colour();
        auto pill_fg = clr.editor_fg.to_wx_colour();
        auto pill_font = GetFont();
        pill_font.SetPointSize(9);
        paint_dc.SetFont(pill_font);
        auto tip_extent = paint_dc.GetTextExtent(hov_item.label);
        int pill_x = kBarWidth + 4;
        int pill_y =
            hov_item.bounds.GetY() + (hov_item.bounds.GetHeight() - tip_extent.GetHeight() - 8) / 2;
        paint_dc.SetBrush(wxBrush(pill_bg));
        paint_dc.SetPen(wxPen(clr.border_light.to_wx_colour()));
        paint_dc.DrawRoundedRectangle(
            pill_x, pill_y, tip_extent.GetWidth() + 16, tip_extent.GetHeight() + 8, 6);
        paint_dc.SetTextForeground(pill_fg);
        paint_dc.DrawText(hov_item.label, pill_x + 8, pill_y + 4);
    }

    // Separator line on the right edge
    auto border = clr.border_light.to_wx_colour();
    paint_dc.SetPen(wxPen(border));
    paint_dc.DrawLine(size.GetWidth() - 1, 0, size.GetWidth() - 1, size.GetHeight());
}

void ActivityBar::OnMouseDown(wxMouseEvent& event)
{
    int idx = HitTest(event.GetPosition());
    // R20 Fix 18: Track pressed item for visual feedback
    pressed_index_ = idx;
    Refresh();

    if (idx >= 0 && idx < static_cast<int>(items_.size()))
    {
        auto item = items_[static_cast<std::size_t>(idx)].item_id;
        SetActiveItem(item);

        core::events::ActivityBarSelectionEvent evt(item);
        event_bus_.publish(evt);
    }
}

// R20 Fix 18: Mouse-up restores press state
void ActivityBar::OnMouseUp(wxMouseEvent& /*event*/)
{
    pressed_index_ = -1;
    Refresh();
}

// R20 Fix 16: Double-click active item collapses sidebar
void ActivityBar::OnDoubleClick(wxMouseEvent& event)
{
    int idx = HitTest(event.GetPosition());
    if (idx >= 0 && idx < static_cast<int>(items_.size()))
    {
        auto item = items_[static_cast<std::size_t>(idx)].item_id;
        if (item == active_item_)
        {
            core::events::SidebarToggleEvent toggle_evt;
            event_bus_.publish(toggle_evt);
        }
    }
}

void ActivityBar::OnMouseMove(wxMouseEvent& event)
{
    int idx = HitTest(event.GetPosition());
    if (idx != hover_index_)
    {
        hover_index_ = idx;

        // R20 Fix 19: Use themed tooltip pill instead of native (drawn in OnPaint)
        // Still unset native tooltip to avoid double display
        UnsetToolTip();

        Refresh();
    }
}

void ActivityBar::OnMouseLeave(wxMouseEvent& /*event*/)
{
    if (hover_index_ != -1)
    {
        hover_index_ = -1;
        UnsetToolTip();
        Refresh();
    }
}

auto ActivityBar::HitTest(const wxPoint& pos) const -> int
{
    for (int idx = 0; idx < static_cast<int>(items_.size()); ++idx)
    {
        if (items_[static_cast<std::size_t>(idx)].bounds.Contains(pos))
        {
            return idx;
        }
    }
    return -1;
}

// R18 Fix 25: Set badge count on an activity bar item
void ActivityBar::SetBadge(core::events::ActivityBarItem item, int count)
{
    for (auto& bar_item : items_)
    {
        if (bar_item.item_id == item)
        {
            bar_item.badge_count = count;
            Refresh();
            return;
        }
    }
}

} // namespace markamp::ui
