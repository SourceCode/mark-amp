#include "NotificationService.h"

namespace markamp::core
{

NotificationService::NotificationService(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

void NotificationService::show_info(const std::string& message, int duration_ms)
{
    show(
        {.message = message, .level = events::NotificationLevel::Info, .duration_ms = duration_ms});
}

void NotificationService::show_warning(const std::string& message, int duration_ms)
{
    show({.message = message,
          .level = events::NotificationLevel::Warning,
          .duration_ms = duration_ms});
}

void NotificationService::show_error(const std::string& message, int duration_ms)
{
    show({.message = message,
          .level = events::NotificationLevel::Error,
          .duration_ms = duration_ms});
}

void NotificationService::show(const NotificationOptions& options)
{
    event_bus_.publish(events::NotificationEvent{
        options.message,
        options.level,
        options.duration_ms,
    });
}

void NotificationService::show_with_actions(
    const NotificationOptions& options, const std::function<void(const std::string&)>& on_action)
{
    // Publish the notification event first
    show(options);
    // TODO(extensions): Wire action button callback via EventBus response channel
    // For now, store the callback for future UI integration
    (void)on_action;
}

} // namespace markamp::core
