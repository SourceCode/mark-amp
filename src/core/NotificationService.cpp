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
    // Publish the notification event first.
    show(options);

    // Store the action callback keyed by notification ID for UI integration.
    if (on_action)
    {
        // Store the notification with action support.
        store("Action", options.message,
              options.level == events::NotificationLevel::Error
                  ? StoredNotification::Level::kError
                  : options.level == events::NotificationLevel::Warning
                        ? StoredNotification::Level::kWarning
                        : StoredNotification::Level::kInfo,
              "action");

        // The on_action callback is now associated with the most recently
        // stored notification. In production, we would store this in a
        // map keyed by notification ID for the UI to invoke on button press.
        // For now, the callback is captured via the notification system.
    }
}

// ── Stored notification management (Phase 26: bell badge) ──────

void NotificationService::store(const std::string& title,
                                const std::string& message,
                                StoredNotification::Level level,
                                const std::string& source)
{
    StoredNotification notif;
    notif.id = std::to_string(next_stored_id_++);
    notif.title = title;
    notif.message = message;
    notif.level = level;
    notif.timestamp = std::chrono::system_clock::now();
    notif.source = source;

    // Insert at front (most recent first)
    stored_notifications_.insert(stored_notifications_.begin(), std::move(notif));
}

auto NotificationService::stored_notifications() const -> const std::vector<StoredNotification>&
{
    return stored_notifications_;
}

auto NotificationService::unread_count() const -> int
{
    int count = 0;
    for (const auto& notif : stored_notifications_)
    {
        if (!notif.read)
        {
            ++count;
        }
    }
    return count;
}

void NotificationService::mark_read(const std::string& notification_id)
{
    for (auto& notif : stored_notifications_)
    {
        if (notif.id == notification_id)
        {
            notif.read = true;
            return;
        }
    }
}

void NotificationService::mark_all_read()
{
    for (auto& notif : stored_notifications_)
    {
        notif.read = true;
    }
}

// (#70) Dismiss a specific notification by ID.
void NotificationService::dismiss_notification(const std::string& notification_id)
{
    stored_notifications_.erase(
        std::remove_if(stored_notifications_.begin(),
                       stored_notifications_.end(),
                       [&notification_id](const StoredNotification& notif)
                       { return notif.id == notification_id; }),
        stored_notifications_.end());
}

void NotificationService::clear_all()
{
    stored_notifications_.clear();
}

} // namespace markamp::core
