#include "ActivityBar.h"

#include "ComponentSizeResolver.h"
#include "FocusManager.h"
#include "FocusRingRenderer.h"
#include "LayoutMetrics.h"
#include "SpacingGrid.h"
#include "TooltipWindow.h"
#include "TypographyScale.h"
#include "core/Config.h"
#include "core/Logger.h"
#include "core/ThemeEngine.h"
#include "ui/IconManager.h"
#include "ui/accessibility/AccessibilityController.h"

#include <nlohmann/json.hpp>
#include <wx/app.h>
#include <wx/dataobj.h>
#include <wx/dcbuffer.h>
#include <wx/dnd.h>
#include <wx/graphics.h>
#include <wx/menu.h>

namespace markamp::ui
{

ActivityBar::ActivityBar(wxWindow* parent,
                         DesignSystemContext& ds,
                         core::EventBus& event_bus,
                         core::Config* config)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxWANTS_CHARS)
    , ds_(ds)
    , event_bus_(event_bus)
    , config_(config)
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
             auto vis_items = model_.visible_items();
             if (hover_index_ == -2 && !overflow_button_bounds_.IsEmpty())
             {
                 auto* tooltip = TooltipWindow::GetOrCreate(wxTheApp->GetTopWindow(), ds_);
                 wxString tooltip_text = "Additional Views";
                 int tooltip_y = overflow_button_bounds_.GetY() +
                                 (overflow_button_bounds_.GetHeight() - 24) / 2;
                 wxPoint screen_pos = ClientToScreen(wxPoint(GetSize().GetWidth() + 4, tooltip_y));
                 tooltip->ShowTooltip(tooltip_text, screen_pos);
             }
             else if (hover_index_ >= 0 && hover_index_ < static_cast<int>(vis_items.size()) &&
                      hover_index_ < static_cast<int>(item_bounds_.size()))
             {
                 auto* tooltip = TooltipWindow::GetOrCreate(wxTheApp->GetTopWindow(), ds_);
                 const auto& item = vis_items[static_cast<std::size_t>(hover_index_)];
                 const auto& bounds = item_bounds_[static_cast<std::size_t>(hover_index_)];

                 wxString tooltip_text = item.label;
                 if (item.badge_count > 0 || item.badge_style == BadgeStyle::kDot ||
                     item.badge_style == BadgeStyle::kUrgent)
                 {
                     tooltip_text += wxString::Format(
                         " (%s)",
                         ActivityBarModel::badge_display(item.badge_style, item.badge_count));
                 }
                 if (!item.shortcut_hint.empty())
                 {
                     tooltip_text += wxString::Format(" [%s]", item.shortcut_hint);
                 }

                 int tooltip_y = bounds.GetY() + (bounds.GetHeight() - 24) / 2;
                 wxPoint screen_pos = ClientToScreen(wxPoint(GetSize().GetWidth() + 4, tooltip_y));
                 tooltip->ShowTooltip(tooltip_text, screen_pos);
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
        { SetBadge(core::events::ActivityBarItemId::kSearch, evt.count); });

    diagnostics_sub_ = event_bus_.subscribe<core::events::DiagnosticsCountChangedEvent>(
        [this](const core::events::DiagnosticsCountChangedEvent& evt) {
            SetBadge(core::events::ActivityBarItemId::kSettings,
                     evt.error_count + evt.warning_count);
        });

    extension_updates_sub_ = event_bus_.subscribe<core::events::ExtensionUpdatesAvailableEvent>(
        [this](const core::events::ExtensionUpdatesAvailableEvent& evt)
        { SetBadge(core::events::ActivityBarItemId::kExtensions, evt.update_count); });

    keyboard_mode_sub_ = event_bus_.subscribe<core::events::KeyboardModeChangedEvent>(
        [this](const core::events::KeyboardModeChangedEvent& /*evt*/) { Refresh(); });

    // Phase 18 Task 16: Subscribe to Git status changes
    git_status_sub_ = event_bus_.subscribe<core::events::GitStatusChangedEvent>(
        [this](const core::events::GitStatusChangedEvent& evt)
        {
            int total_changes = evt.modified + evt.staged + evt.untracked;
            SetBadge(core::events::ActivityBarItemId::kGit, total_changes);
        });

    // Phase 07 Task 18: Extension-contributed panels
    custom_panel_sub_ = event_bus_.subscribe<core::events::CustomPanelRegisteredEvent>(
        [this](const core::events::CustomPanelRegisteredEvent& evt)
        {
            // Only add items intended for the primary sidebar (or unmarked logic)
            if (evt.location.empty() || evt.location == "left_sidebar" || evt.location == "primary")
            {
                model_.add_item({evt.panel_id,
                                 evt.panel_id, // Default label to ID
                                 "",
                                 evt.panel_id,
                                 "activity-extension", // Default extension icon
                                 false});
                LoadLayoutFromConfig();
                UpdateItemBounds();
                Refresh();
            }
        });

    custom_panel_unsub_ = event_bus_.subscribe<core::events::CustomPanelUnregisteredEvent>(
        [this](const core::events::CustomPanelUnregisteredEvent& evt)
        {
            model_.remove_item(evt.panel_id);
            UpdateItemBounds();
            Refresh();
        });

    Bind(wxEVT_SIZE, &ActivityBar::OnSize, this);

    // Phase 09 Task 6: Panel Drop Target for Primary Sidebar
    class PrimaryPanelDropTarget : public wxTextDropTarget
    {
    public:
        explicit PrimaryPanelDropTarget(core::EventBus& bus)
            : bus_(bus)
        {
        }
        bool OnDropText(wxCoord /*x*/, wxCoord /*y*/, const wxString& data) override
        {
            if (data.StartsWith("MARKAMP_PANEL:"))
            {
                std::string panel_id = data.Mid(14).ToStdString();
                core::events::SidebarPanelMovedEvent evt;
                evt.panel_id = panel_id;
                evt.target_sidebar = "primary";
                bus_.publish(evt);
                return true;
            }
            return false;
        }

    private:
        core::EventBus& bus_;
    };
    SetDropTarget(new PrimaryPanelDropTarget(event_bus_));
}

void ActivityBar::CreateItems()
{
    model_ = ActivityBarModel{}; // Reset

    // Top items
    model_.add_item({core::events::ActivityBarItemId::kFileExplorer,
                     "Explorer",
                     "Cmd+Shift+E",
                     "Explorer",
                     "activity-explorer",
                     false});
    model_.add_item({core::events::ActivityBarItemId::kSearch,
                     "Search",
                     "Cmd+Shift+F",
                     "Search",
                     "activity-search",
                     false});
    model_.add_item({core::events::ActivityBarItemId::kNotebooks,
                     "Notebooks",
                     "",
                     "Notebooks",
                     "activity-notebooks",
                     false});
    model_.add_item({core::events::ActivityBarItemId::kCanvas,
                     "Canvas",
                     "",
                     "Canvas",
                     "activity-canvas",
                     false});
    model_.add_item({core::events::ActivityBarItemId::kGraph,
                     "Knowledge Graph",
                     "",
                     "Knowledge Graph",
                     "activity-graph",
                     false});
    model_.add_item({core::events::ActivityBarItemId::kAI,
                     "AI Assistant",
                     "",
                     "AI Assistant",
                     "activity-ai",
                     false});
    model_.add_item({core::events::ActivityBarItemId::kFlashcards,
                     "Flashcards",
                     "",
                     "Flashcards",
                     "activity-flashcards",
                     false});
    model_.add_item(
        {core::events::ActivityBarItemId::kGit, "Git", "", "Git", "activity-git", false});
    model_.add_item(
        {core::events::ActivityBarItemId::kTasks, "Tasks", "", "Tasks", "activity-tasks", false});
    model_.add_item({core::events::ActivityBarItemId::kDatabase,
                     "Database",
                     "",
                     "Database",
                     "activity-database",
                     false});
    model_.add_item({core::events::ActivityBarItemId::kPresentation,
                     "Presentation",
                     "",
                     "Presentation",
                     "activity-presentation",
                     false});
    model_.add_item({core::events::ActivityBarItemId::kExtensions,
                     "Extensions",
                     "",
                     "Extensions",
                     "activity-extensions",
                     false});

    // Bottom items
    model_.add_item({core::events::ActivityBarItemId::kSettings,
                     "Settings",
                     "",
                     "Settings",
                     "activity-settings",
                     true});
    model_.add_item(
        {core::events::ActivityBarItemId::kThemes, "Themes", "", "Themes", "toolbar-themes", true});
    model_.add_item({core::events::ActivityBarItemId::kAccount,
                     "Accounts",
                     "",
                     "Accounts",
                     "activity-account",
                     true});

    LoadLayoutFromConfig();
}

void ActivityBar::SetActiveItem(core::events::ActivityBarItem item)
{
    active_item_ = item;
    Refresh();
}

void ActivityBar::AddItem(const ActivityBarItemModel& item)
{
    model_.add_item(item);
    UpdateItemBounds();
    Refresh();
}

void ActivityBar::RemoveItem(const std::string& item_id)
{
    model_.remove_item(item_id);
    // If the removed item was active, reset appropriately
    if (active_item_ == item_id)
    {
        auto visible = model_.visible_items();
        if (!visible.empty())
        {
            active_item_ = visible.front().item_id;
        }
        else
        {
            active_item_ = "";
        }
    }
    UpdateItemBounds();
    Refresh();
}

void ActivityBar::SetItemVisible(const std::string& item_id, bool visible)
{
    model_.set_item_visible(item_id, visible);
    if (!visible && active_item_ == item_id)
    {
        auto visible_items = model_.visible_items();
        active_item_ = visible_items.empty() ? "" : visible_items.front().item_id;
    }
    UpdateItemBounds();
    Refresh();
}

auto ActivityBar::GetActiveItem() const -> core::events::ActivityBarItem
{
    return active_item_;
}

void ActivityBar::SetSecondaryActiveItem(core::events::ActivityBarItem item)
{
    secondary_active_item_ = item;
    Refresh();
}

auto ActivityBar::GetSecondaryActiveItem() const -> core::events::ActivityBarItem
{
    return secondary_active_item_;
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
    UpdateItemBounds();
    if (GetParent() != nullptr)
    {
        GetParent()->Layout();
    }
    Refresh();
}

void ActivityBar::OnSize(wxSizeEvent& event)
{
    UpdateItemBounds();
    event.Skip();
    Refresh();
}

void ActivityBar::UpdateItemBounds()
{
    auto size = GetClientSize();
    const int kBarWidth = ds_.metrics.activity_bar_slot_height();

    auto vis_items = model_.visible_items();
    item_bounds_.resize(vis_items.size());

    int top_y = ds_.spacing.scaled(SpacingToken::kMd);
    int bottom_y = size.GetHeight() - ds_.spacing.scaled(SpacingToken::kMd);

    int num_bottom_items = 0;
    int num_top_items = 0;

    for (const auto& item : vis_items)
    {
        if (item.is_bottom_item)
            num_bottom_items++;
        else
            num_top_items++;
    }

    int available_top_space = bottom_y - (num_bottom_items * kBarWidth) - top_y;
    int max_top_items = std::max(0, available_top_space / kBarWidth);

    overflow_active_ = (num_top_items > max_top_items);
    int visible_top_items = overflow_active_ ? std::max(0, max_top_items - 1) : num_top_items;

    last_top_item_index_ = -1;
    first_bottom_item_index_ = -1;

    // Calculate bounds: Bottom items go upwards from bottom_y
    for (int item_index = static_cast<int>(vis_items.size()) - 1; item_index >= 0; --item_index)
    {
        const auto& item = vis_items[static_cast<std::size_t>(item_index)];
        if (item.is_bottom_item)
        {
            bottom_y -= kBarWidth;
            item_bounds_[static_cast<std::size_t>(item_index)] =
                wxRect(0, bottom_y, kBarWidth, kBarWidth);
            first_bottom_item_index_ = item_index;
        }
    }

    // Top items go downwards from top_y
    int current_top = 0;
    for (int item_index = 0; item_index < static_cast<int>(vis_items.size()); ++item_index)
    {
        const auto& item = vis_items[static_cast<std::size_t>(item_index)];
        if (!item.is_bottom_item)
        {
            if (current_top < visible_top_items)
            {
                item_bounds_[static_cast<std::size_t>(item_index)] =
                    wxRect(0, top_y, kBarWidth, kBarWidth);
                top_y += kBarWidth;
                last_top_item_index_ = item_index;
            }
            else
            {
                item_bounds_[static_cast<std::size_t>(item_index)] = wxRect(0, 0, 0, 0); // Hidden
            }
            current_top++;
        }
    }

    if (overflow_active_ && max_top_items > 0)
    {
        overflow_button_bounds_ = wxRect(0, top_y, kBarWidth, kBarWidth);
    }
    else
    {
        overflow_button_bounds_ = wxRect(0, 0, 0, 0);
    }
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

    auto vis_items = model_.visible_items();

    for (int item_index = 0; item_index < static_cast<int>(vis_items.size()); ++item_index)
    {
        const auto& item = vis_items[static_cast<std::size_t>(item_index)];
        const auto& bounds = item_bounds_[static_cast<std::size_t>(item_index)];
        if (bounds.IsEmpty())
        {
            continue; // Item is hidden in overflow
        }
        const int item_y = bounds.GetY();

        const bool kIsActive = (item.item_id == active_item_);
        const bool kIsSecondaryActive = (item.item_id == secondary_active_item_);
        const bool kIsHover = (item_index == hover_index_);

        // R17 Fix 30: Active item background highlight — subtle accent tint
        if (kIsActive)
        {
            auto active_bg = clr.accent_primary.with_alpha(0.12F);
            paint_dc.SetBrush(wxBrush(active_bg.to_wx_colour()));
            paint_dc.SetPen(*wxTRANSPARENT_PEN);
            paint_dc.DrawRectangle(bounds);
        }

        // Active indicator (left border bar)
        if (kIsActive)
        {
            paint_dc.SetBrush(wxBrush(clr.accent_primary.to_wx_colour()));
            paint_dc.SetPen(*wxTRANSPARENT_PEN);
            // Task 3 (Active Indicator): precisely 2px instead of 4px
            paint_dc.DrawRectangle(0, item_y, 2, kBarWidth);
        }

        // Hover background
        if (kIsHover && !kIsActive)
        {
            auto hover = clr.bg_panel.lighten(0.1F);
            paint_dc.SetBrush(wxBrush(hover.to_wx_colour()));
            paint_dc.SetPen(*wxTRANSPARENT_PEN);
            paint_dc.DrawRectangle(bounds);
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

        core::Color base_color =
            kIsActive ? clr.editor_fg : clr.editor_fg.blend(clr.bg_panel, 0.3F);
        if (is_dragging_ && item_index == drag_index_)
        {
            base_color = base_color.blend(clr.bg_panel, 0.6F);
        }
        const wxColour icon_color = base_color.to_wx_colour();

        if (kIsActive)
        {
            // Phase 06 Task 41: Active state left border indication
            paint_dc.SetBrush(wxBrush(clr.accent_primary.to_wx_colour()));
            paint_dc.SetPen(*wxTRANSPARENT_PEN);
            // Task 3: 2px wide
            paint_dc.DrawRectangle(0, item_y, 2, kBarWidth);
        }
        else if (kIsSecondaryActive)
        {
            // Phase 09 Task 5: Secondary Active state right side dot
            paint_dc.SetBrush(wxBrush(clr.text_muted.to_wx_colour()));
            paint_dc.SetPen(*wxTRANSPARENT_PEN);
            paint_dc.DrawCircle(kBarWidth - 6, item_y + (kBarWidth / 2), 2);
        }

        const int kIconSize = 24;
        const int kIconX = (kBarWidth - kIconSize) / 2 + press_offset_x;
        const int kIconY = item_y + (kBarWidth - kIconSize) / 2 + press_offset_y;

        IconManager::get().draw_icon(
            paint_dc, item.icon_name, kIconX, kIconY, wxSize(kIconSize, kIconSize), icon_color);

        // R18 Fix 25: Badge count indicator
        if (item.badge_style == BadgeStyle::kCount || item.badge_style == BadgeStyle::kDot ||
            item.badge_style == BadgeStyle::kUrgent)
        {
            auto badge_bg = clr.accent_primary.to_wx_colour();
            paint_dc.SetBrush(wxBrush(badge_bg));
            paint_dc.SetPen(*wxTRANSPARENT_PEN);

            paint_dc.SetTextForeground(*wxWHITE);
            auto badge_font = ds_.typography.font(TypeSlot::kCaption);
            paint_dc.SetFont(badge_font);

            auto badge_text = ActivityBarModel::badge_display(item.badge_style, item.badge_count);
            auto badge_extent = paint_dc.GetTextExtent(badge_text);

            const int kTextW = badge_extent.GetWidth();
            const int kBadgeH = 16;
            const int kBadgeW = std::max(16, kTextW + 8);
            const int kBadgeX = kBarWidth - 8 - kBadgeW / 2;
            const int kBadgeYPos = item_y + 4;

            float scale = 1.0F;
            if (auto it = badge_scales_.find(item.item_id); it != badge_scales_.end())
            {
                scale = it->second;
            }

            const int scaled_w = static_cast<int>(kBadgeW * scale);
            const int scaled_h = static_cast<int>(kBadgeH * scale);

            const int center_x = kBadgeX;
            const int center_y = kBadgeYPos + kBadgeH / 2;
            const int draw_x = center_x - scaled_w / 2;
            const int draw_y = center_y - scaled_h / 2;

            paint_dc.DrawRoundedRectangle(draw_x, draw_y, scaled_w, scaled_h, scaled_h / 2.0);
            paint_dc.DrawText(
                badge_text, center_x - kTextW / 2, center_y - badge_extent.GetHeight() / 2);

            // Restore font
            paint_dc.SetFont(font);
        }

        // Register item bounds with global FocusRingRenderer
        FocusRingRenderer::get().register_item_bounds(
            FocusZoneId::kActivityBar, item_index, this, bounds);
    }

    // Draw overflow indicator if active
    if (overflow_active_ && !overflow_button_bounds_.IsEmpty())
    {
        const bool kIsHover = (hover_index_ == -2); // Use -2 for overflow hover
        if (kIsHover)
        {
            auto hover = clr.bg_panel.lighten(0.1F);
            paint_dc.SetBrush(wxBrush(hover.to_wx_colour()));
            paint_dc.SetPen(*wxTRANSPARENT_PEN);
            paint_dc.DrawRectangle(overflow_button_bounds_);
        }

        const int kIconSize = 24;
        const int kIconX = (kBarWidth - kIconSize) / 2;
        const int kIconY = overflow_button_bounds_.GetY() + (kBarWidth - kIconSize) / 2;

        IconManager::get().draw_icon(paint_dc,
                                     "activity-more",
                                     kIconX,
                                     kIconY,
                                     wxSize(kIconSize, kIconSize),
                                     clr.text_muted.to_wx_colour());
    }

    // R17 Fix 29: Bottom border separator below last top item
    if (last_top_item_index_ >= 0 || overflow_active_)
    {
        int sep_y = 0;
        if (overflow_active_ && !overflow_button_bounds_.IsEmpty())
        {
            sep_y = overflow_button_bounds_.GetBottom();
        }
        else if (last_top_item_index_ >= 0)
        {
            sep_y = item_bounds_[static_cast<std::size_t>(last_top_item_index_)].GetBottom();
        }

        if (sep_y > 0)
        {
            auto border_light = clr.border_light.to_wx_colour();
            paint_dc.SetPen(wxPen(border_light));
            paint_dc.DrawLine(4, sep_y, kBarWidth - 4, sep_y);
        }
    }

    // R20 Fix 20: Separator above the bottom-most item section (first bottom item)
    if (first_bottom_item_index_ >= 0)
    {
        const int kSepY =
            item_bounds_[static_cast<std::size_t>(first_bottom_item_index_)].GetY() - 2;
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

    // Phase 06 Task 12: Drag reorder feedback
    if (is_dragging_ && drag_target_index_ >= 0 && drag_index_ >= 0 &&
        drag_target_index_ != drag_index_ &&
        drag_target_index_ < static_cast<int>(item_bounds_.size()))
    {
        const auto& target_bounds = item_bounds_[static_cast<std::size_t>(drag_target_index_)];
        int insert_y = target_bounds.GetY();
        if (drag_target_index_ > drag_index_)
        {
            insert_y = target_bounds.GetBottom();
        }
        auto accent = ds_.theme.resolve_token("accent.primary")
                          .value_or(ds_.theme.color(core::ThemeColorToken::AccentPrimary));
        paint_dc.SetPen(wxPen(accent, 2));
        paint_dc.DrawLine(0, insert_y, kBarWidth, insert_y);
    }

    // Phase 07 Task 22: Rendering Polish - Draw dragged item floating with strict alpha blend
    if (is_dragging_ && drag_index_ >= 0 && drag_index_ < static_cast<int>(vis_items.size()))
    {
        const auto& item = vis_items[static_cast<std::size_t>(drag_index_)];
        const auto& bounds = item_bounds_[static_cast<std::size_t>(drag_index_)];
        wxPoint current_offset = drag_current_pos_ - drag_start_pos_;

        std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(paint_dc));
        if (gc)
        {
            gc->SetAntialiasMode(wxANTIALIAS_DEFAULT);
            // 85% opacity overlay blend
            wxColour floating_bg(clr.bg_panel.lighten(0.15F).to_wx_colour().Red(),
                                 clr.bg_panel.lighten(0.15F).to_wx_colour().Green(),
                                 clr.bg_panel.lighten(0.15F).to_wx_colour().Blue(),
                                 static_cast<unsigned char>(255 * 0.85F));

            gc->SetBrush(gc->CreateBrush(wxBrush(floating_bg)));
            gc->SetPen(*wxTRANSPARENT_PEN);

            double float_x = bounds.GetX() + current_offset.x;
            double float_y = bounds.GetY() + current_offset.y;
            gc->DrawRoundedRectangle(float_x, float_y, bounds.GetWidth(), bounds.GetHeight(), 4.0);

            // Subtle 1px border for elevation
            wxColour border_col(clr.text_muted.to_wx_colour().Red(),
                                clr.text_muted.to_wx_colour().Green(),
                                clr.text_muted.to_wx_colour().Blue(),
                                static_cast<unsigned char>(255 * 0.3F));
            gc->SetPen(gc->CreatePen(wxPen(border_col, 1)));
            gc->SetBrush(gc->CreateBrush(*wxTRANSPARENT_BRUSH));
            gc->DrawRoundedRectangle(float_x, float_y, bounds.GetWidth(), bounds.GetHeight(), 4.0);
        }

        const int float_int_x = bounds.GetX() + current_offset.x;
        const int float_int_y = bounds.GetY() + current_offset.y;
        const int kIconSize = 24;
        const int kIconX = float_int_x + (kBarWidth - kIconSize) / 2;
        const int kIconY = float_int_y + (kBarWidth - kIconSize) / 2;

        IconManager::get().draw_icon(paint_dc,
                                     item.icon_name,
                                     kIconX,
                                     kIconY,
                                     wxSize(kIconSize, kIconSize),
                                     clr.editor_fg.to_wx_colour());
    }

    // Draw the global animated focus ring over the top
    FocusRingRenderer::get().draw(paint_dc, this, ds_.theme);
}

void ActivityBar::OnMouseDown(wxMouseEvent& event)
{
    const int item_index = HitTest(event.GetPosition());

    // Overflow button click
    if (item_index == -2)
    {
        pressed_index_ = -2;
        Refresh();

        wxMenu menu;
        auto vis_items = model_.visible_items();
        constexpr int kMenuBaseId = 20000;
        int hidden_count = 0;

        for (int idx = 0; idx < static_cast<int>(vis_items.size()); ++idx)
        {
            if (!vis_items[static_cast<std::size_t>(idx)].is_bottom_item &&
                item_bounds_[static_cast<std::size_t>(idx)].IsEmpty())
            {
                const auto& item = vis_items[static_cast<std::size_t>(idx)];
                menu.Append(kMenuBaseId + idx, item.label);

                const std::string item_id = item.item_id;
                menu.Bind(
                    wxEVT_MENU,
                    [this, item_id](wxCommandEvent& /*cmd*/)
                    {
                        SetActiveItem(item_id);
                        const core::events::ActivityBarSelectionEvent evt(item_id);
                        event_bus_.publish(evt);
                    },
                    kMenuBaseId + idx);
                hidden_count++;
            }
        }

        if (hidden_count > 0)
        {
            PopupMenu(&menu, overflow_button_bounds_.GetLeftBottom());
        }

        pressed_index_ = -1;
        Refresh();
        return;
    }

    // R20 Fix 18: Track pressed item for visual feedback
    pressed_index_ = item_index;

    // Set global focus when interacting with mouse
    if (item_index >= 0)
    {
        focus_index_ = item_index;
        FocusManager::get().set_focus(FocusZoneId::kActivityBar, item_index);
    }

    // Phase 06 Task 12: Record drag start
    drag_start_pos_ = event.GetPosition();
    drag_index_ = item_index;
    is_dragging_ = false;
    Refresh();

    if (item_index >= 0 && item_index < static_cast<int>(model_.visible_items().size()))
    {
        auto item = model_.visible_items()[static_cast<std::size_t>(item_index)].item_id;
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
    auto vis_items = model_.visible_items();
    if (item_index >= 0 && item_index < static_cast<int>(vis_items.size()))
    {
        auto item = vis_items[static_cast<std::size_t>(item_index)].item_id;
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
            drag_current_pos_ = event.GetPosition();
            const int target = HitTest(event.GetPosition());
            if (target != drag_target_index_)
            {
                drag_target_index_ = target;
            }
            Refresh();
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
    if (overflow_active_ && overflow_button_bounds_.Contains(pt))
    {
        return -2;
    }

    for (int item_index = 0; item_index < static_cast<int>(item_bounds_.size()); ++item_index)
    {
        if (!item_bounds_[static_cast<std::size_t>(item_index)].IsEmpty() &&
            item_bounds_[static_cast<std::size_t>(item_index)].Contains(pt))
        {
            return item_index;
        }
    }
    return -1;
}

// R18 Fix 25: Set badge count on an activity bar item
// R18 Fix 25: Set badge count on an activity bar item
void ActivityBar::SetBadge(core::events::ActivityBarItem item, int count)
{
    // Find if the count actually increased
    bool increased = false;
    for (const auto& m_item : model_.all_items())
    {
        if (m_item.item_id == item)
        {
            if (count > m_item.badge_count)
            {
                increased = true;
            }
            break;
        }
    }

    model_.set_badge(item, count > 0 ? BadgeStyle::kCount : BadgeStyle::kNone, count);

    // Task 5: Badge Animation
    if (increased)
    {
        const float kPulseScale = 1.3F;
        std::string badge_item = item;
        transition_manager_.start<float>(
            "badge_" + badge_item,
            1.0F,
            kPulseScale,
            [this, badge_item](const float& s) { badge_scales_[badge_item] = s; },
            [this, badge_item, kPulseScale]()
            {
                transition_manager_.start<float>("badge_settle_" + badge_item,
                                                 kPulseScale,
                                                 1.0F,
                                                 [this, badge_item](const float& s)
                                                 { badge_scales_[badge_item] = s; });
            });
    }

    Refresh();
}

// Helper to announce selection
void ActivityBar::AnnounceCurrentItem()
{
    accessibility::AccessibilityController::get().announce_focus(
        model_.focused_announcement(), "Tab", "");
}

// Phase 06 Task 6: Keyboard focus navigation
void ActivityBar::OnKeyDown(wxKeyEvent& event)
{
    if (model_.visible_items().empty())
    {
        event.Skip();
        return;
    }

    const int key_code = event.GetKeyCode();
    bool focus_moved = false;

    switch (key_code)
    {
        case WXK_UP:
            model_.focus_previous();
            focus_index_ = model_.focus_index();
            focus_moved = true;
            break;
        case WXK_DOWN:
            model_.focus_next();
            focus_index_ = model_.focus_index();
            focus_moved = true;
            break;
        case WXK_RETURN:
        case WXK_SPACE:
            model_.activate_focused();
            SetActiveItem(model_.active_item_id());
            event_bus_.publish(core::events::ActivityBarSelectionEvent(model_.active_item_id()));
            AnnounceCurrentItem();
            break;
        case WXK_HOME:
            model_.set_focus(0);
            focus_index_ = 0;
            focus_moved = true;
            break;
        case WXK_END:
            model_.set_focus(model_.item_count() - 1);
            focus_index_ = model_.focus_index();
            focus_moved = true;
            break;
        default:
            event.Skip();
            break;
    }

    if (focus_moved)
    {
        FocusManager::get().set_focus(FocusZoneId::kActivityBar, focus_index_);
        Refresh();
        AnnounceCurrentItem(); // Phase 05 Task 11: Announce new focus
    }
}

void ActivityBar::OnSetFocus(wxFocusEvent& /*event*/)
{
    if (focus_index_ < 0)
    {
        // Default focus to the active item
        auto vis_items = model_.visible_items();
        for (int idx = 0; idx < static_cast<int>(vis_items.size()); ++idx)
        {
            if (vis_items[static_cast<std::size_t>(idx)].item_id == active_item_)
            {
                focus_index_ = idx;
                break;
            }
        }
        if (focus_index_ < 0 && !vis_items.empty())
        {
            focus_index_ = 0;
        }
    }
    model_.set_focus(focus_index_);
    FocusManager::get().set_focus(FocusZoneId::kActivityBar, focus_index_);
    Refresh();
    AnnounceCurrentItem(); // Phase 05 Task 11: Announce when control receives global tab focus
}

void ActivityBar::OnKillFocus(wxFocusEvent& /*event*/)
{
    // Clear global focus indicator from ActivityBar
    if (FocusManager::get().current_zone() == FocusZoneId::kActivityBar)
    {
        FocusManager::get().set_item(-1);
    }
    Refresh();
}

// Phase 06 Task 6: Programmatic focus to a specific item
void ActivityBar::FocusItem(int index)
{
    if (index >= 0 && index < static_cast<int>(model_.visible_items().size()))
    {
        focus_index_ = index;
        model_.set_focus(index);
        SetFocus();
        FocusManager::get().set_focus(FocusZoneId::kActivityBar, focus_index_);
        Refresh();
        AnnounceCurrentItem();
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

    const int item_count = static_cast<int>(model_.visible_items().size());
    if (drag_index_ >= item_count || drag_target_index_ >= item_count)
    {
        return;
    }

    // Move the dragged item to the target position
    model_.reorder(drag_index_, drag_target_index_);
    UpdateItemBounds();
    SaveLayoutToConfig();

    MARKAMP_LOG_INFO(
        "ActivityBar: Reordered item from index {} to {}", drag_index_, drag_target_index_);
}

// Phase 06 Task 13: Right-click context menu
void ActivityBar::OnRightClick(wxMouseEvent& event)
{
    wxMenu menu;
    constexpr int kMenuBaseId = 10000;

    auto all_items = model_.all_items();
    // Add show/hide toggle for each item
    for (int idx = 0; idx < static_cast<int>(all_items.size()); ++idx)
    {
        const auto& item = all_items[static_cast<std::size_t>(idx)];
        wxMenuItem* menu_item = menu.AppendCheckItem(kMenuBaseId + idx, item.label);
        menu_item->Check(item.visible);
    }

    menu.AppendSeparator();

    // "Reset Activity Bar" option to restore default order
    constexpr int kResetId = 10100;
    menu.Append(kResetId, "Reset Activity Bar");

    // Bind the handlers
    for (int idx = 0; idx < static_cast<int>(all_items.size()); ++idx)
    {
        const std::string item_id = all_items[static_cast<std::size_t>(idx)].item_id;
        menu.Bind(
            wxEVT_MENU,
            [this, item_id](wxCommandEvent& cmd)
            {
                model_.set_item_visible(item_id, cmd.IsChecked());
                UpdateItemBounds();
                SaveLayoutToConfig();
                Refresh();
            },
            kMenuBaseId + idx);
    }

    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& /*cmd*/)
        {
            model_.reset_order();
            UpdateItemBounds();
            SaveLayoutToConfig();
            Refresh();
            MARKAMP_LOG_INFO("ActivityBar: Reset to default order");
        },
        kResetId);

    PopupMenu(&menu, event.GetPosition());
}

void ActivityBar::SaveLayoutToConfig()
{
    if (!config_)
        return;

    nlohmann::json j = nlohmann::json::array();
    auto layout = model_.get_layout();
    for (const auto& [id, visible] : layout)
    {
        j.push_back({{"id", id}, {"visible", visible}});
    }

    config_->set("ui.activity_bar.layout", j.dump());
    auto result = config_->save();
    if (!result.has_value())
    {
        MARKAMP_LOG_ERROR("Failed to save activity bar layout to config: {}", result.error());
    }
}

void ActivityBar::LoadLayoutFromConfig()
{
    if (!config_)
        return;

    const std::string layout_str = config_->get_string("ui.activity_bar.layout");
    if (layout_str.empty())
        return;

    try
    {
        auto j = nlohmann::json::parse(layout_str);
        std::vector<std::pair<std::string, bool>> layout;

        if (j.is_array())
        {
            for (const auto& item : j)
            {
                if (item.contains("id") && item["id"].is_string() && item.contains("visible") &&
                    item["visible"].is_boolean())
                {
                    layout.emplace_back(item["id"].get<std::string>(), item["visible"].get<bool>());
                }
            }
            model_.apply_layout(layout);
        }
    }
    catch (const nlohmann::json::exception& e)
    {
        MARKAMP_LOG_ERROR("Failed to parse activity bar layout from config: {}", e.what());
    }
}

} // namespace markamp::ui
