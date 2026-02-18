#pragma once

/// @file WidgetLifecycleManager.h
/// @brief Phase 14: Widget instance lifecycle state machine.
///
/// Tracks the lifecycle state of every widget instance on the canvas.
/// Validates state transitions, triggers callbacks on state changes,
/// and provides suspend/resume API for off-screen optimization.

#include "core/EventBus.h"

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

namespace markamp::canvas
{

/// Lifecycle states for a widget instance.
enum class WidgetLifecycleState : uint8_t
{
    kUninitialized, ///< Registered but not yet initialized
    kInitializing,  ///< Being set up (loading data, etc.)
    kActive,        ///< Running and visible
    kSuspended,     ///< Off-screen or backgrounded; resources limited
    kError,         ///< Encountered a fatal error
    kDestroyed      ///< Cleaned up and removed
};

/// Callback on lifecycle state change: (widget_id, old_state, new_state).
using LifecycleCallback =
    std::function<void(const std::string&, WidgetLifecycleState, WidgetLifecycleState)>;

/// Tracked state for a single widget instance.
struct WidgetInstanceState
{
    std::string widget_id;
    WidgetLifecycleState state{WidgetLifecycleState::kUninitialized};
    std::string error_message;
    int transition_count{0}; ///< Number of state transitions
};

/// Result of a lifecycle transition.
struct LifecycleTransitionResult
{
    bool success{false};
    std::string error_message;
    WidgetLifecycleState previous_state{WidgetLifecycleState::kUninitialized};
    WidgetLifecycleState current_state{WidgetLifecycleState::kUninitialized};
};

/// Manages widget instance lifecycles with validated state transitions.
class WidgetLifecycleManager
{
public:
    explicit WidgetLifecycleManager(core::EventBus& event_bus);

    // ── Instance management ───────────────────────────────────────

    /// Track a new widget instance (starts in kUninitialized).
    auto track_widget(const std::string& widget_id) -> bool;

    /// Remove a widget from tracking.
    auto untrack_widget(const std::string& widget_id) -> bool;

    // ── State transitions ─────────────────────────────────────────

    /// Begin widget initialization.
    auto initialize_widget(const std::string& widget_id) -> LifecycleTransitionResult;

    /// Activate a widget (transition from kInitializing or kSuspended).
    auto activate_widget(const std::string& widget_id) -> LifecycleTransitionResult;

    /// Suspend a widget (e.g., when scrolled off-screen).
    auto suspend_widget(const std::string& widget_id) -> LifecycleTransitionResult;

    /// Mark a widget as in error state.
    auto error_widget(const std::string& widget_id, const std::string& error_message)
        -> LifecycleTransitionResult;

    /// Destroy a widget (final state).
    auto destroy_widget(const std::string& widget_id) -> LifecycleTransitionResult;

    // ── Query ─────────────────────────────────────────────────────

    /// Get current state of a widget.
    [[nodiscard]] auto widget_state(const std::string& widget_id) const
        -> const WidgetInstanceState*;

    /// All tracked widget instances.
    [[nodiscard]] auto tracked_widgets() const
        -> const std::unordered_map<std::string, WidgetInstanceState>&;

    /// Widgets in a specific state.
    [[nodiscard]] auto widgets_in_state(WidgetLifecycleState state) const
        -> std::vector<const WidgetInstanceState*>;

    /// Number of tracked widgets.
    [[nodiscard]] auto tracked_count() const -> size_t;

    /// Number of active widgets.
    [[nodiscard]] auto active_count() const -> size_t;

    // ── Callbacks ─────────────────────────────────────────────────

    /// Register a callback for lifecycle state changes.
    auto set_on_state_change(LifecycleCallback callback) -> void;

    // ── Utility ───────────────────────────────────────────────────

    /// Human-readable name for a lifecycle state.
    [[nodiscard]] static auto state_name(WidgetLifecycleState state) -> std::string;

    /// Check if a transition from->to is valid.
    [[nodiscard]] static auto is_valid_transition(WidgetLifecycleState from_state,
                                                  WidgetLifecycleState to_state) -> bool;

    /// Remove all tracked widgets.
    auto clear() -> void;

private:
    /// Attempt a state transition with validation.
    auto transition(const std::string& widget_id, WidgetLifecycleState target)
        -> LifecycleTransitionResult;

    core::EventBus& event_bus_;
    std::unordered_map<std::string, WidgetInstanceState> instances_;
    LifecycleCallback on_state_change_;
};

} // namespace markamp::canvas
