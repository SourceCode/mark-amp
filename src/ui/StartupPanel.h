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
