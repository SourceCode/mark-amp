/// @file WorkbenchShellController.cpp
/// @brief V19 P01-T01: Central shell orchestration controller implementation.

#include "WorkbenchShellController.h"

#include "Config.h"
#include "Logger.h"
#include "ShellLayoutState.h"
#include "WorkspaceLoadStateModel.h"
#include "WorkspaceOpenOrchestrator.h"
#include "WorkspaceSessionRestore.h"

namespace markamp::core
{

WorkbenchShellController::WorkbenchShellController(EventBus& bus, Config& cfg)
    : event_bus_(bus)
    , config_(cfg)
{
    // Subscribe to mode switch requests
    mode_switch_sub_ = event_bus_.subscribe<events::WorkbenchModeSwitchRequestEvent>(
        [this](const events::WorkbenchModeSwitchRequestEvent& evt)
        { switch_to(evt.target_mode, "mode_switch_request"); });

    // Subscribe to workspace open requests
    workspace_open_sub_ = event_bus_.subscribe<events::WorkspaceOpenRequestEvent>(
        [this](const events::WorkspaceOpenRequestEvent& evt)
        { open_workspace(evt.path, "workspace_open_event"); });

    // Subscribe to settings open requests → switch to settings surface
    settings_open_sub_ = event_bus_.subscribe<events::SettingsOpenRequestEvent>(
        [this](const events::SettingsOpenRequestEvent& /*evt*/)
        { switch_to(events::WorkbenchMode::kSettings, "settings_open_request"); });

    // Subscribe to workspace refresh → re-enter editor mode after workspace load
    workspace_refresh_sub_ = event_bus_.subscribe<events::WorkspaceRefreshRequestEvent>(
        [this](const events::WorkspaceRefreshRequestEvent& /*evt*/)
        {
            if (active_surface_ != events::WorkbenchMode::kEditor)
            {
                switch_to(events::WorkbenchMode::kEditor, "workspace_refresh");
            }
        });

    MARKAMP_LOG_INFO("WorkbenchShellController initialized");
}

void WorkbenchShellController::set_ui_delegate(IShellUIDelegate* delegate)
{
    delegate_ = delegate;
    MARKAMP_LOG_DEBUG("WorkbenchShellController: UI delegate {}",
                      delegate ? "connected" : "disconnected");
}

void WorkbenchShellController::set_workspace_orchestrator(
    WorkspaceOpenOrchestrator* orchestrator)
{
    workspace_orchestrator_ = orchestrator;
}

void WorkbenchShellController::set_load_state_model(WorkspaceLoadStateModel* model)
{
    load_state_model_ = model;
}

void WorkbenchShellController::set_shell_layout_state(ShellLayoutState* state)
{
    shell_layout_state_ = state;
}


void WorkbenchShellController::set_session_restore(WorkspaceSessionRestore* restore)
{
    session_restore_ = restore;
}

void WorkbenchShellController::switch_to(events::WorkbenchMode target,
                                          const std::string& trigger)
{
    if (target == active_surface_)
    {
        MARKAMP_LOG_DEBUG("WorkbenchShellController: switch_to({}) ignored — already active",
                          static_cast<int>(target));
        return;
    }

    auto previous_surface = active_surface_;
    active_surface_ = target;
    ++transition_count_;

    // Record in history
    record_transition(previous_surface, target, trigger);

    // Delegate UI rendering
    if (delegate_ != nullptr)
    {
        delegate_->SetWorkbenchMode(target);

        switch (target)
        {
        case events::WorkbenchMode::kEditor:
            delegate_->ShowEditorWorkspace();
            break;
        case events::WorkbenchMode::kGraph:
        case events::WorkbenchMode::kSettings:
            // These surfaces share the editor workspace layout;
            // mode-specific panel activation is handled by the mode changed event.
            break;
        default:
            break;
        }
    }

    // Publish mode changed event
    events::WorkbenchModeChangedEvent changed_evt;
    changed_evt.previous_mode = previous_surface;
    changed_evt.new_mode = target;
    event_bus_.publish(changed_evt);

    MARKAMP_LOG_INFO("Shell surface: {} → {} (trigger: {})",
                     static_cast<int>(previous_surface),
                     static_cast<int>(target),
                     trigger);
}

void WorkbenchShellController::show_startup(const std::string& trigger)
{
    has_workspace_ = false;
    workspace_root_.clear();

    if (delegate_ != nullptr)
    {
        delegate_->ShowStartupScreen();
    }

    MARKAMP_LOG_INFO("Shell: showing startup screen (trigger: {})", trigger);
}

void WorkbenchShellController::open_workspace(const std::string& path,
                                               const std::string& trigger)
{
    MARKAMP_LOG_INFO("Shell: opening workspace '{}' (trigger: {})", path, trigger);

    // Delegate to workspace orchestrator if available
    if (workspace_orchestrator_ != nullptr)
    {
        // Mark load state as loading
        if (load_state_model_ != nullptr)
        {
            load_state_model_->set_state(WorkspaceLoadState::kLoading, "Opening workspace: " + path);
        }

        const bool open_succeeded = workspace_orchestrator_->open_workspace(path);

        if (open_succeeded)
        {
            has_workspace_ = true;
            workspace_root_ = path;

            if (load_state_model_ != nullptr)
            {
                load_state_model_->set_state(WorkspaceLoadState::kReady);
            }

            // Switch to editor surface and set workspace root
            if (delegate_ != nullptr)
            {
                delegate_->SetWorkspaceRoot(path);
            }
            switch_to(events::WorkbenchMode::kEditor, trigger);
        }
        else
        {
            if (load_state_model_ != nullptr)
            {
                load_state_model_->set_state(WorkspaceLoadState::kFailed,
                                             "Failed to open workspace: " + path);
            }
            MARKAMP_LOG_WARN("Shell: workspace open failed for '{}'", path);
        }
    }
    else
    {
        // No orchestrator — just set state directly
        if (load_state_model_ != nullptr)
        {
            load_state_model_->set_state(WorkspaceLoadState::kLoading, "Opening workspace: " + path);
        }

        has_workspace_ = true;
        workspace_root_ = path;

        if (load_state_model_ != nullptr)
        {
            load_state_model_->set_state(WorkspaceLoadState::kReady);
        }

        if (delegate_ != nullptr)
        {
            delegate_->SetWorkspaceRoot(path);
        }
        switch_to(events::WorkbenchMode::kEditor, trigger);
    }
}

void WorkbenchShellController::open_file(const std::string& file_path,
                                          const std::string& trigger)
{
    MARKAMP_LOG_INFO("Shell: opening file '{}' (trigger: {})", file_path, trigger);

    // If not in editor mode, switch to it
    if (active_surface_ != events::WorkbenchMode::kEditor)
    {
        switch_to(events::WorkbenchMode::kEditor, trigger);
    }

    // Publish file open event for the editor to handle
    events::FileOpenedEvent open_evt;
    open_evt.file_path = file_path;
    event_bus_.publish(open_evt);

    // Focus the editor
    if (delegate_ != nullptr)
    {
        delegate_->FocusEditor();
    }
}

void WorkbenchShellController::record_transition(events::WorkbenchMode from_mode,
                                                  events::WorkbenchMode target_mode,
                                                  const std::string& trigger)
{
    ShellTransitionRecord record;
    record.from = from_mode;
    record.to = target_mode;
    record.trigger = trigger;
    record.timestamp = std::chrono::steady_clock::now();

    if (static_cast<int>(history_.size()) >= kMaxHistory)
    {
        history_.erase(history_.begin());
    }
    history_.push_back(record);
}

} // namespace markamp::core
