#include "Toolbar.h"

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "SpacingGrid.h"
#include "TooltipWindow.h"
#include "TypographyScale.h"
#include "core/Events.h"
#include "core/Logger.h"
#include "ui/IconManager.h"

#include <wx/app.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>

#include <array>
#include <cmath>

namespace markamp::ui
{

Toolbar::Toolbar(wxWindow* parent, DesignSystemContext& ds, core::EventBus& event_bus)
    : ThemeAwareWindow(parent, ds.theme)
    , ds_(ds)
    , event_bus_(event_bus)
{
    const int kHeight = ds_.metrics.toolbar_height();
    SetMinSize(wxSize(-1, kHeight));
    SetMaxSize(wxSize(-1, kHeight));

    SetBackgroundStyle(wxBG_STYLE_PAINT);

    // --- Left buttons: SRC, SPLIT, VIEW, FOCUS ---
    left_buttons_.push_back({{}, "SRC", false, false, true, 0});
    left_buttons_.push_back({{}, "SPLIT", true, false, true, 1}); // default active
    left_buttons_.push_back({{}, "VIEW", false, false, true, 2});
    left_buttons_.push_back({{}, "FOCUS", false, false, true, 6}); // focus mode

    // --- Right buttons: Save, THEMES, Settings ---
    right_buttons_.push_back({{}, "", false, false, true, 3});       // Save (icon only)
    right_buttons_.push_back({{}, "THEMES", false, false, true, 4}); // Palette + label
    right_buttons_.push_back({{}, "", false, false, true, 5});       // Settings (icon only)

    // --- Events ---
    Bind(wxEVT_PAINT, &Toolbar::OnPaint, this);
    Bind(wxEVT_MOTION, &Toolbar::OnMouseMove, this);
    Bind(wxEVT_LEAVE_WINDOW, &Toolbar::OnMouseLeave, this);
    Bind(wxEVT_LEFT_DOWN, &Toolbar::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP,
         [this](wxMouseEvent& /*evt*/)
         {
             pressed_button_index_ = -1;
             Refresh();
         });
    Bind(wxEVT_SIZE, &Toolbar::OnSize, this);

    // Subscribe to view mode changes from external sources
    view_mode_sub_ = event_bus_.subscribe<core::events::ViewModeChangedEvent>(
        [this](const core::events::ViewModeChangedEvent& evt) { SetActiveViewMode(evt.mode); });

    // Subscribe to focus mode changes
    focus_mode_sub_ = event_bus_.subscribe<core::events::FocusModeChangedEvent>(
        [this](const core::events::FocusModeChangedEvent& evt)
        {
            focus_mode_active_ = evt.active;
            if (left_buttons_.size() > 3)
            {
                left_buttons_[3].is_active = evt.active;
            }
            Refresh();
        });

    // Register Transitions for Save feedback
    animation::AnimationConfig pulse_cfg;
    pulse_cfg.duration = std::chrono::milliseconds(150);
    pulse_cfg.repeat_count = 1; // Forward then backward = 1 repeat (total 2 cycles)
    pulse_cfg.auto_reverse = true;
    pulse_cfg.easing_type = animation::EasingType::EaseOutCubic;
    transition_manager_.register_transition("save_pulse", pulse_cfg);

    animation::AnimationConfig flash_cfg;
    flash_cfg.duration = std::chrono::milliseconds(400);
    flash_cfg.easing_type = animation::EasingType::EaseOutCubic;
    transition_manager_.register_transition("save_flash", flash_cfg);

    // ── Phase 26: Build/run lifecycle event subscriptions ────────────
    build_started_sub_ = event_bus_.subscribe<core::events::BuildStartedEvent>(
        [this](const core::events::BuildStartedEvent& /*evt*/)
        {
            build_indicator_state_ = BuildIndicatorState::kBuilding;
            spinner_frame_ = 0;
            Refresh();
        });

    build_finished_sub_ = event_bus_.subscribe<core::events::BuildFinishedEvent>(
        [this](const core::events::BuildFinishedEvent& evt)
        {
            build_indicator_state_ =
                evt.success ? BuildIndicatorState::kSuccess : BuildIndicatorState::kFailure;
            // Auto-decay back to idle after 3 seconds
            build_decay_timer_.SetOwner(this);
            build_decay_timer_.StartOnce(3000);
            build_decay_timer_.Bind(wxEVT_TIMER,
                                    [this](wxTimerEvent& /*timer_evt*/)
                                    {
                                        build_indicator_state_ = BuildIndicatorState::kIdle;
                                        Refresh();
                                    });
            Refresh();
        });

    run_started_sub_ = event_bus_.subscribe<core::events::RunConfigStartedEvent>(
        [this](const core::events::RunConfigStartedEvent& evt)
        {
            process_running_ = true;
            running_config_name_ = evt.config_name;
            Refresh();
        });

    run_finished_sub_ = event_bus_.subscribe<core::events::RunConfigFinishedEvent>(
        [this](const core::events::RunConfigFinishedEvent& /*evt*/)
        {
            process_running_ = false;
            running_config_name_.clear();
            Refresh();
        });

    run_stopped_sub_ = event_bus_.subscribe<core::events::RunConfigStoppedEvent>(
        [this](const core::events::RunConfigStoppedEvent& /*evt*/)
        {
            process_running_ = false;
            running_config_name_.clear();
            Refresh();
        });

    sidebar_toggle_sub_ = event_bus_.subscribe<core::events::SidebarToggleEvent>(
        [this](const core::events::SidebarToggleEvent& evt)
        {
            sidebar_visible_ = evt.visible;
            Refresh();
        });

    RecalculateButtonRects();

    // R20 Fix 8: Tooltip delay timer — waits 400ms before showing tooltip
    tooltip_delay_timer_.SetOwner(this);
    tooltip_delay_timer_.Bind(
        wxEVT_TIMER,
        [this](wxTimerEvent& /*evt*/)
        {
            tooltip_delay_timer_.Stop();
            // Show the pending tooltip
            if (pending_tooltip_index_ >= 0)
            {
                static const std::array<std::string, 4> kLeftTips = {"Editor Only (Ctrl+1)",
                                                                     "Split View (Ctrl+2)",
                                                                     "Preview Only (Ctrl+3)",
                                                                     "Focus Mode (Ctrl+K)"};
                static const std::array<std::string, 3> kRightTips = {
                    "Save (Ctrl+S)", "Themes", "Settings"};

                auto* tooltip = TooltipWindow::GetOrCreate(wxTheApp->GetTopWindow(), ds_);

                if (pending_tooltip_is_left_ &&
                    pending_tooltip_index_ < static_cast<int>(kLeftTips.size()))
                {
                    const auto& btn = left_buttons_[static_cast<size_t>(pending_tooltip_index_)];
                    wxPoint pos =
                        ClientToScreen(wxPoint(btn.rect.GetX(), btn.rect.GetBottom() + 4));
                    tooltip->ShowTooltip(kLeftTips[static_cast<size_t>(pending_tooltip_index_)],
                                         pos);
                }
                else if (!pending_tooltip_is_left_ &&
                         pending_tooltip_index_ < static_cast<int>(kRightTips.size()))
                {
                    const auto& btn = right_buttons_[static_cast<size_t>(pending_tooltip_index_)];
                    wxPoint pos = ClientToScreen(
                        wxPoint(btn.rect.GetX() - 40,
                                btn.rect.GetBottom() + 4)); // Shift slightly left for right-aligned
                                                            // items so it doesn't run off screen
                    tooltip->ShowTooltip(kRightTips[static_cast<size_t>(pending_tooltip_index_)],
                                         pos);
                }
            }
        });
}

// ═══════════════════════════════════════════════════════
// Active Mode
// ═══════════════════════════════════════════════════════

void Toolbar::SetActiveViewMode(core::events::ViewMode mode)
{
    active_mode_ = mode;

    // Update active states
    if (left_buttons_.size() >= 3)
    {
        left_buttons_[0].is_active = (mode == core::events::ViewMode::Editor);
        left_buttons_[1].is_active = (mode == core::events::ViewMode::Split);
        left_buttons_[2].is_active = (mode == core::events::ViewMode::Preview);
    }

    Refresh();
}

auto Toolbar::GetActiveViewMode() const -> core::events::ViewMode
{
    return active_mode_;
}

void Toolbar::UpdateLayoutMetrics()
{
    const int kHeight = ds_.metrics.toolbar_height();
    SetMinSize(wxSize(-1, kHeight));
    SetMaxSize(wxSize(-1, kHeight));
    RecalculateButtonRects();
    Refresh();
}

void Toolbar::SetOnThemeGalleryClick(ThemeGalleryCallback callback)
{
    on_theme_gallery_click_ = std::move(callback);
}

void Toolbar::SetButtonEnabled(int icon_type, bool enabled)
{
    bool changed = false;
    for (auto& btn : left_buttons_)
    {
        if (btn.icon_type == icon_type && btn.is_enabled != enabled)
        {
            btn.is_enabled = enabled;
            changed = true;
        }
    }
    for (auto& btn : right_buttons_)
    {
        if (btn.icon_type == icon_type && btn.is_enabled != enabled)
        {
            btn.is_enabled = enabled;
            changed = true;
        }
    }
    if (changed)
    {
        Refresh();
    }
}

// ═══════════════════════════════════════════════════════
// Button Rects
// ═══════════════════════════════════════════════════════

void Toolbar::RecalculateButtonRects()
{
    const int kLeftMargin = ds_.spacing.scaled(SpacingToken::kXl);
    const int kRightMargin = ds_.spacing.scaled(SpacingToken::kXl);
    const int kButtonPadV = ds_.spacing.scaled(SpacingToken::kSm);
    const int kButtonPadH = ds_.spacing.scaled(SpacingToken::kSm);
    const int kButtonGap = ds_.spacing.scaled(SpacingToken::kMd);
    const int kIconSize = ds_.metrics.icon_size_small();
    const int kHeight = ds_.metrics.toolbar_height();
    const double kFontSizeLabel =
        static_cast<double>(ds_.typography.font(TypeSlot::kCaption).GetPointSize());

    // Measure text widths using a temporary DC
    wxClientDC dc(this);
    auto font = wxFont(wxFontInfo(kFontSizeLabel).Family(wxFONTFAMILY_SWISS).Bold());
    dc.SetFont(font);

    int x = kLeftMargin;
    int btn_height = kHeight - 2 * kButtonPadV;

    // Left buttons
    for (auto& btn : left_buttons_)
    {
        int text_width = 0;
        if (!btn.label.empty() && !compact_mode_) // R18 Fix 16: hide labels in compact mode
        {
            wxSize ts = dc.GetTextExtent(btn.label);
            text_width = ts.GetWidth();
        }
        int btn_width = kIconSize + kButtonPadH * 2 + (text_width > 0 ? text_width + 6 : 0);
        btn.rect = wxRect(x, kButtonPadV, btn_width, btn_height);
        x += btn_width + kButtonGap;
    }

    // Right buttons — position from right edge
    int client_width = GetClientSize().GetWidth();
    if (client_width <= 0)
    {
        client_width = 800; // fallback
    }

    int rx = client_width - kRightMargin;
    for (int i = static_cast<int>(right_buttons_.size()) - 1; i >= 0; --i)
    {
        auto& btn = right_buttons_[static_cast<size_t>(i)];
        int text_width = 0;
        if (!btn.label.empty() && !compact_mode_) // R18 Fix 16: hide labels in compact mode
        {
            wxSize ts = dc.GetTextExtent(btn.label);
            text_width = ts.GetWidth();
        }
        int btn_width = kIconSize + kButtonPadH * 2 + (text_width > 0 ? text_width + 6 : 0);
        rx -= btn_width;
        btn.rect = wxRect(rx, kButtonPadV, btn_width, btn_height);
        rx -= kButtonGap;
    }
}

void Toolbar::OnSize(wxSizeEvent& event)
{
    // R18 Fix 16: Responsive collapse to icons-only
    compact_mode_ = (event.GetSize().GetWidth() < 600);
    // Phase 26: Show overflow chevron in compact mode
    show_overflow_chevron_ = compact_mode_;
    RecalculateButtonRects();
    Refresh();
    event.Skip();
}

// ═══════════════════════════════════════════════════════
// Mouse
// ═══════════════════════════════════════════════════════

void Toolbar::OnMouseMove(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    bool changed = false;
    bool any_hovered = false;

    int new_hovered_index = -1;
    bool new_hovered_is_left = true;

    for (size_t idx = 0; idx < left_buttons_.size(); ++idx)
    {
        auto& btn = left_buttons_[idx];
        bool was = btn.is_hovered;
        btn.is_hovered = btn.rect.Contains(pos);
        if (was != btn.is_hovered)
        {
            changed = true;
        }
        if (btn.is_hovered)
        {
            any_hovered = true;
            new_hovered_index = static_cast<int>(idx);
            new_hovered_is_left = true;
        }
    }
    for (size_t idx = 0; idx < right_buttons_.size(); ++idx)
    {
        auto& btn = right_buttons_[idx];
        bool was = btn.is_hovered;
        btn.is_hovered = btn.rect.Contains(pos);
        if (was != btn.is_hovered)
        {
            changed = true;
        }
        if (btn.is_hovered)
        {
            any_hovered = true;
            new_hovered_index = static_cast<int>(idx);
            new_hovered_is_left = false;
        }
    }

    if (!any_hovered)
    {
        if (auto* tooltip = TooltipWindow::GetOrCreate(wxTheApp->GetTopWindow(), ds_))
        {
            tooltip->HideTooltip();
        }
        tooltip_delay_timer_.Stop();
        pending_tooltip_index_ = -1;
    }
    else if (changed)
    {
        if (auto* tooltip = TooltipWindow::GetOrCreate(wxTheApp->GetTopWindow(), ds_))
        {
            tooltip->HideTooltip();
        }
        pending_tooltip_index_ = new_hovered_index;
        pending_tooltip_is_left_ = new_hovered_is_left;
        tooltip_delay_timer_.StartOnce(400);
    }

    // R5 Fix 2: Hand cursor on button hover
    SetCursor(any_hovered ? wxCursor(wxCURSOR_HAND) : wxNullCursor);

    if (changed)
    {
        Refresh();
    }
}

void Toolbar::OnMouseLeave(wxMouseEvent& /*event*/)
{
    for (auto& btn : left_buttons_)
    {
        btn.is_hovered = false;
    }
    for (auto& btn : right_buttons_)
    {
        btn.is_hovered = false;
    }

    if (auto* tooltip = TooltipWindow::GetOrCreate(wxTheApp->GetTopWindow(), ds_))
    {
        tooltip->HideTooltip();
    }
    tooltip_delay_timer_.Stop();
    pending_tooltip_index_ = -1;

    // R5 Fix 2: Restore default cursor
    SetCursor(wxNullCursor);
    Refresh();
}

void Toolbar::OnMouseDown(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();

    // Check view mode buttons
    for (size_t i = 0; i < left_buttons_.size(); ++i)
    {
        if (left_buttons_[i].rect.Contains(pos))
        {
            // R17 Fix 3: Track pressed button for visual feedback
            pressed_button_index_ = static_cast<int>(i);
            pressed_is_left_ = true;
            Refresh();

            // Focus mode button (index 3)
            if (i == 3)
            {
                event_bus_.publish(core::events::FocusModeChangedEvent{!focus_mode_active_});
                return;
            }

            core::events::ViewMode mode{};
            switch (i)
            {
                case 0:
                    mode = core::events::ViewMode::Editor;
                    break;
                case 1:
                    mode = core::events::ViewMode::Split;
                    break;
                case 2:
                    mode = core::events::ViewMode::Preview;
                    break;
                default:
                    return;
            }

            if (mode != active_mode_)
            {
                event_bus_.publish(core::events::ViewModeChangedEvent{mode});
            }
            return;
        }
    }

    // Right buttons — Save, Themes, Settings
    for (size_t i = 0; i < right_buttons_.size(); ++i)
    {
        if (right_buttons_[i].rect.Contains(pos))
        {
            // R17 Fix 3: Track pressed button
            pressed_button_index_ = static_cast<int>(i);
            pressed_is_left_ = false;
            Refresh();

            if (i == 0)
            {
                // R5 Fix 20: Save button — flash green and publish save event
                event_bus_.publish(core::events::TabSaveRequestEvent{});

                transition_manager_.start<float>("save_pulse",
                                                 1.0F,
                                                 1.2F,
                                                 [this](float scale)
                                                 {
                                                     save_pulse_scale_ = scale;
                                                     Refresh();
                                                 });

                transition_manager_.start<float>("save_flash",
                                                 1.0F,
                                                 0.0F,
                                                 [this](float a)
                                                 {
                                                     save_flash_alpha_ = a;
                                                     Refresh();
                                                 });

                Refresh();
            }
            else if (i == 1 && on_theme_gallery_click_)
            {
                on_theme_gallery_click_();
            }
            else if (i == 2)
            {
                // Phase 5: Settings button publishes SettingsOpenRequestEvent
                event_bus_.publish(core::events::SettingsOpenRequestEvent{});
            }
            return;
        }
    }
}

// ═══════════════════════════════════════════════════════
// Painting
// ═══════════════════════════════════════════════════════

void Toolbar::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);
    const auto& t = theme();

    dc.SetBackground(wxBrush(wxColour(t.colors.bg_panel.to_rgba_string())));
    dc.Clear();

    // 8D: subtle gradient overlay — lighter at bottom
    {
        auto base_col = wxColour(t.colors.bg_panel.to_rgba_string());
        auto lighter = base_col.ChangeLightness(103);
        int bar_h = GetClientSize().GetHeight();
        int bar_w = GetClientSize().GetWidth();
        for (int row = 0; row < bar_h - 1; ++row)
        {
            const double frac =
                static_cast<double>(row) / static_cast<double>(std::max(bar_h - 1, 1));
            auto lerp = [](int from, int to, double ratio) -> unsigned char
            {
                return static_cast<unsigned char>(
                    std::clamp(static_cast<int>(from + ratio * (to - from)), 0, 255));
            };
            dc.SetPen(wxPen(wxColour(lerp(base_col.Red(), lighter.Red(), frac),
                                     lerp(base_col.Green(), lighter.Green(), frac),
                                     lerp(base_col.Blue(), lighter.Blue(), frac)),
                            1));
            dc.DrawLine(0, row, bar_w, row);
        }
    }

    // 8B: soft bottom border — BorderLight at 40% alpha
    {
        auto border_col = wxColour(t.colors.border_light.to_rgba_string());
        dc.SetPen(wxPen(wxColour(border_col.Red(), border_col.Green(), border_col.Blue(), 102), 1));
    }
    int bottom_y = GetClientSize().GetHeight() - 1;
    dc.DrawLine(0, bottom_y, GetClientSize().GetWidth(), bottom_y);

    // R19 Fix 8: Subtle drop-shadow below toolbar bottom border
    {
        auto shadow_base = wxColour(t.colors.border_dark.to_rgba_string());
        for (int shadow_row = 0; shadow_row < kDropShadowHeight; ++shadow_row)
        {
            int alpha = 60 - (shadow_row * 30);
            if (alpha < 0)
            {
                alpha = 0;
            }
            dc.SetPen(wxPen(wxColour(shadow_base.Red(),
                                     shadow_base.Green(),
                                     shadow_base.Blue(),
                                     static_cast<unsigned char>(alpha)),
                            1));
            dc.DrawLine(0,
                        bottom_y + 1 + shadow_row,
                        GetClientSize().GetWidth(),
                        bottom_y + 1 + shadow_row);
        }
    }

    // Draw buttons using wxGraphicsContext
    std::unique_ptr<wxGraphicsContext> gc_owner(wxGraphicsContext::Create(dc));
    if (gc_owner == nullptr)
    {
        return;
    }
    auto& gc = *gc_owner;

    for (int idx = 0; idx < static_cast<int>(left_buttons_.size()); ++idx)
    {
        DrawButton(gc, left_buttons_[static_cast<size_t>(idx)], t);

        // R17 Fix 1: Active button underline — 2px accent line beneath active view mode buttons
        if (left_buttons_[static_cast<size_t>(idx)].is_active && idx < 3)
        {
            auto accent = wxColour(t.colors.accent_primary.to_rgba_string());
            gc.SetPen(gc.CreatePen(wxGraphicsPenInfo(accent).Width(2.0)));
            gc.SetBrush(*wxTRANSPARENT_BRUSH);
            double ux = left_buttons_[static_cast<size_t>(idx)].rect.GetX() + 2;
            double uw = left_buttons_[static_cast<size_t>(idx)].rect.GetWidth() - 4;
            double uy = left_buttons_[static_cast<size_t>(idx)].rect.GetBottom();
            gc.StrokeLine(ux, uy, ux + uw, uy);
        }

        // R20 Fix 10: Hover underline slide for inactive hovered buttons
        if (left_buttons_[static_cast<size_t>(idx)].is_hovered &&
            !left_buttons_[static_cast<size_t>(idx)].is_active)
        {
            auto muted = wxColour(t.colors.text_muted.to_rgba_string());
            gc.SetPen(gc.CreatePen(
                wxGraphicsPenInfo(muted).Width(static_cast<double>(kHoverUnderlineH))));
            gc.SetBrush(*wxTRANSPARENT_BRUSH);
            double ux = left_buttons_[static_cast<size_t>(idx)].rect.GetX() + 4;
            double uw = left_buttons_[static_cast<size_t>(idx)].rect.GetWidth() - 8;
            double uy = left_buttons_[static_cast<size_t>(idx)].rect.GetBottom() - 1;
            gc.StrokeLine(ux, uy, ux + uw, uy);
        }

        // R20 Fix 7: Separator between view mode group and focus button
        if (idx == 2 && left_buttons_.size() > 3)
        {
            auto sep_col = wxColour(t.colors.border_light.to_rgba_string());
            gc.SetPen(gc.CreatePen(wxGraphicsPenInfo(sep_col).Width(1.0)));
            int sep_x = (left_buttons_[2].rect.GetRight() + left_buttons_[3].rect.GetLeft()) / 2;
            gc.StrokeLine(sep_x, 10, sep_x, GetClientSize().GetHeight() - 10);
        }
    }

    // R17 Fix 5: Vertical separator between left and right button groups
    if (!left_buttons_.empty() && !right_buttons_.empty())
    {
        int sep_x =
            (left_buttons_.back().rect.GetRight() + right_buttons_.front().rect.GetLeft()) / 2;
        DrawZoneDivider(gc, sep_x, t);
    }

    // Phase 26: Build indicator — positioned between left and right button groups
    if (build_indicator_state_ != BuildIndicatorState::kIdle && !left_buttons_.empty() &&
        !right_buttons_.empty())
    {
        int bi_x = left_buttons_.back().rect.GetRight() + 8;
        int bi_y = (GetClientSize().GetHeight() - 14) / 2;
        DrawBuildIndicator(gc, bi_x, bi_y, t);
    }

    for (const auto& btn : right_buttons_)
    {
        DrawButton(gc, btn, t);

        // R20 Fix 10: Hover underline for right-group hovered buttons
        if (btn.is_hovered && !btn.is_active)
        {
            auto muted = wxColour(t.colors.text_muted.to_rgba_string());
            gc.SetPen(gc.CreatePen(
                wxGraphicsPenInfo(muted).Width(static_cast<double>(kHoverUnderlineH))));
            gc.SetBrush(*wxTRANSPARENT_BRUSH);
            double ux = btn.rect.GetX() + 4;
            double uw = btn.rect.GetWidth() - 8;
            double uy = btn.rect.GetBottom() - 1;
            gc.StrokeLine(ux, uy, ux + uw, uy);
        }
    }

    // Phase 26: Notification bell unread badge — red dot with count
    if (notification_unread_count_ > 0 && !right_buttons_.empty())
    {
        // Draw badge on the last right button (settings) area
        const auto& last_btn = right_buttons_.back();
        auto badge_font = wxFont(wxFontInfo(7).Family(wxFONTFAMILY_SWISS).Bold());
        auto badge_bg = theme_engine().color(core::ThemeColorToken::ErrorColor);
        auto badge_text = std::to_string(notification_unread_count_);
        gc.SetFont(badge_font, wxColour(255, 255, 255));
        wxDouble bw = 0;
        wxDouble bh = 0;
        gc.GetTextExtent(badge_text, &bw, &bh);
        double bx = last_btn.rect.GetRight() - bw;
        double by = last_btn.rect.GetY() - 2;
        gc.SetBrush(gc.CreateBrush(wxBrush(badge_bg)));
        gc.SetPen(*wxTRANSPARENT_PEN);
        gc.DrawRoundedRectangle(bx - 3, by, bw + 6, bh + 2, (bh + 2) / 2.0);
        gc.SetFont(badge_font, wxColour(255, 255, 255));
        gc.DrawText(badge_text, bx, by + 1);
    }

    // Phase 26: Overflow chevron in compact mode
    if (show_overflow_chevron_)
    {
        auto chevron_color = wxColour(t.colors.text_muted.to_rgba_string());
        gc.SetPen(gc.CreatePen(wxGraphicsPenInfo(chevron_color).Width(1.5)));
        gc.SetBrush(*wxTRANSPARENT_BRUSH);
        int cx = GetClientSize().GetWidth() - 16;
        int cy = GetClientSize().GetHeight() / 2;
        auto path = gc.CreatePath();
        path.MoveToPoint(cx - 3, cy - 4);
        path.AddLineToPoint(cx + 2, cy);
        path.AddLineToPoint(cx - 3, cy + 4);
        path.MoveToPoint(cx + 1, cy - 4);
        path.AddLineToPoint(cx + 6, cy);
        path.AddLineToPoint(cx + 1, cy + 4);
        gc.StrokePath(path);
    }

    // R20 Fix 9: Active mode badge — small count beside active view mode label
    {
        int active_count = 0;
        for (const auto& btn : left_buttons_)
        {
            if (btn.is_active)
            {
                ++active_count;
            }
        }
        if (active_count > 1)
        {
            // Find the last active button and draw a badge on it
            for (int idx = static_cast<int>(left_buttons_.size()) - 1; idx >= 0; --idx)
            {
                if (left_buttons_[static_cast<size_t>(idx)].is_active)
                {
                    const auto& btn = left_buttons_[static_cast<size_t>(idx)];
                    auto badge_text = std::to_string(active_count);
                    auto badge_font = wxFont(wxFontInfo(7).Family(wxFONTFAMILY_SWISS).Bold());
                    auto accent = wxColour(t.colors.accent_primary.to_rgba_string());
                    gc.SetFont(badge_font, theme_engine().color(core::ThemeColorToken::BgApp));
                    wxDouble bw = 0;
                    wxDouble bh = 0;
                    gc.GetTextExtent(badge_text, &bw, &bh);
                    double badge_x = btn.rect.GetRight() - bw - 2;
                    double badge_y = btn.rect.GetY() - 2;
                    gc.SetBrush(gc.CreateBrush(wxBrush(accent)));
                    gc.SetPen(*wxTRANSPARENT_PEN);
                    gc.DrawRoundedRectangle(badge_x - 3, badge_y, bw + 6, bh + 2, (bh + 2) / 2.0);
                    gc.SetFont(badge_font, wxColour(255, 255, 255));
                    gc.DrawText(badge_text, badge_x, badge_y + 1);
                    break;
                }
            }
        }
    }

    // R19 Fix 10: Zoom percentage badge (right-aligned) when zoom != 100%
    if (zoom_level_ != 0)
    {
        std::string zoom_text = std::to_string(100 + zoom_level_ * 10) + "%";
        auto zoom_font = wxFont(wxFontInfo(8).Family(wxFONTFAMILY_SWISS));
        auto zoom_color = wxColour(t.colors.text_muted.to_rgba_string());
        gc.SetFont(zoom_font, zoom_color);
        wxDouble zw = 0;
        wxDouble zh = 0;
        gc.GetTextExtent(zoom_text, &zw, &zh);
        double zoom_x = GetClientSize().GetWidth() - zw - 4;
        if (!right_buttons_.empty())
        {
            zoom_x = right_buttons_.back().rect.GetLeft() - zw - 12;
        }
        double zoom_y = (ds_.metrics.toolbar_height() - zh) / 2.0;
        // Draw badge background pill
        auto badge_bg = wxColour(t.colors.bg_app.to_rgba_string());
        gc.SetBrush(gc.CreateBrush(wxBrush(badge_bg)));
        gc.SetPen(*wxTRANSPARENT_PEN);
        gc.DrawRoundedRectangle(zoom_x - 4, zoom_y - 1, zw + 8, zh + 2, (zh + 2) / 2.0);
        gc.SetFont(zoom_font, zoom_color);
        gc.DrawText(zoom_text, zoom_x, zoom_y);
    }
}

void Toolbar::DrawButton(wxGraphicsContext& gc, const ButtonInfo& btn, const core::Theme& t) const
{
    const auto& c = t.colors;
    double rx = btn.rect.GetX();
    double ry = btn.rect.GetY();
    double rw = btn.rect.GetWidth();
    double rh = btn.rect.GetHeight();

    // R17 Fix 2: Dim save button when no file is modified (icon_type 3 = save)
    // (Opacity handled in text color below — skip background for non-flash clean save)

    // Background
    // R5 Fix 20: Green flash for save button after save
    if (save_flash_alpha_ > 0.01F && btn.icon_type == 3)
    {
        auto flash_col = theme_engine().color(core::ThemeColorToken::SuccessColor);
        int alpha = static_cast<int>(save_flash_alpha_ * 80.0F);
        gc.SetBrush(gc.CreateBrush(wxBrush(wxColour(
            flash_col.Red(), flash_col.Green(), flash_col.Blue(), static_cast<wxByte>(alpha)))));
        gc.SetPen(*wxTRANSPARENT_PEN);
        gc.DrawRoundedRectangle(rx, ry, rw, rh, 4.0);
    }
    else if (btn.is_active)
    {
        auto bg = c.accent_primary.with_alpha(0.20f).to_rgba_string();
        gc.SetBrush(gc.CreateBrush(wxBrush(wxColour(bg))));
        gc.SetPen(*wxTRANSPARENT_PEN);
        // R19 Fix 6: Pill shape for active buttons (half-height radius)
        gc.DrawRoundedRectangle(rx, ry, rw, rh, rh / 2.0);
    }
    else if (btn.is_hovered)
    {
        auto bg = c.text_main.with_alpha(0.08f).to_rgba_string();
        gc.SetBrush(gc.CreateBrush(wxBrush(wxColour(bg))));
        gc.SetPen(*wxTRANSPARENT_PEN);
        gc.DrawRoundedRectangle(rx, ry, rw, rh, 4.0);
    }

    // Text/icon color
    wxColour text_color;
    if (!btn.is_enabled)
    {
        // Phase 06 Task 49: Visually grayed out disabled tools
        auto muted = c.text_muted;
        text_color = wxColour(muted.r, muted.g, muted.b, 60); // Low opacity for disabled state
    }
    else if (btn.is_active)
    {
        text_color = wxColour(c.accent_primary.to_rgba_string());
    }
    else if (btn.is_hovered)
    {
        text_color = wxColour(c.text_main.to_rgba_string());
    }
    else
    {
        text_color = wxColour(c.text_muted.to_rgba_string());
    }

    gc.SetPen(gc.CreatePen(wxGraphicsPenInfo(text_color).Width(1.5)));
    gc.SetBrush(*wxTRANSPARENT_BRUSH);

    const int kButtonPadH = ds_.spacing.scaled(SpacingToken::kSm);
    const int kIconSize = ds_.metrics.icon_size_small();
    const double kFontSizeLabel =
        static_cast<double>(ds_.typography.font(TypeSlot::kCaption).GetPointSize());

    // Draw icon (centered vertically, left-aligned in button)
    double icon_x = rx + kButtonPadH;
    double icon_y = ry + (rh - kIconSize) / 2.0;

    // R20 Fix 6: Scale icon down slightly when pressed
    const bool is_pressed =
        (pressed_button_index_ >= 0 &&
         ((pressed_is_left_ && static_cast<size_t>(pressed_button_index_) < left_buttons_.size() &&
           &left_buttons_[static_cast<size_t>(pressed_button_index_)] == &btn) ||
          (!pressed_is_left_ &&
           static_cast<size_t>(pressed_button_index_) < right_buttons_.size() &&
           &right_buttons_[static_cast<size_t>(pressed_button_index_)] == &btn)));
    double icon_scale = is_pressed ? static_cast<double>(kPressScale) : 1.0;
    double actual_size = kIconSize * icon_scale;
    if (is_pressed)
    {
        // Center the scaled-down icon
        double offset = (kIconSize - actual_size) / 2.0;
        icon_x += offset;
        icon_y += offset;
    }

    std::string icon_name;
    switch (btn.icon_type)
    {
        case 0:
            icon_name = "toolbar-source";
            break;
        case 1:
            icon_name = "toolbar-split";
            break;
        case 2:
            icon_name = "toolbar-preview";
            break;
        case 3:
            icon_name = "toolbar-save";
            break;
        case 4:
            icon_name = "toolbar-themes";
            break;
        case 5:
            icon_name = "toolbar-settings";
            break;
        case 6:
            icon_name = "toolbar-focus";
            break;
    }

    if (!icon_name.empty())
    {
        // R19 Fix 7: Apply pulse scale to save icon during animation
        double final_size = actual_size;
        if (btn.icon_type == 3)
        {
            final_size *= static_cast<double>(save_pulse_scale_);
        }

        auto size_int = static_cast<int>(final_size);
        if (size_int > 0)
        {
            auto bmp = IconManager::get().get_icon_bitmap(
                icon_name, wxSize(size_int, size_int), text_color);
            gc.DrawBitmap(bmp, icon_x, icon_y, final_size, final_size);
        }
    }

    // Draw label (R18 Fix 16: skip in compact mode)
    if (!btn.label.empty() && !compact_mode_)
    {
        auto font = wxFont(wxFontInfo(kFontSizeLabel).Family(wxFONTFAMILY_SWISS).Bold());
        gc.SetFont(font, text_color);
        double text_x = icon_x + kIconSize + 6;
        double text_y = ry + (rh - kFontSizeLabel - 2) / 2.0;
        gc.DrawText(btn.label, text_x, text_y);
    }

    // R19 Fix 9: Keyboard focus ring
    if (focused_button_index_ >= 0)
    {
        bool is_focused_btn = false;
        if (focus_is_left_)
        {
            for (size_t fi = 0; fi < left_buttons_.size(); ++fi)
            {
                if (static_cast<int>(fi) == focused_button_index_ && &left_buttons_[fi] == &btn)
                {
                    is_focused_btn = true;
                }
            }
        }
        else
        {
            for (size_t fi = 0; fi < right_buttons_.size(); ++fi)
            {
                if (static_cast<int>(fi) == focused_button_index_ && &right_buttons_[fi] == &btn)
                {
                    is_focused_btn = true;
                }
            }
        }
        if (is_focused_btn)
        {
            auto accent = wxColour(c.accent_primary.to_rgba_string());
            gc.SetPen(gc.CreatePen(wxGraphicsPenInfo(accent).Width(2.0)));
            gc.SetBrush(
                gc.CreateBrush(wxBrush(wxColour(accent.Red(), accent.Green(), accent.Blue(), 40))));
            gc.DrawRoundedRectangle(rx - 1, ry - 1, rw + 2, rh + 2, 4.0);
        }
    }
}

// ═══════════════════════════════════════════════════════
// Icon Drawing
// ═══════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════
// Theme
// ═══════════════════════════════════════════════════════

void Toolbar::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);
    Refresh();
}

// ═══════════════════════════════════════════════════════
// Phase 26: Zone Divider
// ═══════════════════════════════════════════════════════

void Toolbar::DrawZoneDivider(wxGraphicsContext& graphics_ctx,
                              int x_position,
                              const core::Theme& current_theme) const
{
    const int bar_height = GetClientSize().GetHeight();
    const int divider_margin = 8;
    const auto divider_color = wxColour(current_theme.colors.border_light.to_rgba_string());

    graphics_ctx.SetPen(graphics_ctx.CreatePen(wxGraphicsPenInfo(divider_color).Width(1.0)));
    graphics_ctx.StrokeLine(x_position, divider_margin, x_position, bar_height - divider_margin);
}

// ═══════════════════════════════════════════════════════
// Phase 26: Build Indicator
// ═══════════════════════════════════════════════════════

void Toolbar::DrawBuildIndicator(wxGraphicsContext& graphics_ctx,
                                 int x_position,
                                 int y_position,
                                 const core::Theme& current_theme)
{
    constexpr int kIndicatorSize = 14;
    constexpr double kHalfSize = kIndicatorSize / 2.0;

    switch (build_indicator_state_)
    {
        case BuildIndicatorState::kIdle:
            // No indicator drawn when idle
            break;

        case BuildIndicatorState::kBuilding:
        {
            // Spinning arc indicator
            const auto accent = wxColour(current_theme.colors.accent_primary.to_rgba_string());
            graphics_ctx.SetPen(graphics_ctx.CreatePen(wxGraphicsPenInfo(accent).Width(2.0)));
            graphics_ctx.SetBrush(*wxTRANSPARENT_BRUSH);

            const double start_angle =
                (static_cast<double>(spinner_frame_) * 30.0) * 3.14159265 / 180.0;
            const double sweep_angle = 270.0 * 3.14159265 / 180.0;

            auto path = graphics_ctx.CreatePath();
            path.AddArc(x_position + kHalfSize,
                        y_position + kHalfSize,
                        kHalfSize - 1.0,
                        start_angle,
                        start_angle + sweep_angle,
                        true);
            graphics_ctx.StrokePath(path);

            // Advance spinner frame
            ++spinner_frame_;
            break;
        }

        case BuildIndicatorState::kSuccess:
        {
            // Green checkmark
            const auto success_col = wxColour(current_theme.success_fg_token.to_rgba_string());
            graphics_ctx.SetPen(graphics_ctx.CreatePen(wxGraphicsPenInfo(success_col).Width(2.0)));
            graphics_ctx.SetBrush(*wxTRANSPARENT_BRUSH);

            auto path = graphics_ctx.CreatePath();
            path.MoveToPoint(x_position + 3, y_position + kHalfSize);
            path.AddLineToPoint(x_position + kHalfSize, y_position + kIndicatorSize - 3);
            path.AddLineToPoint(x_position + kIndicatorSize - 2, y_position + 3);
            graphics_ctx.StrokePath(path);
            break;
        }

        case BuildIndicatorState::kFailure:
        {
            // Red X
            const auto error_col = wxColour(current_theme.error_fg_token.to_rgba_string());
            graphics_ctx.SetPen(graphics_ctx.CreatePen(wxGraphicsPenInfo(error_col).Width(2.0)));
            graphics_ctx.SetBrush(*wxTRANSPARENT_BRUSH);

            graphics_ctx.StrokeLine(x_position + 3,
                                    y_position + 3,
                                    x_position + kIndicatorSize - 3,
                                    y_position + kIndicatorSize - 3);
            graphics_ctx.StrokeLine(x_position + kIndicatorSize - 3,
                                    y_position + 3,
                                    x_position + 3,
                                    y_position + kIndicatorSize - 3);
            break;
        }
    }
}

} // namespace markamp::ui
