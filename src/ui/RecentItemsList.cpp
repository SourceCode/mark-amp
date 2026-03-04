// ============================================================================
// File: src/ui/RecentItemsList.cpp
// Phase 48: Welcome and Onboarding — Recent items list model
// ============================================================================
#include "RecentItemsList.h"

#include <algorithm>

namespace markamp::ui
{

void RecentItemsListModel::set_items(std::vector<RecentItem> items)
{
    items_ = std::move(items);
    filter(current_filter_);
}

void RecentItemsListModel::filter(const std::string& query)
{
    current_filter_ = query;
    if (query.empty())
    {
        filtered_ = items_;
        return;
    }

    filtered_.clear();
    std::string lower_query = query;
    std::ranges::transform(lower_query,
                           lower_query.begin(),
                           [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    for (const auto& item : items_)
    {
        std::string lower_name = item.name;
        std::ranges::transform(lower_name,
                               lower_name.begin(),
                               [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (lower_name.find(lower_query) != std::string::npos)
        {
            filtered_.push_back(item);
        }
    }
}

void RecentItemsListModel::remove(const std::string& path)
{
    std::erase_if(items_, [&](const RecentItem& item) { return item.path == path; });
    filter(current_filter_);
}

void RecentItemsListModel::clear()
{
    items_.clear();
    filtered_.clear();
}

} // namespace markamp::ui
