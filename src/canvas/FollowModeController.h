// ============================================================================
// File: src/canvas/FollowModeController.h
// Phase 13: Canvas Collaboration — follow mode and presenter mode
// ============================================================================
#pragma once

#include "canvas/CanvasTypes.h"
#include "core/EventBus.h"

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Viewport state for follow mode synchronization.
struct ViewportState
{
    Point2D center{0.0, 0.0}; ///< Center of viewport in world space
    double zoom{1.0};         ///< Current zoom level
    double width{0.0};        ///< Viewport width in world units
    double height{0.0};       ///< Viewport height in world units
};

/// Follow mode status.
enum class FollowStatus : uint8_t
{
    kNone,      ///< Not following anyone
    kFollowing, ///< Following a specific participant
    kPresenting ///< Broadcasting own viewport (presenter mode)
};

/// Configuration for follow mode behavior.
struct FollowModeConfig
{
    double viewport_lerp_speed{0.1};         ///< Smooth viewport interpolation
    double zoom_lerp_speed{0.08};            ///< Smooth zoom interpolation
    bool break_on_pan{true};                 ///< Stop following on manual pan
    bool break_on_zoom{true};                ///< Stop following on manual zoom
    bool break_on_object_interaction{false}; ///< Stop following on object click
    double viewport_dead_zone{5.0};          ///< Ignore viewport changes smaller than this
};

/// Controls follow-mode and presenter-mode for collaborative canvas sessions.
///
/// Features:
///   - Follow a remote participant's viewport (pan & zoom sync)
///   - Presenter mode: one user broadcasts viewport to all followers
///   - Smooth viewport interpolation for comfortable following
///   - Break-on-interact: automatically unfollow on manual input
class FollowModeController
{
public:
    explicit FollowModeController(core::EventBus& event_bus);

    // ── Follow Mode ───────────────────────────────────────────────

    /// Start following a participant's viewport.
    auto follow(const std::string& participant_id) -> void;

    /// Stop following.
    auto unfollow() -> void;

    /// Is currently following someone?
    [[nodiscard]] auto is_following() const -> bool;

    /// Who is being followed.
    [[nodiscard]] auto following_id() const -> const std::string&;

    // ── Presenter Mode ────────────────────────────────────────────

    /// Start presenting (broadcast own viewport to all).
    auto start_presenting() -> void;

    /// Stop presenting.
    auto stop_presenting() -> void;

    /// Is currently in presenter mode?
    [[nodiscard]] auto is_presenting() const -> bool;

    // ── Viewport Sync ─────────────────────────────────────────────

    /// Receive a remote viewport update from the followed participant.
    auto handle_remote_viewport(const std::string& participant_id, const ViewportState& viewport)
        -> void;

    /// Broadcast the local viewport (called when presenting or when viewport changes).
    auto broadcast_viewport(const ViewportState& viewport) -> void;

    /// Get the target viewport to animate toward.
    [[nodiscard]] auto target_viewport() const -> const ViewportState&;

    /// Advance interpolation. Call once per frame. Returns interpolated viewport.
    auto tick(double delta_seconds) -> ViewportState;

    // ── Break-on-Interact ─────────────────────────────────────────

    /// Notify that the user panned manually. May break follow mode.
    auto on_user_pan() -> void;

    /// Notify that the user zoomed manually. May break follow mode.
    auto on_user_zoom() -> void;

    /// Notify that the user interacted with an object.
    auto on_user_object_interaction() -> void;

    // ── Status ────────────────────────────────────────────────────

    [[nodiscard]] auto status() const -> FollowStatus;

    /// Callback when follow status changes.
    using StatusChangeCallback = std::function<void(FollowStatus new_status)>;
    auto set_on_status_change(StatusChangeCallback callback) -> void;

    // ── Configuration ─────────────────────────────────────────────

    [[nodiscard]] auto config() const -> const FollowModeConfig&;
    auto set_config(const FollowModeConfig& config) -> void;

    /// Get a human-readable status name.
    [[nodiscard]] static auto status_name(FollowStatus follow_status) -> std::string;

private:
    core::EventBus& event_bus_;
    FollowModeConfig config_;
    FollowStatus status_{FollowStatus::kNone};
    std::string following_id_;

    ViewportState current_viewport_;
    ViewportState target_viewport_;

    StatusChangeCallback on_status_change_;

    /// Emit status change notification.
    auto notify_status_change(FollowStatus new_status) -> void;

    /// Lerp between viewports.
    static auto lerp_viewport(ViewportState& current,
                              const ViewportState& target,
                              double position_factor,
                              double zoom_factor) -> void;
};

} // namespace markamp::canvas
