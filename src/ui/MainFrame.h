#pragma once

#include "CustomChrome.h"
#include "platform/PlatformAbstraction.h"

#include <wx/wx.h>

#include <memory>

namespace markamp::core
{
class EventBus;
class AppStateManager;
class Config;
class ThemeEngine;
} // namespace markamp::core

namespace markamp::ui
{

class LayoutManager;

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title,
              const wxPoint& pos,
              const wxSize& size,
              markamp::core::EventBus* event_bus,
              markamp::core::Config* config,
              markamp::platform::PlatformAbstraction* platform,
              markamp::core::ThemeEngine* theme_engine);

private:
    // Core references (owned by MarkAmpApp)
    markamp::core::EventBus* event_bus_;
    markamp::core::Config* config_;
    markamp::platform::PlatformAbstraction* platform_;
    markamp::core::ThemeEngine* theme_engine_;

    // UI components
    CustomChrome* chrome_{nullptr};
    LayoutManager* layout_{nullptr};

    // Event handlers
    void onClose(wxCloseEvent& event);
    void onSize(wxSizeEvent& event);

    // Edge resize
    static constexpr int kResizeZone = 5;
    static constexpr int kCornerZone = 10;

    /// Determine which resize edge the mouse is near (or None).
    [[nodiscard]] auto detectResizeEdge(const wxPoint& pos) const -> markamp::platform::ResizeEdge;
    void onFrameMouseMove(wxMouseEvent& event);
    void onFrameMouseDown(wxMouseEvent& event);

    // Window state persistence
    void saveWindowState();
    void restoreWindowState();

    // DPI reporting
    void logDpiInfo();

    // Menu bar
    void createMenuBar();
};

} // namespace markamp::ui
