#pragma once

#include "ThemeAwareWindow.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/ThemeEngine.h"

#include <wx/choice.h>
#include <wx/listctrl.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/srchctrl.h>

#include <string>
#include <vector>

namespace markamp::ui
{

class GitLogPanel : public ThemeAwareWindow
{
public:
    GitLogPanel(wxWindow* parent,
                core::ThemeEngine& theme_engine,
                core::EventBus& event_bus,
                std::string workspace_root);
    ~GitLogPanel() override;

    GitLogPanel(const GitLogPanel&) = delete;
    auto operator=(const GitLogPanel&) -> GitLogPanel& = delete;
    GitLogPanel(GitLogPanel&&) = delete;
    auto operator=(GitLogPanel&&) -> GitLogPanel& = delete;

    void RefreshLog();
    void SetWorkspaceRoot(const std::string& root);

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    void CreateLayout();
    void OnBranchSelected(wxCommandEvent& event);
    void OnSearch(wxCommandEvent& event);
    void OnItemActivated(wxListEvent& event);
    void OnItemRightClicked(wxListEvent& event);

    core::EventBus& event_bus_;
    std::string workspace_root_;

    wxChoice* branch_choice_{nullptr};
    wxSearchCtrl* search_box_{nullptr};
    wxListCtrl* log_list_{nullptr};
};

} // namespace markamp::ui
