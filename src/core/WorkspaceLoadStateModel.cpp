/// @file WorkspaceLoadStateModel.cpp
/// @brief P05-T05: Workspace load state machine.

#include "WorkspaceLoadStateModel.h"

#include "EventBus.h"
#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

WorkspaceLoadStateModel::WorkspaceLoadStateModel(EventBus& bus)
    : event_bus_(bus)
{
}

void WorkspaceLoadStateModel::set_state(WorkspaceLoadState state, const std::string& message)
{
    if (state == state_ && message == message_)
    {
        return;
    }

    state_ = state;
    message_ = message;

    // Publish state change event
    events::WorkspaceLoadStateChangedEvent evt;
    evt.state = static_cast<int>(state);
    evt.message = message;
    event_bus_.publish(evt);

    // Provide user feedback for specific states
    switch (state)
    {
    case WorkspaceLoadState::kLoading:
        MARKAMP_LOG_INFO("Workspace loading...");
        break;
    case WorkspaceLoadState::kReady:
        MARKAMP_LOG_INFO("Workspace ready");
        break;
    case WorkspaceLoadState::kFailed:
        event_bus_.publish(events::NotificationEvent{
            "Workspace load failed: " + message,
            events::NotificationLevel::Error, 5000});
        MARKAMP_LOG_ERROR("Workspace load failed: {}", message);
        break;
    case WorkspaceLoadState::kEmpty:
        MARKAMP_LOG_INFO("Workspace is empty");
        break;
    case WorkspaceLoadState::kIdle:
        break;
    }
}

} // namespace markamp::core
