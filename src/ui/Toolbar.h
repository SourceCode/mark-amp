#pragma once

#include "ThemeAwareWindow.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <wx/timer.h>

#include <functional>
#include <vector>

namespace markamp::ui
{

/// Toolbar with view mode toggle buttons (SRC / SPLIT / VIEW) and
/// right-side action buttons (Save, Themes, Settings).
/// Height: 40px, background: bg_panel, bottom border: border_dark.
class Toolbar : public ThemeAwareWindow
{
public:
    Toolbar(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus);

    /// Update which view mode button is shown as active.
    void SetActiveViewMode(core::events::ViewMode mode);

    /// Get the current active view mode shown in the toolbar.
    [[nodiscard]] auto GetActiveViewMode() const -> core::events::ViewMode;

    static constexpr int kHeight = 40;
    static constexpr int kDropShadowHeight = 2;    // R19 Fix 8
    static constexpr float kSavePulseSpeed = 0.1F; // R19 Fix 7: scale step per tick
    static constexpr float kPressScale = 0.9F;     // R20 Fix 6: press scale-down factor
    static constexpr int kHoverUnderlineH = 2;     // R20 Fix 10: hover underline height
    /// Callback type for opening the theme gallery.
    using ThemeGalleryCallback = std::function<void()>;
    void SetOnThemeGalleryClick(ThemeGalleryCallback callback);

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    core::EventBus& event_bus_;
    core::events::ViewMode active_mode_{core::events::ViewMode::Split};

    // Button geometry
    struct ButtonInfo
    {
        wxRect rect;
        std::string label;
        bool is_active{false};
        bool is_hovered{false};
        int icon_type{0}; // 0=code, 1=columns, 2=eye, 3=save, 4=palette, 5=gear, 6=focus
    };

    std::vector<ButtonInfo> left_buttons_;  // View mode toggles
    std::vector<ButtonInfo> right_buttons_; // Action buttons

    // Painting
    void OnPaint(wxPaintEvent& event);
    void DrawButton(wxGraphicsContext& gc, const ButtonInfo& btn, const core::Theme& t) const;
    void DrawCodeIcon(wxGraphicsContext& gc, double x, double y, double size) const;
    void DrawColumnsIcon(wxGraphicsContext& gc, double x, double y, double size) const;
    void DrawEyeIcon(wxGraphicsContext& gc, double x, double y, double size) const;
    void DrawSaveIcon(wxGraphicsContext& gc, double x, double y, double size) const;
    void DrawPaletteIcon(wxGraphicsContext& gc, double x, double y, double size) const;
    void DrawGearIcon(wxGraphicsContext& gc, double x, double y, double size) const;
    void DrawFocusIcon(wxGraphicsContext& gc, double x, double y, double size) const;

    // Mouse
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);

    // Layout
    void RecalculateButtonRects();
    void OnSize(wxSizeEvent& event);

    // Focus mode
    bool focus_mode_active_{false};

    // Event subscriptions
    core::Subscription view_mode_sub_;
    core::Subscription focus_mode_sub_;
    ThemeGalleryCallback on_theme_gallery_click_;

    // R5 Fix 20: Save button flash feedback
    bool save_flash_active_{false};
    wxTimer save_flash_timer_;
    float save_pulse_scale_{1.0F}; // R19 Fix 7: save icon scale during pulse
    wxTimer save_pulse_timer_;     // R19 Fix 7: timer for save icon pulse
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
};

} // namespace markamp::ui
