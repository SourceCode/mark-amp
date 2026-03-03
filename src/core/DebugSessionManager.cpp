#include "DebugSessionManager.h"

#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

DebugSessionManager::DebugSessionManager(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

void DebugSessionManager::start_session(const LaunchConfig& config)
{
    DebugSession session;
    session.state = DebugState::kStarting;
    session.config = config;
    session.session_id = config.name;

    if (sessions_.empty())
    {
        sessions_.push_back(std::move(session));
        active_session_index_ = 0;
    }
    else
    {
        // Replace inactive session or add new one
        sessions_[active_session_index_] = std::move(session);
    }

    MARKAMP_LOG_INFO("Debug session started: {}", config.name);
    publish_state_change();
}

void DebugSessionManager::set_state(DebugState new_state)
{
    if (!sessions_.empty())
    {
        sessions_[active_session_index_].state = new_state;
        publish_state_change();
    }
}

void DebugSessionManager::on_stopped(const std::string& file, int line, const std::string& reason)
{
    if (!sessions_.empty())
    {
        auto& session = sessions_[active_session_index_];
        session.state = DebugState::kPaused;
        session.stopped_file = file;
        session.stopped_line = line;
        session.stopped_reason = reason;

        MARKAMP_LOG_INFO("Debug stopped: {}:{} ({})", file, line, reason);
        publish_state_change();
    }
}

void DebugSessionManager::end_session()
{
    if (!sessions_.empty())
    {
        auto& session = sessions_[active_session_index_];
        MARKAMP_LOG_INFO("Debug session ended: {}", session.config.name);
        session.state = DebugState::kInactive;
        publish_state_change();
    }
}

auto DebugSessionManager::state() const -> DebugState
{
    if (sessions_.empty())
    {
        return DebugState::kInactive;
    }
    return sessions_[active_session_index_].state;
}

auto DebugSessionManager::session() const -> const DebugSession&
{
    return sessions_[active_session_index_];
}

auto DebugSessionManager::is_active() const -> bool
{
    return state() != DebugState::kInactive && state() != DebugState::kStopped;
}

auto DebugSessionManager::sessions() const -> const std::vector<DebugSession>&
{
    return sessions_;
}

void DebugSessionManager::add_session(const LaunchConfig& config)
{
    DebugSession session;
    session.state = DebugState::kStarting;
    session.config = config;
    session.session_id = config.name;
    sessions_.push_back(std::move(session));
    MARKAMP_LOG_INFO("Additional debug session added: {}", config.name);
}

void DebugSessionManager::select_session(const std::string& session_id)
{
    for (std::size_t idx = 0; idx < sessions_.size(); ++idx)
    {
        if (sessions_[idx].session_id == session_id)
        {
            active_session_index_ = idx;
            MARKAMP_LOG_INFO("Active debug session switched to: {}", session_id);
            return;
        }
    }
    MARKAMP_LOG_WARN("Debug session not found: {}", session_id);
}

void DebugSessionManager::publish_state_change()
{
    if (!sessions_.empty())
    {
        events::DebugStateChangedEvent state_event;
        state_event.session_id = sessions_[active_session_index_].session_id;
        event_bus_.publish(state_event);
    }
}

} // namespace markamp::core
