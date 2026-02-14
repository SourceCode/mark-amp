#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// A tree item displayed in a tree view (mirrors VS Code's TreeItem).
struct TreeItem
{
    std::string label;
    std::string description;
    std::string tooltip;
    std::string icon_path;
    std::string context_value; // For menus
    bool collapsible{false};

    /// Unique identifier for this item.
    std::string item_id;

    /// Command to execute on click.
    std::string command;
};

/// Interface for tree data providers (mirrors VS Code's TreeDataProvider).
class ITreeDataProvider
{
public:
    virtual ~ITreeDataProvider() = default;

    /// Get children of an element. Empty parent_id = root elements.
    [[nodiscard]] virtual auto get_children(const std::string& parent_id) const
        -> std::vector<TreeItem> = 0;

    /// Get a specific tree item by ID.
    [[nodiscard]] virtual auto get_tree_item(const std::string& item_id) const -> TreeItem = 0;

    /// Listener type for tree data changes.
    using ChangeListener = std::function<void()>;

    /// Subscribe to data changes.
    virtual auto on_did_change_tree_data(ChangeListener listener) -> std::size_t = 0;
};

/// Registry that maps view IDs to tree data providers.
class TreeDataProviderRegistry
{
public:
    TreeDataProviderRegistry() = default;

    /// Register a provider for a view ID. Takes ownership via shared_ptr.
    void register_provider(const std::string& view_id, std::shared_ptr<ITreeDataProvider> provider);

    /// Get the provider for a view ID. Returns nullptr if not registered.
    [[nodiscard]] auto get_provider(const std::string& view_id) const
        -> std::shared_ptr<ITreeDataProvider>;

    /// Check if a provider is registered for a view ID.
    [[nodiscard]] auto has_provider(const std::string& view_id) const -> bool;

    /// Unregister a provider.
    void unregister_provider(const std::string& view_id);

    /// Get all registered view IDs.
    [[nodiscard]] auto view_ids() const -> std::vector<std::string>;

private:
    std::unordered_map<std::string, std::shared_ptr<ITreeDataProvider>> providers_;
};

} // namespace markamp::core
