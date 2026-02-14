#pragma once

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus; // Forward declaration

/// A single item in a quick pick list.
/// Mirrors VS Code's `vscode.QuickPickItem`.
struct QuickPickItem
{
    std::string label;
    std::string description; // Secondary text next to label
    std::string detail;      // Third line detail text
    bool picked{false};      // Pre-selected in multi-select mode
};

/// Options for showing a quick pick to the user.
/// Mirrors VS Code's `vscode.window.showQuickPick()`.
struct QuickPickOptions
{
    std::string title;
    std::string placeholder;   // Filter box placeholder
    bool can_pick_many{false}; // Multi-select mode
    bool match_on_description{false};
    bool match_on_detail{false};
};

/// Service for extensions to show filterable pick lists.
///
/// Mirrors VS Code's `vscode.window.showQuickPick()`.
///
/// Injected into `PluginContext` so extensions can call:
///   `ctx.quick_pick_service->show(items, {.title = "Pick one"}, callback);`
class QuickPickService
{
public:
    /// Callback for single-select mode: receives the selected item, or nullopt if cancelled.
    using SingleResultCallback = std::function<void(const std::optional<QuickPickItem>&)>;

    /// Callback for multi-select mode: receives selected items, or empty if cancelled.
    using MultiResultCallback = std::function<void(const std::vector<QuickPickItem>&)>;

    QuickPickService() = default;

    /// Show a single-select quick pick.
    void show(const std::vector<QuickPickItem>& items,
              const QuickPickOptions& options,
              SingleResultCallback on_result);

    /// Show a multi-select quick pick.
    void show_many(const std::vector<QuickPickItem>& items,
                   const QuickPickOptions& options,
                   MultiResultCallback on_result);

    /// Is a quick pick currently visible?
    [[nodiscard]] auto is_visible() const -> bool
    {
        return visible_;
    }

    /// For testing: simulate selecting an item by index.
    void test_select(std::size_t index);

    /// For testing: simulate selecting multiple items.
    void test_select_many(const std::vector<std::size_t>& indices);

    /// For testing: simulate user cancellation.
    void test_cancel();

    /// Set the EventBus to publish UI request events.
    void set_event_bus(EventBus* bus);

private:
    bool visible_{false};
    std::vector<QuickPickItem> current_items_;
    QuickPickOptions current_options_;
    SingleResultCallback single_callback_;
    MultiResultCallback multi_callback_;
    EventBus* event_bus_{nullptr};
};

} // namespace markamp::core
