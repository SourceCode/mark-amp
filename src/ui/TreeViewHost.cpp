#include "TreeViewHost.h"

#include <wx/sizer.h>

namespace markamp::ui
{

// ── Data-only constructor (for tests) ──

TreeViewHost::TreeViewHost()
    : wxPanel()
{
}

// ── UI constructor ──

TreeViewHost::TreeViewHost(wxWindow* parent, core::TreeDataProviderRegistry* registry)
    : wxPanel(parent, wxID_ANY)
    , registry_(registry)
{
    CreateLayout();
    RefreshContent();
}

void TreeViewHost::CreateLayout()
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    tree_ctrl_ = new wxTreeCtrl(this,
                                wxID_ANY,
                                wxDefaultPosition,
                                wxDefaultSize,
                                wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxBORDER_NONE);
    sizer->Add(tree_ctrl_, 1, wxEXPAND);

    SetSizer(sizer);
}

void TreeViewHost::PopulateTree(const std::string& view_id,
                                const wxTreeItemId& parent_node,
                                const std::string& parent_id)
{
    if (registry_ == nullptr || tree_ctrl_ == nullptr)
    {
        return;
    }

    auto children = get_children(view_id, parent_id);
    for (const auto& child : children)
    {
        auto node = tree_ctrl_->AppendItem(parent_node, wxString(child.label));
        if (child.collapsible)
        {
            PopulateTree(view_id, node, child.item_id);
        }
    }
}

void TreeViewHost::RefreshContent()
{
    if (tree_ctrl_ == nullptr || registry_ == nullptr)
    {
        return;
    }

    tree_ctrl_->DeleteAllItems();
    auto root = tree_ctrl_->AddRoot("Views");

    auto ids = view_ids();
    for (const auto& view_id : ids)
    {
        auto view_node = tree_ctrl_->AppendItem(root, wxString(view_id));
        PopulateTree(view_id, view_node, "");
        tree_ctrl_->Expand(view_node);
    }
}

void TreeViewHost::ApplyTheme(const wxColour& bg_colour, const wxColour& fg_colour)
{
    SetBackgroundColour(bg_colour);
    if (tree_ctrl_ != nullptr)
    {
        tree_ctrl_->SetBackgroundColour(bg_colour);
        tree_ctrl_->SetForegroundColour(fg_colour);
    }
    Refresh();
}

// ── Data-layer API (unchanged for test compatibility) ──

void TreeViewHost::set_registry(core::TreeDataProviderRegistry* registry)
{
    registry_ = registry;
}

auto TreeViewHost::view_ids() const -> std::vector<std::string>
{
    return registry_ != nullptr ? registry_->view_ids() : std::vector<std::string>{};
}

auto TreeViewHost::get_children(const std::string& view_id, const std::string& parent_id) const
    -> std::vector<core::TreeItem>
{
    if (registry_ == nullptr)
    {
        return {};
    }
    auto provider = registry_->get_provider(view_id);
    if (provider == nullptr)
    {
        return {};
    }
    return provider->get_children(parent_id);
}

auto TreeViewHost::get_tree_item(const std::string& view_id, const std::string& item_id) const
    -> core::TreeItem
{
    if (registry_ == nullptr)
    {
        return {};
    }
    auto provider = registry_->get_provider(view_id);
    if (provider == nullptr)
    {
        return {};
    }
    return provider->get_tree_item(item_id);
}

void TreeViewHost::set_collapsed(const std::string& view_id,
                                 const std::string& node_id,
                                 bool collapsed)
{
    collapsed_state_[view_id + ":" + node_id] = collapsed;
}

auto TreeViewHost::is_collapsed(const std::string& view_id, const std::string& node_id) const
    -> bool
{
    auto iter = collapsed_state_.find(view_id + ":" + node_id);
    return iter != collapsed_state_.end() && iter->second;
}

} // namespace markamp::ui
