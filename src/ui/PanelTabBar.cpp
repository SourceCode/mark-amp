/**
 * @file PanelTabBar.cpp
 */

#include "ui/PanelTabBar.h"

#include <wx/dataobj.h>
#include <wx/dcbuffer.h>
#include <wx/dnd.h>
#include <wx/graphics.h>
#include <wx/menu.h>

#include <algorithm>

namespace markamp::ui
{

PanelTabBar::PanelTabBar(wxWindow* parent,
                         DesignSystemContext& ds,
                         core::EventBus& event_bus,
                         PanelAreaModel& model)
    : ThemeAwareWindow(parent,
                       ds.theme,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxSize(-1, kTabHeight),
                       wxBORDER_NONE | wxFULL_REPAINT_ON_RESIZE | wxWANTS_CHARS)
    , event_bus_(event_bus)
    , model_(model)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    Bind(wxEVT_PAINT, &PanelTabBar::OnPaint, this);
    Bind(wxEVT_SIZE, &PanelTabBar::OnSize, this);
    Bind(wxEVT_MOTION, &PanelTabBar::OnMouseMove, this);
    Bind(wxEVT_LEFT_DOWN, &PanelTabBar::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &PanelTabBar::OnMouseUp, this);
    Bind(wxEVT_LEAVE_WINDOW, &PanelTabBar::OnMouseLeave, this);
    Bind(wxEVT_RIGHT_DOWN, &PanelTabBar::OnRightDown, this);

    Bind(wxEVT_SET_FOCUS, &PanelTabBar::OnSetFocus, this);
    Bind(wxEVT_KILL_FOCUS, &PanelTabBar::OnKillFocus, this);
    Bind(wxEVT_KEY_DOWN, &PanelTabBar::OnKeyDown, this);

    // Subscribe to model changes
    model_sub_ = event_bus_.subscribe<core::events::PanelAreaTabsChangedEvent>(
        [this](const auto&) { SyncWithModel(); });

    badge_sub_ = event_bus_.subscribe<core::events::PanelAreaBadgeChangedEvent>(
        [this](const auto&) { SyncWithModel(); });

    SyncWithModel();
}

PanelTabBar::~PanelTabBar() = default;

void PanelTabBar::OnThemeChanged(const core::Theme&)
{
    Refresh();
}

auto PanelTabBar::GetActivePanelId() const -> std::string
{
    return model_.active_panel();
}

auto PanelTabBar::GetTabCount() const -> int
{
    return static_cast<int>(tabs_.size());
}

void PanelTabBar::UpdateLayoutMetrics()
{
    RecalculateTabRects();
    Refresh();
}

void PanelTabBar::SyncWithModel()
{
    auto configs = model_.panels();
    auto active_id = model_.active_panel();

    tabs_.clear();
    tabs_.reserve(configs.size());

    for (const auto& config : configs)
    {
        PanelTabRenderInfo info;
        info.id = config.id;
        info.title = config.title;
        info.icon_name = config.icon_name;
        info.badge = config.badge;
        info.badge_count = config.badge_count;
        info.closable = config.closable;
        info.is_active = (config.id == active_id);
        info.order = config.order;
        tabs_.push_back(info);
    }

    RecalculateTabRects();
    Refresh();
}

void PanelTabBar::RecalculateTabRects()
{
    if (tabs_.empty())
        return;

    int width = 0;
    int height = 0;
    GetClientSize(&width, &height);

    // Right-aligned maximize button
    maximize_button_rect_ = wxRect(width - 24, (height - 16) / 2, 16, 16);

    int available_tabs_width = width - 24;
    has_overflow_ = false;

    int total_desired_width = static_cast<int>(tabs_.size()) * kMaxTabWidth;
    int tab_width = kMaxTabWidth;

    if (total_desired_width > available_tabs_width)
    {
        int min_required_width = static_cast<int>(tabs_.size()) * kMinTabWidth;
        if (min_required_width > available_tabs_width)
        {
            has_overflow_ = true;
            available_tabs_width -= 24; // Allocate space for overflow button
            overflow_button_rect_ = wxRect(available_tabs_width, (height - 16) / 2, 16, 16);
            tab_width = kMinTabWidth;
        }
        else
        {
            tab_width = available_tabs_width / static_cast<int>(tabs_.size());
        }
    }

    int current_x = 0;
    for (auto& tab : tabs_)
    {
        if (has_overflow_ && current_x + tab_width > available_tabs_width)
        {
            tab.is_visible = false;
            tab.rect = wxRect();
            tab.close_rect = wxRect();
        }
        else
        {
            tab.is_visible = true;
            tab.rect = wxRect(current_x, 0, tab_width, height);

            if (tab.closable)
            {
                tab.close_rect = wxRect(
                    current_x + tab_width - kCloseButtonMargin - 14, (height - 14) / 2, 14, 14);
            }
            else
            {
                tab.close_rect = wxRect();
            }
            current_x += tab_width;
        }
    }
}

void PanelTabBar::OnSize(wxSizeEvent& event)
{
    RecalculateTabRects();
    Refresh();
    event.Skip();
}

void PanelTabBar::OnPaint(wxPaintEvent&)
{
    wxAutoBufferedPaintDC dc(this);
    std::unique_ptr<wxGraphicsContext> gc{wxGraphicsContext::Create(dc)};
    if (!gc)
        return;

    // Draw background
    int width = 0;
    int height = 0;
    GetClientSize(&width, &height);
    gc->SetBrush(
        wxBrush(theme_engine().color(core::ThemeColorToken::BgPanel))); // or tab bar background
    gc->SetPen(*wxTRANSPARENT_PEN);
    gc->DrawRectangle(0, 0, width, height);

    // Draw tabs
    for (const auto& tab : tabs_)
    {
        if (tab.is_visible)
        {
            DrawTab(*gc, tab);
        }
    }

    if (has_overflow_)
    {
        DrawOverflowButton(*gc);
    }

    // Draw maximize button
    DrawMaximizeButton(*gc);
}

void PanelTabBar::DrawTab(wxGraphicsContext& gc, const PanelTabRenderInfo& tab) const
{
    auto bg_color = tab.is_active ? theme_engine().color(core::ThemeColorToken::BgApp)
                                  : theme_engine().color(core::ThemeColorToken::BgPanel);

    if (hovered_tab_index_ >= 0 && tabs_[static_cast<size_t>(hovered_tab_index_)].id == tab.id &&
        !tab.is_active)
    {
        bg_color = theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(115);
    }

    gc.SetBrush(wxBrush(bg_color));
    gc.SetPen(*wxTRANSPARENT_PEN);
    gc.DrawRectangle(tab.rect.x, tab.rect.y, tab.rect.width, tab.rect.height);

    // Active glow top border
    if (tab.is_active)
    {
        gc.SetBrush(wxBrush(theme_engine().color(core::ThemeColorToken::AccentPrimary)));
        gc.DrawRectangle(tab.rect.x, tab.rect.y, tab.rect.width, 2);
    }

    // Draw Focus Ring if this tab is keyboard focused
    if (has_focus_ && (&tab - &tabs_[0]) == keyboard_focused_tab_index_)
    {
        gc.SetPen(
            wxPen(theme_engine().color(core::ThemeColorToken::AccentPrimary), 1, wxPENSTYLE_DOT));
        gc.SetBrush(*wxTRANSPARENT_BRUSH);
        gc.DrawRectangle(tab.rect.x + 1, tab.rect.y + 1, tab.rect.width - 2, tab.rect.height - 2);
    }

    // Text bounds
    auto fg_color = tab.is_active ? theme_engine().color(core::ThemeColorToken::TextMain)
                                  : theme_engine().color(core::ThemeColorToken::TextMuted);
    wxFont font = theme_engine().font(core::ThemeFontToken::SansRegular);
    gc.SetFont(font, fg_color);

    double text_w = 0;
    double text_h = 0;
    gc.GetTextExtent(tab.title, &text_w, &text_h);

    const double x_pos = tab.rect.x + 10;

    // Draw icon space placeholder
    double text_x_pos = x_pos;
    if (!tab.icon_name.empty())
    {
        // Icon drawing depends on your icon catalog, leaving space for now
        text_x_pos += kIconSize + 4;
    }

    const double y_pos = tab.rect.y + (tab.rect.height - text_h) / 2.0;
    gc.DrawText(tab.title, text_x_pos, y_pos);

    // Draw Badge
    if (tab.badge != core::events::BadgeState::kNone)
    {
        DrawBadge(gc, tab);
    }

    // Draw Close Button
    if (tab.closable && (tab.is_active || (&tab - &tabs_[0]) == hovered_tab_index_))
    {
        auto close_fg = fg_color;
        if (tab.close_hovered)
        {
            close_fg = theme_engine().color(core::ThemeColorToken::ErrorColor);
        }
        // Simple X
        gc.SetPen(wxPen(close_fg, 1));
        int cx = tab.close_rect.x + 3;
        int cy = tab.close_rect.y + 3;
        int cw = tab.close_rect.width - 6;
        gc.StrokeLine(cx, cy, cx + cw, cy + cw);
        gc.StrokeLine(cx + cw, cy, cx, cy + cw);
    }
}

void PanelTabBar::DrawBadge(wxGraphicsContext& gc, const PanelTabRenderInfo& tab) const
{
    // Badges render next to text, before close button
    double text_w = 0.0;
    double text_h = 0.0;
    gc.GetTextExtent(tab.title, &text_w, &text_h);
    double start_x = tab.rect.x + 10 + text_w + 6;
    if (!tab.icon_name.empty())
        start_x += kIconSize + 4;

    wxColour badge_bg = theme_engine().color(core::ThemeColorToken::AccentPrimary); // default

    if (tab.badge == core::events::BadgeState::kError)
    {
        badge_bg = theme_engine().color(core::ThemeColorToken::ErrorColor);
    }
    else if (tab.badge == core::events::BadgeState::kWarning)
    {
        badge_bg = theme_engine().color(
            core::ThemeColorToken::AccentSecondary); // Warning usually mapped to secondary or
                                                     // another token
    }
    else if (tab.badge == core::events::BadgeState::kInfo)
    {
        badge_bg = theme_engine().color(core::ThemeColorToken::TextMuted);
    }

    gc.SetBrush(wxBrush(badge_bg));
    gc.SetPen(*wxTRANSPARENT_PEN);

    if (tab.badge == core::events::BadgeState::kDot)
    {
        gc.DrawEllipse(start_x, tab.rect.y + (tab.rect.height - 8) / 2.0, 8, 8);
    }
    else
    {
        auto text = std::to_string(tab.badge_count);
        const wxFont font = theme_engine().font(core::ThemeFontToken::UISmall);
        gc.SetFont(font, theme_engine().color(core::ThemeColorToken::BgApp));
        double bw = 0.0;
        double bh = 0.0;
        gc.GetTextExtent(text, &bw, &bh);
        const double rad = std::max(bw + 8, static_cast<double>(kBadgeSize));
        gc.DrawRoundedRectangle(start_x,
                                tab.rect.y + (tab.rect.height - kBadgeSize) / 2.0,
                                rad,
                                kBadgeSize,
                                kBadgeSize / 2.0);
        gc.DrawText(text, start_x + (rad - bw) / 2.0, tab.rect.y + (tab.rect.height - bh) / 2.0);
    }
}

void PanelTabBar::DrawMaximizeButton(wxGraphicsContext& gc) const
{
    auto fg = maximize_hovered_ ? theme_engine().color(core::ThemeColorToken::TextMain)
                                : theme_engine().color(core::ThemeColorToken::TextMuted);
    gc.SetPen(wxPen(fg, 1));
    gc.SetBrush(*wxTRANSPARENT_BRUSH);

    const int mx = maximize_button_rect_.x;
    const int my = maximize_button_rect_.y;
    const int mw = maximize_button_rect_.width;
    const int mh = maximize_button_rect_.height;

    // Chevron up or down based on is_maximized_
    if (is_maximized_)
    {
        gc.StrokeLine(mx + 2, my + 8, mx + mw / 2.0, my + mh - 2);
        gc.StrokeLine(mx + mw / 2.0, my + mh - 2, mx + mw - 2, my + 8);
    }
    else
    {
        gc.StrokeLine(mx + 2, my + mh - 2, mx + mw / 2.0, my + 2);
        gc.StrokeLine(mx + mw / 2.0, my + 2, mx + mw - 2, my + mh - 2);
    }
}

void PanelTabBar::DrawOverflowButton(wxGraphicsContext& gc) const
{
    auto fg = overflow_hovered_ ? theme_engine().color(core::ThemeColorToken::TextMain)
                                : theme_engine().color(core::ThemeColorToken::TextMuted);
    gc.SetPen(wxPen(fg, 1));
    gc.SetBrush(wxBrush(fg));

    double cx = overflow_button_rect_.x + overflow_button_rect_.width / 2.0;
    double cy = overflow_button_rect_.y + overflow_button_rect_.height / 2.0;

    // Draw 3 dots for overflow
    gc.DrawEllipse(cx - 5, cy - 1, 2, 2);
    gc.DrawEllipse(cx - 1, cy - 1, 2, 2);
    gc.DrawEllipse(cx + 3, cy - 1, 2, 2);
}

auto PanelTabBar::HitTestTab(const wxPoint& point) const -> int
{
    for (size_t i = 0; i < tabs_.size(); ++i)
    {
        if (tabs_[i].is_visible && tabs_[i].rect.Contains(point))
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}

auto PanelTabBar::HitTestCloseButton(const wxPoint& point, int tab_index) const -> bool
{
    if (tab_index < 0 || tab_index >= static_cast<int>(tabs_.size()))
        return false;
    if (!tabs_[static_cast<size_t>(tab_index)].is_visible)
        return false;
    return tabs_[static_cast<size_t>(tab_index)].closable &&
           tabs_[static_cast<size_t>(tab_index)].close_rect.Contains(point);
}

auto PanelTabBar::HitTestMaximizeButton(const wxPoint& point) const -> bool
{
    return maximize_button_rect_.Contains(point);
}

auto PanelTabBar::HitTestOverflowButton(const wxPoint& point) const -> bool
{
    if (!has_overflow_)
        return false;
    return overflow_button_rect_.Contains(point);
}

void PanelTabBar::OnMouseMove(wxMouseEvent& event)
{
    const auto pt = event.GetPosition();

    // Maximize button hover
    const bool max_hover = HitTestMaximizeButton(pt);
    if (max_hover != maximize_hovered_)
    {
        maximize_hovered_ = max_hover;
        Refresh();
    }

    // Overflow button hover
    const bool overflow_hover = HitTestOverflowButton(pt);
    if (overflow_hover != overflow_hovered_)
    {
        overflow_hovered_ = overflow_hover;
        Refresh();
    }

    // Tabs hover
    const int tab_idx = HitTestTab(pt);
    if (tab_idx != hovered_tab_index_)
    {
        if (hovered_tab_index_ >= 0)
        {
            tabs_[static_cast<size_t>(hovered_tab_index_)].close_hovered = false;
        }
        hovered_tab_index_ = tab_idx;
        Refresh();
    }

    if (tab_idx >= 0)
    {
        const bool close_hover = HitTestCloseButton(pt, tab_idx);
        if (tabs_[static_cast<size_t>(tab_idx)].close_hovered != close_hover)
        {
            tabs_[static_cast<size_t>(tab_idx)].close_hovered = close_hover;
            Refresh();
        }
    }

    // Drag handling logic here (Task 7)
    if (is_dragging_ && event.Dragging() && drag_tab_index_ >= 0)
    {
        // Check for drag out of the panel bar bounds (to sidebars)
        if (pt.y < -15 || pt.y > GetClientSize().y + 15)
        {
            const std::string payload =
                "MARKAMP_PANEL:" + tabs_[static_cast<size_t>(drag_tab_index_)].id;
            wxTextDataObject drag_data(payload);

            is_dragging_ = false;
            drag_tab_index_ = -1;
            Refresh();

            wxDropSource drag_source(this);
            drag_source.SetData(drag_data);
            drag_source.DoDragDrop(wxDrag_CopyOnly);

            return;
        }

        const int target_idx = HitTestTab(pt);
        if (target_idx >= 0 && target_idx != drag_tab_index_)
        {
            const auto dragged_id = tabs_[static_cast<size_t>(drag_tab_index_)].id;
            const auto target_id = tabs_[static_cast<size_t>(target_idx)].id;
            const int dragged_order = tabs_[static_cast<size_t>(drag_tab_index_)].order;
            const int target_order = tabs_[static_cast<size_t>(target_idx)].order;

            model_.set_order(dragged_id, target_order);
            model_.set_order(target_id, dragged_order);

            // Resync drag index after synchronous order updates
            for (size_t i = 0; i < tabs_.size(); ++i)
            {
                if (tabs_[i].id == dragged_id)
                {
                    drag_tab_index_ = static_cast<int>(i);
                    break;
                }
            }
        }
    }

    event.Skip();
}

void PanelTabBar::OnMouseDown(wxMouseEvent& event)
{
    const auto pt = event.GetPosition();

    if (HitTestMaximizeButton(pt))
    {
        is_maximized_ = !is_maximized_;
        core::events::PanelAreaMaximizeToggledEvent evt;
        evt.is_maximized = is_maximized_;
        event_bus_.publish(evt);
        maximize_hovered_ = false;
        Refresh();
        return;
    }

    if (HitTestOverflowButton(pt))
    {
        overflow_hovered_ = false;
        wxMenu menu;
        for (const auto& tab : tabs_)
        {
            if (!tab.is_visible)
            {
                wxMenuItem* item = menu.Append(wxID_ANY, tab.title);
                Bind(
                    wxEVT_MENU,
                    [this, id = tab.id](wxCommandEvent&) { model_.set_active(id); },
                    item->GetId());
            }
        }
        if (menu.GetMenuItemCount() > 0)
        {
            PopupMenu(&menu, overflow_button_rect_.x, overflow_button_rect_.GetBottom());
        }
        Refresh();
        return;
    }

    const int tab_idx = HitTestTab(pt);
    if (tab_idx >= 0)
    {
        if (HitTestCloseButton(pt, tab_idx))
        {
            model_.set_visible(tabs_[static_cast<size_t>(tab_idx)].id, false);
            return;
        }

        model_.set_active(tabs_[static_cast<size_t>(tab_idx)].id);

        drag_start_x_ = pt.x;
        drag_tab_index_ = tab_idx;
        is_dragging_ = true;
    }
}

void PanelTabBar::OnMouseUp(wxMouseEvent& event)
{
    if (is_dragging_)
    {
        is_dragging_ = false;
        drag_tab_index_ = -1;
    }
    event.Skip();
}

void PanelTabBar::OnMouseLeave(wxMouseEvent& event)
{
    if (hovered_tab_index_ >= 0)
    {
        tabs_[static_cast<size_t>(hovered_tab_index_)].close_hovered = false;
        hovered_tab_index_ = -1;
        Refresh();
    }
    if (maximize_hovered_)
    {
        maximize_hovered_ = false;
        Refresh();
    }
    event.Skip();
}

void PanelTabBar::OnSetFocus(wxFocusEvent& event)
{
    has_focus_ = true;
    if (keyboard_focused_tab_index_ < 0 && !tabs_.empty())
    {
        // Try to focus the active tab first
        const auto active_id = GetActivePanelId();
        keyboard_focused_tab_index_ = 0;
        for (size_t i = 0; i < tabs_.size(); ++i)
        {
            if (tabs_[i].id == active_id)
            {
                keyboard_focused_tab_index_ = static_cast<int>(i);
                break;
            }
        }
    }
    Refresh();
    event.Skip();
}

void PanelTabBar::OnKillFocus(wxFocusEvent& event)
{
    has_focus_ = false;
    Refresh();
    event.Skip();
}

void PanelTabBar::OnKeyDown(wxKeyEvent& event)
{
    if (tabs_.empty())
    {
        event.Skip();
        return;
    }

    const int keycode = event.GetKeyCode();
    if (keycode == WXK_LEFT)
    {
        keyboard_focused_tab_index_--;
        if (keyboard_focused_tab_index_ < 0)
        {
            keyboard_focused_tab_index_ = static_cast<int>(tabs_.size()) - 1;
        }
        Refresh();
    }
    else if (keycode == WXK_RIGHT)
    {
        keyboard_focused_tab_index_++;
        if (keyboard_focused_tab_index_ >= static_cast<int>(tabs_.size()))
        {
            keyboard_focused_tab_index_ = 0;
        }
        Refresh();
    }
    else if (keycode == WXK_RETURN || keycode == WXK_SPACE)
    {
        if (keyboard_focused_tab_index_ >= 0 &&
            keyboard_focused_tab_index_ < static_cast<int>(tabs_.size()))
        {
            model_.set_active(tabs_[static_cast<size_t>(keyboard_focused_tab_index_)].id);
        }
    }
    else if (keycode == WXK_TAB || keycode == WXK_ESCAPE)
    {
        // Let wxWidgets handle navigation out of the tab bar
        event.Skip();
    }
    else
    {
        event.Skip();
    }
}

void PanelTabBar::OnRightDown(wxMouseEvent& event)
{
    // Context menu
    const int tab_idx = HitTestTab(event.GetPosition());
    if (tab_idx >= 0)
    {
        const auto& target_tab = tabs_[static_cast<size_t>(tab_idx)];

        wxMenu menu;

        // Close Panel
        auto* close_item = menu.Append(wxID_ANY, "Close Panel");
        if (!target_tab.closable)
        {
            close_item->Enable(false);
        }
        else
        {
            menu.Bind(
                wxEVT_MENU,
                [this, id = target_tab.id](wxCommandEvent&) { model_.set_visible(id, false); },
                close_item->GetId());
        }

        // Close Other Panels
        auto* close_others_item = menu.Append(wxID_ANY, "Close Other Panels");
        menu.Bind(
            wxEVT_MENU,
            [this, keep_id = target_tab.id](wxCommandEvent&)
            {
                auto all_panels = model_.panels();
                for (const auto& p : all_panels)
                {
                    if (p.id != keep_id && p.closable)
                    {
                        model_.set_visible(p.id, false);
                    }
                }
            },
            close_others_item->GetId());

        // Close All Panels
        auto* close_all_item = menu.Append(wxID_ANY, "Close All Panels");
        menu.Bind(
            wxEVT_MENU,
            [this](wxCommandEvent&)
            {
                auto all_panels = model_.panels();
                for (const auto& p : all_panels)
                {
                    if (p.closable)
                    {
                        model_.set_visible(p.id, false);
                    }
                }
            },
            close_all_item->GetId());

        menu.AppendSeparator();

        // Maximize Panel Area
        auto* maximize_item = menu.Append(
            wxID_ANY, is_maximized_ ? "Restore Panel Area Size" : "Maximize Panel Area Size");
        menu.Bind(
            wxEVT_MENU,
            [this](wxCommandEvent&)
            {
                is_maximized_ = !is_maximized_;
                core::events::PanelAreaMaximizeToggledEvent evt;
                evt.is_maximized = is_maximized_;
                event_bus_.publish(evt);
                Refresh();
            },
            maximize_item->GetId());

        PopupMenu(&menu, event.GetPosition());
    }
}

} // namespace markamp::ui
