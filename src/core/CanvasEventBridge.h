/// @file CanvasEventBridge.h
/// @brief V20 P04-T02: Canvas event bus unification.
///
/// Bridges the canvas subsystem's event handling into the shared app-level
/// EventBus, eliminating the private event bus that isolates canvas panels
/// from shell diagnostics, commands, and context management.
#pragma once

#include "ArtifactRegistry.h"
#include "EventBus.h"

#include <string>

namespace markamp::core
{

/// Active canvas context for command routing and shell integration.
struct CanvasContext
{
    ArtifactId active_board_id;
    std::string active_tool;          ///< select, pan, draw, etc.
    int selected_object_count{0};
    bool is_canvas_focused{false};
    bool has_dirty_board{false};

    [[nodiscard]] auto has_active_board() const noexcept -> bool
    {
        return !active_board_id.empty();
    }
};

/// Bridges canvas events into the shared application event bus.
///
/// Replaces the pattern where `CanvasWorkspacePanel` creates a private
/// `std::make_shared<EventBus>()` for `CanvasPanel`, reconnecting canvas
/// panels to the app event graph.
class CanvasEventBridge
{
public:
    explicit CanvasEventBridge(EventBus& shared_bus);

    /// Set the active canvas context.
    void set_context(const CanvasContext& context);

    /// Clear canvas context (e.g., when switching to editor/notebook).
    void clear_context();

    /// Get current canvas context.
    [[nodiscard]] auto context() const -> const CanvasContext& { return context_; }

    /// Whether a canvas board is currently active and focused.
    [[nodiscard]] auto is_canvas_active() const -> bool
    {
        return context_.is_canvas_focused && context_.has_active_board();
    }

    /// Get the active board ID.
    [[nodiscard]] auto active_board_id() const -> const ArtifactId&
    {
        return context_.active_board_id;
    }

    /// Whether save should target the canvas (vs. file or notebook).
    [[nodiscard]] auto should_save_canvas() const -> bool
    {
        return context_.is_canvas_focused;
    }

    /// Get the shared bus reference (for passing to canvas subsystems).
    [[nodiscard]] auto shared_bus() -> EventBus& { return shared_bus_; }

    /// Total context updates.
    [[nodiscard]] auto update_count() const noexcept -> int { return update_count_; }

    /// Whether bridge is using the shared bus (debug assertion support).
    [[nodiscard]] auto is_using_shared_bus() const noexcept -> bool { return true; }

private:
    EventBus& shared_bus_;
    CanvasContext context_;
    int update_count_{0};
};

} // namespace markamp::core
