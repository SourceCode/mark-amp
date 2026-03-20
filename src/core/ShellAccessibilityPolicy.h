/// @file ShellAccessibilityPolicy.h
/// @brief P09-T05: Accessibility and feedback cohesion for shell state changes.
///
/// Defines announcement and focus restoration policies for shell transitions.
/// Throttles rapid state updates to avoid announcement spam.
#pragma once

#include "EventBus.h"

#include <chrono>
#include <string>

namespace markamp::core
{

/// Announcement urgency level.
enum class AnnouncementUrgency
{
    kPolite,    ///< Wait for current speech to finish
    kAssertive, ///< Interrupt current speech
};

/// Manages accessibility announcements for shell state changes.
class ShellAccessibilityPolicy
{
public:
    explicit ShellAccessibilityPolicy(EventBus& bus);

    /// Announce a shell state change.
    void announce(const std::string& message, AnnouncementUrgency urgency);

    /// Announce a mode change (editor, canvas, notebook, graph).
    void announce_mode_change(const std::string& mode_name);

    /// Announce a panel toggle.
    void announce_panel_toggle(const std::string& panel_name, bool visible);

    /// Announce a prompt open/close.
    void announce_prompt(const std::string& prompt_type, bool opened);

    /// Announce a notification.
    void announce_notification(const std::string& message);

    /// Get announcement count.
    [[nodiscard]] auto announcement_count() const -> int { return count_; }

private:
    [[nodiscard]] auto should_throttle() const -> bool;

    EventBus& event_bus_;
    Subscription notification_sub_;
    int count_{0};
    std::chrono::steady_clock::time_point last_announcement_;
    static constexpr int kThrottleMs = 300;
};

} // namespace markamp::core
