#include "DebugOverlay.h"

#include <algorithm>

namespace markamp::node_editor
{

// ---------------------------------------------------------------------------
// Breakpoints
// ---------------------------------------------------------------------------

void DebugOverlay::toggle_breakpoint(NodeId node_id)
{
    if (breakpoints_.contains(node_id))
    {
        breakpoints_.erase(node_id);
    }
    else
    {
        breakpoints_.insert(node_id);
    }
}

auto DebugOverlay::has_breakpoint(NodeId node_id) const -> bool
{
    return breakpoints_.contains(node_id);
}

void DebugOverlay::set_breakpoint(NodeId node_id)
{
    breakpoints_.insert(node_id);
}

void DebugOverlay::remove_breakpoint(NodeId node_id)
{
    breakpoints_.erase(node_id);
}

auto DebugOverlay::breakpoints() const -> std::vector<NodeId>
{
    return {breakpoints_.begin(), breakpoints_.end()};
}

void DebugOverlay::clear_breakpoints()
{
    breakpoints_.clear();
}

auto DebugOverlay::breakpoint_count() const -> std::size_t
{
    return breakpoints_.size();
}

// ---------------------------------------------------------------------------
// Execution tracing
// ---------------------------------------------------------------------------

void DebugOverlay::record_trace(const TraceEntry& entry)
{
    trace_.push_back(entry);
}

auto DebugOverlay::trace_entries() const -> const std::vector<TraceEntry>&
{
    return trace_;
}

auto DebugOverlay::trace_for(NodeId node_id) const -> const TraceEntry*
{
    // Return the most recent trace for this node.
    for (auto iter = trace_.rbegin(); iter != trace_.rend(); ++iter)
    {
        if (iter->node_id == node_id)
        {
            return &(*iter);
        }
    }
    return nullptr;
}

auto DebugOverlay::execution_order() const -> std::vector<NodeId>
{
    std::vector<NodeId> order;
    order.reserve(trace_.size());
    for (const auto& entry : trace_)
    {
        order.push_back(entry.node_id);
    }
    return order;
}

void DebugOverlay::clear_trace()
{
    trace_.clear();
}

auto DebugOverlay::trace_count() const -> std::size_t
{
    return trace_.size();
}

// ---------------------------------------------------------------------------
// Debug session controls
// ---------------------------------------------------------------------------

void DebugOverlay::start_session()
{
    phase_ = DebugPhase::kRunning;
    paused_node_ = NodeId{};
    trace_.clear();
}

void DebugOverlay::pause()
{
    if (phase_ == DebugPhase::kRunning)
    {
        phase_ = DebugPhase::kPaused;
    }
}

void DebugOverlay::continue_execution()
{
    if (phase_ == DebugPhase::kPaused || phase_ == DebugPhase::kStepping)
    {
        phase_ = DebugPhase::kRunning;
        paused_node_ = NodeId{};
    }
}

void DebugOverlay::step_over()
{
    if (phase_ == DebugPhase::kPaused)
    {
        phase_ = DebugPhase::kStepping;
    }
}

void DebugOverlay::step_into()
{
    if (phase_ == DebugPhase::kPaused)
    {
        phase_ = DebugPhase::kStepping;
    }
}

void DebugOverlay::stop_session()
{
    phase_ = DebugPhase::kIdle;
    paused_node_ = NodeId{};
}

} // namespace markamp::node_editor
