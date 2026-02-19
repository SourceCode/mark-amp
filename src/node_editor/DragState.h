#pragma once

// V11 Phase 26: Generic Drag State Machine
// Tracks all types of drag interactions: node movement, box select, lasso, viewport pan.

#include "NodeEditorTypes.h"

#include <vector>

namespace markamp::node_editor
{

/// Type of drag interaction.
enum class DragKind : uint8_t
{
    None,
    MoveNodes,
    BoxSelect,
    LassoSelect,
    PanViewport,
    ResizeFrame
};

/// Phase of a drag interaction.
enum class DragPhase : uint8_t
{
    Idle,
    Active,
    Committed,
    Cancelled
};

/// Generic drag interaction state.
struct DragState
{
    DragKind kind{DragKind::None};
    DragPhase phase{DragPhase::Idle};
    Vec2 start_position;   // Where the drag started (world space)
    Vec2 current_position; // Current mouse position (world space)

    // Lasso mode accumulates points
    std::vector<Vec2> lasso_points;

    /// Begin a drag of the specified kind.
    void begin(DragKind drag_kind, Vec2 start_pos)
    {
        kind = drag_kind;
        phase = DragPhase::Active;
        start_position = start_pos;
        current_position = start_pos;
        lasso_points.clear();
        if (drag_kind == DragKind::LassoSelect)
        {
            lasso_points.push_back(start_pos);
        }
    }

    /// Update the drag position.
    void update(Vec2 world_pos)
    {
        current_position = world_pos;
        if (kind == DragKind::LassoSelect)
        {
            lasso_points.push_back(world_pos);
        }
    }

    /// Commit the drag (apply changes).
    void commit()
    {
        phase = DragPhase::Committed;
    }

    /// Cancel the drag (rollback).
    void cancel()
    {
        phase = DragPhase::Cancelled;
    }

    /// Reset to idle.
    void reset()
    {
        *this = {};
    }

    // --- Queries ---
    [[nodiscard]] auto is_active() const noexcept -> bool
    {
        return phase == DragPhase::Active;
    }

    [[nodiscard]] auto delta() const noexcept -> Vec2
    {
        return current_position - start_position;
    }

    /// The bounding rect of the drag (for box selection).
    [[nodiscard]] auto drag_rect() const noexcept -> Rect
    {
        const float kMinX =
            (start_position.x < current_position.x) ? start_position.x : current_position.x;
        const float kMinY =
            (start_position.y < current_position.y) ? start_position.y : current_position.y;
        const float kMaxX =
            (start_position.x > current_position.x) ? start_position.x : current_position.x;
        const float kMaxY =
            (start_position.y > current_position.y) ? start_position.y : current_position.y;
        return {kMinX, kMinY, kMaxX - kMinX, kMaxY - kMinY};
    }
};

} // namespace markamp::node_editor
