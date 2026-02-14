#include "TreeViewHost.h"

namespace markamp::ui
{

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
    return provider != nullptr ? provider->get_children(parent_id) : std::vector<core::TreeItem>{};
}

auto TreeViewHost::get_tree_item(const std::string& view_id, const std::string& item_id) const
    -> core::TreeItem
{
    if (registry_ == nullptr)
    {
        return {};
    }
    auto provider = registry_->get_provider(view_id);
    return provider != nullptr ? provider->get_tree_item(item_id) : core::TreeItem{};
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
    auto key = view_id + ":" + node_id;
    auto found = collapsed_state_.find(key);
    return found != collapsed_state_.end() && found->second;
}

} // namespace markamp::ui
