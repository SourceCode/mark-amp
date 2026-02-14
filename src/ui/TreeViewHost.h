#pragma once

#include "core/TreeDataProviderRegistry.h"

#include <string>
#include <vector>

namespace markamp::ui
{

/// Generic sidebar panel that renders any ITreeDataProvider as a tree control (#46).
/// Each extension view gets its own collapsible section. Supports icons and context menus.
/// Mirrors VS Code's tree view containers.
class TreeViewHost
{
public:
    TreeViewHost() = default;

    /// Set the registry to pull providers from.
    void set_registry(core::TreeDataProviderRegistry* registry);

    /// Get the list of registered view IDs.
    [[nodiscard]] auto view_ids() const -> std::vector<std::string>;

    /// Get children for a particular view and parent element.
    [[nodiscard]] auto get_children(const std::string& view_id, const std::string& parent_id) const
        -> std::vector<core::TreeItem>;

    /// Get the tree item for a particular view and element ID.
    [[nodiscard]] auto get_tree_item(const std::string& view_id, const std::string& item_id) const
        -> core::TreeItem;

    /// Track collapsed state of tree nodes.
    void set_collapsed(const std::string& view_id, const std::string& node_id, bool collapsed);
    [[nodiscard]] auto is_collapsed(const std::string& view_id, const std::string& node_id) const
        -> bool;

private:
    core::TreeDataProviderRegistry* registry_{nullptr};
    // Collapsed state: view_id + ":" + node_id -> collapsed
    std::unordered_map<std::string, bool> collapsed_state_;
};

} // namespace markamp::ui
