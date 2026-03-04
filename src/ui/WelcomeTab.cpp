// ============================================================================
// File: src/ui/WelcomeTab.cpp
// Phase 48: Welcome and Onboarding — Welcome tab model
// ============================================================================
#include "WelcomeTab.h"

#include <algorithm>

namespace markamp::ui
{

void WelcomeTabModel::add_action(WelcomeAction action)
{
    actions_.push_back(std::move(action));
}

void WelcomeTabModel::set_recent_items(std::vector<RecentItem> items)
{
    recent_items_ = std::move(items);
    filtered_items_ = recent_items_;
}

void WelcomeTabModel::filter_recent(const std::string& query)
{
    if (query.empty())
    {
        filtered_items_ = recent_items_;
        return;
    }

    filtered_items_.clear();
    std::string lower_query = query;
    std::ranges::transform(lower_query,
                           lower_query.begin(),
                           [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    for (const auto& item : recent_items_)
    {
        std::string lower_name = item.name;
        std::ranges::transform(lower_name,
                               lower_name.begin(),
                               [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (lower_name.find(lower_query) != std::string::npos)
        {
            filtered_items_.push_back(item);
        }
    }
}

void WelcomeTabModel::add_walkthrough(WalkthroughCard card)
{
    walkthroughs_.push_back(std::move(card));
}

void WelcomeTabModel::set_width(int width)
{
    if (width < 800)
    {
        layout_ = WelcomeLayout::Compact;
    }
    else if (width < 1200)
    {
        layout_ = WelcomeLayout::Normal;
    }
    else
    {
        layout_ = WelcomeLayout::Wide;
    }
}

} // namespace markamp::ui
