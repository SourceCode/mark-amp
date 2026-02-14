#pragma once

#include "core/TreeDataProviderRegistry.h"

#include <wx/panel.h>
#include <wx/treectrl.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::ui
{

/// Generic sidebar panel that renders any ITreeDataProvider as a tree control (#46).
/// Each extension view gets its own collapsible section. Supports icons and context menus.
/// Mirrors VS Code's tree view containers.
class TreeViewHost : public wxPanel
{
public:
    /// Data-only constructor (for tests).
    TreeViewHost();

    /// UI constructor with rendering support.
    TreeViewHost(wxWindow* parent, core::TreeDataProviderRegistry* registry);

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

    /// Refresh the tree display from providers.
    void RefreshContent();

    /// Apply theme colors.
    void ApplyTheme(const wxColour& bg_colour, const wxColour& fg_colour);

private:
    void CreateLayout();
    void PopulateTree(const std::string& view_id,
                      const wxTreeItemId& parent_node,
                      const std::string& parent_id);

    core::TreeDataProviderRegistry* registry_{nullptr};
    std::unordered_map<std::string, bool> collapsed_state_;

    // UI controls (null in data-only / test mode)
    wxTreeCtrl* tree_ctrl_{nullptr};
};

} // namespace markamp::ui
