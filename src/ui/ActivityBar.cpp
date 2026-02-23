#include "ActivityBar.h"

#include "ComponentSizeResolver.h"
#include "LayoutMetrics.h"
#include "SpacingGrid.h"
#include "TooltipWindow.h"
#include "TypographyScale.h"
#include "core/Logger.h"
#include "core/ThemeEngine.h"
#include "ui/IconManager.h"

#include <wx/app.h>
#include <wx/dcbuffer.h>
#include <wx/menu.h>

namespace markamp::ui
{

ActivityBar::ActivityBar(wxWindow* parent, DesignSystemContext& ds, core::EventBus& event_bus)
    : wxPanel(parent,
              wxID_ANY,
              wxDefaultPosition,
              wxSize(ds.metrics.activity_bar_slot_height(), -1),
              wxTAB_TRAVERSAL | wxNO_BORDER | wxWANTS_CHARS)
    , ds_(ds)
    , event_bus_(event_bus)
{
    const int kBarWidth = ds_.metrics.activity_bar_slot_height();
    SetMinSize(wxSize(kBarWidth, -1));
    SetMaxSize(wxSize(kBarWidth, -1));
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    CreateItems();
    ApplyTheme();

    // Phase 06 Task 19: Accessibility labels for screen readers
    SetName("Activity Bar");
    SetHelpText("Vertical navigation bar for switching sidebar modes. "
                "Use arrow keys to navigate, Enter/Space to activate.");

    Bind(wxEVT_PAINT, &ActivityBar::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &ActivityBar::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &ActivityBar::OnMouseUp, this);         // R20 Fix 18
    Bind(wxEVT_LEFT_DCLICK, &ActivityBar::OnDoubleClick, this); // R20 Fix 16
    Bind(wxEVT_MOTION, &ActivityBar::OnMouseMove, this);
    Bind(wxEVT_LEAVE_WINDOW, &ActivityBar::OnMouseLeave, this);
    Bind(wxEVT_KEY_DOWN, &ActivityBar::OnKeyDown, this);     // Phase 06 Task 6
    Bind(wxEVT_SET_FOCUS, &ActivityBar::OnSetFocus, this);   // Phase 06 Task 6
    Bind(wxEVT_KILL_FOCUS, &ActivityBar::OnKillFocus, this); // Phase 06 Task 6
    Bind(wxEVT_RIGHT_UP, &ActivityBar::OnRightClick, this);  // Phase 06 Task 13

    tooltip_timer_.SetOwner(this);
    Bind(wxEVT_TIMER,
         [this](wxTimerEvent& /*evt*/)
         {
             if (hover_index_ >= 0 && hover_index_ < static_cast<int>(items_.size()))
             {
                 auto* tooltip = TooltipWindow::GetOrCreate(wxTheApp->GetTopWindow(), ds_);
                 const auto& item = items_[static_cast<std::size_t>(hover_index_)];
                 int tooltip_y = item.bounds.GetY() + (item.bounds.GetHeight() - 24) / 2;
                 wxPoint screen_pos = ClientToScreen(wxPoint(GetSize().GetWidth() + 4, tooltip_y));
                 tooltip->ShowTooltip(item.label, screen_pos);
             }
         });

    // Make the activity bar focusable for keyboard navigation
    SetCanFocus(true);

    theme_sub_ = event_bus_.subscribe<core::events::ThemeChangedEvent>(
        [this](const core::events::ThemeChangedEvent& /*evt*/) { ApplyTheme(); });

    density_sub_ = event_bus_.subscribe<core::events::DensityProfileChangedEvent>(
        [this](const core::events::DensityProfileChangedEvent& /*evt*/) { UpdateLayoutMetrics(); });

    // Phase 06 Task 7: Subscribe to badge notification events
    search_count_sub_ = event_bus_.subscribe<core::events::SearchResultCountEvent>(
        [this](const core::events::SearchResultCountEvent& evt)
        { SetBadge(core::events::ActivityBarItem::Search, evt.count); });

    diagnostics_sub_ = event_bus_.subscribe<core::events::DiagnosticsCountChangedEvent>(
        [this](const core::events::DiagnosticsCountChangedEvent& evt) {
            SetBadge(core::events::ActivityBarItem::Settings, evt.error_count + evt.warning_count);
        });

    extension_updates_sub_ = event_bus_.subscribe<core::events::ExtensionUpdatesAvailableEvent>(
        [this](const core::events::ExtensionUpdatesAvailableEvent& evt)
        { SetBadge(core::events::ActivityBarItem::Extensions, evt.update_count); });
}

void ActivityBar::CreateItems()
{
    items_ = {
        {core::events::ActivityBarItem::FileExplorer, "Explorer", "activity-explorer", {}},
        {core::events::ActivityBarItem::Search, "Search", "activity-search", {}},
        {core::events::ActivityBarItem::kNotebooks, "Notebooks", "activity-notebooks", {}},
        {core::events::ActivityBarItem::kCanvas, "Canvas", "activity-canvas", {}},
        {core::events::ActivityBarItem::kGraph, "Knowledge Graph", "activity-graph", {}},
        {core::events::ActivityBarItem::kAI, "AI Assistant", "activity-ai", {}},
        {core::events::ActivityBarItem::kFlashcards, "Flashcards", "activity-flashcards", {}},
        {core::events::ActivityBarItem::kGit, "Git", "activity-git", {}},
        {core::events::ActivityBarItem::kTasks, "Tasks", "activity-tasks", {}},
        {core::events::ActivityBarItem::kDatabase, "Database", "activity-database", {}},
        {core::events::ActivityBarItem::kPresentation, "Presentation", "activity-presentation", {}},
        {core::events::ActivityBarItem::Extensions, "Extensions", "activity-extensions", {}},
        {core::events::ActivityBarItem::Settings, "Settings", "activity-settings", {}},
        {core::events::ActivityBarItem::Themes, "Themes", "toolbar-themes", {}},
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
    SetBackgroundColour(ds_.theme.resolve_token("activityBar.bg")
                            .value_or(ds_.theme.color(core::ThemeColorToken::ActivityBarBg)));
    Refresh();
}

void ActivityBar::UpdateLayoutMetrics()
{
    const int kBarWidth = ds_.metrics.activity_bar_slot_height();
    SetMinSize(wxSize(kBarWidth, -1));
    SetMaxSize(wxSize(kBarWidth, -1));
    if (GetParent() != nullptr)
    {
        GetParent()->Layout();
    }
    Refresh();
}

void ActivityBar::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC paint_dc(this);
    const auto& theme = ds_.theme.current_theme();
    const auto& clr = theme.colors;

    wxFont font = paint_dc.GetFont();

    // Background
    paint_dc.SetBackground(
        wxBrush(ds_.theme.resolve_token("activityBar.bg")
                    .value_or(ds_.theme.color(core::ThemeColorToken::ActivityBarBg))));
    paint_dc.Clear();

    auto size = GetClientSize();
    const int kBarWidth = ds_.metrics.activity_bar_slot_height();
    int item_y = ds_.spacing.scaled(SpacingToken::kMd);

    for (int item_index = 0; item_index < static_cast<int>(items_.size()); ++item_index)
    {
        auto& item = items_[static_cast<std::size_t>(item_index)];
        item.bounds = wxRect(0, item_y, kBarWidth, kBarWidth);

        const bool kIsActive = (item.item_id == active_item_);
        const bool kIsHover = (item_index == hover_index_);

        // R17 Fix 30: Active item background highlight — subtle accent tint
        if (kIsActive)
        {
            auto active_bg = clr.accent_primary.with_alpha(0.12F);
            paint_dc.SetBrush(wxBrush(active_bg.to_wx_colour()));
            paint_dc.SetPen(*wxTRANSPARENT_PEN);
            paint_dc.DrawRectangle(0, item_y, kBarWidth, kBarWidth);
        }

        // Active indicator (left border bar)
        if (kIsActive)
        {
            paint_dc.SetBrush(wxBrush(clr.accent_primary.to_wx_colour()));
            paint_dc.SetPen(*wxTRANSPARENT_PEN);
            paint_dc.DrawRectangle(0, item_y, 4, kBarWidth);
        }

        // Hover background
        if (kIsHover && !kIsActive)
        {
            auto hover = clr.bg_panel.lighten(0.1F);
            paint_dc.SetBrush(wxBrush(hover.to_wx_colour()));
            paint_dc.SetPen(*wxTRANSPARENT_PEN);
            paint_dc.DrawRectangle(0, item_y, kBarWidth, kBarWidth);
        }

        // Icon rendering via IconManager
        // R20 Fix 18: press offset — shift icon 1px when pressed
        int press_offset_x = 0;
        int press_offset_y = 0;
        if (item_index == pressed_index_)
        {
            press_offset_x = 1;
            press_offset_y = 1;
        }

        wxColour icon_color;
        if (kIsActive)
        {
            icon_color = clr.editor_fg.to_wx_colour();

            // Phase 06 Task 41: Active state left border indication
            paint_dc.SetBrush(wxBrush(clr.accent_primary.to_wx_colour()));
            paint_dc.SetPen(*wxTRANSPARENT_PEN);
            paint_dc.DrawRectangle(0, item_y, 4, kBarWidth);
        }
        else
        {
            // Dimmed for inactive — blend fg towards bg
            auto dimmed = clr.editor_fg.blend(clr.bg_panel, 0.3F);
            icon_color = dimmed.to_wx_colour();
        }

        const int kIconSize = 24;
        const int kIconX = (kBarWidth - kIconSize) / 2 + press_offset_x;
        const int kIconY = item_y + (kBarWidth - kIconSize) / 2 + press_offset_y;

        IconManager::get().draw_icon(
            paint_dc, item.icon_name, kIconX, kIconY, wxSize(kIconSize, kIconSize), icon_color);

        // R18 Fix 25: Badge count indicator
        if (item.badge_count > 0)
        {
            auto badge_bg = clr.accent_primary.to_wx_colour();
            paint_dc.SetBrush(wxBrush(badge_bg));
            paint_dc.SetPen(*wxTRANSPARENT_PEN);

            paint_dc.SetTextForeground(*wxWHITE);
            auto badge_font = ds_.typography.font(TypeSlot::kCaption);
            paint_dc.SetFont(badge_font);
            auto badge_text = wxString::Format("%d", item.badge_count);
            auto badge_extent = paint_dc.GetTextExtent(badge_text);

            const int kTextW = badge_extent.GetWidth();
            const int kBadgeH = 16;
            const int kBadgeW = std::max(16, kTextW + 8);
            const int kBadgeX = kBarWidth - 8 - kBadgeW / 2;
            const int kBadgeYPos = item_y + 4;

            paint_dc.DrawRoundedRectangle(
                kBadgeX - kBadgeW / 2, kBadgeYPos, kBadgeW, kBadgeH, kBadgeH / 2.0);
            paint_dc.DrawText(badge_text,
                              kBadgeX - kTextW / 2,
                              kBadgeYPos + kBadgeH / 2 - badge_extent.GetHeight() / 2);

            // Restore font
            paint_dc.SetFont(font);
        }

        // Phase 06 Task 6: Focus ring around focused item
        if (item_index == focus_index_ && HasFocus())
        {
            auto focus_col = clr.accent_primary.with_alpha(0.6F);
            paint_dc.SetBrush(*wxTRANSPARENT_BRUSH);
            paint_dc.SetPen(wxPen(focus_col.to_wx_colour(), 2));
            paint_dc.DrawRoundedRectangle(3, item_y + 2, kBarWidth - 6, kBarWidth - 4, 4);
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
        const int kSepY = last_item.bounds.GetY() - 2;
        auto sep_col = clr.border_light.to_wx_colour();
        paint_dc.SetPen(wxPen(sep_col));
        paint_dc.DrawLine(8, kSepY, kBarWidth - 8, kSepY);
    }

    // R20 Fix 17: Drag handle dots — 3 small dots centered in bar
    {
        const int kDragY = size.GetHeight() - 40;
        auto dot_col = clr.text_muted.to_wx_colour();
        paint_dc.SetBrush(wxBrush(dot_col));
        paint_dc.SetPen(*wxTRANSPARENT_PEN);
        const int kDotX = kBarWidth / 2;
        for (int dot_idx = 0; dot_idx < 3; ++dot_idx)
        {
            paint_dc.DrawCircle(kDotX, kDragY + dot_idx * 6, 2);
        }
    }

    // R18 Fix 19: Removed custom drawn tooltip pill here in favor of TooltipWindow

    // Separator line on the right edge
    auto border = clr.border_light.to_wx_colour();
    paint_dc.SetPen(wxPen(border));
    paint_dc.DrawLine(size.GetWidth() - 1, 0, size.GetWidth() - 1, size.GetHeight());

    // Soft shadow logic directly inside right edge
    auto shadow_col = clr.bg_panel.to_wx_colour().ChangeLightness(90);
    paint_dc.SetPen(wxPen(shadow_col));
    paint_dc.DrawLine(size.GetWidth() - 2, 0, size.GetWidth() - 2, size.GetHeight());
}

void ActivityBar::OnMouseDown(wxMouseEvent& event)
{
    const int item_index = HitTest(event.GetPosition());
    // R20 Fix 18: Track pressed item for visual feedback
    pressed_index_ = item_index;
    // Phase 06 Task 12: Record drag start
    drag_start_pos_ = event.GetPosition();
    drag_index_ = item_index;
    is_dragging_ = false;
    Refresh();

    if (item_index >= 0 && item_index < static_cast<int>(items_.size()))
    {
        auto item = items_[static_cast<std::size_t>(item_index)].item_id;
        SetActiveItem(item);

        const core::events::ActivityBarSelectionEvent evt(item);
        event_bus_.publish(evt);
    }
}

// R20 Fix 18: Mouse-up restores press state
void ActivityBar::OnMouseUp(wxMouseEvent& /*event*/)
{
    if (is_dragging_)
    {
        FinishDrag();
    }
    pressed_index_ = -1;
    drag_index_ = -1;
    is_dragging_ = false;
    drag_target_index_ = -1;
    Refresh();
}

// R20 Fix 16: Double-click active item collapses sidebar
void ActivityBar::OnDoubleClick(wxMouseEvent& event)
{
    const int item_index = HitTest(event.GetPosition());
    if (item_index >= 0 && item_index < static_cast<int>(items_.size()))
    {
        auto item = items_[static_cast<std::size_t>(item_index)].item_id;
        if (item == active_item_)
        {
            core::events::SidebarToggleEvent toggle_evt;
            event_bus_.publish(toggle_evt);
        }
    }
}

void ActivityBar::OnMouseMove(wxMouseEvent& event)
{
    // Phase 06 Task 12: Drag detection
    if (drag_index_ >= 0 && event.LeftIsDown())
    {
        const auto delta = event.GetPosition() - drag_start_pos_;
        constexpr int kDragThreshold = 5;
        if (!is_dragging_ && std::abs(delta.y) > kDragThreshold)
        {
            is_dragging_ = true;
        }
        if (is_dragging_)
        {
            const int target = HitTest(event.GetPosition());
            if (target != drag_target_index_)
            {
                drag_target_index_ = target;
                Refresh();
            }
            return;
        }
    }

    const int item_index = HitTest(event.GetPosition());
    if (item_index != hover_index_)
    {
        hover_index_ = item_index;
        if (auto* tooltip = TooltipWindow::GetOrCreate(wxTheApp->GetTopWindow(), ds_))
        {
            tooltip->HideTooltip();
        }
        tooltip_visible_ = false;
        if (hover_index_ >= 0)
        {
            tooltip_timer_.StartOnce(300); // 300ms delay for tooltip
        }
        else
        {
            tooltip_timer_.Stop();
        }

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
        if (auto* tooltip = TooltipWindow::GetOrCreate(wxTheApp->GetTopWindow(), ds_))
        {
            tooltip->HideTooltip();
        }
        tooltip_visible_ = false;
        tooltip_timer_.Stop();
        UnsetToolTip();
        Refresh();
    }
}

auto ActivityBar::HitTest(const wxPoint& pt) const -> int
{
    for (int item_index = 0; item_index < static_cast<int>(items_.size()); ++item_index)
    {
        if (items_[static_cast<std::size_t>(item_index)].bounds.Contains(pt))
        {
            return item_index;
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

// Phase 06 Task 6: Keyboard focus navigation
void ActivityBar::OnKeyDown(wxKeyEvent& event)
{
    if (items_.empty())
    {
        event.Skip();
        return;
    }

    const int key_code = event.GetKeyCode();
    const int item_count = static_cast<int>(items_.size());

    switch (key_code)
    {
        case WXK_UP:
        {
            if (focus_index_ <= 0)
            {
                focus_index_ = item_count - 1;
            }
            else
            {
                --focus_index_;
            }
            Refresh();
            break;
        }
        case WXK_DOWN:
        {
            if (focus_index_ < 0 || focus_index_ >= item_count - 1)
            {
                focus_index_ = 0;
            }
            else
            {
                ++focus_index_;
            }
            Refresh();
            break;
        }
        case WXK_RETURN:
        case WXK_SPACE:
        {
            if (focus_index_ >= 0 && focus_index_ < item_count)
            {
                auto item = items_[static_cast<std::size_t>(focus_index_)].item_id;
                SetActiveItem(item);
                core::events::ActivityBarSelectionEvent evt(item);
                event_bus_.publish(evt);
            }
            break;
        }
        case WXK_HOME:
        {
            focus_index_ = 0;
            Refresh();
            break;
        }
        case WXK_END:
        {
            focus_index_ = item_count - 1;
            Refresh();
            break;
        }
        default:
            event.Skip();
            break;
    }
}

void ActivityBar::OnSetFocus(wxFocusEvent& /*event*/)
{
    if (focus_index_ < 0)
    {
        // Default focus to the active item
        for (int idx = 0; idx < static_cast<int>(items_.size()); ++idx)
        {
            if (items_[static_cast<std::size_t>(idx)].item_id == active_item_)
            {
                focus_index_ = idx;
                break;
            }
        }
        if (focus_index_ < 0 && !items_.empty())
        {
            focus_index_ = 0;
        }
    }
    Refresh();
}

void ActivityBar::OnKillFocus(wxFocusEvent& /*event*/)
{
    Refresh();
}

// Phase 06 Task 6: Programmatic focus to a specific item
void ActivityBar::FocusItem(int index)
{
    if (index >= 0 && index < static_cast<int>(items_.size()))
    {
        focus_index_ = index;
        SetFocus();
        Refresh();
    }
}

// Phase 06 Task 12: Complete drag reorder — swap items
void ActivityBar::FinishDrag()
{
    if (drag_index_ < 0 || drag_target_index_ < 0)
    {
        return;
    }
    if (drag_index_ == drag_target_index_)
    {
        return;
    }

    const int item_count = static_cast<int>(items_.size());
    if (drag_index_ >= item_count || drag_target_index_ >= item_count)
    {
        return;
    }

    // Move the dragged item to the target position
    auto dragged = std::move(items_[static_cast<std::size_t>(drag_index_)]);
    items_.erase(items_.begin() + drag_index_);
    items_.insert(items_.begin() + drag_target_index_, std::move(dragged));

    MARKAMP_LOG_INFO(
        "ActivityBar: Reordered item from index {} to {}", drag_index_, drag_target_index_);
}

// Phase 06 Task 13: Right-click context menu
void ActivityBar::OnRightClick(wxMouseEvent& event)
{
    wxMenu menu;
    constexpr int kMenuBaseId = 10000;

    // Add show/hide toggle for each item
    for (int idx = 0; idx < static_cast<int>(items_.size()); ++idx)
    {
        const auto& item = items_[static_cast<std::size_t>(idx)];
        wxMenuItem* menu_item = menu.AppendCheckItem(kMenuBaseId + idx, item.label);
        menu_item->Check(true); // All items visible by default
    }

    menu.AppendSeparator();

    // "Reset Activity Bar" option to restore default order
    constexpr int kResetId = 10100;
    menu.Append(kResetId, "Reset Activity Bar");

    // Bind the reset handler
    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& /*cmd*/)
        {
            CreateItems();
            Refresh();
            MARKAMP_LOG_INFO("ActivityBar: Reset to default order");
        },
        kResetId);

    PopupMenu(&menu, event.GetPosition());
}

} // namespace markamp::ui
