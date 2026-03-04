#include "TaskListPanel.h"

#include <wx/sizer.h>

namespace markamp::ui
{

TaskListPanel::TaskListPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    CreateLayout();
}

TaskListPanel::TaskListPanel(wxWindow* parent, core::TaskConfig* config)
    : wxPanel(parent, wxID_ANY)
    , config_(config)
{
    CreateLayout();
    RefreshContent();
}

void TaskListPanel::CreateLayout()
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    // ── Header ──
    header_label_ = new wxStaticText(this, wxID_ANY, "TASKS");
    const wxFont header_font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    header_label_->SetFont(header_font);
    sizer->Add(header_label_, 0, wxEXPAND | wxALL, 8);

    // ── Task list ──
    task_list_ = new wxListCtrl(this,
                                wxID_ANY,
                                wxDefaultPosition,
                                wxDefaultSize,
                                wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_NO_HEADER | wxBORDER_NONE);
    task_list_->InsertColumn(0, "Icon", wxLIST_FORMAT_LEFT, 24);
    task_list_->InsertColumn(1, "Task", wxLIST_FORMAT_LEFT, 200);
    task_list_->InsertColumn(2, "Command", wxLIST_FORMAT_LEFT, 150);

    const wxFont list_font(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    task_list_->SetFont(list_font);

    // Double-click to run task
    task_list_->Bind(wxEVT_LIST_ITEM_ACTIVATED,
                     [this](wxListEvent& evt)
                     {
                         if (on_task_run_ && config_ != nullptr)
                         {
                             auto task_name =
                                 task_list_->GetItemText(evt.GetIndex(), 1).ToStdString();
                             on_task_run_(task_name);
                         }
                     });

    sizer->Add(task_list_, 1, wxEXPAND);

    SetSizer(sizer);
}

void TaskListPanel::RefreshContent()
{
    if (task_list_ == nullptr || config_ == nullptr)
    {
        return;
    }

    task_list_->DeleteAllItems();

    // Group: Build
    auto build_tasks = config_->tasks_by_group(core::TaskGroup::kBuild);
    if (!build_tasks.empty())
    {
        AddGroupHeader("BUILD");
        for (const auto& task : build_tasks)
        {
            AddTaskRow(task);
        }
    }

    // Group: Test
    auto test_tasks = config_->tasks_by_group(core::TaskGroup::kTest);
    if (!test_tasks.empty())
    {
        AddGroupHeader("TEST");
        for (const auto& task : test_tasks)
        {
            AddTaskRow(task);
        }
    }

    // Group: Deploy
    auto deploy_tasks = config_->tasks_by_group(core::TaskGroup::kDeploy);
    if (!deploy_tasks.empty())
    {
        AddGroupHeader("DEPLOY");
        for (const auto& task : deploy_tasks)
        {
            AddTaskRow(task);
        }
    }

    // Group: Other
    auto other_tasks = config_->tasks_by_group(core::TaskGroup::kOther);
    if (!other_tasks.empty())
    {
        AddGroupHeader("OTHER");
        for (const auto& task : other_tasks)
        {
            AddTaskRow(task);
        }
    }
}

void TaskListPanel::AddGroupHeader(const wxString& group_name)
{
    if (task_list_ == nullptr)
    {
        return;
    }
    const long row = task_list_->InsertItem(task_list_->GetItemCount(), wxEmptyString);
    task_list_->SetItem(row, 1, group_name);
    task_list_->SetItemTextColour(row, wxColour(128, 128, 128));
}

void TaskListPanel::AddTaskRow(const core::TaskDefinition& task)
{
    if (task_list_ == nullptr)
    {
        return;
    }

    const long row = task_list_->InsertItem(task_list_->GetItemCount(), wxEmptyString);

    wxString icon;
    switch (task.group)
    {
        case core::TaskGroup::kBuild:
            icon = "🔨";
            break;
        case core::TaskGroup::kTest:
            icon = "🧪";
            break;
        case core::TaskGroup::kDeploy:
            icon = "🚀";
            break;
        case core::TaskGroup::kOther:
            icon = "⚙";
            break;
    }

    wxString label = wxString(task.label.empty() ? task.name : task.label);
    if (task.is_default)
    {
        label += " ★";
    }

    task_list_->SetItem(row, 0, icon);
    task_list_->SetItem(row, 1, label);
    task_list_->SetItem(row, 2, wxString(task.full_command()));

    task_list_->SetItemTextColour(row, wxColour(204, 204, 204));
}

void TaskListPanel::set_config(core::TaskConfig* config)
{
    config_ = config;
}

void TaskListPanel::set_on_task_run(TaskRunCallback callback)
{
    on_task_run_ = std::move(callback);
}

void TaskListPanel::ApplyTheme(const wxColour& bg_colour, const wxColour& fg_colour)
{
    SetBackgroundColour(bg_colour);
    if (task_list_ != nullptr)
    {
        task_list_->SetBackgroundColour(bg_colour);
        task_list_->SetForegroundColour(fg_colour);
    }
    if (header_label_ != nullptr)
    {
        header_label_->SetForegroundColour(fg_colour);
    }
    Refresh();
}

} // namespace markamp::ui
