#pragma once

#include "ThemeAwareWindow.h"

#include <wx/timer.h>

namespace markamp::ui
{

/// Custom-drawn scrollbar overlay with theme-aware colors.
/// Width: 8px. Track uses bg_panel, thumb uses accent_secondary,
/// thumb hover uses accent_primary.
class ThemedScrollbar : public ThemeAwareWindow
{
public:
    ThemedScrollbar(wxWindow* parent, core::ThemeEngine& theme_engine, wxWindow* scroll_target);

    /// Update scrollbar position based on content scroll state.
    void UpdateScrollPosition(int position, int visible_range, int total_range);

    /// Get the current scroll offset.
    [[nodiscard]] auto scroll_position() const -> int;

    /// Scrollbar width in pixels.
    static constexpr int kWidth = 8;
    static constexpr int kThumbMinHeight = 20;
    static constexpr int kAutoHideDelayMs = 1500; // R20 Fix 37: Auto-hide delay

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    wxWindow* scroll_target_;
    int position_{0};
    int visible_range_{0};
    int total_range_{0};
    bool is_hovering_{false};
    bool is_dragging_{false};
    int drag_start_y_{0};
    int drag_start_position_{0};
    float thumb_opacity_{1.0F}; // R20 Fix 37: current thumb opacity
    wxTimer auto_hide_timer_;   // R20 Fix 37: auto-hide timer

    void OnPaint(wxPaintEvent& event);
    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);

    [[nodiscard]] auto thumb_rect() const -> wxRect;
    [[nodiscard]] auto content_to_pixel(int content_pos) const -> int;
    [[nodiscard]] auto pixel_to_content(int pixel_y) const -> int;
    void OnAutoHideTimer(wxTimerEvent& event); // R20 Fix 37
    void ResetAutoHideTimer();                 // R20 Fix 37
};

} // namespace markamp::ui
