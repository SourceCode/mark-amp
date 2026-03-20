/// @file WorkspaceLoadStateModel.h
/// @brief P05-T05: Workspace load state machine.
///
/// Provides clear shell feedback for empty, loading, failed, and ready
/// workspace states. Each shell surface queries this model for rendering.
#pragma once

#include <string>

namespace markamp::core
{
class EventBus;

/// Workspace load states.
enum class WorkspaceLoadState
{
    kIdle,    ///< No workspace action in progress
    kLoading, ///< Workspace scanning in progress
    kReady,   ///< Workspace loaded successfully
    kFailed,  ///< Workspace load failed
    kEmpty,   ///< Workspace opened but contains no files
};

/// State machine for workspace load lifecycle.
class WorkspaceLoadStateModel
{
public:
    explicit WorkspaceLoadStateModel(EventBus& bus);

    /// Transition to a new state and publish event.
    void set_state(WorkspaceLoadState state, const std::string& message = "");

    /// Get current state.
    [[nodiscard]] auto state() const -> WorkspaceLoadState { return state_; }

    /// Get current state message (e.g., error description).
    [[nodiscard]] auto message() const -> const std::string& { return message_; }

    /// Convenience state checks.
    [[nodiscard]] auto is_loading() const -> bool
    {
        return state_ == WorkspaceLoadState::kLoading;
    }
    [[nodiscard]] auto is_ready() const -> bool
    {
        return state_ == WorkspaceLoadState::kReady;
    }
    [[nodiscard]] auto has_failed() const -> bool
    {
        return state_ == WorkspaceLoadState::kFailed;
    }
    [[nodiscard]] auto is_empty() const -> bool
    {
        return state_ == WorkspaceLoadState::kEmpty;
    }

private:
    EventBus& event_bus_;
    WorkspaceLoadState state_{WorkspaceLoadState::kIdle};
    std::string message_;
};

} // namespace markamp::core
