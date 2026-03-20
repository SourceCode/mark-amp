/// @file StatusBarAdapter.h
/// @brief P09-T02: Extension status bar adapter.
///
/// Reads StatusBarItemService items and projects them into StatusBarPanel.
/// Handles ordering, alignment, click routing, and plugin lifecycle.
#pragma once

#include "EventBus.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Alignment of a status bar item.
enum class StatusBarAlignment
{
    kLeft,
    kRight,
};

/// A projected status bar item for rendering.
struct ProjectedStatusItem
{
    std::string item_id;
    std::string text;
    std::string tooltip;
    std::string command;
    StatusBarAlignment alignment{StatusBarAlignment::kLeft};
    int priority{0};
    bool from_extension{false};
};

/// Adapts extension status bar contributions for the visible status bar.
class StatusBarAdapter
{
public:
    explicit StatusBarAdapter(EventBus& bus);

    /// Refresh projected items from the status bar item service.
    void refresh();

    /// Add a projected item.
    void add_item(const ProjectedStatusItem& item);

    /// Remove a projected item.
    void remove_item(const std::string& item_id);

    /// Get all projected items (sorted by priority).
    [[nodiscard]] auto items() const -> const std::vector<ProjectedStatusItem>&
    {
        return items_;
    }

    /// Get item count.
    [[nodiscard]] auto item_count() const -> int
    {
        return static_cast<int>(items_.size());
    }

    /// Handle click on a status bar item.
    void handle_click(const std::string& item_id);

private:
    void sort_items();

    EventBus& event_bus_;
    Subscription plugin_deactivated_sub_;
    std::vector<ProjectedStatusItem> items_;
    static constexpr int kMaxItems = 20;
};

} // namespace markamp::core
