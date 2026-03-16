#pragma once

/// @file FloatingToolbar.h
/// @brief V13 Phase 30 — Base class for all floating toolbars.
///
/// Provides common behavior: themed rendering, drag handle for repositioning,
/// auto-hide with fade-out, snap-to-edges, button layout, and z-order support.

#include "core/EventBus.h"
#include "core/ThemeEngine.h"

#include <wx/popupwin.h>
#include <wx/timer.h>

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// A button definition for a floating toolbar.
struct FloatingToolbarButton
{
    std::string id;
    std::string label; ///< Unicode label text for renderin (e.g. "▶", "B")
    std::string tooltip;
    std::string shortcut; ///< Optional shortcut text (e.g. "⌘B")
    std::function<void()> callback;
    std::function<bool()> is_enabled; ///< Returns false to grey out; default = always enabled
    std::function<bool()> is_toggled; ///< Returns true for toggled state; default = not toggled
    bool is_toggle{false};
    bool is_separator{false};
};

/// Window edge for snap behaviour.
enum class FloatingToolbarEdge : uint8_t
{
    kNone,
    kTop,
    kBottom,
    kLeft,
    kRight,
};

/// Base class for all floating toolbars.
///
/// Inherits from wxPopupTransientWindow for lightweight overlay rendering.
/// All concrete toolbars (DebugToolbar, SelectionActionBar, etc.) inherit
/// from this and call SetButtons() to define their button set.
class FloatingToolbar : public wxPopupTransientWindow
{
public:
    FloatingToolbar(wxWindow* parent,
                    core::ThemeEngine& theme_engine,
                    core::EventBus& event_bus,
                    const std::string& toolbar_id);

    /// Show the toolbar at a specific screen position.
    void ShowAt(const wxPoint& position);

    /// Show centered horizontally at the top of the parent.
    void ShowAtScreenCenter();

    /// Hide the toolbar with optional fade-out.
    void HideToolbar();

    /// Set auto-hide delay in milliseconds. 0 disables auto-hide.
    void SetAutoHideMs(int delay_ms);

    /// Set whether the toolbar can be dragged.
    void SetDraggable(bool draggable);

    /// Set whether the toolbar snaps to parent window edges.
    void SetSnapToEdges(bool snap);

    [[nodiscard]] auto toolbar_id() const -> const std::string&;
    [[nodiscard]] auto is_visible() const -> bool;
    [[nodiscard]] auto last_position() const -> wxPoint;
    [[nodiscard]] auto button_count() const -> int;
    [[nodiscard]] auto is_draggable() const -> bool;

    /// Whether any button in the toolbar is a separator.
    [[nodiscard]] auto has_separator() const -> bool;

    /// Number of currently enabled (non-greyed) buttons.
    [[nodiscard]] auto enabled_count() const -> int;

    /// Number of currently toggled buttons.
    [[nodiscard]] auto toggled_count() const -> int;

    // Layout constants
    static constexpr int kButtonSize = 28;
    static constexpr int kButtonPadding = 2;
    static constexpr int kDragHandleWidth = 12;
    static constexpr int kBorderRadius = 8;
    static constexpr int kShadowOffset = 3;
    static constexpr int kSnapDistance = 20;
    static constexpr int kDefaultAutoHideMs = 5000;

    // ── 100 Editor UX/UI Improvements: Batch 9 — Float Toolbar Accessors (#89–#90) ──

    /// #89 True when auto-hide is enabled (delay > 0).
    [[nodiscard]] inline auto is_auto_hide_active() const noexcept -> bool
    {
        return auto_hide_ms_ > 0;
    }

    /// #90 Auto-hide delay in milliseconds.
    [[nodiscard]] inline auto auto_hide_delay() const noexcept -> int
    {
        return auto_hide_ms_;
    }

protected:
    /// Subclasses call this to define the toolbar's button set.
    void SetButtons(std::vector<FloatingToolbarButton> buttons);

    /// Refresh enabled/toggled states of all buttons.
    void RefreshButtonStates();

    /// Calculate the total size needed for the toolbar.
    [[nodiscard]] auto CalculateSize() const -> wxSize;

    core::ThemeEngine& theme_engine_; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    core::EventBus& event_bus_;       // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

private:
    std::string toolbar_id_;
    std::vector<FloatingToolbarButton> buttons_;
    core::Subscription theme_sub_;

    bool draggable_{true};
    bool snap_to_edges_{true};
    int auto_hide_ms_{0};
    bool visible_{false};
    wxPoint last_position_{0, 0};

    // Drag state
    bool is_dragging_{false};
    wxPoint drag_start_;
    wxPoint window_start_;

    // Button interaction
    int hovered_button_{-1};
    int pressed_button_{-1};
    int focused_button_{-1};

    // Auto-hide
    wxTimer auto_hide_timer_;

    // Rendering
    void OnPaint(wxPaintEvent& event);
    void ApplyTheme();

    // Interaction
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    // Layout
    [[nodiscard]] auto ButtonRectAt(int index) const -> wxRect;
    [[nodiscard]] auto HitTestButton(const wxPoint& point) const -> int;
    [[nodiscard]] auto IsDragHandleHit(const wxPoint& point) const -> bool;

    // Snapping
    void SnapToNearestEdge();

    // Auto-hide
    void OnAutoHideTimer(wxTimerEvent& event);
    void ResetAutoHideTimer();

    wxDECLARE_EVENT_TABLE(); // NOLINT
};

} // namespace markamp::ui
