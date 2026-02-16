/// @file SurfaceTransitionCoordinator.h
/// @brief V8 Phase 12 (Phase 36) — Smooth Surface Transition Pipeline.
/// Eliminates visual jumps and focus loss during surface transitions by
/// capturing pre-transition state, preloading target, and restoring focus.

#pragma once

#include "core/EventBus.h"
#include "core/SurfaceLink.h"

#include <string>

namespace markamp::core
{
class Config;
} // namespace markamp::core

namespace markamp::ui
{

/// Snapshot of surface state before a transition.
struct TransitionSnapshot
{
    int scroll_position{0};
    int cursor_line{0};
    int cursor_column{0};
    std::string selection_text;
    double zoom_level{1.0};
    std::string active_object_id; ///< Canvas: active object; Notebook: active cell
    core::SurfaceKind surface{core::SurfaceKind::kEditor};
};

/// Manages smooth transitions between surfaces.
/// Captures pre-transition state, orchestrates preload, executes transitions,
/// and restores focus/anchor in the target. Supports cancellation for rapid jumps.
class SurfaceTransitionCoordinator
{
public:
    SurfaceTransitionCoordinator(core::EventBus& event_bus, core::Config& config);

    /// Begin a surface transition. Captures current state and emits start event.
    /// Returns false if a transition is already in progress.
    auto begin_transition(core::SurfaceKind from,
                          core::SurfaceKind to_surface,
                          const std::string& reason = "") -> bool;

    /// Commit the transition after target is ready. Emits complete event.
    void commit();

    /// Cancel the current transition (e.g. due to rapid repeated jumps).
    void cancel(const std::string& reason = "");

    /// Is a transition currently in progress?
    [[nodiscard]] auto is_transitioning() const -> bool;

    /// Get the snapshot captured at transition start.
    [[nodiscard]] auto snapshot() const -> const TransitionSnapshot&;

    /// Get the source surface of the current transition.
    [[nodiscard]] auto from_surface() const -> core::SurfaceKind;

    /// Get the target surface of the current transition.
    [[nodiscard]] auto to_surface() const -> core::SurfaceKind;

    /// Count of completed transitions (for testing).
    [[nodiscard]] auto completed_count() const -> int;

    /// Count of cancelled transitions (for testing).
    [[nodiscard]] auto cancelled_count() const -> int;

private:
    core::EventBus& event_bus_;
    core::Config& config_;

    bool transitioning_{false};
    TransitionSnapshot snapshot_;
    core::SurfaceKind from_{core::SurfaceKind::kEditor};
    core::SurfaceKind to_{core::SurfaceKind::kEditor};

    int completed_count_{0};
    int cancelled_count_{0};
};

} // namespace markamp::ui
