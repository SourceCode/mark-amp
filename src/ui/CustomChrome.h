#pragma once

#include "core/EventBus.h"
#include "platform/PlatformAbstraction.h"

#include <wx/wx.h>

#include <memory>
#include <string>

namespace markamp::core
{
class EventBus;
class ThemeEngine;
} // namespace markamp::core

namespace markamp::ui
{

/// Custom-drawn window chrome that replaces the OS title bar.
/// Renders a retro-futuristic title bar with logo, filename, sidebar toggle, and window controls.
class CustomChrome : public wxPanel
{
public:
    static constexpr int kChromeHeight = 40;
    static constexpr int kButtonWidth = 36;
    static constexpr int kButtonIconSize = 12;
    static constexpr int kLogoDotRadius = 7;
    static constexpr int kResizeZone = 4;

    CustomChrome(wxWindow* parent,
                 markamp::core::EventBus* event_bus,
                 markamp::platform::PlatformAbstraction* platform,
                 markamp::core::ThemeEngine* theme_engine = nullptr);

    /// Update the displayed filename in the center of the chrome.
    void set_filename(const std::string& filename);

    /// Update the maximized state (changes maximize button icon).
    void set_maximized(bool maximized);

    /// Describes which region of the chrome was hit.
    enum class HitZone
    {
        None,
        DragArea,
        MinimizeButton,
        MaximizeButton,
        CloseButton,
        SidebarToggle
    };

    /// Determine which zone a local point falls into.
    [[nodiscard]] auto hit_test(const wxPoint& local_pos) const -> HitZone;

protected:
    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseDoubleClick(wxMouseEvent& event);
    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);

private:
    markamp::core::EventBus* event_bus_;
    markamp::platform::PlatformAbstraction* platform_;
    markamp::core::ThemeEngine* theme_engine_;
    markamp::core::Subscription theme_subscription_;

    std::string filename_{"Untitled"};
    bool is_maximized_{false};
    HitZone hover_zone_{HitZone::None};
    HitZone pressed_zone_{HitZone::None};
    bool dragging_{false};
    wxPoint drag_start_;

    // Layout rectangles (computed in OnPaint, cached for hit testing)
    wxRect minimize_rect_;
    wxRect maximize_rect_;
    wxRect close_rect_;
    wxRect sidebar_rect_;

    // Drawing helpers
    void drawLogoDot(wxDC& dc, int x, int y);
    void drawWindowButton(wxDC& dc, const wxRect& rect, HitZone zone, const wxString& glyph);
    void drawSidebarToggle(wxDC& dc, const wxRect& rect);
    void drawBevelBorder(wxDC& dc, int width, int height);

    void computeLayout(int width);

    // Themed colour helpers (fall back to hardcoded if no theme engine)
    [[nodiscard]] auto chromeBg() const -> wxColour;
    [[nodiscard]] auto textMain() const -> wxColour;
    [[nodiscard]] auto textMuted() const -> wxColour;
    [[nodiscard]] auto accentPrimary() const -> wxColour;
    [[nodiscard]] auto hoverBg() const -> wxColour;
    [[nodiscard]] auto borderLight() const -> wxColour;
    [[nodiscard]] auto borderDark() const -> wxColour;

    auto getParentFrame() -> wxFrame*;
};

} // namespace markamp::ui
