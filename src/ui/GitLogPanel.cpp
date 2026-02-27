#include "GitLogPanel.h"

#include "core/Events.h"
#include "core/GitCommandRunner.h"

#include <wx/clipbrd.h>
#include <wx/menu.h>

namespace markamp::ui
{

GitLogPanel::GitLogPanel(wxWindow* parent,
                         core::ThemeEngine& theme_engine,
                         core::EventBus& event_bus,
                         std::string workspace_root)
    : ThemeAwareWindow(parent, theme_engine)
    , event_bus_(event_bus)
    , workspace_root_(std::move(workspace_root))
{
    CreateLayout();
    RefreshLog();
}

GitLogPanel::~GitLogPanel() = default;

void GitLogPanel::CreateLayout()
{
    SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgPanel));

    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    // Toolbar
    auto* toolbar_sizer = new wxBoxSizer(wxHORIZONTAL);

    branch_choice_ = new wxChoice(this, wxID_ANY);
    branch_choice_->Bind(wxEVT_CHOICE, &GitLogPanel::OnBranchSelected, this);
    toolbar_sizer->Add(branch_choice_, 0, wxALIGN_CENTER_VERTICAL | wxALL, 8);

    search_box_ =
        new wxSearchCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1));
    search_box_->SetDescriptiveText("Search commits...");
    search_box_->ShowSearchButton(true);
    search_box_->ShowCancelButton(true);
    search_box_->Bind(wxEVT_TEXT, &GitLogPanel::OnSearch, this);
    search_box_->Bind(wxEVT_SEARCHCTRL_SEARCH_BTN, &GitLogPanel::OnSearch, this);
    search_box_->Bind(wxEVT_SEARCHCTRL_CANCEL_BTN,
                      [this](wxCommandEvent&)
                      {
                          search_box_->SetValue("");
                          RefreshLog();
                      });
    toolbar_sizer->Add(search_box_, 0, wxALIGN_CENTER_VERTICAL | wxALL, 8);

    main_sizer->Add(toolbar_sizer, 0, wxEXPAND);

    // List Control
    log_list_ = new wxListCtrl(this,
                               wxID_ANY,
                               wxDefaultPosition,
                               wxDefaultSize,
                               wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_NONE);

    // Use a monospaced font so the graph alignment stays intact
    wxFont mono_font(wxFontInfo(12).Family(wxFONTFAMILY_TELETYPE));
    log_list_->SetFont(mono_font);

    log_list_->SetBackgroundColour(GetBackgroundColour());
    log_list_->SetTextColour(theme_engine().color(core::ThemeColorToken::TextMain));

    log_list_->InsertColumn(0, "Graph", wxLIST_FORMAT_LEFT, 150);
    log_list_->InsertColumn(1, "Hash", wxLIST_FORMAT_LEFT, 80);
    log_list_->InsertColumn(2, "Message", wxLIST_FORMAT_LEFT, 400);
    log_list_->InsertColumn(3, "Author", wxLIST_FORMAT_LEFT, 120);
    log_list_->InsertColumn(4, "Date", wxLIST_FORMAT_LEFT, 150);

    log_list_->Bind(wxEVT_LIST_ITEM_ACTIVATED, &GitLogPanel::OnItemActivated, this);
    log_list_->Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &GitLogPanel::OnItemRightClicked, this);

    main_sizer->Add(log_list_, 1, wxEXPAND | wxALL, 0);

    SetSizer(main_sizer);
}

void GitLogPanel::SetWorkspaceRoot(const std::string& root)
{
    workspace_root_ = root;
    RefreshLog();
}

void GitLogPanel::RefreshLog()
{
    core::GitCommandRunner runner(workspace_root_);

    // Refresh branches
    auto branches = runner.GetBranches();
    auto current_branch = runner.GetBranch();

    if (!branches.empty())
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
            for (const auto& branch : branches)
            {
                branch_choice_->Append(branch);
            }
        }
        if (branch_choice_->GetSelection() == wxNOT_FOUND)
        {
            branch_choice_->SetStringSelection(current_branch);
        }
    }

    log_list_->DeleteAllItems();

    std::string target = branch_choice_->GetStringSelection().ToStdString();

    // Actually we want all branches graphed usually, or just the current branch. Let's do all.
    auto logs = runner.GetLog("--all", 500, true);

    for (const auto& log : logs)
    {
        long row = log_list_->InsertItem(log_list_->GetItemCount(), log.graph);
        log_list_->SetItem(row, 1, log.hash);
        log_list_->SetItem(row, 2, log.message);
        log_list_->SetItem(row, 3, log.author);
        log_list_->SetItem(row, 4, log.date);

        // Subtly color branches versus commits
        if (log.hash.empty())
        {
            log_list_->SetItemTextColour(row,
                                         theme_engine().color(core::ThemeColorToken::TextMuted));
        }
    }
}

void GitLogPanel::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgPanel));
    if (log_list_ != nullptr)
    {
        log_list_->SetBackgroundColour(GetBackgroundColour());
        log_list_->SetTextColour(theme_engine().color(core::ThemeColorToken::TextMain));
    }
    Refresh();
}

void GitLogPanel::OnBranchSelected(wxCommandEvent& /*event*/)
{
    RefreshLog();
}

void GitLogPanel::OnSearch(wxCommandEvent& /*event*/)
{
    // Basic search filtering: reload or local filter. For now just standard log
}

void GitLogPanel::OnItemActivated(wxListEvent& event)
{
    long item = event.GetIndex();
    if (item < 0)
    {
        return;
    }

    wxString hash = log_list_->GetItemText(item, 1);
    if (hash.IsEmpty())
    {
        return;
    }

    core::GitCommandRunner runner(workspace_root_);
    auto result = runner.RunSync("git -C \"" + workspace_root_ +
                                 "\" show --format=fuller --stat -p " + hash.ToStdString());

    if (result.success())
    {
        core::events::OpenDiffRequestEvent diff_evt;
        diff_evt.title = "Commit: " + hash.ToStdString();
        diff_evt.left_path = "commit_" + hash.ToStdString();
        diff_evt.right_path = "commit_" + hash.ToStdString();
        diff_evt.left_content = "";
        diff_evt.right_content = result.stdout_text;

        event_bus_.publish(diff_evt);
    }
}

void GitLogPanel::OnItemRightClicked(wxListEvent& event)
{
    long item = event.GetIndex();
    if (item < 0)
    {
        return;
    }

    wxString hash = log_list_->GetItemText(item, 1);
    if (hash.IsEmpty())
    {
        return;
    }

    wxMenu menu;
    menu.Append(1, "Copy Commit Hash");

    menu.Bind(wxEVT_MENU,
              [hash](wxCommandEvent& cmd)
              {
                  if (cmd.GetId() == 1)
                  {
                      if (wxTheClipboard->Open())
                      {
                          wxTheClipboard->SetData(new wxTextDataObject(hash));
                          wxTheClipboard->Close();
                      }
                  }
              });

    PopupMenu(&menu);
}

} // namespace markamp::ui
