#include "MindMapModel.h"

#include <algorithm>

namespace markamp::canvas
{

void MindMapModel::set_nodes(std::vector<MindMapNode> nodes)
{
    nodes_ = std::move(nodes);
}
auto MindMapModel::nodes() const -> const std::vector<MindMapNode>&
{
    return nodes_;
}
auto MindMapModel::node_count() const -> int
{
    return static_cast<int>(nodes_.size());
}

auto MindMapModel::find_index(const std::string& node_id) -> int
{
    for (int i = 0; i < static_cast<int>(nodes_.size()); ++i)
    {
        if (nodes_[static_cast<size_t>(i)].node_id == node_id)
        {
            return i;
        }
    }
    return -1;
}

void MindMapModel::add_child(const std::string& parent_id,
                             const std::string& node_id,
                             const std::string& label)
{
    const int parent_idx = find_index(parent_id);
    const int parent_depth = (parent_idx >= 0) ? nodes_[static_cast<size_t>(parent_idx)].depth : -1;
    nodes_.push_back({node_id, label, parent_id, false, parent_depth + 1, ""});
}

void MindMapModel::add_sibling(const std::string& sibling_id,
                               const std::string& node_id,
                               const std::string& label)
{
    const int sibling_idx = find_index(sibling_id);
    if (sibling_idx >= 0)
    {
        const auto& sibling = nodes_[static_cast<size_t>(sibling_idx)];
        nodes_.push_back({node_id, label, sibling.parent_id, false, sibling.depth, ""});
    }
}

void MindMapModel::remove_node(const std::string& node_id)
{
    // Remove node and all descendants
    std::vector<std::string> to_remove = {node_id};
    bool found = true;
    while (found)
    {
        found = false;
        for (const auto& node : nodes_)
        {
            if (std::find(to_remove.begin(), to_remove.end(), node.parent_id) != to_remove.end() &&
                std::find(to_remove.begin(), to_remove.end(), node.node_id) == to_remove.end())
            {
                to_remove.push_back(node.node_id);
                found = true;
            }
        }
    }
    nodes_.erase(std::remove_if(nodes_.begin(),
                                nodes_.end(),
                                [&](const MindMapNode& node) {
                                    return std::find(to_remove.begin(),
                                                     to_remove.end(),
                                                     node.node_id) != to_remove.end();
                                }),
                 nodes_.end());
}

auto MindMapModel::children_of(const std::string& parent_id) const -> std::vector<MindMapNode>
{
    std::vector<MindMapNode> result;
    for (const auto& node : nodes_)
    {
        if (node.parent_id == parent_id)
        {
            result.push_back(node);
        }
    }
    return result;
}

void MindMapModel::toggle_collapse(const std::string& node_id)
{
    const int idx = find_index(node_id);
    if (idx >= 0)
    {
        nodes_[static_cast<size_t>(idx)].collapsed = !nodes_[static_cast<size_t>(idx)].collapsed;
    }
}

auto MindMapModel::is_collapsed(const std::string& node_id) const -> bool
{
    for (const auto& node : nodes_)
    {
        if (node.node_id == node_id)
        {
            return node.collapsed;
        }
    }
    return false;
}

void MindMapModel::set_layout(MindMapLayout layout)
{
    layout_ = layout;
}
auto MindMapModel::layout() const -> MindMapLayout
{
    return layout_;
}

void MindMapModel::set_depth_colors(std::vector<std::string> colors)
{
    depth_colors_ = std::move(colors);
}

auto MindMapModel::color_for_depth(int depth) const -> std::string
{
    if (depth_colors_.empty())
    {
        return "#333333";
    }
    return depth_colors_[static_cast<size_t>(depth) % depth_colors_.size()];
}

} // namespace markamp::canvas
