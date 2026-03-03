#include "SourceControlPanel.h"

#include "EmptyPanelState.h"
#include "PanelHeader.h"
#include "SidebarFooter.h"
#include "core/Events.h"
#include "ui/DesignSystemContext.h"
#include "ui/IconManager.h"

#include <wx/clipbrd.h>
#include <wx/filename.h>
#include <wx/menu.h>
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
    git_provider_->SetEventBus(&event_bus_);
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
        {"scm.refresh", "refresh", "Refresh Source Control"},
        {"scm.commit", "check", "Commit"},
        {"scm.sync", "sync", "Sync (Pull & Push)"},
        {"scm.log", "history", "View Git Log"},
        {"scm.more", "ellipsis", "More Actions"}};
    header->set_actions(actions);
    main_sizer->Add(header, 0, wxEXPAND);

    action_sub_ = event_bus_.subscribe<core::events::PanelHeaderActionEvent>(
        [this](const core::events::PanelHeaderActionEvent& evt)
        {
            if (evt.action_id == "scm.refresh")
            {
                RefreshStatus();
            }
            else if (evt.action_id == "scm.commit")
            {
                wxCommandEvent dummy;
                OnCommitButtonClicked(dummy);
            }
            else if (evt.action_id == "scm.sync")
            {
                core::GitCommandRunner runner(workspace_root_);
                runner.Pull();
                runner.Push();
                RefreshStatus();
            }
            else if (evt.action_id == "scm.log")
            {
                core::events::OpenGitLogRequestEvent req;
                req.workspace_root = workspace_root_;
                event_bus_.publish(req);
            }
            else if (evt.action_id == "scm.more")
            {
                wxMenu menu;
                menu.Append(2001, "Stash Changes");
                menu.Append(2002, "Pop Stash");

                menu.Bind(wxEVT_MENU,
                          [this](wxCommandEvent& menu_evt)
                          {
                              core::GitCommandRunner runner(workspace_root_);
                              int id = menu_evt.GetId();
                              if (id == 2001)
                              {
                                  runner.Stash("Stashed by MarkAmp UI");
                              }
                              else if (id == 2002)
                              {
                                  runner.StashPop();
                              }
                              RefreshStatus();
                          });

                PopupMenu(&menu);
            }
        });

    app_activate_sub_ = event_bus_.subscribe<core::events::AppActivatedEvent>(
        [this](const core::events::AppActivatedEvent& /*evt*/)
        {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_refresh_time_)
                    .count() > 1000)
            {
                last_refresh_time_ = now;
                RefreshStatus();
            }
        });

    file_saved_sub_ = event_bus_.subscribe<core::events::FileSavedEvent>(
        [this](const core::events::FileSavedEvent& /*evt*/)
        {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_refresh_time_)
                    .count() > 500)
            {
                last_refresh_time_ = now;
                RefreshStatus();
            }
        });

    // 2. Sections Container
    auto* sections_sizer = new wxBoxSizer(wxVERTICAL);
    const std::string prefix = persistence_id_.empty() ? "scm_" : persistence_id_ + "_scm_";

    // 3. Commit Input area
    commit_panel_ = new wxPanel(this);
    commit_panel_->SetBackgroundColour(GetBackgroundColour());
    auto* commit_sizer = new wxBoxSizer(wxVERTICAL);

    branch_choice_ = new wxChoice(commit_panel_, wxID_ANY);
    branch_choice_->SetName("Branch Selector");
    branch_choice_->SetHelpText("Select the branch to work on");
    branch_choice_->Bind(wxEVT_CHOICE, &SourceControlPanel::OnBranchSelected, this);
    commit_sizer->Add(branch_choice_, 0, wxEXPAND | wxALL, 8);

    commit_message_input_ = new wxTextCtrl(
        commit_panel_, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    commit_message_input_->SetHint("Message (Cmd+Enter to commit)");
    commit_message_input_->SetName("Commit Message Input");
    commit_message_input_->SetHelpText("Enter the message for your commit here");
    commit_sizer->Add(commit_message_input_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    auto* buttons_sizer = new wxBoxSizer(wxHORIZONTAL);

    template_button_ = new wxButton(commit_panel_, wxID_ANY, "Template");
    template_button_->SetToolTip("Insert Commit Message Template");
    template_button_->SetName("Template Button");
    template_button_->SetHelpText("Insert a conventional commit template");
    template_button_->Bind(wxEVT_BUTTON, &SourceControlPanel::OnTemplateButtonClicked, this);
    buttons_sizer->Add(template_button_, 0, wxRIGHT, 8);

    commit_button_ = new wxButton(commit_panel_, wxID_ANY, "Commit");
    commit_button_->SetName("Commit Button");
    commit_button_->SetHelpText("Commit your staged changes");
    commit_button_->Bind(wxEVT_BUTTON, &SourceControlPanel::OnCommitButtonClicked, this);
    buttons_sizer->Add(commit_button_, 1, wxEXPAND, 0);

    commit_sizer->Add(buttons_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    commit_panel_->SetSizer(commit_sizer);
    sections_sizer->Add(commit_panel_, 0, wxEXPAND);

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
    staged_list_->InsertColumn(0, "File", wxLIST_FORMAT_LEFT, 180);
    staged_list_->InsertColumn(1, "Status", wxLIST_FORMAT_RIGHT, 30);
    staged_list_->InsertColumn(2, "Stats", wxLIST_FORMAT_RIGHT, 50);
    staged_list_->SetBackgroundColour(GetBackgroundColour());
    staged_list_->Bind(wxEVT_LIST_ITEM_ACTIVATED, &SourceControlPanel::OnItemActivated, this);
    staged_list_->SetName("Staged Changes List");
    staged_list_->SetHelpText("Files ready to be committed");
    staged_changes_section_->set_content(staged_list_);
    staged_changes_section_->set_expanded(true);
    sections_sizer->Add(staged_changes_section_, 1, wxEXPAND); // Flex height based on layout

    // 4b. Merge Changes Section (shown only when unmerged files exist)
    merge_changes_section_ = new SidebarSection(
        this, design_system, icon_manager, event_bus_, config_, "MERGE CHANGES", prefix + "merge");
    merge_list_ = new wxListCtrl(merge_changes_section_,
                                 wxID_ANY,
                                 wxDefaultPosition,
                                 wxDefaultSize,
                                 wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_NONE | wxLC_NO_HEADER);
    merge_list_->InsertColumn(0, "File", wxLIST_FORMAT_LEFT, 210);
    merge_list_->InsertColumn(1, "Status", wxLIST_FORMAT_RIGHT, 20);
    merge_list_->SetBackgroundColour(GetBackgroundColour());
    merge_list_->Bind(
        wxEVT_LIST_ITEM_RIGHT_CLICK, &SourceControlPanel::OnMergeItemRightClicked, this);
    merge_list_->SetName("Merge Conflicts List");
    merge_list_->SetHelpText("Files with merge conflicts requiring resolution");
    merge_changes_section_->set_content(merge_list_);
    merge_changes_section_->set_expanded(true);
    merge_changes_section_->Show(false); // Hidden until conflicts detected
    sections_sizer->Add(merge_changes_section_, 0, wxEXPAND);

    // 5. All Changes Section
    all_changes_section_ = new SidebarSection(
        this, design_system, icon_manager, event_bus_, config_, "CHANGES", prefix + "changes");
    changes_list_ = new wxListCtrl(all_changes_section_,
                                   wxID_ANY,
                                   wxDefaultPosition,
                                   wxDefaultSize,
                                   wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_NONE | wxLC_NO_HEADER);
    changes_list_->InsertColumn(0, "File", wxLIST_FORMAT_LEFT, 180);
    changes_list_->InsertColumn(1, "Status", wxLIST_FORMAT_RIGHT, 30);
    changes_list_->InsertColumn(2, "Stats", wxLIST_FORMAT_RIGHT, 50);
    changes_list_->SetBackgroundColour(GetBackgroundColour());
    changes_list_->Bind(wxEVT_LIST_ITEM_ACTIVATED, &SourceControlPanel::OnItemActivated, this);
    changes_list_->SetName("Unstaged Changes List");
    changes_list_->SetHelpText("Modified workspace files");
    all_changes_section_->set_content(changes_list_);
    all_changes_section_->set_expanded(true);
    sections_sizer->Add(all_changes_section_, 1, wxEXPAND);

    // 6. Timeline Section (Task 13 area)
    timeline_section_ = new SidebarSection(
        this, design_system, icon_manager, event_bus_, config_, "TIMELINE", prefix + "timeline");
    timeline_list_ = new wxListCtrl(timeline_section_,
                                    wxID_ANY,
                                    wxDefaultPosition,
                                    wxDefaultSize,
                                    wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_NONE | wxLC_NO_HEADER);
    timeline_list_->InsertColumn(0, "Message", wxLIST_FORMAT_LEFT, 200);
    timeline_list_->InsertColumn(1, "Date", wxLIST_FORMAT_RIGHT, 60);
    timeline_list_->SetBackgroundColour(GetBackgroundColour());
    timeline_list_->SetName("Timeline List");
    timeline_list_->SetHelpText("Recent commits history");
    timeline_section_->set_content(timeline_list_);
    timeline_section_->set_expanded(false);
    sections_sizer->Add(timeline_section_, 0, wxEXPAND);

    main_sizer->Add(sections_sizer, 1, wxEXPAND);

    // 6b. Empty State
    empty_state_ = new EmptyPanelState(this, design_system, icon_manager);
    empty_state_->set_icon("git-branch");
    empty_state_->set_message("No Source Control Providers Registered");
    empty_state_->set_action("Initialize Repository",
                             [this]()
                             {
                                 core::GitCommandRunner runner(workspace_root_);
                                 runner.RunSync("init");
                                 RefreshStatus();
                             });
    main_sizer->Add(empty_state_, 1, wxEXPAND | wxALL, 16);
    empty_state_->Hide();

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

    core::GitCommandRunner runner(workspace_root_);
    bool is_git = runner.RunSync("rev-parse --is-inside-work-tree").success();

    if (!is_git)
    {
        if (empty_state_)
            empty_state_->Show(true);
        if (commit_panel_)
            commit_panel_->Show(false);
        if (staged_changes_section_)
            staged_changes_section_->Show(false);
        if (all_changes_section_)
            all_changes_section_->Show(false);
        if (timeline_section_)
            timeline_section_->Show(false);
        Layout();
        if (footer_ != nullptr)
            footer_->set_text("");
        return;
    }

    if (empty_state_)
        empty_state_->Show(false);
    if (commit_panel_)
        commit_panel_->Show(true);
    if (staged_changes_section_)
        staged_changes_section_->Show(true);
    if (all_changes_section_)
        all_changes_section_->Show(true);
    if (timeline_section_)
        timeline_section_->Show(true);

    auto branches = runner.GetBranches();
    auto current_branch = runner.GetBranch();

    if (branch_choice_ != nullptr && !branches.empty())
    {
        wxArrayString current_strings = branch_choice_->GetStrings();
        bool needs_update = current_strings.GetCount() != branches.size();
        if (!needs_update)
        {
            for (size_t i = 0; i < branches.size(); ++i)
            {
                if (current_strings[i].ToStdString() != branches[i])
                {
                    needs_update = true;
                    break;
                }
            }
        }

        if (needs_update)
        {
            branch_choice_->Clear();
            for (const auto& b : branches)
            {
                branch_choice_->Append(b);
            }
        }
        branch_choice_->SetStringSelection(current_branch);
    }

    auto changes = git_provider_->GetChanges();

    if (staged_list_)
        staged_list_->DeleteAllItems();
    if (merge_list_)
        merge_list_->DeleteAllItems();
    if (changes_list_)
        changes_list_->DeleteAllItems();

    int staged_count = 0;
    int merge_count = 0;
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
        // Task 10: Detect unmerged (conflicted) files and display in merge section
        bool is_conflict = (change.index_status == core::GitChangeStatus::Unmerged ||
                            change.working_status == core::GitChangeStatus::Unmerged);

        if (is_conflict && merge_list_ != nullptr)
        {
            long row = merge_list_->InsertItem(merge_list_->GetItemCount(), change.path);
            merge_list_->SetItem(row, 1, "C");
            merge_list_->SetItemTextColour(row, {235, 87, 87}); // Red for conflicts
            merge_list_->SetItemData(row, merge_count);
            merge_count++;
            continue; // Conflicts go only in the merge section
        }

        if (change.index_status != core::GitChangeStatus::None && staged_list_ != nullptr)
        {
            long row = staged_list_->InsertItem(staged_list_->GetItemCount(), change.path);
            staged_list_->SetItem(row, 1, format_status(change.index_status));
            staged_list_->SetItemTextColour(row, get_status_color(change.index_status));
            if (change.staged_additions > 0 || change.staged_deletions > 0)
            {
                staged_list_->SetItem(row,
                                      2,
                                      "+" + std::to_string(change.staged_additions) + " -" +
                                          std::to_string(change.staged_deletions));
            }
            // Store index as custom data for click handling
            staged_list_->SetItemData(row, staged_count);
            staged_count++;
        }

        if (change.working_status != core::GitChangeStatus::None && changes_list_ != nullptr)
        {
            long row = changes_list_->InsertItem(changes_list_->GetItemCount(), change.path);
            changes_list_->SetItem(row, 1, format_status(change.working_status));
            changes_list_->SetItemTextColour(row, get_status_color(change.working_status));
            if (change.unstaged_additions > 0 || change.unstaged_deletions > 0)
            {
                changes_list_->SetItem(row,
                                       2,
                                       "+" + std::to_string(change.unstaged_additions) + " -" +
                                           std::to_string(change.unstaged_deletions));
            }
            // Store index map for click handling (offset by staged list size for unique IDs if
            // needed)
            changes_list_->SetItemData(row, unstaged_count);
            unstaged_count++;
        }
    }

    if (staged_changes_section_ != nullptr)
    {
        staged_changes_section_->set_title(
            wxString::Format("STAGED CHANGES (%d)", staged_count).ToStdString());
    }

    // Task 10: Show/hide merge changes section
    if (merge_changes_section_ != nullptr)
    {
        merge_changes_section_->Show(merge_count > 0);
        if (merge_count > 0)
        {
            merge_changes_section_->set_title(
                wxString::Format("MERGE CHANGES (%d)", merge_count).ToStdString());
        }
    }

    if (all_changes_section_ != nullptr)
    {
        all_changes_section_->set_title(
            wxString::Format("CHANGES (%d)", unstaged_count).ToStdString());
    }

    if (footer_ != nullptr)
    {
        const int total = staged_count + merge_count + unstaged_count;
        std::string text = std::to_string(total) + " pending change(s)";
        if (merge_count > 0)
        {
            text += " \xE2\x80\x94 " + std::to_string(merge_count) + " conflict(s)";
        }
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
    auto bg = theme_engine().color(core::ThemeColorToken::BgPanel);
    auto fg = theme_engine().color(core::ThemeColorToken::TextMain);
    SetBackgroundColour(bg);

    if (staged_list_)
    {
        staged_list_->SetBackgroundColour(bg);
        staged_list_->SetTextColour(fg);
    }
    if (changes_list_)
    {
        changes_list_->SetBackgroundColour(bg);
        changes_list_->SetTextColour(fg);
    }
    if (merge_list_)
    {
        merge_list_->SetBackgroundColour(bg);
        merge_list_->SetTextColour(fg);
    }
    if (timeline_list_)
    {
        timeline_list_->SetBackgroundColour(bg);
        timeline_list_->SetTextColour(fg);
    }
    if (commit_message_input_)
    {
        commit_message_input_->SetBackgroundColour(bg);
        commit_message_input_->SetForegroundColour(fg);
    }
    if (commit_panel_)
    {
        commit_panel_->SetBackgroundColour(bg);
    }

    Refresh();
}

void SourceControlPanel::OnBranchSelected(wxCommandEvent& /*event*/)
{
    int sel = branch_choice_->GetSelection();
    if (sel != wxNOT_FOUND)
    {
        std::string branch =
            branch_choice_->GetString(static_cast<unsigned int>(sel)).ToStdString();
        core::GitCommandRunner runner(workspace_root_);
        runner.SwitchBranch(branch);
        RefreshStatus();
    }
}

void SourceControlPanel::OnCommitButtonClicked(wxCommandEvent& /*event*/)
{
    std::string msg = commit_message_input_->GetValue().ToStdString();
    if (!msg.empty())
    {
        core::GitCommandRunner runner(workspace_root_);
        runner.Commit(msg);
        commit_message_input_->Clear();
        RefreshStatus();
    }
}

void SourceControlPanel::OnItemRightClicked(wxListEvent& event)
{
    auto* list = wxDynamicCast(event.GetEventObject(), wxListCtrl);
    if (!list || !git_provider_)
        return;

    long item_idx = event.GetIndex();
    std::string file_path = list->GetItemText(item_idx).ToStdString();
    bool is_staged = (list == staged_list_);

    wxMenu menu;
    menu.Append(3004, "Open File");
    menu.Append(3005, "Open Changes (Diff)");
    menu.AppendSeparator();

    if (is_staged)
    {
        menu.Append(3001, "Unstage Changes");
    }
    else
    {
        menu.Append(3002, "Stage Changes");
        menu.Append(3003, "Discard Changes");
        // Could also add Accept Current etc for Unmerged files here
    }

    menu.AppendSeparator();
    menu.Append(3006, "Stash File");
    menu.AppendSeparator();
    menu.Append(3007, "Copy Path");
    menu.Append(3008, "Copy Relative Path");
    menu.AppendSeparator();

#if defined(__WXOSX__)
    menu.Append(3009, "Reveal in Finder");
#else
    menu.Append(3009, "Reveal in Explorer");
#endif

    menu.Bind(wxEVT_MENU,
              [this, file_path, is_staged](wxCommandEvent& menu_evt)
              {
                  core::GitCommandRunner runner(workspace_root_);
                  int id = menu_evt.GetId();
                  if (id == 3001)
                  {
                      runner.Unstage(file_path);
                  }
                  else if (id == 3002)
                  {
                      runner.Stage(file_path);
                  }
                  else if (id == 3003)
                  {
                      runner.Discard(file_path);
                  }
                  else if (id == 3004) // Open File
                  {
                      std::string full_path =
                          (std::filesystem::path(workspace_root_) / file_path).string();
                      core::events::FileOpenRequestEvent open_evt;
                      open_evt.file_path = full_path;
                      event_bus_.publish(open_evt);
                  }
                  else if (id == 3005) // Open Changes (Diff)
                  {
                      std::string full_path =
                          (std::filesystem::path(workspace_root_) / file_path).string();
                      core::events::OpenDiffRequestEvent diff_evt;
                      diff_evt.left_path = full_path;
                      diff_evt.right_path = full_path;

                      if (is_staged)
                      {
                          diff_evt.title = file_path + " (Staged vs HEAD)";
                          diff_evt.left_content = runner.GetFileContentAtHEAD(file_path);
                          diff_evt.right_content = runner.GetFileContentFromIndex(file_path);
                      }
                      else
                      {
                          diff_evt.title = file_path + " (Working vs Staged)";
                          diff_evt.left_content = runner.GetFileContentFromIndex(file_path);

                          std::ifstream file_stream(full_path);
                          if (file_stream.is_open())
                          {
                              diff_evt.right_content.assign(
                                  std::istreambuf_iterator<char>(file_stream),
                                  std::istreambuf_iterator<char>());
                          }
                          else
                          {
                              diff_evt.right_content = "";
                          }
                      }
                      event_bus_.publish(diff_evt);
                  }
                  else if (id == 3006) // Stash File
                  {
                      runner.StashFile(file_path, "Stashed from UI");
                  }
                  else if (id == 3007) // Copy Path
                  {
                      std::string full_path =
                          (std::filesystem::path(workspace_root_) / file_path).string();
                      if (wxTheClipboard->Open())
                      {
                          wxTheClipboard->SetData(new wxTextDataObject(full_path));
                          wxTheClipboard->Close();
                      }
                  }
                  else if (id == 3008) // Copy Relative Path
                  {
                      if (wxTheClipboard->Open())
                      {
                          wxTheClipboard->SetData(new wxTextDataObject(file_path));
                          wxTheClipboard->Close();
                      }
                  }
                  else if (id == 3009) // Reveal in Explorer
                  {
                      std::string full_path =
                          (std::filesystem::path(workspace_root_) / file_path).string();
                      if (wxFileName::FileExists(full_path) || wxFileName::DirExists(full_path))
                      {
#if defined(__WXOSX__)
                          wxString cmd = wxString::Format("open -R \"%s\"", full_path);
                          wxExecute(cmd);
#elif defined(__WXMSW__)
                          wxString cmd = wxString::Format("explorer /select,\"%s\"", full_path);
                          wxExecute(cmd);
#else
                          wxLaunchDefaultApplication(wxFileName(full_path).GetPath());
#endif
                      }
                  }

                  if (id == 3001 || id == 3002 || id == 3003 || id == 3006)
                  {
                      RefreshStatus();
                  }
              });

    PopupMenu(&menu);
}

void SourceControlPanel::OnTemplateButtonClicked(wxCommandEvent& /*event*/)
{
    if (!config_)
        return;

    auto templates = config_->get_commit_templates();
    if (templates.empty())
        return;

    wxMenu menu;
    int id = 4001; // Base id for template items
    for (const auto& tmpl : templates)
    {
        menu.Append(id++, tmpl);
    }

    menu.Bind(wxEVT_MENU,
              [this, templates](wxCommandEvent& menu_evt)
              {
                  int idx = menu_evt.GetId() - 4001;
                  if (idx >= 0 && idx < static_cast<int>(templates.size()))
                  {
                      std::string tmpl = templates[static_cast<std::size_t>(idx)];
                      wxString current = commit_message_input_->GetValue();

                      if (current.Trim().IsEmpty())
                      {
                          commit_message_input_->SetValue(tmpl);
                      }
                      else
                      {
                          bool replaced = false;
                          for (const auto& t : templates)
                          {
                              if (current.StartsWith(t))
                              {
                                  wxString new_msg = tmpl + current.Mid(t.length());
                                  commit_message_input_->SetValue(new_msg);
                                  replaced = true;
                                  break;
                              }
                          }
                          if (!replaced)
                          {
                              commit_message_input_->SetValue(tmpl + current);
                          }
                      }

                      commit_message_input_->SetInsertionPointEnd();
                      commit_message_input_->SetFocus();
                  }
              });

    PopupMenu(&menu);
}

void SourceControlPanel::OnMergeItemRightClicked(wxListEvent& event)
{
    if (merge_list_ == nullptr || git_provider_ == nullptr)
    {
        return;
    }

    const long item_idx = event.GetIndex();
    const std::string file_path = merge_list_->GetItemText(item_idx).ToStdString();

    wxMenu menu;
    menu.Append(5001, "Accept Current Change");
    menu.Append(5002, "Accept Incoming Change");
    menu.AppendSeparator();
    menu.Append(5003, "Open File");

    menu.Bind(wxEVT_MENU,
              [this, file_path](wxCommandEvent& menu_evt)
              {
                  core::GitCommandRunner runner(workspace_root_);
                  const int menu_id = menu_evt.GetId();
                  if (menu_id == 5001) // Accept Current
                  {
                      runner.ResolveConflict(file_path, /*accept_current=*/true);
                  }
                  else if (menu_id == 5002) // Accept Incoming
                  {
                      runner.ResolveConflict(file_path, /*accept_current=*/false);
                  }
                  else if (menu_id == 5003) // Open File
                  {
                      const std::string full_path =
                          (std::filesystem::path(workspace_root_) / file_path).string();
                      core::events::FileOpenRequestEvent open_evt;
                      open_evt.file_path = full_path;
                      event_bus_.publish(open_evt);
                  }

                  if (menu_id == 5001 || menu_id == 5002)
                  {
                      RefreshStatus();
                  }
              });

    PopupMenu(&menu);
}

} // namespace markamp::ui
