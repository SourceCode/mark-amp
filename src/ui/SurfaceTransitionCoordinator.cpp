/// @file SurfaceTransitionCoordinator.cpp
/// @brief V8 Phase 12 (Phase 36) — Smooth Surface Transition Pipeline implementation.

#include "ui/SurfaceTransitionCoordinator.h"

#include "core/Config.h"
#include "core/Events.h"
#include "core/Logger.h"

namespace markamp::ui
{

SurfaceTransitionCoordinator::SurfaceTransitionCoordinator(core::EventBus& event_bus,
                                                           core::Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
    (void)config_; // Will be used for transition timing budgets
}

auto SurfaceTransitionCoordinator::begin_transition(core::SurfaceKind from,
                                                    core::SurfaceKind to_surface,
                                                    const std::string& reason) -> bool
{
    if (transitioning_)
    {
        // Cancel current transition before starting a new one (rapid jump support)
        cancel("Superseded by new transition");
    }

    transitioning_ = true;
    from_ = from;
    to_ = to_surface;

    // Reset snapshot for new transition
    snapshot_ = TransitionSnapshot{};
    snapshot_.surface = from;

    // Emit start event
    core::events::SurfaceTransitionStartEvent evt;
    evt.from_surface = from;
    evt.to_surface = to_surface;
    evt.reason = reason;
    event_bus_.publish(evt);

    MARKAMP_LOG_INFO("Surface transition started: {} -> {}",
                     std::string(core::SurfaceLinkRouter::surface_name(from)),
                     std::string(core::SurfaceLinkRouter::surface_name(to_surface)));
    return true;
}

void SurfaceTransitionCoordinator::commit()
{
    if (!transitioning_)
    {
        MARKAMP_LOG_WARN("Attempted to commit transition but none in progress");
        return;
    }

    transitioning_ = false;
    ++completed_count_;

    // Emit complete event
    core::events::SurfaceTransitionCompleteEvent evt;
    evt.from_surface = from_;
    evt.to_surface = to_;
    event_bus_.publish(evt);

    MARKAMP_LOG_INFO("Surface transition completed: {} -> {}",
                     std::string(core::SurfaceLinkRouter::surface_name(from_)),
                     std::string(core::SurfaceLinkRouter::surface_name(to_)));
}

void SurfaceTransitionCoordinator::cancel(const std::string& reason)
{
    if (!transitioning_)
    {
        return;
    }

    transitioning_ = false;
    ++cancelled_count_;

    core::events::SurfaceTransitionCancelEvent evt;
    evt.reason = reason;
    event_bus_.publish(evt);

    MARKAMP_LOG_INFO("Surface transition cancelled: {}", reason);
}

auto SurfaceTransitionCoordinator::is_transitioning() const -> bool
{
    return transitioning_;
}

auto SurfaceTransitionCoordinator::snapshot() const -> const TransitionSnapshot&
{
    return snapshot_;
}

auto SurfaceTransitionCoordinator::from_surface() const -> core::SurfaceKind
{
    return from_;
}

auto SurfaceTransitionCoordinator::to_surface() const -> core::SurfaceKind
{
    return to_;
}

auto SurfaceTransitionCoordinator::completed_count() const -> int
{
    return completed_count_;
}

auto SurfaceTransitionCoordinator::cancelled_count() const -> int
{
    return cancelled_count_;
}

} // namespace markamp::ui
