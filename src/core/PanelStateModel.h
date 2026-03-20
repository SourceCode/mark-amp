/// @file PanelStateModel.h
/// @brief P09-T03: Normalized error, empty, and recovery states for panels.
///
/// Shared panel state model applicable to explorer, search, settings,
/// canvas, and notebook hosts.
#pragma once

#include <functional>
#include <string>

namespace markamp::core
{

/// Panel display state.
enum class PanelDisplayState
{
    kEmpty,
    kLoading,
    kReady,
    kError,
};

/// A panel state with optional retry action.
class PanelStateModel
{
public:
    PanelStateModel() = default;

    /// Set the panel state.
    void set_state(PanelDisplayState state);

    /// Set state with a message.
    void set_state(PanelDisplayState state, const std::string& message);

    /// Get current state.
    [[nodiscard]] auto state() const -> PanelDisplayState { return state_; }

    /// Get state message.
    [[nodiscard]] auto message() const -> const std::string& { return message_; }

    /// Check if the panel has content.
    [[nodiscard]] auto has_content() const -> bool
    {
        return state_ == PanelDisplayState::kReady;
    }

    /// Check if a retry action is available.
    [[nodiscard]] auto can_retry() const -> bool
    {
        return state_ == PanelDisplayState::kError && retry_action_ != nullptr;
    }

    /// Set the retry action.
    void set_retry_action(std::function<void()> action) { retry_action_ = std::move(action); }

    /// Execute the retry action.
    void retry();

private:
    PanelDisplayState state_{PanelDisplayState::kEmpty};
    std::string message_;
    std::function<void()> retry_action_;
};

} // namespace markamp::core
