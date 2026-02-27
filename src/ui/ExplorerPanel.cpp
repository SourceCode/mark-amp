#include "ExplorerPanel.h"

#include "EmptyPanelState.h"
#include "FileTreeCtrl.h"
#include "PanelHeader.h"
#include "SidebarFooter.h"
#include "core/Events.h"
#include "ui/DesignSystemContext.h"
#include "ui/ExplorerToolbar.h"
#include "ui/IconManager.h"
#include "ui/OpenEditorsSection.h"

#include <wx/stattext.h>

namespace markamp::ui
{

ExplorerPanel::ExplorerPanel(wxWindow* parent,
                             core::ThemeEngine& theme_engine,
                             core::EventBus& event_bus,
                             core::Config* config,
                             DesignSystemContext& ds,
                             IconManager& icon_manager,
                             const std::string& persistence_id)
    : ThemeAwareWindow(
          parent, theme_engine, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
    , event_bus_(event_bus)
    , config_(config)
    , persistence_id_(persistence_id)
{
    SetBackgroundColour(theme_engine.color(core::ThemeColorToken::BgPanel));
    CreateLayout(ds, icon_manager);

    if (config_ && file_tree_)
    {
        std::string key = "sidebar." + (persistence_id_.empty() ? "" : persistence_id_ + ".") +
                          "explorer.state.scroll_y";
        int scroll_y = config_->get_int(key, 0);
        file_tree_->SetScrollOffset(scroll_y);
    }
}

ExplorerPanel::~ExplorerPanel()
{
    if (config_ && file_tree_)
    {
        std::string key = "sidebar." + (persistence_id_.empty() ? "" : persistence_id_ + ".") +
                          "explorer.state.scroll_y";
        config_->set(key, file_tree_->GetScrollOffset());
        auto result = config_->save();
        if (!result)
        {
            // Log error or handle failure
        }
    }
}

void ExplorerPanel::CreateLayout(DesignSystemContext& ds, IconManager& icon_manager)
{
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    // 1. Panel Header
    auto* header = new PanelHeader(this, ds, icon_manager, event_bus_);
    header->set_title("EXPLORER");
    header->set_panel_id("explorer");
    header->set_display_mode(PanelHeaderMode::kBreadcrumb);

    // Task 8 contextual icons (partially set up here, full logic later)
    std::vector<PanelHeader::ActionIcon> actions = {
        {"explorer.refresh", "refresh", "Refresh Explorer"},
        {"explorer.collapse", "panel-collapse", "Collapse Folders"}};
    header->set_actions(actions);
    main_sizer->Add(header, 0, wxEXPAND);

    action_sub_ = event_bus_.subscribe<core::events::PanelHeaderActionEvent>(
        [this](const core::events::PanelHeaderActionEvent& evt)
        {
            if (evt.action_id == "explorer.collapse")
            {
                if (file_tree_)
                {
                    file_tree_->CollapseAllNodes();
                }
            }
            // "explorer.refresh" would be handled via LayoutManager or a global refresh event
        });

    // 2. Sections Container (Scrollable or just expanded?)
    // Usually Explorer is a vertical list of sections
    auto* sections_sizer = new wxBoxSizer(wxVERTICAL);

    std::string prefix = persistence_id_.empty() ? "" : persistence_id_ + "_";

    // "Open Editors"
    open_editors_section_ = new OpenEditorsSection(this, ds, icon_manager, event_bus_, config_);
    open_editors_section_->set_expanded(false); // Collapsed by default

    // "Folders" (Workspace)
    folders_section_ = new SidebarSection(
        this, ds, icon_manager, event_bus_, config_, "WORKSPACE", prefix + "folders");

    // We'll use a container panel so we can swap between the tree and the empty state
    auto* workspace_container = new wxPanel(folders_section_, wxID_ANY);
    workspace_container->SetBackgroundColour(GetBackgroundColour());
    auto* workspace_sizer = new wxBoxSizer(wxVERTICAL);
    workspace_container->SetSizer(workspace_sizer);

    // Explorer Toolbar (Task 3)
    auto* explorer_toolbar =
        new ExplorerToolbar(folders_section_->get_header_window(), theme_engine(), icon_manager);
    explorer_toolbar->SetActionCallback(
        [this](ExplorerToolbar::Action action)
        {
            if (action == ExplorerToolbar::Action::kCollapseAll && file_tree_)
            {
                file_tree_->CollapseAllNodes();
            }
            // Other actions: kNewFile, kNewFolder, kRefresh, kFilter
            else if (action == ExplorerToolbar::Action::kNewFile)
            {
                if (file_tree_)
                    file_tree_->CreateNewFile();
            }
            else if (action == ExplorerToolbar::Action::kNewFolder)
            {
                if (file_tree_)
                    file_tree_->CreateNewFolder();
            }
            else if (action == ExplorerToolbar::Action::kRefresh)
            {
                core::events::WorkspaceRefreshRequestEvent evt;
                event_bus_.publish(evt);
            }
            // kFilter would toggle search_field_ or something similar
        });

    folders_section_->add_header_widget(explorer_toolbar);

    file_tree_ = new FileTreeCtrl(workspace_container, theme_engine(), event_bus_, icon_registry_);
    workspace_sizer->Add(file_tree_, 1, wxEXPAND);

    empty_state_ = new EmptyPanelState(workspace_container, ds, icon_manager);
    empty_state_->set_icon("open-folder");
    empty_state_->set_message("You have not yet opened a folder.");
    empty_state_->set_action("Open Folder",
                             [this]
                             {
                                 // Dispatch open folder event
                                 core::events::WorkspaceOpenRequestEvent evt;
                                 event_bus_.publish(evt);
                             });
    workspace_sizer->Add(empty_state_, 1, wxEXPAND);

    // Initial state: show empty state if no workspace open
    // For now, assume file tree is active but you can easily toggle:
    // empty_state_->Show(); file_tree_->Hide(); workspace_container->Layout();
    empty_state_->Hide();

    folders_section_->set_content(workspace_container);
    folders_section_->set_expanded(true);

    // Outline
    outline_section_ = new SidebarSection(
        this, ds, icon_manager, event_bus_, config_, "OUTLINE", prefix + "outline");
    auto* empty_outline = new wxPanel(outline_section_);
    empty_outline->SetSizer(new wxBoxSizer(wxVERTICAL));
    outline_section_->set_content(empty_outline);
    outline_section_->set_expanded(false);

    // Timeline
    timeline_section_ = new SidebarSection(
        this, ds, icon_manager, event_bus_, config_, "TIMELINE", prefix + "timeline");
    auto* empty_timeline = new wxPanel(timeline_section_);
    empty_timeline->SetSizer(new wxBoxSizer(wxVERTICAL));
    timeline_section_->set_content(empty_timeline);
    timeline_section_->set_expanded(false);

    // Assemble sections
    sections_sizer->Add(open_editors_section_, 0, wxEXPAND);
    sections_sizer->Add(folders_section_, 1, wxEXPAND); // Flexible height
    sections_sizer->Add(outline_section_, 0, wxEXPAND);
    sections_sizer->Add(timeline_section_, 0, wxEXPAND);

    main_sizer->Add(sections_sizer, 1, wxEXPAND);

    // Footer
    footer_ = new SidebarFooter(this, ds, event_bus_);
    footer_->set_text("0 items selected");
    main_sizer->Add(footer_, 0, wxEXPAND);

    SetSizer(main_sizer);
}

void ExplorerPanel::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgPanel));
}

} // namespace markamp::ui
