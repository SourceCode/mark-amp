// ============================================================================
// File: src/canvas/PrivateRevealController.h
// Phase 13: Canvas Collaboration — private-then-reveal facilitation
// ============================================================================
#pragma once

#include "canvas/Board.h"
#include "canvas/CanvasTypes.h"
#include "core/EventBus.h"

#include <chrono>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::canvas
{

/// State of a private-reveal round.
enum class RevealRoundState : uint8_t
{
    kNotStarted,
    kPrivatePhase, ///< Participants are creating objects privately
    kRevealed,     ///< All objects have been revealed
    kCompleted     ///< Round finalized
};

/// A single private-reveal round.
struct RevealRound
{
    std::string round_id;
    std::string topic;        ///< Facilitation prompt
    std::string initiated_by; ///< Host who started the round
    RevealRoundState state{RevealRoundState::kNotStarted};

    /// Objects submitted by each participant (hidden until reveal).
    std::unordered_map<std::string, std::vector<ObjectId>> participant_objects;

    /// All object IDs in this round.
    std::vector<ObjectId> all_object_ids;

    std::string started_at;  ///< ISO-8601
    std::string revealed_at; ///< ISO-8601

    /// Whether the round is in the private phase.
    [[nodiscard]] auto is_private() const noexcept -> bool
    {
        return state == RevealRoundState::kPrivatePhase;
    }

    /// Whether the round has been revealed.
    [[nodiscard]] auto is_revealed() const noexcept -> bool
    {
        return state == RevealRoundState::kRevealed || state == RevealRoundState::kCompleted;
    }

    /// Number of participants in this round.
    [[nodiscard]] auto participant_count() const noexcept -> std::size_t
    {
        return participant_objects.size();
    }

    /// Total objects submitted across all participants.
    [[nodiscard]] auto total_objects() const noexcept -> std::size_t
    {
        return all_object_ids.size();
    }

    // ── Round 3 Batch 7 (#61-64) ────────────────────────────────

    /// (#61) Whether the round has not yet started.
    [[nodiscard]] auto is_not_started() const noexcept -> bool
    {
        return state == RevealRoundState::kNotStarted;
    }

    /// (#62) Whether the round is finalized.
    [[nodiscard]] auto is_completed() const noexcept -> bool
    {
        return state == RevealRoundState::kCompleted;
    }

    /// (#63) Whether a topic prompt is set.
    [[nodiscard]] auto has_topic() const noexcept -> bool
    {
        return !topic.empty();
    }

    /// (#64) Whether a start timestamp is recorded.
    [[nodiscard]] auto has_started_at() const noexcept -> bool
    {
        return !started_at.empty();
    }
};

/// Result of a private-reveal operation.
struct RevealResult
{
    bool success{false};
    size_t objects_revealed{0};
    std::string error_message;

    /// Whether the reveal failed.
    [[nodiscard]] auto failed() const noexcept -> bool
    {
        return !success;
    }

    // ── Round 3 Batch 7 (#65-66) ────────────────────────────────

    /// (#65) Whether there's an error message.
    [[nodiscard]] auto has_error() const noexcept -> bool
    {
        return !error_message.empty();
    }

    /// (#66) Whether the reveal produced zero objects.
    [[nodiscard]] auto is_empty_reveal() const noexcept -> bool
    {
        return objects_revealed == 0;
    }
};

/// Controls private-then-reveal facilitation mode.
///
/// Features:
///   - Start a private round with a topic prompt
///   - Participants submit hidden objects only visible to themselves
///   - Host triggers simultaneous reveal — all objects become visible
///   - Multiple round support with history
class PrivateRevealController
{
public:
    PrivateRevealController(Board& board, core::EventBus& event_bus);

    // ── Round Lifecycle ───────────────────────────────────────────

    /// Start a new private round. Returns the round ID.
    auto start_private_round(const std::string& host_id, const std::string& topic) -> std::string;

    /// Submit a private object for the current round.
    auto submit_private_object(const std::string& participant_id, ObjectId obj_id) -> bool;

    /// Reveal all private objects simultaneously (host only).
    auto reveal_all(const std::string& host_id) -> RevealResult;

    /// Complete/finalize the current round.
    auto complete_round(const std::string& host_id) -> bool;

    // ── Query ─────────────────────────────────────────────────────

    /// Is there an active private round?
    [[nodiscard]] auto is_round_active() const -> bool;

    /// Get the current round, if any.
    [[nodiscard]] auto current_round() const -> const RevealRound*;

    /// Get round history.
    [[nodiscard]] auto round_history() const -> const std::vector<RevealRound>&;

    /// Get all rounds count.
    [[nodiscard]] auto round_count() const -> size_t;

    /// Check if a specific object is private (hidden from others).
    [[nodiscard]] auto is_object_private(ObjectId obj_id) const -> bool;

    /// Check if a specific object belongs to the given participant in the current round.
    [[nodiscard]] auto object_owner(ObjectId obj_id) const -> std::string;

    /// How many objects a participant has submitted in the current round.
    [[nodiscard]] auto participant_submission_count(const std::string& participant_id) const
        -> size_t;

    // ── Visibility ────────────────────────────────────────────────

    /// Should this object be visible to the given participant?
    [[nodiscard]] auto is_visible_to(ObjectId obj_id, const std::string& participant_id) const
        -> bool;

    // ── Utility ───────────────────────────────────────────────────

    /// Human-readable round state name.
    [[nodiscard]] static auto state_name(RevealRoundState state) -> std::string;

    /// Whether there are any completed rounds.
    [[nodiscard]] auto has_history() const noexcept -> bool
    {
        return rounds_.size() > 1 || (!rounds_.empty() && rounds_.back().state == RevealRoundState::kCompleted);
    }

private:
    Board& board_;
    core::EventBus& event_bus_;
    std::vector<RevealRound> rounds_;
    int next_round_id_{1};

    /// Get a mutable reference to the current round, or nullptr.
    auto current_round_mut() -> RevealRound*;
};

} // namespace markamp::canvas
