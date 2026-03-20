/// @file WorkbenchShellController.h
/// @brief V19 P01-T01: Central shell orchestration controller.
///
/// Consolidates workbench surface transitions (editor, canvas, notebook, graph,
/// settings) into a single controller. Owns the authoritative active-surface
/// state, subscribes to mode-switch requests, workspace-open requests, and
/// settings-open requests, and delegates rendering to LayoutManager through a
/// thin UI-delegate interface.
#pragma once

#include "EventBus.h"
#include "Events.h"

#include <chrono>
#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{
class Config;
class WorkspaceOpenOrchestrator;
class WorkspaceLoadStateModel;
class ShellLayoutState;
class CanvasWorkbenchMode;
class WorkspaceSessionRestore;

/// Thin delegate interface for UI rendering. LayoutManager implements this.
/// Keeps the controller free of wxWidgets dependencies.
struct IShellUIDelegate
{
    virtual ~IShellUIDelegate() = default;

protected:
    IShellUIDelegate() = default;

public:
    IShellUIDelegate(const IShellUIDelegate&) = delete;
    auto operator=(const IShellUIDelegate&) -> IShellUIDelegate& = delete;
    IShellUIDelegate(IShellUIDelegate&&) = delete;
    auto operator=(IShellUIDelegate&&) -> IShellUIDelegate& = delete;

    virtual void ShowEditorWorkspace() = 0;
    virtual void ShowCanvasWorkspace() = 0;
    virtual void ShowStartupScreen() = 0;
    virtual void SetWorkbenchMode(events::WorkbenchMode mode) = 0;
    virtual void SetWorkspaceRoot(const std::string& root_path) = 0;
    virtual void FocusEditor() = 0;
};

/// Structured log entry for shell transitions.
struct ShellTransitionRecord
{
    events::WorkbenchMode from{events::WorkbenchMode::kEditor};
    events::WorkbenchMode to{events::WorkbenchMode::kEditor};
    std::string trigger;                              ///< What caused this transition
    std::chrono::steady_clock::time_point timestamp;  ///< When it happened
};

/// Central shell orchestration controller.
///
/// Subscribes to EventBus events that affect the active workbench surface
/// (workspace open, settings open, canvas mode, notebook host). On each
/// event, it performs the canonical shell transition sequence:
///   1. Resolve target surface
///   2. Publish WorkbenchModeChangedEvent
///   3. Delegate UI rendering to IShellUIDelegate
///   4. Log the transition
class WorkbenchShellController
{
public:
    WorkbenchShellController(EventBus& bus, Config& cfg);

    /// Set the UI delegate (typically LayoutManager). Must be called before
    /// any surface transitions can execute.
    void set_ui_delegate(IShellUIDelegate* delegate);

    /// Set optional service references for enriched shell control.
    void set_workspace_orchestrator(WorkspaceOpenOrchestrator* orchestrator);
    void set_load_state_model(WorkspaceLoadStateModel* model);
    void set_shell_layout_state(ShellLayoutState* state);
    void set_canvas_mode(CanvasWorkbenchMode* canvas);
    void set_session_restore(WorkspaceSessionRestore* restore);

    // ── Surface transitions ──

    /// Switch to a specific workbench surface with a recorded trigger reason.
    void switch_to(events::WorkbenchMode target, const std::string& trigger = "explicit");

    /// Show the startup/welcome screen (no workspace loaded).
    void show_startup(const std::string& trigger = "startup");

    /// Open a workspace by path; delegates to WorkspaceOpenOrchestrator.
    void open_workspace(const std::string& path, const std::string& trigger = "user");

    /// Handle a file-open request (command-line arg, drag/drop, recent file).
    void open_file(const std::string& file_path, const std::string& trigger = "user");

    // ── Queries ──

    /// Current active workbench surface.
    [[nodiscard]] auto active_surface() const -> events::WorkbenchMode { return active_surface_; }

    /// Whether a workspace is currently loaded.
    [[nodiscard]] auto has_workspace() const -> bool { return has_workspace_; }

    /// Current workspace root path.
    [[nodiscard]] auto workspace_root() const -> const std::string& { return workspace_root_; }

    /// Transition history (most recent first, capped at kMaxHistory).
    [[nodiscard]] auto transition_history() const -> const std::vector<ShellTransitionRecord>&
    {
        return history_;
    }

    /// Number of transitions since controller creation.
    [[nodiscard]] auto transition_count() const -> int { return transition_count_; }

    /// Check if the UI delegate has been set.
    [[nodiscard]] auto has_delegate() const -> bool { return delegate_ != nullptr; }

private:
    EventBus& event_bus_;
    Config& config_;

    // Optional service references
    IShellUIDelegate* delegate_{nullptr};
    WorkspaceOpenOrchestrator* workspace_orchestrator_{nullptr};
    WorkspaceLoadStateModel* load_state_model_{nullptr};
    ShellLayoutState* shell_layout_state_{nullptr};
    CanvasWorkbenchMode* canvas_mode_{nullptr};
    WorkspaceSessionRestore* session_restore_{nullptr};

    // State
    events::WorkbenchMode active_surface_{events::WorkbenchMode::kEditor};
    bool has_workspace_{false};
    std::string workspace_root_;
    int transition_count_{0};

    // History
    static constexpr int kMaxHistory = 50;
    std::vector<ShellTransitionRecord> history_;

    void record_transition(events::WorkbenchMode from_mode, events::WorkbenchMode target_mode,
                           const std::string& trigger);

    // Event subscriptions
    Subscription mode_switch_sub_;
    Subscription workspace_open_sub_;
    Subscription settings_open_sub_;
    Subscription workspace_refresh_sub_;
};

} // namespace markamp::core
