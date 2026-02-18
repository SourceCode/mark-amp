// ============================================================================
// File: src/canvas/FollowModeController.cpp
// Phase 13: Canvas Collaboration — follow mode and presenter mode
// ============================================================================
#include "canvas/FollowModeController.h"

#include "core/Events.h"

#include <cmath>

namespace markamp::canvas
{

FollowModeController::FollowModeController(core::EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ── Follow Mode ───────────────────────────────────────────────────

auto FollowModeController::follow(const std::string& participant_id) -> void
{
    if (status_ == FollowStatus::kPresenting)
    {
        stop_presenting();
    }

    following_id_ = participant_id;
    notify_status_change(FollowStatus::kFollowing);
}

auto FollowModeController::unfollow() -> void
{
    if (status_ != FollowStatus::kFollowing)
    {
        return;
    }

    following_id_.clear();
    notify_status_change(FollowStatus::kNone);
}

auto FollowModeController::is_following() const -> bool
{
    return status_ == FollowStatus::kFollowing;
}

auto FollowModeController::following_id() const -> const std::string&
{
    return following_id_;
}

// ── Presenter Mode ────────────────────────────────────────────────

auto FollowModeController::start_presenting() -> void
{
    if (status_ == FollowStatus::kFollowing)
    {
        unfollow();
    }

    notify_status_change(FollowStatus::kPresenting);
}

auto FollowModeController::stop_presenting() -> void
{
    if (status_ != FollowStatus::kPresenting)
    {
        return;
    }

    notify_status_change(FollowStatus::kNone);
}

auto FollowModeController::is_presenting() const -> bool
{
    return status_ == FollowStatus::kPresenting;
}

// ── Viewport Sync ─────────────────────────────────────────────────

auto FollowModeController::handle_remote_viewport(const std::string& participant_id,
                                                  const ViewportState& viewport) -> void
{
    // Only process if we're following this specific participant
    if (status_ != FollowStatus::kFollowing || participant_id != following_id_)
    {
        return;
    }

    // Check dead zone — ignore tiny changes
    const double dx = std::abs(viewport.center.x - target_viewport_.center.x);
    const double dy = std::abs(viewport.center.y - target_viewport_.center.y);
    if (dx < config_.viewport_dead_zone && dy < config_.viewport_dead_zone &&
        std::abs(viewport.zoom - target_viewport_.zoom) < 0.01)
    {
        return;
    }

    target_viewport_ = viewport;
}

auto FollowModeController::broadcast_viewport(const ViewportState& viewport) -> void
{
    if (status_ != FollowStatus::kPresenting)
    {
        return;
    }

    current_viewport_ = viewport;

    // Publish viewport event via EventBus
    core::events::CanvasFollowModeChangedEvent evt;
    evt.status = "presenting";
    evt.participant_id = "";
    event_bus_.publish(evt);
}

auto FollowModeController::target_viewport() const -> const ViewportState&
{
    return target_viewport_;
}

auto FollowModeController::tick(double /*delta_seconds*/) -> ViewportState
{
    if (status_ == FollowStatus::kFollowing)
    {
        lerp_viewport(current_viewport_,
                      target_viewport_,
                      config_.viewport_lerp_speed,
                      config_.zoom_lerp_speed);
    }
    return current_viewport_;
}

// ── Break-on-Interact ─────────────────────────────────────────────

auto FollowModeController::on_user_pan() -> void
{
    if (status_ == FollowStatus::kFollowing && config_.break_on_pan)
    {
        unfollow();
    }
}

auto FollowModeController::on_user_zoom() -> void
{
    if (status_ == FollowStatus::kFollowing && config_.break_on_zoom)
    {
        unfollow();
    }
}

auto FollowModeController::on_user_object_interaction() -> void
{
    if (status_ == FollowStatus::kFollowing && config_.break_on_object_interaction)
    {
        unfollow();
    }
}

// ── Status ────────────────────────────────────────────────────────

auto FollowModeController::status() const -> FollowStatus
{
    return status_;
}

auto FollowModeController::set_on_status_change(StatusChangeCallback callback) -> void
{
    on_status_change_ = std::move(callback);
}

// ── Configuration ─────────────────────────────────────────────────

auto FollowModeController::config() const -> const FollowModeConfig&
{
    return config_;
}

auto FollowModeController::set_config(const FollowModeConfig& config) -> void
{
    config_ = config;
}

auto FollowModeController::status_name(FollowStatus follow_status) -> std::string
{
    switch (follow_status)
    {
        case FollowStatus::kNone:
            return "Not Following";
        case FollowStatus::kFollowing:
            return "Following";
        case FollowStatus::kPresenting:
            return "Presenting";
    }
    return "Unknown";
}

// ── Private ───────────────────────────────────────────────────────

auto FollowModeController::notify_status_change(FollowStatus new_status) -> void
{
    status_ = new_status;
    if (on_status_change_)
    {
        on_status_change_(new_status);
    }
}

auto FollowModeController::lerp_viewport(ViewportState& current,
                                         const ViewportState& target,
                                         double position_factor,
                                         double zoom_factor) -> void
{
    current.center.x += (target.center.x - current.center.x) * position_factor;
    current.center.y += (target.center.y - current.center.y) * position_factor;
    current.zoom += (target.zoom - current.zoom) * zoom_factor;
    current.width += (target.width - current.width) * position_factor;
    current.height += (target.height - current.height) * position_factor;
}

} // namespace markamp::canvas
