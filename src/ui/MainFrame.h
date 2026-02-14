#pragma once

#include "CustomChrome.h"
#include "core/FileNode.h"
#include "core/ShortcutManager.h"
#include "platform/PlatformAbstraction.h"

#include <wx/filename.h>
#include <wx/wx.h>

#include <memory>

namespace markamp::core
{
#include "core/EventBus.h"
class AppStateManager;
class Config;
class ThemeEngine;
class RecentWorkspaces;
class FeatureRegistry;
} // namespace markamp::core

namespace markamp::ui
{

class LayoutManager;
class StartupPanel;
class CommandPalette;
class ShortcutOverlay;

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title,
              const wxPoint& pos,
              const wxSize& size,
              markamp::core::EventBus* event_bus,
              markamp::core::Config* config,
              markamp::core::RecentWorkspaces* recent_workspaces,
              markamp::platform::PlatformAbstraction* platform,
              markamp::core::ThemeEngine* theme_engine,
              markamp::core::FeatureRegistry* feature_registry);

private:
    // Core references (owned by MarkAmpApp)
    markamp::core::EventBus* event_bus_;
    markamp::core::Config* config_;
    markamp::core::RecentWorkspaces* recent_workspaces_;
    markamp::platform::PlatformAbstraction* platform_;
    markamp::core::ThemeEngine* theme_engine_;
    markamp::core::FeatureRegistry* feature_registry_;

    // Subscriptions
    std::vector<markamp::core::Subscription> subscriptions_;

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

    // Opening folder
    void onOpenFolder(wxCommandEvent& event);
    void scanDirectory(const std::string& path, std::vector<core::FileNode>& out_nodes);

    // DPI reporting
    void logDpiInfo();

    // Menu bar
    void createMenuBar();
    void updateMenuBarForStartup();
    void updateMenuBarForEditor();
    void rebuildRecentMenu();

    // QoL Actions
    void toggleZenMode();
    void onSave(wxCommandEvent& event);

    // Tab management (QoL features 9-11)
    void onCloseActiveTab();
    void onCycleTab(bool forward);
    void updateWindowTitle();

    // State
    std::string last_active_file_;
    std::string workspace_folder_name_{"MarkAmp"}; // R3 Fix 12
    bool zen_mode_{false};

    // View switching
    void showStartupScreen();
    void showEditor();

    // IDs
    static constexpr int kMenuOpenRecentBase = 6000;
    static constexpr int kMenuOpenRecentMax = 6010;

    StartupPanel* startup_panel_{nullptr};

    // ── Phase 7: Command Palette & Keyboard UX ──
    CommandPalette* command_palette_{nullptr};
    ShortcutOverlay* shortcut_overlay_{nullptr};
    core::ShortcutManager shortcut_manager_;

    void RegisterPaletteCommands();
    void RegisterDefaultShortcuts();
    void ShowCommandPalette();
    void ToggleShortcutOverlay();
};
} // namespace markamp::ui
