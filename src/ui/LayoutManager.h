#pragma once

#include "ThemeAwareWindow.h"
#include "core/EventBus.h"
#include "core/FileNode.h"
#include "core/ThemeEngine.h"

#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/timer.h>

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{
class Config;
} // namespace markamp::core

namespace markamp::ui
{

class FileTreeCtrl;
class PreviewPanel;
class SplitterBar;
class SplitView;
class StatusBarPanel;
class TabBar;
class Toolbar;

/// Orchestrates the three-zone layout below CustomChrome:
///   Sidebar (256px) | Content Area (flex)
///   StatusBar (24px, bottom)
class LayoutManager : public ThemeAwareWindow
{
public:
    LayoutManager(wxWindow* parent,
                  core::ThemeEngine& theme_engine,
                  core::EventBus& event_bus,
                  core::Config* config);

    // Zone access (for later phases to populate)
    [[nodiscard]] auto sidebar_container() -> wxPanel*;
    [[nodiscard]] auto content_container() -> wxPanel*;
    [[nodiscard]] auto statusbar_container() -> StatusBarPanel*;

    // Data
    void setFileTree(const std::vector<core::FileNode>& roots);
    void SaveFile(const std::string& path);

    // Multi-file tab management (QoL features 1-5)
    void OpenFileInTab(const std::string& path);
    void CloseTab(const std::string& path);
    void SwitchToTab(const std::string& path);
    void SaveActiveFile();
    void SaveActiveFileAs();
    [[nodiscard]] auto GetActiveFilePath() const -> std::string;
    [[nodiscard]] auto GetTabBar() -> TabBar*;

    // Auto-save (feature 12)
    void StartAutoSave();
    void StopAutoSave();

    // File reload (feature 13)
    void CheckExternalFileChanges();

    // Sidebar control
    void toggle_sidebar();
    void set_sidebar_visible(bool visible);
    [[nodiscard]] auto is_sidebar_visible() const -> bool;

    // Sidebar width
    void set_sidebar_width(int width);
    [[nodiscard]] auto sidebar_width() const -> int;

    // Phase 6D/7A: Forward minimap toggle to editor
    void ToggleEditorMinimap();

    static constexpr int kDefaultSidebarWidth = 256;
    static constexpr int kMinSidebarWidth = 180;
    static constexpr int kMaxSidebarWidth = 400;
    static constexpr int kStatusBarHeight = 24;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    core::EventBus& event_bus_;
    core::Config* config_;

    // Child panels
    wxPanel* sidebar_panel_{nullptr};
    wxPanel* content_panel_{nullptr};
    StatusBarPanel* statusbar_panel_{nullptr};
    SplitterBar* splitter_{nullptr};
    FileTreeCtrl* file_tree_{nullptr};
    TabBar* tab_bar_{nullptr};
    wxTextCtrl* search_field_{nullptr};
    SplitView* split_view_{nullptr};
    Toolbar* toolbar_{nullptr};

    // Sizer management
    wxBoxSizer* main_sizer_{nullptr};
    wxBoxSizer* body_sizer_{nullptr};

    bool sidebar_visible_{true};
    int sidebar_width_{kDefaultSidebarWidth};
    int sidebar_current_width_{kDefaultSidebarWidth};

    // Animation
    wxTimer sidebar_anim_timer_;
    int sidebar_anim_start_width_{0};
    int sidebar_anim_target_width_{0};
    double sidebar_anim_progress_{0.0};
    bool sidebar_anim_showing_{false};
    static constexpr int kAnimFrameMs = 16; // ~60fps
    static constexpr double kShowDurationMs = 300.0;
    static constexpr double kHideDurationMs = 200.0;

    void OnSidebarAnimTimer(wxTimerEvent& event);

    // Event subscriptions
    core::Subscription sidebar_toggle_sub_;

    void CreateLayout();
    void UpdateSidebarSize(int width);
    void SaveLayoutState();
    void RestoreLayoutState();

    // Multi-file state
    struct FileBuffer
    {
        std::string content;
        bool is_modified{false};
        int cursor_position{0};
        int first_visible_line{0};
        std::filesystem::file_time_type last_write_time{};
    };
    std::unordered_map<std::string, FileBuffer> file_buffers_;
    std::string active_file_path_;

    // Event subscriptions for tabs
    core::Subscription tab_switched_sub_;
    core::Subscription tab_close_sub_;
    core::Subscription tab_save_sub_;
    core::Subscription tab_save_as_sub_;
    core::Subscription content_changed_sub_;
    core::Subscription file_reload_sub_;
    core::Subscription goto_line_sub_;

    // Auto-save
    wxTimer auto_save_timer_;
    static constexpr int kAutoSaveIntervalMs = 30000; // 30 seconds
    void OnAutoSaveTimer(wxTimerEvent& event);

    // Sidebar custom painting
    void OnSidebarPaint(wxPaintEvent& event);
};

} // namespace markamp::ui
