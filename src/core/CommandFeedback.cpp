/// @file CommandFeedback.cpp
/// @brief P02-T05: Standardized command execution feedback.

#include "CommandFeedback.h"

#include "EventBus.h"
#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

CommandFeedbackHelper::CommandFeedbackHelper(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

void CommandFeedbackHelper::report_success(const std::string& command_id,
                                           const std::string& message)
{
    MARKAMP_LOG_DEBUG("Command '{}' succeeded: {}", command_id, message);
    // Publish a brief status bar notification (auto-dismiss)
    event_bus_.publish(events::NotificationEvent{
        message,
        events::NotificationLevel::Success,
        2000, // 2 seconds
    });
}

void CommandFeedbackHelper::report_no_op(const std::string& command_id, const std::string& reason)
{
    MARKAMP_LOG_DEBUG("Command '{}' no-op: {}", command_id, reason);
    // Lightweight feedback — short info notification
    event_bus_.publish(events::NotificationEvent{
        reason,
        events::NotificationLevel::Info,
        1500, // 1.5 seconds
    });
}

void CommandFeedbackHelper::report_error(const std::string& command_id, const std::string& error)
{
    MARKAMP_LOG_WARN("Command '{}' failed: {}", command_id, error);
    // Error notification — sticky (longer duration)
    event_bus_.publish(events::NotificationEvent{
        error,
        events::NotificationLevel::Error,
        5000, // 5 seconds
    });
}

void CommandFeedbackHelper::report_disabled(const std::string& command_id, const std::string& reason)
{
    MARKAMP_LOG_DEBUG("Command '{}' disabled: {}", command_id, reason);
    // Info notification with short duration
    event_bus_.publish(events::NotificationEvent{
        reason,
        events::NotificationLevel::Info,
        2000,
    });
}

void CommandFeedbackHelper::report(const std::string& command_id,
                                   CommandResult result,
                                   const std::string& message)
{
    switch (result)
    {
    case CommandResult::kSuccess:
        report_success(command_id, message);
        break;
    case CommandResult::kNoOp:
        report_no_op(command_id, message);
        break;
    case CommandResult::kError:
        report_error(command_id, message);
        break;
    case CommandResult::kDisabled:
        report_disabled(command_id, message);
        break;
    }
}

} // namespace markamp::core
