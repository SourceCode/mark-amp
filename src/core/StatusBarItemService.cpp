#include "StatusBarItemService.h"

namespace markamp::core
{

StatusBarItem::StatusBarItem(StatusBarItemOptions options)
    : options_(std::move(options))
{
}

auto StatusBarItemService::create_item(const StatusBarItemOptions& options) -> StatusBarItem*
{
    auto item = std::make_unique<StatusBarItem>(options);
    auto* ptr = item.get();
    items_[options.id] = std::move(item);
    return ptr;
}

auto StatusBarItemService::get_item(const std::string& item_id) -> StatusBarItem*
{
    auto found = items_.find(item_id);
    return found != items_.end() ? found->second.get() : nullptr;
}

void StatusBarItemService::dispose_item(const std::string& item_id)
{
    auto found = items_.find(item_id);
    if (found != items_.end())
    {
        found->second->dispose();
        items_.erase(found);
    }
}

auto StatusBarItemService::get_visible_items() const -> std::vector<const StatusBarItem*>
{
    std::vector<const StatusBarItem*> result;
    for (const auto& [id, item] : items_)
    {
        if (item->is_visible() && !item->is_disposed())
        {
            result.push_back(item.get());
        }
    }
    return result;
}

auto StatusBarItemService::get_all_items() const -> std::vector<const StatusBarItem*>
{
    std::vector<const StatusBarItem*> result;
    result.reserve(items_.size());
    for (const auto& [id, item] : items_)
    {
        result.push_back(item.get());
    }
    return result;
}

} // namespace markamp::core
