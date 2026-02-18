#pragma once

#include "core/EventBus.h"

#include <wx/wx.h>

#include <filesystem>
#include <vector>

namespace markamp::core
{
class EventBus;
class RecentWorkspaces;
class ThemeEngine;
} // namespace markamp::core

namespace markamp::ui
{

class StartupPanel : public wxPanel
{
public:
    StartupPanel(wxWindow* parent,
                 markamp::core::EventBus* event_bus,
                 markamp::core::RecentWorkspaces* recent_workspaces,
                 markamp::core::ThemeEngine* theme_engine);

    /// Refresh the list of recent workspaces from the data source.
    void refreshRecentWorkspaces();

    // --- Phase 35: Welcome experience ---

    /// Show release notes for a specific version.
    void show_release_notes(const std::string& version);

    /// Set whether "Getting Started" section is visible.
    void set_show_getting_started(bool show);

    /// Check if the "Getting Started" section is visible.
    [[nodiscard]] auto is_getting_started_visible() const -> bool;

    /// Set the "don't show startup screen" preference.
    void set_dont_show_again(bool value);

    /// Check the "don't show startup screen" preference.
    [[nodiscard]] auto dont_show_again() const -> bool;

    /// Get the currently displayed release notes version (empty if none).
    [[nodiscard]] auto displayed_version() const -> const std::string&;

private:
    markamp::core::EventBus* event_bus_;
    markamp::core::RecentWorkspaces* recent_workspaces_;
    markamp::core::ThemeEngine* theme_engine_;
    core::Subscription theme_sub_;

    // UI Components
    wxBoxSizer* main_sizer_{nullptr};
    wxPanel* recent_list_container_{nullptr};
    wxBoxSizer* recent_list_sizer_{nullptr};
    int selected_workspace_idx_{-1}; // R18 Fix 31: keyboard nav selection

    // Phase 35 state
    bool show_getting_started_{true};
    bool dont_show_again_{false};
    std::string displayed_version_;

    void initUi();
    void createLogo(wxSizer* parent);
    void createButtons(wxSizer* parent);
    void createRecentList(wxSizer* parent);

    // Event handlers
    void onOpenFolder(wxCommandEvent& event);
    void onOpenRepository(wxCommandEvent& event);
    void onWorkspaceClick(const std::filesystem::path& path);
    void onPaint(wxPaintEvent& event);
};

} // namespace markamp::ui
