/// @file CanvasSessionContext.cpp
/// @brief P07-T03: Canvas session context for comments, facilitation, and collab.

#include "CanvasSessionContext.h"

#include "EventBus.h"
#include "Events.h"
#include "Logger.h"

#include <algorithm>

namespace markamp::core
{

CanvasSessionContext::CanvasSessionContext(EventBus& bus)
    : event_bus_(bus)
{
}

void CanvasSessionContext::set_board(const std::string& board_id)
{
    if (board_id == board_id_)
    {
        return;
    }

    // Clear previous session if switching boards
    if (session_active_)
    {
        leave_session();
    }

    board_id_ = board_id;
    MARKAMP_LOG_INFO("Canvas session board: {}", board_id);
}

void CanvasSessionContext::clear()
{
    if (session_active_)
    {
        leave_session();
    }
    board_id_.clear();
    MARKAMP_LOG_DEBUG("Canvas session context cleared");
}

void CanvasSessionContext::join_session(const std::string& session_id)
{
    session_id_ = session_id;
    session_active_ = true;

    event_bus_.publish(events::NotificationEvent{
        "Joined collaboration session",
        events::NotificationLevel::Success, 2000});

    MARKAMP_LOG_INFO("Joined session: {}", session_id);
}

void CanvasSessionContext::leave_session()
{
    if (!session_active_)
    {
        return;
    }

    MARKAMP_LOG_INFO("Left session: {}", session_id_);
    session_id_.clear();
    session_active_ = false;
    participants_.clear();

    event_bus_.publish(events::NotificationEvent{
        "Left collaboration session",
        events::NotificationLevel::Info, 2000});
}

void CanvasSessionContext::add_participant(const SessionParticipant& participant)
{
    participants_.push_back(participant);
    MARKAMP_LOG_DEBUG("Participant added: {}", participant.display_name);
}

void CanvasSessionContext::remove_participant(const std::string& user_id)
{
    auto iter = std::remove_if(participants_.begin(), participants_.end(),
                               [&user_id](const SessionParticipant& participant)
                               { return participant.user_id == user_id; });
    if (iter != participants_.end())
    {
        MARKAMP_LOG_DEBUG("Participant removed: {}", user_id);
        participants_.erase(iter, participants_.end());
    }
}

} // namespace markamp::core
