#pragma once

#include <wx/wx.h>

#include <memory>

namespace markamp::platform
{

/// Resize edge/corner identifier for platform-specific resize operations.
enum class ResizeEdge
{
    None,
    Top,
    Bottom,
    Left,
    Right,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
};

/// Abstract platform interface for native window operations.
/// Each OS implements this to provide optimal drag, resize, and frameless behavior.
class PlatformAbstraction
{
public:
    virtual ~PlatformAbstraction() = default;

    /// Configure the window for frameless chrome (hide OS title bar, keep resize).
    virtual void set_frameless_window_style(wxFrame* frame) = 0;

    /// Begin a native window drag operation (called on mouse-down in title bar).
    /// Returns true if the platform handled the drag natively.
    virtual auto begin_native_drag(wxFrame* frame, const wxPoint& mouse_screen_pos) -> bool = 0;

    /// Begin a native edge-resize operation.
    /// Returns true if the platform handled the resize natively.
    virtual auto begin_native_resize(wxFrame* frame, ResizeEdge edge) -> bool = 0;

    /// Returns true if the platform provides its own window controls (e.g. macOS traffic lights).
    [[nodiscard]] virtual auto uses_native_window_controls() const -> bool
    {
        return false;
    }

    /// Returns the rectangle occupied by native window controls in window coordinates.
    /// Returns empty rect if native controls are not used or not visible.
    [[nodiscard]] virtual auto get_window_controls_rect(wxFrame* frame) const -> wxRect
    {
        (void)frame;
        return wxRect();
    }

    /// Check if the frame is currently maximized / zoomed.
    [[nodiscard]] virtual auto is_maximized(const wxFrame* frame) const -> bool = 0;

    /// Toggle between maximized and restored state.
    virtual void toggle_maximize(wxFrame* frame) = 0;

    /// Enter fullscreen mode (macOS native fullscreen, or simulated on other platforms).
    virtual void enter_fullscreen(wxFrame* frame) = 0;

    /// Exit fullscreen mode.
    virtual void exit_fullscreen(wxFrame* frame) = 0;

    // ── Accessibility ──

    /// Returns true if the OS is in high contrast mode.
    [[nodiscard]] virtual auto is_high_contrast() const -> bool = 0;

    /// Returns true if the OS prefers reduced motion.
    [[nodiscard]] virtual auto prefers_reduced_motion() const -> bool = 0;

    /// Announce a message to screen readers via platform-native API.
    virtual void announce_to_screen_reader(wxWindow* window, const wxString& message) = 0;

    // ── System Appearance ──

    /// Returns true if the OS is currently in dark mode / dark appearance.
    [[nodiscard]] virtual auto is_dark_mode() const -> bool
    {
        return false;
    }

    // ── Display ──

    /// Returns the content scale factor for the primary display.
    /// 1.0 = standard (96 DPI), 2.0 = Retina/HiDPI.
    [[nodiscard]] virtual auto get_content_scale_factor() const -> double
    {
        return 1.0;
    }
};

/// Factory: creates the correct platform implementation for the current OS.
auto create_platform() -> std::unique_ptr<PlatformAbstraction>;

} // namespace markamp::platform
