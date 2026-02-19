#include "NodeGroup.h"

namespace markamp::node_editor
{

auto NodeGroupManager::create_group(const std::string& name, NodeId parent_node_id) -> NodeId
{
    auto group = std::make_unique<NodeGroup>();
    group->group_node_id = parent_node_id;
    group->name = name;
    group->inner_graph = std::make_unique<NodeGraph>(GraphId(next_inner_graph_id_++));
    group->inner_graph->set_name(name);

    groups_[parent_node_id] = std::move(group);
    return parent_node_id;
}

auto NodeGroupManager::remove_group(NodeId group_node_id) -> bool
{
    return groups_.erase(group_node_id) > 0;
}

auto NodeGroupManager::find_group(NodeId group_node_id) const -> const NodeGroup*
{
    const auto iter = groups_.find(group_node_id);
    return (iter != groups_.end()) ? iter->second.get() : nullptr;
}

auto NodeGroupManager::find_group_mut(NodeId group_node_id) -> NodeGroup*
{
    auto iter = groups_.find(group_node_id);
    return (iter != groups_.end()) ? iter->second.get() : nullptr;
}

auto NodeGroupManager::is_group(NodeId node_id) const -> bool
{
    return groups_.contains(node_id);
}

auto NodeGroupManager::group_count() const -> std::size_t
{
    return groups_.size();
}

auto NodeGroupManager::all_group_ids() const -> std::vector<NodeId>
{
    std::vector<NodeId> ids;
    ids.reserve(groups_.size());
    for (const auto& [group_id, group] : groups_)
    {
        ids.push_back(group_id);
    }
    return ids;
}

void NodeGroupManager::add_group_input(NodeId group_node_id,
                                       const std::string& name,
                                       SocketDataType data_type)
{
    auto* group = find_group_mut(group_node_id);
    if (group != nullptr)
    {
        GroupIOSocket io_socket;
        io_socket.name = name;
        io_socket.data_type = data_type;
        io_socket.direction = SocketDirection::Input;
        group->input_sockets.push_back(io_socket);
    }
}

void NodeGroupManager::add_group_output(NodeId group_node_id,
                                        const std::string& name,
                                        SocketDataType data_type)
{
    auto* group = find_group_mut(group_node_id);
    if (group != nullptr)
    {
        GroupIOSocket io_socket;
        io_socket.name = name;
        io_socket.data_type = data_type;
        io_socket.direction = SocketDirection::Output;
        group->output_sockets.push_back(io_socket);
    }
}

void NodeGroupManager::push_into_group(NodeId group_node_id)
{
    if (is_group(group_node_id))
    {
        navigation_stack_.push_back(group_node_id);
    }
}

void NodeGroupManager::pop_to_parent()
{
    if (!navigation_stack_.empty())
    {
        navigation_stack_.pop_back();
    }
}

auto NodeGroupManager::navigation_depth() const -> std::size_t
{
    return navigation_stack_.size();
}

auto NodeGroupManager::current_group() const -> NodeId
{
    if (navigation_stack_.empty())
    {
        return {};
    }
    return navigation_stack_.back();
}

auto NodeGroupManager::breadcrumb_path() const -> std::vector<NodeId>
{
    return navigation_stack_;
}

void NodeGroupManager::clear()
{
    groups_.clear();
    navigation_stack_.clear();
}

} // namespace markamp::node_editor
