#pragma once

#include "core/EventBus.h"
#include "core/Events.h"
#include "ui/SidebarSection.h"

#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>

#include <string>
#include <vector>

namespace markamp::core
{
class Config;
}

namespace markamp::ui
{

class IconManager;

/// A sidebar section that lists currently open editors/tabs
class OpenEditorsSection : public SidebarSection
{
public:
    OpenEditorsSection(wxWindow* parent,
                       DesignSystemContext& ds,
                       IconManager& icon_manager,
                       core::EventBus& event_bus,
                       core::Config* config);

    ~OpenEditorsSection() override;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    void RefreshEditorsList();
    void BuildEditorsList();

    wxScrolledWindow* scroll_container_{nullptr};
    wxBoxSizer* list_sizer_{nullptr};

    core::EventBus& event_bus_;
    std::vector<core::Subscription> subscriptions_;

    struct EditorItem
    {
        std::string tab_id;
        std::string filename;
        std::string path;
        bool is_active{false};
        bool is_dirty{false};
    };

    std::vector<EditorItem> open_editors_;

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui
