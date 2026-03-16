#pragma once

#include "DesignSystemContext.h"
#include "LayoutMetrics.h"
#include "ThemeAwareWindow.h"
#include "animation/TransitionManager.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <functional>
#include <vector>

namespace markamp::ui
{

/// @file Toolbar.h
/// @brief Main Application Toolbar — zone-based layout with run controls, layout modes, and utility
/// toggles.
///
/// Features:
///   - Three-zone layout: Left (run controls), Center (layout modes), Right (utility toggles)
///   - Run configuration dropdown with play/debug/stop buttons
///   - Build status indicator (spinner/check/X)
///   - Layout mode buttons (Default/Zen/Presentation)
///   - Sidebar, panel, breadcrumb, minimap, and search toggles
///   - Notification bell with unread badge
///   - Responsive collapse and overflow menu
///   - Toolbar customization via config
///
/// @see RunConfigService, NotificationService, LayoutManager
class Toolbar : public ThemeAwareWindow
{
public:
    Toolbar(wxWindow* parent, DesignSystemContext& ds, core::EventBus& event_bus);

    /// Update which view mode button is shown as active.
    void SetActiveViewMode(core::events::ViewMode mode);

    /// Get the current active view mode shown in the toolbar.
    [[nodiscard]] auto GetActiveViewMode() const -> core::events::ViewMode;

    void UpdateLayoutMetrics();

    [[deprecated("Use ElevationSystem for shadow values")]] static constexpr int kDropShadowHeight =
        2;                                         // R19 Fix 8
    static constexpr float kSavePulseSpeed = 0.1F; // R19 Fix 7: scale step per tick
    static constexpr float kPressScale = 0.9F;     // R20 Fix 6: press scale-down factor
    static constexpr int kHoverUnderlineH = 2;     // R20 Fix 10: hover underline height
    /// Callback type for opening the theme gallery.
    using ThemeGalleryCallback = std::function<void()>;
    void SetOnThemeGalleryClick(ThemeGalleryCallback callback);

    /// Set whether a specific action button is enabled.
    void SetButtonEnabled(int icon_type, bool enabled);

    // ── 100 Editor UX/UI Improvements: Batch 8 — Toolbar Accessors (#71–#77) ──

    /// #71 True when focus mode is active.
    [[nodiscard]] inline auto is_focus_mode() const noexcept -> bool
    {
        return focus_mode_active_;
    }

    /// #72 True when the toolbar is in compact (icons-only) mode.
    [[nodiscard]] inline auto is_compact_mode() const noexcept -> bool
    {
        return compact_mode_;
    }

    /// #73 Number of left-zone (view mode) buttons.
    [[nodiscard]] inline auto left_button_count() const noexcept -> std::size_t
    {
        return left_buttons_.size();
    }

    /// #74 Number of right-zone (action) buttons.
    [[nodiscard]] inline auto right_button_count() const noexcept -> std::size_t
    {
        return right_buttons_.size();
    }

    /// #75 True when a process (run/debug) is active.
    [[nodiscard]] inline auto is_process_running() const noexcept -> bool
    {
        return process_running_;
    }

    /// #76 Number of unread notifications.
    [[nodiscard]] inline auto notification_count() const noexcept -> int
    {
        return notification_unread_count_;
    }

    /// #77 True when an overflow chevron is visible.
    [[nodiscard]] inline auto has_overflow() const noexcept -> bool
    {
        return show_overflow_chevron_;
    }

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    DesignSystemContext& ds_;
    core::EventBus& event_bus_;
    core::events::ViewMode active_mode_{core::events::ViewMode::Split};

    // Button geometry
    struct ButtonInfo
    {
        wxRect rect;
        std::string label;
        bool is_active{false};
        bool is_hovered{false};
        bool is_enabled{true}; // Phase 06 Task 49: Disabled state
        int icon_type{0};      // 0=code, 1=columns, 2=eye, 3=save, 4=palette, 5=gear, 6=focus
    };

    std::vector<ButtonInfo> left_buttons_;  // View mode toggles
    std::vector<ButtonInfo> right_buttons_; // Action buttons

    // Painting
    void OnPaint(wxPaintEvent& event);
    void DrawButton(wxGraphicsContext& gc, const ButtonInfo& btn, const core::Theme& t) const;

    // Mouse
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);

    // Layout
    void RecalculateButtonRects();
    void OnSize(wxSizeEvent& event);

    // Phase 26: Zone-aware divider drawing
    void DrawZoneDivider(wxGraphicsContext& graphics_ctx,
                         int x_position,
                         const core::Theme& current_theme) const;

    // Phase 26: Build status indicator rendering
    void DrawBuildIndicator(wxGraphicsContext& graphics_ctx,
                            int x_position,
                            int y_position,
                            const core::Theme& current_theme);

    // Focus mode
    bool focus_mode_active_{false};

    // Event subscriptions
    core::Subscription view_mode_sub_;
    core::Subscription focus_mode_sub_;
    ThemeGalleryCallback on_theme_gallery_click_;

    // R5 Fix 20 & R19 Fix 7: Save button flash and pulse
    float save_flash_alpha_{0.0F};
    float save_pulse_scale_{1.0F};
    animation::TransitionManager transition_manager_{this};

    int zoom_level_{0};            // R19 Fix 10: current zoom level (%)
    int focused_button_index_{-1}; // R19 Fix 9: keyboard focus ring index
    bool focus_is_left_{true};     // R19 Fix 9: which button group has focus

    // R17 Fix 3: Button press feedback
    int pressed_button_index_{-1};
    bool pressed_is_left_{false};

    // R18 Fix 16: Responsive collapse to icons-only
    bool compact_mode_{false};

    // R20 Fix 8: Tooltip delay timer (prevents tooltip flash)
    wxTimer tooltip_delay_timer_;
    int pending_tooltip_index_{-1};
    bool pending_tooltip_is_left_{false};

    // ── Phase 26: Toolbar customization config ──────────────────────

    /// User-configurable toolbar layout settings.
    struct ToolbarCustomization
    {
        bool show_run_controls{true};
        bool show_layout_modes{true};
        bool show_sidebar_toggle{true};
        bool show_panel_toggle{true};
        bool show_notification_bell{true};
        bool show_search_button{true};
        bool show_breadcrumb_toggle{false}; // Off by default
        bool show_minimap_toggle{false};    // Off by default
    };

    ToolbarCustomization toolbar_config_;

    // ── Phase 26: Zone-based toolbar layout ──────────────────────────
    // These fields are declared ahead of the full Toolbar.cpp rebuild.
    // They will be wired to event handlers and paint routines in subsequent tasks.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"

    /// Toolbar zone alignment.
    enum class ZoneAlignment : uint8_t
    {
        kLeft,
        kCenter,
        kRight
    };

    /// A toolbar zone containing a group of buttons.
    struct ToolbarZone
    {
        ZoneAlignment alignment;
        std::vector<ButtonInfo> buttons;
        int total_width{0};
        int x_offset{0};
    };

    ToolbarZone left_zone_;   ///< Run controls
    ToolbarZone center_zone_; ///< Layout mode toggles
    ToolbarZone right_zone_;  ///< Utility toggles + notification bell

    // ── Phase 26: Build indicator ────────────────────────────────────

    /// Build indicator animation state.
    enum class BuildIndicatorState : uint8_t
    {
        kIdle,
        kBuilding,
        kSuccess,
        kFailure
    };

    BuildIndicatorState build_indicator_state_{BuildIndicatorState::kIdle};
    int spinner_frame_{0};
    wxTimer build_decay_timer_;

    // ── Phase 26: Layout modes ───────────────────────────────────────

    /// Layout mode presets.
    enum class LayoutMode : uint8_t
    {
        kDefault,
        kZen,
        kPresentation
    };

    /// Preset configuration for a layout mode.
    struct LayoutModePreset
    {
        bool show_sidebar{true};
        bool show_panel{true};
        bool show_toolbar{true};
        bool show_status_bar{true};
        bool show_tab_bar{true};
        int editor_font_size_delta{0};
    };

    LayoutMode current_layout_mode_{LayoutMode::kDefault};

    // ── Phase 26: Toggle button states ───────────────────────────────
    // These will be wired to event handlers in subsequent Toolbar.cpp updates.
    bool sidebar_visible_{true};
    bool panel_visible_{true};
    bool breadcrumb_visible_{true};
    bool minimap_visible_{true};
    bool search_visible_{false};
    int notification_unread_count_{0};

    // ── Phase 26: Process running state ──────────────────────────────

    bool process_running_{false};
    std::string running_config_name_;

    // ── Phase 26: Overflow ───────────────────────────────────────────

    std::vector<int> overflow_button_indices_;
    bool show_overflow_chevron_{false};

#pragma clang diagnostic pop

    // ── Phase 26: Event subscriptions ────────────────────────────────

    core::Subscription build_started_sub_;
    core::Subscription build_finished_sub_;
    core::Subscription run_started_sub_;
    core::Subscription run_finished_sub_;
    core::Subscription run_stopped_sub_;
    core::Subscription sidebar_toggle_sub_;
};

} // namespace markamp::ui
