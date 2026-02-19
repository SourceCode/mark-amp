#include "StatusBarModel.h"

#include <algorithm>

namespace markamp::ui
{

void StatusBarModel::add_item(StatusBarItemModel item)
{
    items_.push_back(std::move(item));
}

void StatusBarModel::remove_item(const std::string& item_id)
{
    items_.erase(std::remove_if(items_.begin(),
                                items_.end(),
                                [&item_id](const StatusBarItemModel& status_item)
                                { return status_item.item_id == item_id; }),
                 items_.end());
}

auto StatusBarModel::sorted_items() const -> std::vector<StatusBarItemModel>
{
    auto sorted = items_;
    std::stable_sort(sorted.begin(),
                     sorted.end(),
                     [](const StatusBarItemModel& lhs, const StatusBarItemModel& rhs)
                     { return static_cast<int>(lhs.priority) < static_cast<int>(rhs.priority); });
    return sorted;
}

auto StatusBarModel::item_count() const -> int
{
    return static_cast<int>(items_.size());
}

auto StatusBarModel::clickable_items() const -> std::vector<StatusBarItemModel>
{
    std::vector<StatusBarItemModel> result;
    for (const auto& item : items_)
    {
        if (item.kind == StatusItemKind::kClickable)
        {
            result.push_back(item);
        }
    }
    return result;
}

auto StatusBarModel::progress_items() const -> std::vector<StatusBarItemModel>
{
    std::vector<StatusBarItemModel> result;
    for (const auto& item : items_)
    {
        if (item.kind == StatusItemKind::kProgress)
        {
            result.push_back(item);
        }
    }
    return result;
}

void StatusBarModel::update_label(const std::string& item_id, const std::string& new_label)
{
    auto* item = find_item(item_id);
    if (item != nullptr)
    {
        item->label = new_label;
    }
}

void StatusBarModel::update_progress(const std::string& item_id, float progress)
{
    auto* item = find_item(item_id);
    if (item != nullptr && item->kind == StatusItemKind::kProgress)
    {
        item->progress = std::clamp(progress, 0.0F, 1.0F);
    }
}

void StatusBarModel::complete_progress(const std::string& item_id)
{
    remove_item(item_id);
}

auto StatusBarModel::truncated_indices(int viewport_width, int item_width) const -> std::vector<int>
{
    std::vector<int> result;
    if (item_width <= 0)
    {
        return result;
    }

    const auto sorted = sorted_items();
    int used_width = 0;
    for (int idx = 0; idx < static_cast<int>(sorted.size()); ++idx)
    {
        used_width += item_width;
        if (used_width > viewport_width)
        {
            result.push_back(idx);
        }
    }
    return result;
}

auto StatusBarModel::find_item(const std::string& item_id) -> StatusBarItemModel*
{
    for (auto& item : items_)
    {
        if (item.item_id == item_id)
        {
            return &item;
        }
    }
    return nullptr;
}

} // namespace markamp::ui
