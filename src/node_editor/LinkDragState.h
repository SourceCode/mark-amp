#pragma once

// V11 Phase 25: Link Drag State Machine
// Tracks transient state during link creation/reconnection.

#include "NodeEditorTypes.h"

namespace markamp::node_editor
{

/// Current phase of a link-drag interaction.
enum class LinkDragPhase : uint8_t
{
    Idle,
    Dragging,
    OverTarget,
    Completed,
    Cancelled
};

/// Transient state for link creation drag interactions.
struct LinkDragState
{
    LinkDragPhase phase{LinkDragPhase::Idle};
    SocketId source_socket; // Socket drag originated from
    SocketDirection source_direction;
    Vec2 current_position;     // Current mouse position in world space
    SocketId candidate_target; // Socket under cursor (if compatible)
    bool compatible{false};    // Whether candidate_target is a valid connection

    /// Begin a link drag from a socket.
    void begin(SocketId socket, SocketDirection direction, Vec2 start_pos)
    {
        phase = LinkDragPhase::Dragging;
        source_socket = socket;
        source_direction = direction;
        current_position = start_pos;
        candidate_target = {};
        compatible = false;
    }

    /// Update the drag position and optional hover candidate.
    void update(Vec2 world_pos, SocketId hover_socket = {}, bool is_compatible = false)
    {
        current_position = world_pos;
        candidate_target = hover_socket;
        compatible = is_compatible;
        phase = (hover_socket.is_valid() && is_compatible) ? LinkDragPhase::OverTarget
                                                           : LinkDragPhase::Dragging;
    }

    /// Complete the drag — link can be created.
    void complete()
    {
        phase = LinkDragPhase::Completed;
    }

    /// Cancel the drag — no link created.
    void cancel()
    {
        phase = LinkDragPhase::Cancelled;
        candidate_target = {};
        compatible = false;
    }

    /// Reset to idle.
    void reset()
    {
        *this = {};
    }

    [[nodiscard]] auto is_active() const noexcept -> bool
    {
        return phase == LinkDragPhase::Dragging || phase == LinkDragPhase::OverTarget;
    }
};

} // namespace markamp::node_editor
