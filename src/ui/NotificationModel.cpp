#include "NotificationModel.h"

#include <algorithm>

namespace markamp::ui
{

void NotificationModel::push(NotificationItem item)
{
    items_.push_back(std::move(item));
}

void NotificationModel::dismiss(const std::string& notification_id)
{
    auto* item = find_item(notification_id);
    if (item != nullptr)
    {
        item->is_dismissed = true;
    }
}

void NotificationModel::mark_read(const std::string& notification_id)
{
    auto* item = find_item(notification_id);
    if (item != nullptr)
    {
        item->is_read = true;
    }
}

void NotificationModel::clear_history()
{
    items_.erase(std::remove_if(items_.begin(),
                                items_.end(),
                                [](const NotificationItem& item)
                                { return item.is_dismissed || item.is_read; }),
                 items_.end());
}

auto NotificationModel::active() const -> std::vector<NotificationItem>
{
    std::vector<NotificationItem> result;
    for (const auto& item : items_)
    {
        if (!item.is_dismissed)
        {
            result.push_back(item);
        }
    }
    return result;
}

auto NotificationModel::all() const -> const std::vector<NotificationItem>&
{
    return items_;
}

auto NotificationModel::unread_count() const -> int
{
    int count = 0;
    for (const auto& item : items_)
    {
        if (!item.is_read && !item.is_dismissed)
        {
            ++count;
        }
    }
    return count;
}

auto NotificationModel::by_severity(NotificationSeverity severity) const
    -> std::vector<NotificationItem>
{
    std::vector<NotificationItem> result;
    for (const auto& item : items_)
    {
        if (item.severity == severity && !item.is_dismissed)
        {
            result.push_back(item);
        }
    }
    return result;
}

auto NotificationModel::by_source(const std::string& source) const -> std::vector<NotificationItem>
{
    std::vector<NotificationItem> result;
    for (const auto& item : items_)
    {
        if (item.source == source && !item.is_dismissed)
        {
            result.push_back(item);
        }
    }
    return result;
}

void NotificationModel::set_quiet_hours(bool enabled)
{
    quiet_hours_ = enabled;
}

auto NotificationModel::should_suppress(NotificationSeverity severity) const -> bool
{
    if (!quiet_hours_)
    {
        return false;
    }
    // Errors always show; info and warnings are suppressed in quiet hours
    return severity != NotificationSeverity::kError;
}

auto NotificationModel::quiet_hours_enabled() const -> bool
{
    return quiet_hours_;
}

auto NotificationModel::find_item(const std::string& notification_id) -> NotificationItem*
{
    for (auto& item : items_)
    {
        if (item.notification_id == notification_id)
        {
            return &item;
        }
    }
    return nullptr;
}

} // namespace markamp::ui
