/// @file WorkspaceOpenOrchestrator.h
/// @brief P03-T05: Single entry point for all workspace-open paths.
///
/// Consolidates folder scan, recents update, view switching, and
/// session restore trigger into one deterministic pipeline.
#pragma once

#include <string>

namespace markamp::core
{
class EventBus;
class Config;
class RecentWorkspaces;
class WorkspaceSessionRestore;

/// Orchestrates workspace opening from any entry point.
///
/// All workspace-open paths (menu File > Open, startup panel, drag-drop,
/// recent workspaces, command-line argument) should route through this
/// orchestrator to ensure consistent behavior.
class WorkspaceOpenOrchestrator
{
public:
    WorkspaceOpenOrchestrator(EventBus& bus,
                              Config& cfg,
                              RecentWorkspaces& recents,
                              WorkspaceSessionRestore& session_restore);

    /// Open a workspace folder. This is the single entry point.
    ///
    /// Steps:
    /// 1. Validate the path exists and is a directory
    /// 2. Publish WorkspaceOpenRequestEvent
    /// 3. Update recents list
    /// 4. Trigger session restore for this workspace
    /// 5. Publish loading/error feedback
    ///
    /// @return true if the workspace was opened successfully
    auto open_workspace(const std::string& folder_path) -> bool;

    /// Check if a workspace open is currently in progress.
    [[nodiscard]] auto is_opening() const -> bool { return opening_; }

    /// Get the currently open workspace path (empty if none).
    [[nodiscard]] auto current_workspace() const -> const std::string&
    {
        return current_workspace_;
    }

private:
    EventBus& event_bus_;
    Config& config_;
    RecentWorkspaces& recents_;
    WorkspaceSessionRestore& session_restore_;
    bool opening_{false};
    std::string current_workspace_;
};

} // namespace markamp::core
