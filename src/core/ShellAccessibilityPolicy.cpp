/// @file ShellAccessibilityPolicy.cpp
/// @brief P09-T05: Accessibility and feedback cohesion for shell state changes.

#include "ShellAccessibilityPolicy.h"

#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

ShellAccessibilityPolicy::ShellAccessibilityPolicy(EventBus& bus)
    : event_bus_(bus)
    , notification_sub_(event_bus_.subscribe<events::NotificationEvent>(
          [this](const events::NotificationEvent& evt)
          {
              announce_notification(evt.message);
          }))
{
}

void ShellAccessibilityPolicy::announce(const std::string& message,
                                         AnnouncementUrgency /*urgency*/)
{
    if (should_throttle())
    {
        return;
    }
    ++count_;
    last_announcement_ = std::chrono::steady_clock::now();
    MARKAMP_LOG_DEBUG("[A11Y] {}", message);
}

void ShellAccessibilityPolicy::announce_mode_change(const std::string& mode_name)
{
    announce("Switched to " + mode_name + " mode", AnnouncementUrgency::kAssertive);
}

void ShellAccessibilityPolicy::announce_panel_toggle(const std::string& panel_name,
                                                      bool visible)
{
    announce(panel_name + (visible ? " opened" : " closed"), AnnouncementUrgency::kPolite);
}

void ShellAccessibilityPolicy::announce_prompt(const std::string& prompt_type, bool opened)
{
    announce(prompt_type + (opened ? " opened" : " closed"), AnnouncementUrgency::kAssertive);
}

void ShellAccessibilityPolicy::announce_notification(const std::string& message)
{
    announce(message, AnnouncementUrgency::kPolite);
}

auto ShellAccessibilityPolicy::should_throttle() const -> bool
{
    const auto elapsed = std::chrono::steady_clock::now() - last_announcement_;
    return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() < kThrottleMs;
}

} // namespace markamp::core
