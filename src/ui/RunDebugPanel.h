#pragma once

#include "core/DebugSessionManager.h"
#include "core/EventBus.h"
#include "core/LaunchConfig.h"
#include "core/ThemeEngine.h"

#include <wx/button.h>
#include <wx/choice.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{
class Config;
} // namespace markamp::core

namespace markamp::ui
{

struct DesignSystemContext;
class SidebarSection;
class IconManager;

/// Phase 19 Task 1: Run and Debug sidebar panel.
/// Provides launch configuration selection, debug controls, breakpoints list,
/// call stack view, and variable inspection.
class RunDebugPanel : public wxPanel
{
public:
    RunDebugPanel(wxWindow* parent,
                  core::ThemeEngine& theme_engine,
                  core::EventBus& event_bus,
                  core::DebugSessionManager& session_mgr,
                  DesignSystemContext& design_system,
                  IconManager& icon_manager,
                  core::Config* config);

    /// Apply current theme styling.
    void ApplyTheme();

    /// Refresh all debug sections (after state change).
    void RefreshAll();

    /// Task 3: Set the list of launch configurations for the dropdown.
    void SetLaunchConfigs(const std::vector<core::LaunchConfig>& configs);

    /// Task 7: Update the breakpoints list display.
    void UpdateBreakpoints(const std::vector<std::string>& breakpoint_labels);

    /// Task 8: Update the call stack display.
    void UpdateCallStack(const std::vector<core::StackFrame>& frames);

    /// Task 9: Update the locals variables display.
    void UpdateVariables(const std::vector<core::DebugVariable>& variables);

    /// Set callback for when a launch config is selected and Play pressed.
    void SetOnLaunch(std::function<void(const std::string& config_name)> callback);

    /// Set callback for debug control actions.
    void SetOnContinue(std::function<void()> callback);
    void SetOnPause(std::function<void()> callback);
    void SetOnStepOver(std::function<void()> callback);
    void SetOnStepInto(std::function<void()> callback);
    void SetOnStepOut(std::function<void()> callback);
    void SetOnStop(std::function<void()> callback);
    void SetOnRestart(std::function<void()> callback);

private:
    core::ThemeEngine& theme_engine_;
    core::EventBus& event_bus_;
    core::DebugSessionManager& session_mgr_;
    DesignSystemContext& ds_;
    IconManager& icon_manager_;
    core::Config* config_{nullptr};

    // Task 3: Launch configuration area
    wxChoice* config_selector_{nullptr};
    wxButton* play_button_{nullptr};

    // Task 6: Debug controls area
    wxPanel* controls_panel_{nullptr};
    wxButton* btn_continue_{nullptr};
    wxButton* btn_pause_{nullptr};
    wxButton* btn_step_over_{nullptr};
    wxButton* btn_step_into_{nullptr};
    wxButton* btn_step_out_{nullptr};
    wxButton* btn_restart_{nullptr};
    wxButton* btn_stop_{nullptr};

    // Collapsible sections
    SidebarSection* variables_section_{nullptr};
    SidebarSection* callstack_section_{nullptr};
    SidebarSection* breakpoints_section_{nullptr};
    SidebarSection* watch_section_{nullptr};

    // Section content areas
    wxScrolledWindow* variables_scroll_{nullptr};
    wxScrolledWindow* callstack_scroll_{nullptr};
    wxScrolledWindow* breakpoints_scroll_{nullptr};
    wxScrolledWindow* watch_scroll_{nullptr};

    wxBoxSizer* variables_sizer_{nullptr};
    wxBoxSizer* callstack_sizer_{nullptr};
    wxBoxSizer* breakpoints_sizer_{nullptr};
    wxBoxSizer* watch_sizer_{nullptr};

    // Callbacks
    std::function<void(const std::string&)> on_launch_;
    std::function<void()> on_continue_;
    std::function<void()> on_pause_;
    std::function<void()> on_step_over_;
    std::function<void()> on_step_into_;
    std::function<void()> on_step_out_;
    std::function<void()> on_stop_;
    std::function<void()> on_restart_;

    void CreateLayout();
    void UpdateControlButtons();
};

} // namespace markamp::ui
