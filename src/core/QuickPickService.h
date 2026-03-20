#pragma once

#include <algorithm>
#include <cctype>
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

    /// Access the current pick items (for UI consumers building the list).
    [[nodiscard]] auto current_items() const -> const std::vector<QuickPickItem>&
    {
        return current_items_;
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

// ============================================================================
// V9 Phase 36 Tasks 5, 13 — Command argument type system
// ============================================================================

/// Type of a command argument for palette-driven prompts.
enum class ArgumentType
{
    kString, // Free-text string input
    kNumber, // Numeric input with validation
    kChoice, // Pick from a list of options
    kFile    // File picker
};

/// Defines an argument that a command can accept.
struct CommandArgument
{
    std::string name; // Argument display name
    ArgumentType type{ArgumentType::kString};
    std::string default_value;        // Default value
    std::vector<std::string> choices; // For kChoice type: available options
    std::string placeholder;          // Input placeholder text
    std::optional<int> min_value;     // For kNumber: minimum
    std::optional<int> max_value;     // For kNumber: maximum
};

/// Fuzzy filter items by query, matching label and optionally description.
[[nodiscard]] inline auto filter_quick_pick_items(const std::string& query,
                                                  const std::vector<QuickPickItem>& items,
                                                  bool match_description = false)
    -> std::vector<QuickPickItem>
{
    if (query.empty())
    {
        return items;
    }

    std::vector<QuickPickItem> result;
    for (const auto& item : items)
    {
        // Simple case-insensitive substring match
        auto lower_query = query;
        std::transform(lower_query.begin(),
                       lower_query.end(),
                       lower_query.begin(),
                       [](unsigned char character)
                       { return static_cast<char>(std::tolower(character)); });

        auto lower_label = item.label;
        std::transform(lower_label.begin(),
                       lower_label.end(),
                       lower_label.begin(),
                       [](unsigned char character)
                       { return static_cast<char>(std::tolower(character)); });

        if (lower_label.find(lower_query) != std::string::npos)
        {
            result.push_back(item);
            continue;
        }

        if (match_description)
        {
            auto lower_desc = item.description;
            std::transform(lower_desc.begin(),
                           lower_desc.end(),
                           lower_desc.begin(),
                           [](unsigned char character)
                           { return static_cast<char>(std::tolower(character)); });
            if (lower_desc.find(lower_query) != std::string::npos)
            {
                result.push_back(item);
            }
        }
    }
    return result;
}

} // namespace markamp::core
