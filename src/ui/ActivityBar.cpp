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
        int text_x = (kBarWidth - text_extent.GetWidth()) / 2;
        int text_y = item_y + (kBarWidth - text_extent.GetHeight()) / 2;
        paint_dc.DrawText(item.icon_char, text_x, text_y);

        item_y += kBarWidth;
    }

    // R17 Fix 29: Bottom border separator below last item
    if (!items_.empty())
    {
        auto border_light = clr.border_light.to_wx_colour();
        paint_dc.SetPen(wxPen(border_light));
        paint_dc.DrawLine(4, item_y, kBarWidth - 4, item_y);
    }

    // Separator line on the right edge
    auto border = clr.border_light.to_wx_colour();
    paint_dc.SetPen(wxPen(border));
    paint_dc.DrawLine(size.GetWidth() - 1, 0, size.GetWidth() - 1, size.GetHeight());
}

void ActivityBar::OnMouseDown(wxMouseEvent& event)
{
    int idx = HitTest(event.GetPosition());
    if (idx >= 0 && idx < static_cast<int>(items_.size()))
    {
        auto item = items_[static_cast<std::size_t>(idx)].item_id;
        SetActiveItem(item);

        core::events::ActivityBarSelectionEvent evt(item);
        event_bus_.publish(evt);
    }
}

void ActivityBar::OnMouseMove(wxMouseEvent& event)
{
    int idx = HitTest(event.GetPosition());
    if (idx != hover_index_)
    {
        hover_index_ = idx;

        // Set tooltip
        if (idx >= 0 && idx < static_cast<int>(items_.size()))
        {
            SetToolTip(items_[static_cast<std::size_t>(idx)].label);
        }
        else
        {
            UnsetToolTip();
        }

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

} // namespace markamp::ui
