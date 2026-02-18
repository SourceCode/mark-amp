/// @file NotificationCenter.cpp
/// @brief V9 Phase 39 — NotificationCenter implementation.

#include "NotificationCenter.h"

#include <algorithm>
#include <set>

namespace markamp::core
{

void NotificationCenter::add_notification(CenterNotification notification)
{
    if (notification.notification_id.empty())
    {
        notification.notification_id = "notif_" + std::to_string(notifications_.size());
    }
    if (notification.timestamp == std::chrono::system_clock::time_point{})
    {
        notification.timestamp = std::chrono::system_clock::now();
    }
    notifications_.push_back(std::move(notification));
    enforce_limit();
}

auto NotificationCenter::remove_notification(const std::string& notification_id) -> bool
{
    auto iter = std::remove_if(notifications_.begin(),
                               notifications_.end(),
                               [&](const CenterNotification& entry)
                               { return entry.notification_id == notification_id; });
    if (iter == notifications_.end())
    {
        return false;
    }
    notifications_.erase(iter, notifications_.end());
    return true;
}

auto NotificationCenter::find_notification(const std::string& notification_id) const
    -> const CenterNotification*
{
    for (const auto& entry : notifications_)
    {
        if (entry.notification_id == notification_id)
        {
            return &entry;
        }
    }
    return nullptr;
}

auto NotificationCenter::all_notifications() const -> const std::vector<CenterNotification>&
{
    return notifications_;
}

auto NotificationCenter::notification_count() const -> int
{
    return static_cast<int>(notifications_.size());
}

// ── Status management ─────────────────────────────────────────────

auto NotificationCenter::mark_read(const std::string& notification_id) -> bool
{
    auto* entry = find_mut(notification_id);
    if (entry == nullptr)
    {
        return false;
    }
    entry->status = NotificationStatus::kRead;
    return true;
}

auto NotificationCenter::mark_unread(const std::string& notification_id) -> bool
{
    auto* entry = find_mut(notification_id);
    if (entry == nullptr)
    {
        return false;
    }
    entry->status = NotificationStatus::kUnread;
    return true;
}

auto NotificationCenter::dismiss(const std::string& notification_id) -> bool
{
    auto* entry = find_mut(notification_id);
    if (entry == nullptr)
    {
        return false;
    }
    entry->status = NotificationStatus::kDismissed;
    return true;
}

auto NotificationCenter::archive(const std::string& notification_id) -> bool
{
    auto* entry = find_mut(notification_id);
    if (entry == nullptr)
    {
        return false;
    }
    entry->status = NotificationStatus::kArchived;
    return true;
}

void NotificationCenter::mark_all_read()
{
    for (auto& entry : notifications_)
    {
        if (entry.status == NotificationStatus::kUnread)
        {
            entry.status = NotificationStatus::kRead;
        }
    }
}

void NotificationCenter::dismiss_all()
{
    for (auto& entry : notifications_)
    {
        if (entry.status != NotificationStatus::kArchived)
        {
            entry.status = NotificationStatus::kDismissed;
        }
    }
}

// ── Filtering & grouping ──────────────────────────────────────────

auto NotificationCenter::unread_notifications() const -> std::vector<const CenterNotification*>
{
    std::vector<const CenterNotification*> result;
    for (const auto& entry : notifications_)
    {
        if (entry.status == NotificationStatus::kUnread)
        {
            result.push_back(&entry);
        }
    }
    return result;
}

auto NotificationCenter::notifications_by_group(const std::string& group) const
    -> std::vector<const CenterNotification*>
{
    std::vector<const CenterNotification*> result;
    for (const auto& entry : notifications_)
    {
        if (entry.group == group)
        {
            result.push_back(&entry);
        }
    }
    return result;
}

auto NotificationCenter::notifications_by_source(const std::string& source) const
    -> std::vector<const CenterNotification*>
{
    std::vector<const CenterNotification*> result;
    for (const auto& entry : notifications_)
    {
        if (entry.source == source)
        {
            result.push_back(&entry);
        }
    }
    return result;
}

auto NotificationCenter::notifications_by_priority(NotificationPriority priority) const
    -> std::vector<const CenterNotification*>
{
    std::vector<const CenterNotification*> result;
    for (const auto& entry : notifications_)
    {
        if (entry.priority == priority)
        {
            result.push_back(&entry);
        }
    }
    return result;
}

// ── Summary ───────────────────────────────────────────────────────

auto NotificationCenter::get_summary() const -> NotificationSummary
{
    NotificationSummary summary;
    summary.total = static_cast<int>(notifications_.size());
    std::set<std::string> unique_groups;

    for (const auto& entry : notifications_)
    {
        if (entry.status == NotificationStatus::kUnread)
        {
            ++summary.unread;
        }
        if (entry.priority >= NotificationPriority::kHigh)
        {
            ++summary.high_priority;
        }
        if (!entry.group.empty())
        {
            unique_groups.insert(entry.group);
        }
    }
    summary.groups = static_cast<int>(unique_groups.size());
    return summary;
}

auto NotificationCenter::groups() const -> std::vector<std::string>
{
    std::set<std::string> unique_groups;
    for (const auto& entry : notifications_)
    {
        if (!entry.group.empty())
        {
            unique_groups.insert(entry.group);
        }
    }
    return {unique_groups.begin(), unique_groups.end()};
}

// ── Limits & cleanup ──────────────────────────────────────────────

void NotificationCenter::set_max_history(int max_entries)
{
    max_history_ = max_entries;
    enforce_limit();
}

void NotificationCenter::clear_dismissed()
{
    auto iter = std::remove_if(notifications_.begin(),
                               notifications_.end(),
                               [](const CenterNotification& entry)
                               { return entry.status == NotificationStatus::kDismissed; });
    notifications_.erase(iter, notifications_.end());
}

void NotificationCenter::clear_all()
{
    notifications_.clear();
}

auto NotificationCenter::find_mut(const std::string& notification_id) -> CenterNotification*
{
    for (auto& entry : notifications_)
    {
        if (entry.notification_id == notification_id)
        {
            return &entry;
        }
    }
    return nullptr;
}

void NotificationCenter::enforce_limit()
{
    while (static_cast<int>(notifications_.size()) > max_history_)
    {
        notifications_.erase(notifications_.begin());
    }
}

} // namespace markamp::core
