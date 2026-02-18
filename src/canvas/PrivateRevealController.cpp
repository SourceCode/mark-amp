// ============================================================================
// File: src/canvas/PrivateRevealController.cpp
// Phase 13: Canvas Collaboration — private-then-reveal facilitation
// ============================================================================
#include "canvas/PrivateRevealController.h"

#include "core/Events.h"

#include <algorithm>
#include <fmt/format.h>

namespace markamp::canvas
{

PrivateRevealController::PrivateRevealController(Board& board, core::EventBus& event_bus)
    : board_(board)
    , event_bus_(event_bus)
{
}

// ── Round Lifecycle ───────────────────────────────────────────────

auto PrivateRevealController::start_private_round(const std::string& host_id,
                                                  const std::string& topic) -> std::string
{
    RevealRound round;
    round.round_id = fmt::format("round_{}", next_round_id_++);
    round.topic = topic;
    round.initiated_by = host_id;
    round.state = RevealRoundState::kPrivatePhase;

    rounds_.push_back(std::move(round));

    core::events::CanvasPrivateRoundStartedEvent evt;
    evt.round_id = rounds_.back().round_id;
    evt.host_id = host_id;
    evt.topic = topic;
    event_bus_.publish(evt);

    return rounds_.back().round_id;
}

auto PrivateRevealController::submit_private_object(const std::string& participant_id,
                                                    ObjectId obj_id) -> bool
{
    auto* active_round = current_round_mut();
    if (active_round == nullptr || active_round->state != RevealRoundState::kPrivatePhase)
    {
        return false;
    }

    active_round->participant_objects[participant_id].push_back(obj_id);
    active_round->all_object_ids.push_back(obj_id);

    // Hide the object on the board (only owner sees it)
    auto* obj = board_.get_object_mut(obj_id);
    if (obj != nullptr)
    {
        obj->set_visible(false);
    }

    return true;
}

auto PrivateRevealController::reveal_all(const std::string& host_id) -> RevealResult
{
    auto* active_round = current_round_mut();
    if (active_round == nullptr)
    {
        return {false, 0, "No active round"};
    }

    if (active_round->state != RevealRoundState::kPrivatePhase)
    {
        return {false, 0, "Round is not in private phase"};
    }

    if (active_round->initiated_by != host_id)
    {
        return {false, 0, "Only the host can reveal"};
    }

    // Make all private objects visible
    size_t revealed = 0;
    for (const auto object_id : active_round->all_object_ids)
    {
        auto* obj = board_.get_object_mut(object_id);
        if (obj != nullptr)
        {
            obj->set_visible(true);
            ++revealed;
        }
    }

    active_round->state = RevealRoundState::kRevealed;

    core::events::CanvasObjectsRevealedEvent evt;
    evt.round_id = active_round->round_id;
    evt.revealed_count = revealed;
    event_bus_.publish(evt);

    return {true, revealed, ""};
}

auto PrivateRevealController::complete_round(const std::string& host_id) -> bool
{
    auto* active_round = current_round_mut();
    if (active_round == nullptr || active_round->initiated_by != host_id)
    {
        return false;
    }

    active_round->state = RevealRoundState::kCompleted;
    return true;
}

// ── Query ─────────────────────────────────────────────────────────

auto PrivateRevealController::is_round_active() const -> bool
{
    if (rounds_.empty())
    {
        return false;
    }
    const auto& last = rounds_.back();
    return last.state == RevealRoundState::kPrivatePhase ||
           last.state == RevealRoundState::kRevealed;
}

auto PrivateRevealController::current_round() const -> const RevealRound*
{
    if (rounds_.empty())
    {
        return nullptr;
    }
    const auto& last = rounds_.back();
    if (last.state == RevealRoundState::kCompleted)
    {
        return nullptr;
    }
    return &last;
}

auto PrivateRevealController::round_history() const -> const std::vector<RevealRound>&
{
    return rounds_;
}

auto PrivateRevealController::round_count() const -> size_t
{
    return rounds_.size();
}

auto PrivateRevealController::is_object_private(ObjectId obj_id) const -> bool
{
    const auto* active_round = current_round();
    if (active_round == nullptr || active_round->state != RevealRoundState::kPrivatePhase)
    {
        return false;
    }

    return std::ranges::find(active_round->all_object_ids, obj_id) !=
           active_round->all_object_ids.end();
}

auto PrivateRevealController::object_owner(ObjectId obj_id) const -> std::string
{
    const auto* active_round = current_round();
    if (active_round == nullptr)
    {
        return "";
    }

    for (const auto& [pid, objects] : active_round->participant_objects)
    {
        if (std::ranges::find(objects, obj_id) != objects.end())
        {
            return pid;
        }
    }
    return "";
}

auto PrivateRevealController::participant_submission_count(const std::string& participant_id) const
    -> size_t
{
    const auto* active_round = current_round();
    if (active_round == nullptr)
    {
        return 0;
    }

    auto iter = active_round->participant_objects.find(participant_id);
    if (iter != active_round->participant_objects.end())
    {
        return iter->second.size();
    }
    return 0;
}

// ── Visibility ────────────────────────────────────────────────────

auto PrivateRevealController::is_visible_to(ObjectId obj_id,
                                            const std::string& participant_id) const -> bool
{
    const auto* active_round = current_round();
    if (active_round == nullptr || active_round->state != RevealRoundState::kPrivatePhase)
    {
        return true; // All visible when not in private phase
    }

    // Check if this is a private object
    if (!is_object_private(obj_id))
    {
        return true;
    }

    // Only the owner can see their own private objects
    return object_owner(obj_id) == participant_id;
}

// ── Utility ───────────────────────────────────────────────────────

auto PrivateRevealController::state_name(RevealRoundState state) -> std::string
{
    switch (state)
    {
        case RevealRoundState::kNotStarted:
            return "Not Started";
        case RevealRoundState::kPrivatePhase:
            return "Private Phase";
        case RevealRoundState::kRevealed:
            return "Revealed";
        case RevealRoundState::kCompleted:
            return "Completed";
    }
    return "Unknown";
}

// ── Private ───────────────────────────────────────────────────────

auto PrivateRevealController::current_round_mut() -> RevealRound*
{
    if (rounds_.empty())
    {
        return nullptr;
    }
    auto& last = rounds_.back();
    if (last.state == RevealRoundState::kCompleted)
    {
        return nullptr;
    }
    return &last;
}

} // namespace markamp::canvas
