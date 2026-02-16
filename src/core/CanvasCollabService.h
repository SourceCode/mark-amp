#pragma once

#include "core/EventBus.h"

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ── Participant ────────────────────────────────────────────────────

/// Connection status for a remote participant.
enum class PresenceStatus : uint8_t
{
    kActive,
    kIdle,
    kAway,
    kDisconnected
};

/// Describes a single participant in a collaborative session.
struct Participant
{
    std::string participant_id;
    std::string display_name;
    std::string avatar_url;
    PresenceStatus status{PresenceStatus::kActive};
    double cursor_x{0.0};
    double cursor_y{0.0};
    std::vector<std::string> selected_object_ids;
};

// ── Vote ───────────────────────────────────────────────────────────

/// Tracks a collaborative vote running during a session.
struct CollabVote
{
    std::string vote_id;
    std::string topic;
    std::string initiator_id;
    std::vector<std::string> options;
    std::unordered_map<std::string, std::string> votes; ///< participant_id → option
    bool is_closed{false};
};

// ── Timer ──────────────────────────────────────────────────────────

/// Tracks a facilitation timer.
struct CollabTimer
{
    std::string timer_id;
    int duration_seconds{0};
    int remaining_seconds{0};
    bool is_running{false};
    bool is_expired{false};
};

// ── CanvasCollabService ────────────────────────────────────────────

/// Manages collaborative board sessions: participants, presence, cursor
/// positions, votes, timers, and conflict-safe object patch operations.
///
/// Publishes collaboration events via the injected EventBus so that
/// UI layers (collab bar, overlays, panels) can react.
class CanvasCollabService
{
public:
    explicit CanvasCollabService(EventBus& event_bus);
    ~CanvasCollabService();

    // Rule-of-five: non-copyable, non-movable (owns subscriptions)
    CanvasCollabService(const CanvasCollabService&) = delete;
    auto operator=(const CanvasCollabService&) -> CanvasCollabService& = delete;
    CanvasCollabService(CanvasCollabService&&) = delete;
    auto operator=(CanvasCollabService&&) -> CanvasCollabService& = delete;

    // ── Session lifecycle ──────────────────────────────────────────

    /// Start or join a collaborative session for the given board.
    auto join_session(const std::string& session_id,
                      const std::string& local_participant_id,
                      const std::string& display_name) -> void;

    /// Leave the current session.
    auto leave_session() -> void;

    /// Is there an active session?
    [[nodiscard]] auto is_in_session() const -> bool;
    [[nodiscard]] auto session_id() const -> const std::string&;
    [[nodiscard]] auto local_participant_id() const -> const std::string&;

    // ── Participants ───────────────────────────────────────────────

    /// Get all participants (including local).
    [[nodiscard]] auto participants() const -> const std::unordered_map<std::string, Participant>&;

    /// Look up a single participant.
    [[nodiscard]] auto find_participant(const std::string& participant_id) const
        -> const Participant*;

    /// Number of active participants.
    [[nodiscard]] auto participant_count() const -> size_t;

    // ── Presence / cursor ──────────────────────────────────────────

    /// Update local cursor position and broadcast.
    auto update_local_cursor(double cursor_x, double cursor_y) -> void;

    /// Update local selection and broadcast.
    auto update_local_selection(const std::vector<std::string>& object_ids) -> void;

    /// Process a remote cursor update.
    auto handle_remote_cursor(const std::string& participant_id, double cursor_x, double cursor_y)
        -> void;

    /// Process a remote selection change.
    auto handle_remote_selection(const std::string& participant_id,
                                 const std::vector<std::string>& object_ids) -> void;

    // ── Object patching ────────────────────────────────────────────

    /// Apply a local patch (enqueues for broadcast).
    auto apply_local_patch(const std::string& object_id,
                           const std::string& patch_type,
                           const std::string& patch_json) -> void;

    /// Apply a received remote patch (conflict-safe merge).
    auto apply_remote_patch(const std::string& participant_id,
                            const std::string& object_id,
                            const std::string& patch_type,
                            const std::string& patch_json) -> void;

    // ── Votes ──────────────────────────────────────────────────────

    /// Start a new vote.
    auto start_vote(const std::string& topic, const std::vector<std::string>& options)
        -> std::string;

    /// Cast a vote (local user).
    auto cast_vote(const std::string& vote_id, const std::string& option) -> void;

    /// Close a vote (initiator only).
    auto close_vote(const std::string& vote_id) -> void;

    [[nodiscard]] auto active_vote() const -> const CollabVote*;

    // ── Timer ──────────────────────────────────────────────────────

    /// Start a facilitation timer.
    auto start_timer(int duration_seconds) -> std::string;

    /// Stop the running timer.
    auto stop_timer(const std::string& timer_id) -> void;

    /// Tick the timer (called by a periodic callback).
    auto tick_timer() -> void;

    [[nodiscard]] auto active_timer() const -> const CollabTimer*;

private:
    EventBus& event_bus_;
    std::string session_id_;
    std::string local_participant_id_;
    bool in_session_{false};

    std::unordered_map<std::string, Participant> participants_;

    CollabVote active_vote_;
    bool has_active_vote_{false};

    CollabTimer active_timer_;
    bool has_active_timer_{false};

    int next_vote_id_{1};
    int next_timer_id_{1};

    /// Track event subscriptions.
    std::vector<Subscription> subscriptions_;

    /// Wire EventBus subscriptions for remote events.
    auto subscribe_to_events() -> void;

    /// Add or update a participant entry.
    auto upsert_participant(const std::string& participant_id,
                            const std::string& display_name,
                            const std::string& avatar_url) -> void;

    /// Remove a participant entry.
    auto remove_participant(const std::string& participant_id) -> void;
};

} // namespace markamp::core
