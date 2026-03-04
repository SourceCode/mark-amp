#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Phase 43: Notification category for filtering.
enum class NotificationCategory : uint8_t
{
    kSystem,    ///< System messages (updates, startup)
    kBuild,     ///< Build/compile results
    kGit,       ///< Git operations
    kExtension, ///< Extension notifications
    kUser,      ///< User-triggered
};

/// Phase 43: Notification priority.
enum class NotificationPriority : uint8_t
{
    kLow,    ///< Background info
    kNormal, ///< Standard notification
    kHigh,   ///< Important, bypasses stacking
    kUrgent, ///< Overrides DND
};

/// Phase 43: Action on a notification.
struct NotificationAction
{
    std::string action_id;
    std::string label;
    bool is_primary{false};
};

/// Phase 43: Notification entry with extended metadata.
struct NotificationItem
{
    uint64_t id{0};
    std::string title;
    std::string message;
    std::string source;
    NotificationCategory category{NotificationCategory::kSystem};
    NotificationPriority priority{NotificationPriority::kNormal};
    std::vector<NotificationAction> actions;
    std::string group_key; ///< Items with same key are grouped
    bool is_read{false};
    bool is_dismissed{false};
    double progress{-1.0}; ///< -1 = no progress, 0..1 = percentage
    int64_t timestamp_ms{0};
    int64_t expiry_ms{0};       ///< 0 = no expiry
    int64_t snooze_until_ms{0}; ///< 0 = not snoozed
};

/// Phase 43: Grouped notification container.
struct NotificationGroup
{
    std::string group_key;
    std::vector<uint64_t> item_ids;
    int unread_count{0};
};

/// Phase 43: Testable model for Notification Center.
///
/// Manages notification items, groups, filtering, read/dismiss state,
/// DND mode, and snooze. Pure state machine, no wxWidgets.
class NotificationCenterModel
{
public:
    /// Add a notification. Returns assigned ID.
    auto add(NotificationItem item) -> uint64_t;

    /// Remove (permanently delete) a notification.
    void remove(uint64_t id);

    /// Mark as read.
    void mark_read(uint64_t id);

    /// Mark all as read.
    void mark_all_read();

    /// Dismiss a notification.
    void dismiss(uint64_t id);

    /// Snooze for a duration.
    void snooze(uint64_t id, int64_t duration_ms);

    /// Get all notifications (newest first).
    [[nodiscard]] auto all_items() const -> std::vector<NotificationItem>;

    /// Get items filtered by category.
    [[nodiscard]] auto items_by_category(NotificationCategory cat) const
        -> std::vector<NotificationItem>;

    /// Get items filtered by source.
    [[nodiscard]] auto items_by_source(const std::string& source) const
        -> std::vector<NotificationItem>;

    /// Get groups (for grouped display).
    [[nodiscard]] auto groups() const -> std::vector<NotificationGroup>;

    /// Get unread count.
    [[nodiscard]] auto unread_count() const -> int;

    /// Get total count (excluding dismissed).
    [[nodiscard]] auto total_count() const -> int;

    /// DND mode.
    void set_dnd_enabled(bool enabled);
    [[nodiscard]] auto is_dnd_enabled() const -> bool;

    /// Check if a notification should be shown (respects DND, snooze, expiry).
    [[nodiscard]] auto should_show(uint64_t id, int64_t now_ms) const -> bool;

    /// Remove expired notifications.
    void cleanup_expired(int64_t now_ms);

    /// Get item by ID.
    [[nodiscard]] auto find(uint64_t id) const -> const NotificationItem*;

private:
    std::vector<NotificationItem> items_;
    uint64_t next_id_{1};
    bool dnd_enabled_{false};
};

} // namespace markamp::ui
