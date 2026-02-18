/// @file NotificationCenter.h
/// @brief V9 Phase 39 — Centralized notification center with history, filtering, grouping.
#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Priority levels for notifications in the center.
enum class NotificationPriority : uint8_t
{
    kLow = 0,
    kNormal = 1,
    kHigh = 2,
    kUrgent = 3,
};

/// Notification status in the center.
enum class NotificationStatus : uint8_t
{
    kUnread = 0,
    kRead = 1,
    kDismissed = 2,
    kArchived = 3,
};

/// A notification entry stored in the notification center.
struct CenterNotification
{
    std::string notification_id; ///< Unique identifier
    std::string title;           ///< Short title
    std::string message;         ///< Full message body
    std::string source;          ///< Source module/plugin
    std::string group;           ///< Grouping key (e.g., "sync", "build")
    NotificationPriority priority{NotificationPriority::kNormal};
    NotificationStatus status{NotificationStatus::kUnread};
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::string> actions; ///< Available action labels
    bool persistent{false};           ///< If true, does not auto-dismiss
};

/// Result returned from get_summary().
struct NotificationSummary
{
    int total{0};
    int unread{0};
    int high_priority{0};
    int groups{0};
};

/// Centralized notification center that stores notification history,
/// supports filtering, grouping, and bulk operations.
///
/// Extends the existing `NotificationService` (toast-only) by adding
/// persistent storage and a rich query API. Works alongside the
/// `NotificationFilter` to let users customize what they see.
class NotificationCenter
{
public:
    NotificationCenter() = default;

    // ── CRUD ──────────────────────────────────────────────────────────
    void add_notification(CenterNotification notification);
    auto remove_notification(const std::string& notification_id) -> bool;
    [[nodiscard]] auto find_notification(const std::string& notification_id) const
        -> const CenterNotification*;
    [[nodiscard]] auto all_notifications() const -> const std::vector<CenterNotification>&;
    [[nodiscard]] auto notification_count() const -> int;

    // ── Status management ─────────────────────────────────────────────
    auto mark_read(const std::string& notification_id) -> bool;
    auto mark_unread(const std::string& notification_id) -> bool;
    auto dismiss(const std::string& notification_id) -> bool;
    auto archive(const std::string& notification_id) -> bool;
    void mark_all_read();
    void dismiss_all();

    // ── Filtering & grouping ──────────────────────────────────────────
    [[nodiscard]] auto unread_notifications() const -> std::vector<const CenterNotification*>;
    [[nodiscard]] auto notifications_by_group(const std::string& group) const
        -> std::vector<const CenterNotification*>;
    [[nodiscard]] auto notifications_by_source(const std::string& source) const
        -> std::vector<const CenterNotification*>;
    [[nodiscard]] auto notifications_by_priority(NotificationPriority priority) const
        -> std::vector<const CenterNotification*>;

    // ── Summary ───────────────────────────────────────────────────────
    [[nodiscard]] auto get_summary() const -> NotificationSummary;
    [[nodiscard]] auto groups() const -> std::vector<std::string>;

    // ── Limits & cleanup ──────────────────────────────────────────────
    void set_max_history(int max_entries);
    void clear_dismissed();
    void clear_all();

private:
    std::vector<CenterNotification> notifications_;
    int max_history_{500};

    auto find_mut(const std::string& notification_id) -> CenterNotification*;
    void enforce_limit();
};

} // namespace markamp::core
