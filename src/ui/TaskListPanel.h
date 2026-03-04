#pragma once

#include "core/TaskConfig.h"

#include <wx/button.h>
#include <wx/listctrl.h>
#include <wx/panel.h>
#include <wx/stattext.h>

#include <functional>
#include <string>

namespace markamp::ui
{

/// Task List Panel — sidebar list of configured tasks from tasks.json.
/// Displays tasks grouped by type (build, test, deploy, other) with
/// run/stop buttons and status indicators.
class TaskListPanel : public wxPanel
{
public:
    explicit TaskListPanel(wxWindow* parent);

    /// Full constructor with task config.
    TaskListPanel(wxWindow* parent, core::TaskConfig* config);

    ~TaskListPanel() override = default;

    /// Set the task configuration.
    void set_config(core::TaskConfig* config);

    /// Callback when a task is requested to run.
    using TaskRunCallback = std::function<void(const std::string& task_name)>;
    void set_on_task_run(TaskRunCallback callback);

    /// Refresh the task list.
    void RefreshContent();

    /// Apply theme colors.
    void ApplyTheme(const wxColour& bg_colour, const wxColour& fg_colour);

private:
    void CreateLayout();
    void AddGroupHeader(const wxString& group_name);
    void AddTaskRow(const core::TaskDefinition& task);

    core::TaskConfig* config_{nullptr};
    TaskRunCallback on_task_run_;

    // UI controls
    wxListCtrl* task_list_{nullptr};
    wxStaticText* header_label_{nullptr};
};

} // namespace markamp::ui
