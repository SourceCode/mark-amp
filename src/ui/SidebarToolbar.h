#pragma once

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ThemeEngine.h"

#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Action button that can appear in the sidebar toolbar header.
struct ToolbarAction
{
    std::string label;   // Display text or emoji glyph
    std::string tooltip; // Tooltip on hover
    std::function<void()> on_click;
};

/// Contextual header bar at the top of the sidebar showing the active panel
/// title and per-mode action buttons (collapse, filter, refresh, etc.).
class [[deprecated("Use PanelHeader and SidebarHeader from Phase 08")]] SidebarToolbar
    : public wxPanel
{
public:
    SidebarToolbar(wxWindow * parent, core::ThemeEngine & theme_engine, core::EventBus & event_bus);

    /// Set the title label shown in the toolbar header.
    void SetTitle(const std::string& title);

    /// Replace the action buttons with a new set of actions.
    void SetActions(const std::vector<ToolbarAction>& actions);

    /// Clear all action buttons.
    void ClearActions();

private:
    core::ThemeEngine& theme_engine_;
    core::EventBus& event_bus_;

    wxStaticText* title_label_{nullptr};
    wxBoxSizer* actions_sizer_{nullptr};
    wxBoxSizer* main_sizer_{nullptr};

    core::Subscription theme_sub_;

    void ApplyTheme();
    void RebuildActions(const std::vector<ToolbarAction>& actions);
};

} // namespace markamp::ui
