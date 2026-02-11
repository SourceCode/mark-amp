#pragma once

#include "PlatformAbstraction.h"

namespace markamp::platform
{

/// macOS platform implementation using Cocoa/NSWindow APIs.
class MacPlatform : public PlatformAbstraction
{
public:
    void set_frameless_window_style(wxFrame* frame) override;
    auto begin_native_drag(wxFrame* frame, const wxPoint& mouse_screen_pos) -> bool override;
    auto begin_native_resize(wxFrame* frame, ResizeEdge edge) -> bool override;

    [[nodiscard]] auto uses_native_window_controls() const -> bool override;
    [[nodiscard]] auto get_window_controls_rect(wxFrame* frame) const -> wxRect override;

    [[nodiscard]] auto is_maximized(const wxFrame* frame) const -> bool override;
    void toggle_maximize(wxFrame* frame) override;
    void enter_fullscreen(wxFrame* frame) override;
    void exit_fullscreen(wxFrame* frame) override;

    // Accessibility
    [[nodiscard]] auto is_high_contrast() const -> bool override;
    [[nodiscard]] auto prefers_reduced_motion() const -> bool override;
    void announce_to_screen_reader(wxWindow* window, const wxString& message) override;

    // System Appearance
    [[nodiscard]] auto is_dark_mode() const -> bool override;

    // Display
    [[nodiscard]] auto get_content_scale_factor() const -> double override;
};

} // namespace markamp::platform
