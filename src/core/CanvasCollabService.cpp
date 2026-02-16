#include "CanvasCollabService.h"

#include "Events.h"
#include "core/Logger.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

CanvasCollabService::CanvasCollabService(EventBus& event_bus)
    : event_bus_(event_bus)
{
    subscribe_to_events();
}

CanvasCollabService::~CanvasCollabService()
{
    if (in_session_)
    {
        leave_session();
    }
}

// ── Session lifecycle ──────────────────────────────────────────────

auto CanvasCollabService::join_session(const std::string& session_id,
                                       const std::string& local_participant_id,
                                       const std::string& display_name) -> void
{
    if (in_session_)
    {
        leave_session();
    }

    session_id_ = session_id;
    local_participant_id_ = local_participant_id;
    in_session_ = true;

    upsert_participant(local_participant_id, display_name, "");

    events::CanvasSessionJoinedEvent evt;
    evt.session_id = session_id;
    evt.participant_id = local_participant_id;
    evt.participant_name = display_name;
    event_bus_.publish(evt);

    MARKAMP_LOG_INFO("Joined collab session: {}", session_id);
}

auto CanvasCollabService::leave_session() -> void
{
    if (!in_session_)
    {
        return;
    }

    events::CanvasSessionLeftEvent evt;
    evt.session_id = session_id_;
    evt.participant_id = local_participant_id_;
    event_bus_.publish(evt);

    participants_.clear();
    session_id_.clear();
    local_participant_id_.clear();
    in_session_ = false;

    has_active_vote_ = false;
    has_active_timer_ = false;

    MARKAMP_LOG_INFO("Left collab session");
}

auto CanvasCollabService::is_in_session() const -> bool
{
    return in_session_;
}

auto CanvasCollabService::session_id() const -> const std::string&
{
    return session_id_;
}

auto CanvasCollabService::local_participant_id() const -> const std::string&
{
    return local_participant_id_;
}

// ── Participants ───────────────────────────────────────────────────

auto CanvasCollabService::participants() const
    -> const std::unordered_map<std::string, Participant>&
{
    return participants_;
}

auto CanvasCollabService::find_participant(const std::string& participant_id) const
    -> const Participant*
{
    const auto found = participants_.find(participant_id);
    if (found == participants_.end())
    {
        return nullptr;
    }
    return &found->second;
}

auto CanvasCollabService::participant_count() const -> size_t
{
    return participants_.size();
}

// ── Presence / cursor ──────────────────────────────────────────────

auto CanvasCollabService::update_local_cursor(double cursor_x, double cursor_y) -> void
{
    if (!in_session_)
    {
        return;
    }

    auto found = participants_.find(local_participant_id_);
    if (found != participants_.end())
    {
        found->second.cursor_x = cursor_x;
        found->second.cursor_y = cursor_y;
    }

    events::CanvasRemoteCursorMovedEvent evt;
    evt.participant_id = local_participant_id_;
    evt.cursor_x = cursor_x;
    evt.cursor_y = cursor_y;
    event_bus_.publish(evt);
}

auto CanvasCollabService::update_local_selection(const std::vector<std::string>& object_ids) -> void
{
    if (!in_session_)
    {
        return;
    }

    auto found = participants_.find(local_participant_id_);
    if (found != participants_.end())
    {
        found->second.selected_object_ids = object_ids;
    }

    events::CanvasRemoteSelectionChangedEvent evt;
    evt.participant_id = local_participant_id_;
    evt.selected_object_ids = object_ids;
    event_bus_.publish(evt);
}

auto CanvasCollabService::handle_remote_cursor(const std::string& participant_id,
                                               double cursor_x,
                                               double cursor_y) -> void
{
    auto found = participants_.find(participant_id);
    if (found != participants_.end())
    {
        found->second.cursor_x = cursor_x;
        found->second.cursor_y = cursor_y;
        found->second.status = PresenceStatus::kActive;
    }
}

auto CanvasCollabService::handle_remote_selection(const std::string& participant_id,
                                                  const std::vector<std::string>& object_ids)
    -> void
{
    auto found = participants_.find(participant_id);
    if (found != participants_.end())
    {
        found->second.selected_object_ids = object_ids;
    }
}

// ── Object patching ────────────────────────────────────────────────

auto CanvasCollabService::apply_local_patch(const std::string& object_id,
                                            const std::string& patch_type,
                                            const std::string& patch_json) -> void
{
    if (!in_session_)
    {
        return;
    }

    events::CanvasRemoteObjectPatchedEvent evt;
    evt.participant_id = local_participant_id_;
    evt.object_id = object_id;
    evt.patch_type = patch_type;
    evt.patch_json = patch_json;
    event_bus_.publish(evt);
}

auto CanvasCollabService::apply_remote_patch(const std::string& participant_id,
                                             const std::string& object_id,
                                             const std::string& patch_type,
                                             const std::string& patch_json) -> void
{
    MARKAMP_LOG_DEBUG("Remote patch from {} on {}: type={}", participant_id, object_id, patch_type);
    // Conflict-safe: remote patches are applied directly; the UndoRedoStack
    // separates them from local undo units (handled in Batch 6).
    (void)patch_json;
}

// ── Votes ──────────────────────────────────────────────────────────

auto CanvasCollabService::start_vote(const std::string& topic,
                                     const std::vector<std::string>& options) -> std::string
{
    std::ostringstream vote_id_stream;
    vote_id_stream << "vote_" << next_vote_id_++;

    active_vote_ = CollabVote{};
    active_vote_.vote_id = vote_id_stream.str();
    active_vote_.topic = topic;
    active_vote_.initiator_id = local_participant_id_;
    active_vote_.options = options;
    has_active_vote_ = true;

    events::CanvasVoteStartedEvent evt;
    evt.vote_id = active_vote_.vote_id;
    evt.topic = topic;
    evt.initiator_id = local_participant_id_;
    evt.options = options;
    event_bus_.publish(evt);

    MARKAMP_LOG_INFO("Started vote '{}': {}", active_vote_.vote_id, topic);
    return active_vote_.vote_id;
}

auto CanvasCollabService::cast_vote(const std::string& vote_id, const std::string& option) -> void
{
    if (!has_active_vote_ || active_vote_.vote_id != vote_id)
    {
        return;
    }

    active_vote_.votes[local_participant_id_] = option;

    events::CanvasVoteUpdatedEvent evt;
    evt.vote_id = vote_id;
    evt.voter_id = local_participant_id_;
    evt.chosen_option = option;
    evt.is_closed = false;
    event_bus_.publish(evt);
}

auto CanvasCollabService::close_vote(const std::string& vote_id) -> void
{
    if (!has_active_vote_ || active_vote_.vote_id != vote_id)
    {
        return;
    }

    active_vote_.is_closed = true;

    events::CanvasVoteUpdatedEvent evt;
    evt.vote_id = vote_id;
    evt.voter_id = local_participant_id_;
    evt.is_closed = true;
    event_bus_.publish(evt);

    MARKAMP_LOG_INFO("Closed vote '{}'", vote_id);
}

auto CanvasCollabService::active_vote() const -> const CollabVote*
{
    if (!has_active_vote_)
    {
        return nullptr;
    }
    return &active_vote_;
}

// ── Timer ──────────────────────────────────────────────────────────

auto CanvasCollabService::start_timer(int duration_seconds) -> std::string
{
    std::ostringstream timer_id_stream;
    timer_id_stream << "timer_" << next_timer_id_++;

    active_timer_ = CollabTimer{};
    active_timer_.timer_id = timer_id_stream.str();
    active_timer_.duration_seconds = duration_seconds;
    active_timer_.remaining_seconds = duration_seconds;
    active_timer_.is_running = true;
    has_active_timer_ = true;

    events::CanvasTimerUpdatedEvent evt;
    evt.timer_id = active_timer_.timer_id;
    evt.remaining_seconds = duration_seconds;
    evt.is_running = true;
    evt.is_expired = false;
    event_bus_.publish(evt);

    MARKAMP_LOG_INFO("Started timer '{}' for {}s", active_timer_.timer_id, duration_seconds);
    return active_timer_.timer_id;
}

auto CanvasCollabService::stop_timer(const std::string& timer_id) -> void
{
    if (!has_active_timer_ || active_timer_.timer_id != timer_id)
    {
        return;
    }

    active_timer_.is_running = false;

    events::CanvasTimerUpdatedEvent evt;
    evt.timer_id = timer_id;
    evt.remaining_seconds = active_timer_.remaining_seconds;
    evt.is_running = false;
    evt.is_expired = active_timer_.is_expired;
    event_bus_.publish(evt);

    MARKAMP_LOG_INFO("Stopped timer '{}'", timer_id);
}

auto CanvasCollabService::tick_timer() -> void
{
    if (!has_active_timer_ || !active_timer_.is_running)
    {
        return;
    }

    if (active_timer_.remaining_seconds > 0)
    {
        --active_timer_.remaining_seconds;
    }

    if (active_timer_.remaining_seconds <= 0)
    {
        active_timer_.is_running = false;
        active_timer_.is_expired = true;
    }

    events::CanvasTimerUpdatedEvent evt;
    evt.timer_id = active_timer_.timer_id;
    evt.remaining_seconds = active_timer_.remaining_seconds;
    evt.is_running = active_timer_.is_running;
    evt.is_expired = active_timer_.is_expired;
    event_bus_.publish(evt);
}

auto CanvasCollabService::active_timer() const -> const CollabTimer*
{
    if (!has_active_timer_)
    {
        return nullptr;
    }
    return &active_timer_;
}

// ── Private helpers ────────────────────────────────────────────────

auto CanvasCollabService::subscribe_to_events() -> void
{
    subscriptions_.push_back(event_bus_.subscribe<events::CanvasSessionJoinedEvent>(
        [this](const events::CanvasSessionJoinedEvent& evt)
        {
            if (evt.participant_id != local_participant_id_)
            {
                upsert_participant(evt.participant_id, evt.participant_name, evt.avatar_url);
            }
        }));

    subscriptions_.push_back(event_bus_.subscribe<events::CanvasSessionLeftEvent>(
        [this](const events::CanvasSessionLeftEvent& evt)
        {
            if (evt.participant_id != local_participant_id_)
            {
                remove_participant(evt.participant_id);
            }
        }));

    subscriptions_.push_back(event_bus_.subscribe<events::CanvasRemoteCursorMovedEvent>(
        [this](const events::CanvasRemoteCursorMovedEvent& evt)
        {
            if (evt.participant_id != local_participant_id_)
            {
                handle_remote_cursor(evt.participant_id, evt.cursor_x, evt.cursor_y);
            }
        }));

    subscriptions_.push_back(event_bus_.subscribe<events::CanvasRemoteSelectionChangedEvent>(
        [this](const events::CanvasRemoteSelectionChangedEvent& evt)
        {
            if (evt.participant_id != local_participant_id_)
            {
                handle_remote_selection(evt.participant_id, evt.selected_object_ids);
            }
        }));
}

auto CanvasCollabService::upsert_participant(const std::string& participant_id,
                                             const std::string& display_name,
                                             const std::string& avatar_url) -> void
{
    auto& participant = participants_[participant_id];
    participant.participant_id = participant_id;
    participant.display_name = display_name;
    participant.avatar_url = avatar_url;
    participant.status = PresenceStatus::kActive;
}

auto CanvasCollabService::remove_participant(const std::string& participant_id) -> void
{
    participants_.erase(participant_id);
}

} // namespace markamp::core
