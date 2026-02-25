#pragma once

#include "SidebarSection.h"
#include "ThemeAwareWindow.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/FileNode.h"
#include "core/ThemeEngine.h"

#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/srchctrl.h>

#include <functional>

namespace markamp::ui
{

class FileTreeCtrl;
class EmptyPanelState;
class SidebarFooter;

class ExplorerPanel : public ThemeAwareWindow
{
public:
    ExplorerPanel(wxWindow* parent,
                  core::ThemeEngine& theme_engine,
                  core::Config* config,
                  DesignSystemContext& ds,
                  IconManager& icon_manager);
    ~ExplorerPanel() override;

    FileTreeCtrl* GetFileTree() const
    {
        return file_tree_;
    }
    wxSearchCtrl* GetSearchField() const
    {
        return search_field_;
    }

    void SetOnFileOpen(std::function<void(const core::FileNode&)> callback)
    {
        on_file_open_ = std::move(callback);
    }

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    core::Subscription action_sub_;

    void CreateLayout(DesignSystemContext& ds, IconManager& icon_manager);

    core::EventBus& event_bus_;
    core::Config* config_{nullptr};
    std::function<void(const core::FileNode&)> on_file_open_;

    SidebarSection* open_editors_section_{nullptr};
    SidebarSection* folders_section_{nullptr};
    SidebarSection* outline_section_{nullptr};
    SidebarSection* timeline_section_{nullptr};
    EmptyPanelState* empty_state_{nullptr};

    FileTreeCtrl* file_tree_{nullptr};
    wxSearchCtrl* search_field_{nullptr};
    SidebarFooter* footer_{nullptr};
};

} // namespace markamp::ui
