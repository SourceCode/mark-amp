#include "NotificationCenterModel.h"

#include <algorithm>

namespace markamp::ui
{

auto NotificationCenterModel::add(NotificationItem item) -> uint64_t
{
    item.id = next_id_++;
    items_.push_back(std::move(item));
    return items_.back().id;
}

void NotificationCenterModel::remove(uint64_t id)
{
    std::erase_if(items_, [id](const NotificationItem& n) { return n.id == id; });
}

void NotificationCenterModel::mark_read(uint64_t id)
{
    for (auto& item : items_)
    {
        if (item.id == id)
        {
            item.is_read = true;
            return;
        }
    }
}

void NotificationCenterModel::mark_all_read()
{
    for (auto& item : items_)
        item.is_read = true;
}

void NotificationCenterModel::dismiss(uint64_t id)
{
    for (auto& item : items_)
    {
        if (item.id == id)
        {
            item.is_dismissed = true;
            return;
        }
    }
}

void NotificationCenterModel::snooze(uint64_t id, int64_t duration_ms)
{
    for (auto& item : items_)
    {
        if (item.id == id)
        {
            item.snooze_until_ms = item.timestamp_ms + duration_ms;
            return;
        }
    }
}

auto NotificationCenterModel::all_items() const -> std::vector<NotificationItem>
{
    std::vector<NotificationItem> result;
    for (const auto& item : items_)
    {
        if (!item.is_dismissed)
            result.push_back(item);
    }
    std::ranges::sort(result,
                      [](const NotificationItem& a, const NotificationItem& b)
                      { return a.timestamp_ms > b.timestamp_ms; });
    return result;
}

auto NotificationCenterModel::items_by_category(NotificationCategory cat) const
    -> std::vector<NotificationItem>
{
    std::vector<NotificationItem> result;
    for (const auto& item : items_)
    {
        if (!item.is_dismissed && item.category == cat)
            result.push_back(item);
    }
    return result;
}

auto NotificationCenterModel::items_by_source(const std::string& source) const
    -> std::vector<NotificationItem>
{
    std::vector<NotificationItem> result;
    for (const auto& item : items_)
    {
        if (!item.is_dismissed && item.source == source)
            result.push_back(item);
    }
    return result;
}

auto NotificationCenterModel::groups() const -> std::vector<NotificationGroup>
{
    std::unordered_map<std::string, NotificationGroup> group_map;
    for (const auto& item : items_)
    {
        if (item.is_dismissed || item.group_key.empty())
            continue;
        auto& group = group_map[item.group_key];
        group.group_key = item.group_key;
        group.item_ids.push_back(item.id);
        if (!item.is_read)
            ++group.unread_count;
    }

    std::vector<NotificationGroup> result;
    result.reserve(group_map.size());
    for (auto& [_, g] : group_map)
        result.push_back(std::move(g));
    return result;
}

auto NotificationCenterModel::unread_count() const -> int
{
    int count = 0;
    for (const auto& item : items_)
    {
        if (!item.is_dismissed && !item.is_read)
            ++count;
    }
    return count;
}

auto NotificationCenterModel::total_count() const -> int
{
    int count = 0;
    for (const auto& item : items_)
    {
        if (!item.is_dismissed)
            ++count;
    }
    return count;
}

void NotificationCenterModel::set_dnd_enabled(bool enabled)
{
    dnd_enabled_ = enabled;
}
auto NotificationCenterModel::is_dnd_enabled() const -> bool
{
    return dnd_enabled_;
}

auto NotificationCenterModel::should_show(uint64_t id, int64_t now_ms) const -> bool
{
    const auto* item = find(id);
    if (!item)
        return false;
    if (item->is_dismissed)
        return false;

    // Snoozed?
    if (item->snooze_until_ms > 0 && now_ms < item->snooze_until_ms)
        return false;

    // Expired?
    if (item->expiry_ms > 0 && now_ms > item->timestamp_ms + item->expiry_ms)
        return false;

    // DND check (urgent bypasses)
    if (dnd_enabled_ && item->priority != NotificationPriority::kUrgent)
        return false;

    return true;
}

void NotificationCenterModel::cleanup_expired(int64_t now_ms)
{
    std::erase_if(items_,
                  [now_ms](const NotificationItem& item)
                  { return item.expiry_ms > 0 && now_ms > item.timestamp_ms + item.expiry_ms; });
}

auto NotificationCenterModel::find(uint64_t id) const -> const NotificationItem*
{
    for (const auto& item : items_)
    {
        if (item.id == id)
            return &item;
    }
    return nullptr;
}

} // namespace markamp::ui
