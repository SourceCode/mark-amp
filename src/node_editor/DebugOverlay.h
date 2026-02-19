#pragma once

#include "NodeEditorTypes.h"

#include <chrono>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::node_editor
{

// ---------------------------------------------------------------------------
// Debug enums and data structures
// ---------------------------------------------------------------------------

enum class DebugPhase
{
    kIdle,    ///< No active debug session
    kRunning, ///< Evaluation running freely
    kPaused,  ///< Paused at breakpoint or by user
    kStepping ///< Single-stepping through nodes
};

enum class TraceStatus
{
    kPending,
    kRunning,
    kCompleted,
    kFailed,
    kSkipped
};

/// A single trace entry recording one node's execution.
struct TraceEntry
{
    NodeId node_id;
    std::size_t execution_index{0};
    TraceStatus status{TraceStatus::kPending};
    float duration_ms{0.0F};
    std::string output_summary;
    std::chrono::steady_clock::time_point timestamp;
};

// ---------------------------------------------------------------------------
// Overlay visibility flags
// ---------------------------------------------------------------------------

struct OverlayFlags
{
    bool show_timings{false};     ///< Show per-node execution time
    bool show_values{false};      ///< Show output values on links
    bool show_exec_order{false};  ///< Show execution order badges
    bool show_breakpoints{true};  ///< Show breakpoint markers
    bool show_dirty_state{false}; ///< Show dirty/cached indicators
};

// ---------------------------------------------------------------------------
// DebugOverlay — manages debug session state, breakpoints, and trace log
// ---------------------------------------------------------------------------

class DebugOverlay
{
public:
    DebugOverlay() = default;

    // --- Breakpoints ------------------------------------------------------

    /// Toggle breakpoint on a node.
    void toggle_breakpoint(NodeId node_id);

    /// Check if a node has a breakpoint.
    [[nodiscard]] auto has_breakpoint(NodeId node_id) const -> bool;

    /// Set breakpoint explicitly.
    void set_breakpoint(NodeId node_id);

    /// Remove breakpoint.
    void remove_breakpoint(NodeId node_id);

    /// Get all breakpoint node IDs.
    [[nodiscard]] auto breakpoints() const -> std::vector<NodeId>;

    /// Clear all breakpoints.
    void clear_breakpoints();

    [[nodiscard]] auto breakpoint_count() const -> std::size_t;

    // --- Execution tracing ------------------------------------------------

    /// Add a trace entry for a node execution.
    void record_trace(const TraceEntry& entry);

    /// Get all trace entries in execution order.
    [[nodiscard]] auto trace_entries() const -> const std::vector<TraceEntry>&;

    /// Get trace entry for a specific node (most recent).
    [[nodiscard]] auto trace_for(NodeId node_id) const -> const TraceEntry*;

    /// Get execution order as ordered NodeId list.
    [[nodiscard]] auto execution_order() const -> std::vector<NodeId>;

    /// Clear trace log.
    void clear_trace();

    [[nodiscard]] auto trace_count() const -> std::size_t;

    // --- Debug session controls -------------------------------------------

    /// Start a debug session.
    void start_session();

    /// Pause execution (at next node boundary).
    void pause();

    /// Continue from pause.
    void continue_execution();

    /// Step to next node.
    void step_over();

    /// Step into group node.
    void step_into();

    /// Stop debug session.
    void stop_session();

    [[nodiscard]] auto phase() const -> DebugPhase
    {
        return phase_;
    }
    [[nodiscard]] auto is_active() const -> bool
    {
        return phase_ != DebugPhase::kIdle;
    }
    [[nodiscard]] auto is_paused() const -> bool
    {
        return phase_ == DebugPhase::kPaused;
    }

    /// Node where execution is currently paused.
    [[nodiscard]] auto paused_at() const -> NodeId
    {
        return paused_node_;
    }

    // --- Overlay flags ----------------------------------------------------

    [[nodiscard]] auto flags() const -> const OverlayFlags&
    {
        return flags_;
    }
    auto flags_mut() -> OverlayFlags&
    {
        return flags_;
    }

    void set_show_timings(bool show)
    {
        flags_.show_timings = show;
    }
    void set_show_values(bool show)
    {
        flags_.show_values = show;
    }
    void set_show_exec_order(bool show)
    {
        flags_.show_exec_order = show;
    }

private:
    std::unordered_set<NodeId> breakpoints_;
    std::vector<TraceEntry> trace_;
    DebugPhase phase_{DebugPhase::kIdle};
    NodeId paused_node_;
    OverlayFlags flags_;
};

} // namespace markamp::node_editor
