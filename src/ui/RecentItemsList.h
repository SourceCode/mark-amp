// ============================================================================
// File: src/ui/RecentItemsList.h
// Phase 48: Welcome and Onboarding — Recent items list model
// ============================================================================
#pragma once

#include "WelcomeTab.h"

#include <string>
#include <vector>

namespace markamp::ui
{

/// Model for the recent items list panel.
class RecentItemsListModel
{
public:
    RecentItemsListModel() = default;

    /// Set the items.
    void set_items(std::vector<RecentItem> items);

    /// Get all items.
    [[nodiscard]] auto items() const -> const std::vector<RecentItem>&
    {
        return items_;
    }

    /// Filter items by search query.
    void filter(const std::string& query);

    /// Get filtered items.
    [[nodiscard]] auto filtered_items() const -> const std::vector<RecentItem>&
    {
        return filtered_;
    }

    /// Get count.
    [[nodiscard]] auto count() const -> int
    {
        return static_cast<int>(filtered_.size());
    }

    /// Whether the list is empty.
    [[nodiscard]] auto is_empty() const -> bool
    {
        return filtered_.empty();
    }

    /// Remove an item by path.
    void remove(const std::string& path);

    /// Clear all items.
    void clear();

private:
    std::vector<RecentItem> items_;
    std::vector<RecentItem> filtered_;
    std::string current_filter_;
};

} // namespace markamp::ui
