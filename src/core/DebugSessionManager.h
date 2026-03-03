#pragma once

#include "EventBus.h"
#include "LaunchConfig.h"

#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Phase 19 Task 17: Debug session states.
enum class DebugState
{
    kInactive,
    kStarting,
    kRunning,
    kPaused,
    kStopped
};

/// A stack frame returned by the debugger.
struct StackFrame
{
    int frame_id{0};
    std::string name;        // function name
    std::string source_file; // file path
    int line{0};
    int column{0};
    bool is_external{false}; // library/system frame
};

/// A debugger variable.
struct DebugVariable
{
    std::string name;
    std::string type;
    std::string value;
    int variable_ref{0}; // Reference for expandable children
    bool has_children{false};
};

/// Phase 19 Task 17: Tracks one debug session's state.
struct DebugSession
{
    DebugState state{DebugState::kInactive};
    LaunchConfig config;
    int active_thread_id{0};
    int active_frame_id{0};
    std::string stopped_file;
    int stopped_line{0};
    std::string stopped_reason; // "breakpoint", "step", "exception"
    std::string session_id;     // Unique session identifier
};

/// Phase 19 Task 17: Centralized debug state manager.
/// Tracks active debug sessions and publishes state change events.
class DebugSessionManager
{
public:
    explicit DebugSessionManager(EventBus& event_bus);

    /// Start a new debug session with the given config.
    void start_session(const LaunchConfig& config);

    /// Update session state (publishes DebugStateChangedEvent).
    void set_state(DebugState new_state);

    /// Record a stop event (breakpoint, step, exception).
    void on_stopped(const std::string& file, int line, const std::string& reason);

    /// End the current session.
    void end_session();

    /// Get the current session state.
    [[nodiscard]] auto state() const -> DebugState;

    /// Get the current session (if active).
    [[nodiscard]] auto session() const -> const DebugSession&;

    /// Check if a debug session is active.
    [[nodiscard]] auto is_active() const -> bool;

    /// Phase 19 Task 23: Multi-target — all active sessions.
    [[nodiscard]] auto sessions() const -> const std::vector<DebugSession>&;

    /// Phase 19 Task 23: Add an additional session.
    void add_session(const LaunchConfig& config);

    /// Phase 19 Task 23: Select a session as active.
    void select_session(const std::string& session_id);

private:
    EventBus& event_bus_;
    std::vector<DebugSession> sessions_;
    std::size_t active_session_index_{0};

    void publish_state_change();
};

} // namespace markamp::core
