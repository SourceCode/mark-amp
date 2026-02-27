#include "SourceControlPanel.h"

#include "EmptyPanelState.h"
#include "PanelHeader.h"
#include "SidebarFooter.h"
#include "core/Events.h"
#include "ui/DesignSystemContext.h"
#include "ui/IconManager.h"

#include <wx/stattext.h>

#include <filesystem>
#include <fstream>

namespace markamp::ui
{

SourceControlPanel::SourceControlPanel(wxWindow* parent,
                                       core::ThemeEngine& theme_engine,
                                       core::EventBus& event_bus,
                                       core::Config* config,
                                       DesignSystemContext& design_system,
                                       IconManager& icon_manager,
                                       std::string workspace_root,
                                       std::string persistence_id)
    : ThemeAwareWindow(
          parent, theme_engine, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
    , event_bus_(event_bus)
    , config_(config)
    , persistence_id_(std::move(persistence_id))
    , workspace_root_(std::move(workspace_root)) // Initialize workspace_root_
    // Initialize status provider
    , git_provider_(std::make_unique<ui::GitStatusProvider>())
{
    SetBackgroundColour(theme_engine.color(core::ThemeColorToken::BgPanel));
    git_provider_->SetWorkspaceRoot(workspace_root_);
    CreateLayout(design_system, icon_manager);
    RefreshStatus();
}

SourceControlPanel::~SourceControlPanel() = default;

void SourceControlPanel::CreateLayout(DesignSystemContext& design_system, IconManager& icon_manager)
{
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    // 1. Panel Header
    auto* header = new PanelHeader(this, design_system, icon_manager, event_bus_);
    header->set_title("SOURCE CONTROL");
    header->set_panel_id("scm");
    header->set_display_mode(PanelHeaderMode::kBreadcrumb);

    const std::vector<PanelHeader::ActionIcon> actions = {
        {"scm.refresh", "refresh", "Refresh Source Control"}, {"scm.commit", "check", "Commit"}};
    header->set_actions(actions);
    main_sizer->Add(header, 0, wxEXPAND);

    action_sub_ = event_bus_.subscribe<core::events::PanelHeaderActionEvent>(
        [this](const core::events::PanelHeaderActionEvent& evt)
        {
            if (evt.action_id == "scm.refresh")
            {
                RefreshStatus();
            }
        });

    // 2. Sections Container
    auto* sections_sizer = new wxBoxSizer(wxVERTICAL);
    const std::string prefix = persistence_id_.empty() ? "scm_" : persistence_id_ + "_scm_";

    // 3. Commit Input area
    auto* commit_panel = new wxPanel(this);
    commit_panel->SetBackgroundColour(GetBackgroundColour());
    auto* commit_sizer = new wxBoxSizer(wxVERTICAL);

    commit_message_input_ = new wxTextCtrl(
        commit_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    commit_message_input_->SetHint("Message (Cmd+Enter to commit)");
    commit_sizer->Add(commit_message_input_, 0, wxEXPAND | wxALL, 8);

    commit_button_ = new wxButton(commit_panel, wxID_ANY, "Commit");
    commit_button_->Bind(wxEVT_BUTTON,
                         [](wxCommandEvent&)
                         {
                             // Implement commit logic later
                         });
    commit_sizer->Add(commit_button_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    commit_panel->SetSizer(commit_sizer);
    sections_sizer->Add(commit_panel, 0, wxEXPAND);

    // 4. Staged Changes Section
    staged_changes_section_ = new SidebarSection(this,
                                                 design_system,
                                                 icon_manager,
                                                 event_bus_,
                                                 config_,
                                                 "STAGED CHANGES",
                                                 prefix + "staged");
    staged_list_ = new wxListCtrl(staged_changes_section_,
                                  wxID_ANY,
                                  wxDefaultPosition,
                                  wxDefaultSize,
                                  wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_NONE | wxLC_NO_HEADER);
    staged_list_->InsertColumn(0, "File", wxLIST_FORMAT_LEFT, 200);
    staged_list_->InsertColumn(1, "Status", wxLIST_FORMAT_RIGHT, 40);
    staged_list_->SetBackgroundColour(GetBackgroundColour());
    staged_list_->Bind(wxEVT_LIST_ITEM_ACTIVATED, &SourceControlPanel::OnItemActivated, this);
    staged_changes_section_->set_content(staged_list_);
    staged_changes_section_->set_expanded(true);
    sections_sizer->Add(staged_changes_section_, 1, wxEXPAND); // Flex height based on layout

    // 5. All Changes Section
    all_changes_section_ = new SidebarSection(
        this, design_system, icon_manager, event_bus_, config_, "CHANGES", prefix + "changes");
    changes_list_ = new wxListCtrl(all_changes_section_,
                                   wxID_ANY,
                                   wxDefaultPosition,
                                   wxDefaultSize,
                                   wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_NONE | wxLC_NO_HEADER);
    changes_list_->InsertColumn(0, "File", wxLIST_FORMAT_LEFT, 200);
    changes_list_->InsertColumn(1, "Status", wxLIST_FORMAT_RIGHT, 40);
    changes_list_->SetBackgroundColour(GetBackgroundColour());
    changes_list_->Bind(wxEVT_LIST_ITEM_ACTIVATED, &SourceControlPanel::OnItemActivated, this);
    all_changes_section_->set_content(changes_list_);
    all_changes_section_->set_expanded(true);
    sections_sizer->Add(all_changes_section_, 1, wxEXPAND);

    // 6. Timeline Section (Task 13 area)
    timeline_section_ = new SidebarSection(
        this, design_system, icon_manager, event_bus_, config_, "TIMELINE", prefix + "timeline");
    auto* empty_timeline = new wxPanel(timeline_section_);
    empty_timeline->SetSizer(new wxBoxSizer(wxVERTICAL));
    timeline_section_->set_content(empty_timeline);
    timeline_section_->set_expanded(false);
    sections_sizer->Add(timeline_section_, 0, wxEXPAND);

    main_sizer->Add(sections_sizer, 1, wxEXPAND);

    // 7. Footer
    footer_ = new SidebarFooter(this, design_system, event_bus_);
    footer_->set_text("0 changed files");
    main_sizer->Add(footer_, 0, wxEXPAND);

    SetSizer(main_sizer);
}

void SourceControlPanel::RefreshStatus()
{
    if (git_provider_)
    {
        // RunGitStatus is private in GitStatusProvider right now, we need to either make it public
        // or see if there's a public update method. Let's just call something that exists or we'll
        // have to add it. Wait, GetChanges() triggers a run if empty usually? No. Let's modify
        // GitStatusProvider to have a public Refresh() or we can just access it. Ah, looking at
        // GitStatusProvider.h, maybe RunGitStatus() is private? Yes. We need to add a public
        // Refresh method or make RunGitStatus public. Let's assume it should have a public Refresh
        // method. For now, I'll bypass the private error by fixing GitStatusProvider.h in a
        // separate step.
        git_provider_->RunGitStatus();
        UpdateDynamicUI();
    }
}

void SourceControlPanel::UpdateDynamicUI()
{
    if (!git_provider_)
        return;

    auto changes = git_provider_->GetChanges();

    if (staged_list_)
        staged_list_->DeleteAllItems();
    if (changes_list_)
        changes_list_->DeleteAllItems();

    int staged_count = 0;
    int unstaged_count = 0;

    auto format_status = [](core::GitChangeStatus status) -> wxString
    {
        switch (status)
        {
            case core::GitChangeStatus::Modified:
                return "M";
            case core::GitChangeStatus::Added:
                return "A";
            case core::GitChangeStatus::Deleted:
                return "D";
            case core::GitChangeStatus::Renamed:
                return "R";
            case core::GitChangeStatus::Copied:
                return "C";
            case core::GitChangeStatus::Untracked:
                return "U";
            default:
                return "";
        }
    };

    auto get_status_color = [this](core::GitChangeStatus status) -> wxColour
    {
        switch (status)
        {
            case core::GitChangeStatus::Modified:
            case core::GitChangeStatus::Renamed:
            case core::GitChangeStatus::Copied:
                return {242, 201, 76}; // Yellow/Orange
            case core::GitChangeStatus::Added:
            case core::GitChangeStatus::Untracked:
                return {115, 201, 145}; // Green
            case core::GitChangeStatus::Deleted:
                return {235, 87, 87}; // Red
            default:
                return theme_engine().color(core::ThemeColorToken::TextMain);
        }
    };

    for (const auto& change : changes)
    {
        if (change.index_status != core::GitChangeStatus::None && staged_list_ != nullptr)
        {
            long row = staged_list_->InsertItem(staged_list_->GetItemCount(), change.path);
            staged_list_->SetItem(row, 1, format_status(change.index_status));
            staged_list_->SetItemTextColour(row, get_status_color(change.index_status));
            // Store index as custom data for click handling
            staged_list_->SetItemData(row, staged_count);
            staged_count++;
        }

        if (change.working_status != core::GitChangeStatus::None && changes_list_ != nullptr)
        {
            long row = changes_list_->InsertItem(changes_list_->GetItemCount(), change.path);
            changes_list_->SetItem(row, 1, format_status(change.working_status));
            changes_list_->SetItemTextColour(row, get_status_color(change.working_status));
            // Store index as custom data
            changes_list_->SetItemData(row, unstaged_count);
            unstaged_count++;
        }
    }

    if (staged_changes_section_ != nullptr)
    {
        staged_changes_section_->set_title(
            wxString::Format("STAGED CHANGES (%d)", staged_count).ToStdString());
    }
    if (all_changes_section_ != nullptr)
    {
        all_changes_section_->set_title(
            wxString::Format("CHANGES (%d)", unstaged_count).ToStdString());
    }

    if (footer_ != nullptr)
    {
        const int total = staged_count + unstaged_count;
        const std::string text = std::to_string(total) + " pending change(s)";
        footer_->set_text(text);
    }
}

void SourceControlPanel::OnItemActivated(wxListEvent& event)
{
    // Task 4: Diff Navigation
    auto* list_ctrl = dynamic_cast<wxListCtrl*>(event.GetEventObject());
    if (list_ctrl == nullptr || git_provider_ == nullptr)
        return;

    wxString file_path = list_ctrl->GetItemText(event.GetIndex());
    std::string path_str = file_path.ToStdString();

    // Convert to absolute path for the right_path (working tree)
    std::string full_path = (std::filesystem::path(workspace_root_) / path_str).string();

    bool activated_staged = (list_ctrl == staged_list_);

    core::GitCommandRunner runner{workspace_root_};

    core::events::OpenDiffRequestEvent diff_evt;
    diff_evt.left_path = full_path;
    diff_evt.right_path = full_path;

    if (activated_staged)
    {
        diff_evt.title = path_str + " (Staged vs HEAD)";
        diff_evt.left_content = runner.GetFileContentAtHEAD(path_str);
        diff_evt.right_content = runner.GetFileContentFromIndex(path_str);
    }
    else
    {
        diff_evt.title = path_str + " (Working vs Staged)";
        diff_evt.left_content = runner.GetFileContentFromIndex(path_str);

        // Read right content from disk
        std::ifstream file_stream(full_path);
        if (file_stream.is_open())
        {
            diff_evt.right_content.assign(std::istreambuf_iterator<char>(file_stream),
                                          std::istreambuf_iterator<char>());
        }
        else
        {
            // Fallback to empty context if file doesn't exist (e.g., deleted in working tree)
            diff_evt.right_content = "";
        }
    }

    event_bus_.publish(diff_evt);
}

void SourceControlPanel::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgPanel));
    // Additional coloring logic if needed
}

} // namespace markamp::ui
