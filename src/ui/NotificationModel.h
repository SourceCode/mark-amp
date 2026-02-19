#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Notification severity level (Phase 18 Task 3).
enum class NotificationSeverity : uint8_t
{
    kInfo,
    kWarning,
    kError,
};

/// A notification item.
struct NotificationItem
{
    std::string notification_id;
    std::string title;
    std::string message;
    NotificationSeverity severity{NotificationSeverity::kInfo};
    std::string source; ///< Origin (e.g., "Extension", "System")
    bool is_read{false};
    bool is_dismissed{false};
    std::vector<std::string> action_labels; ///< Action button labels
    std::chrono::steady_clock::time_point timestamp;
};

/// Testable model for the Notification System (Phase 18).
///
/// Encapsulates:
/// - Toast queue management with auto-dismiss
/// - Notification center persistence with filtering
/// - Severity-based styling rules
/// - Quiet hours suppression
class NotificationModel
{
public:
    /// Push a new notification.
    void push(NotificationItem item);

    /// Dismiss a notification.
    void dismiss(const std::string& notification_id);

    /// Mark as read.
    void mark_read(const std::string& notification_id);

    /// Clear all dismissed/read notifications.
    void clear_history();

    // ── Queries ─────────────────────────────────────────────────────

    /// Get all active (non-dismissed) notifications.
    [[nodiscard]] auto active() const -> std::vector<NotificationItem>;

    /// Get all notifications (including dismissed).
    [[nodiscard]] auto all() const -> const std::vector<NotificationItem>&;

    /// Get unread count.
    [[nodiscard]] auto unread_count() const -> int;

    /// Filter by severity.
    [[nodiscard]] auto by_severity(NotificationSeverity severity) const
        -> std::vector<NotificationItem>;

    /// Filter by source.
    [[nodiscard]] auto by_source(const std::string& source) const -> std::vector<NotificationItem>;

    // ── Quiet hours ─────────────────────────────────────────────────

    /// Enable/disable quiet hours.
    void set_quiet_hours(bool enabled);

    /// Check if a notification should be suppressed (info/warning in quiet hours).
    [[nodiscard]] auto should_suppress(NotificationSeverity severity) const -> bool;

    /// Is quiet hours active?
    [[nodiscard]] auto quiet_hours_enabled() const -> bool;

private:
    std::vector<NotificationItem> items_;
    bool quiet_hours_{false};

    [[nodiscard]] auto find_item(const std::string& notification_id) -> NotificationItem*;
};

} // namespace markamp::ui
