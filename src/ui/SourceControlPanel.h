#pragma once

#include "SidebarSection.h"
#include "ThemeAwareWindow.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/ThemeEngine.h"
#include "ui/GitStatusProvider.h"

#include <wx/button.h>
#include <wx/listctrl.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>

#include <string>

namespace markamp::ui
{

class EmptyPanelState;
class SidebarFooter;

class SourceControlPanel : public ThemeAwareWindow
{
public:
    SourceControlPanel(wxWindow* parent,
                       core::ThemeEngine& theme_engine,
                       core::EventBus& event_bus,
                       core::Config* config,
                       DesignSystemContext& design_system,
                       IconManager& icon_manager,
                       std::string workspace_root,
                       std::string persistence_id = "");
    ~SourceControlPanel() override;

    void RefreshStatus();

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    void CreateLayout(DesignSystemContext& design_system, IconManager& icon_manager);
    void UpdateDynamicUI();
    void OnItemActivated(wxListEvent& event);

    core::EventBus& event_bus_;
    core::Config* config_{nullptr};
    std::string persistence_id_;
    std::string workspace_root_;

    std::unique_ptr<GitStatusProvider> git_provider_;

    wxTextCtrl* commit_message_input_{nullptr};
    wxButton* commit_button_{nullptr};

    SidebarSection* staged_changes_section_{nullptr};
    wxListCtrl* staged_list_{nullptr};

    SidebarSection* all_changes_section_{nullptr};
    wxListCtrl* changes_list_{nullptr};

    SidebarSection* timeline_section_{nullptr};

    SidebarFooter* footer_{nullptr};

    // Core event subscriptions
    core::Subscription action_sub_;
};

} // namespace markamp::ui
