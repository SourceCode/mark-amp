#include "NodeClipboard.h"

#include <unordered_set>

namespace markamp::node_editor
{

auto NodeClipboard::copy(const NodeGraph& graph, const std::vector<NodeId>& selected_ids)
    -> ClipboardData
{
    ClipboardData data;
    data.node_count = selected_ids.size();
    data.valid = !selected_ids.empty();

    // Count internal links (both endpoints in selected set)
    std::unordered_set<NodeId> selected_set(selected_ids.begin(), selected_ids.end());
    std::size_t link_count = 0;

    for (const auto& link_id : graph.all_link_ids())
    {
        const auto* link = graph.find_link(link_id);
        if (link == nullptr)
        {
            continue;
        }

        const auto* source_socket = graph.find_socket(link->source);
        const auto* target_socket = graph.find_socket(link->target);
        if (source_socket == nullptr || target_socket == nullptr)
        {
            continue;
        }

        if (selected_set.contains(source_socket->owner_node) &&
            selected_set.contains(target_socket->owner_node))
        {
            ++link_count;
        }
    }

    data.link_count = link_count;
    // Serialized JSON is a simplified placeholder — real implementation would use GraphSerializer
    data.serialized_json = "{\"nodes\":" + std::to_string(data.node_count) +
                           ",\"links\":" + std::to_string(data.link_count) + "}";

    last_copied_ = data;
    return data;
}

auto NodeClipboard::cut(NodeGraph& graph, const std::vector<NodeId>& selected_ids) -> ClipboardData
{
    auto data = copy(graph, selected_ids);

    // Remove selected nodes (links are auto-cleaned by remove_node)
    for (const auto& node_id : selected_ids)
    {
        graph.remove_node(node_id);
    }

    return data;
}

auto NodeClipboard::paste(NodeGraph& graph, const ClipboardData& data, Vec2 position)
    -> std::vector<NodeId>
{
    if (!data.valid)
    {
        return {};
    }

    // Create placeholder nodes at the paste position.
    // Real implementation would deserialize from data.serialized_json.
    std::vector<NodeId> new_ids;
    for (std::size_t idx = 0; idx < data.node_count; ++idx)
    {
        const Vec2 kOffset{position.x + static_cast<float>(idx) * 20.0F,
                           position.y + static_cast<float>(idx) * 20.0F};
        auto node_id = graph.add_node("pasted_node", kOffset);
        new_ids.push_back(node_id);
    }

    return new_ids;
}

auto NodeClipboard::duplicate(NodeGraph& graph,
                              const std::vector<NodeId>& selected_ids,
                              Vec2 offset) -> std::vector<NodeId>
{
    std::vector<NodeId> new_ids;
    std::unordered_map<NodeId, NodeId> id_remap;

    // Clone each selected node
    for (const auto& old_id : selected_ids)
    {
        const auto* old_node = graph.find_node(old_id);
        if (old_node == nullptr)
        {
            continue;
        }

        const Vec2 kNewPos{old_node->position.x + offset.x, old_node->position.y + offset.y};
        auto new_id = graph.add_node(old_node->type_name, kNewPos);

        auto* new_node = graph.find_node_mut(new_id);
        if (new_node != nullptr)
        {
            new_node->label = old_node->label;
            new_node->dimensions = old_node->dimensions;
        }

        id_remap[old_id] = new_id;
        new_ids.push_back(new_id);
    }

    // Clone sockets for each duplicated node
    for (const auto& old_id : selected_ids)
    {
        const auto* old_node = graph.find_node(old_id);
        if (old_node == nullptr)
        {
            continue;
        }

        for (const auto& socket_id : old_node->inputs)
        {
            const auto* socket = graph.find_socket(socket_id);
            if (socket != nullptr && id_remap.contains(old_id))
            {
                graph.add_socket(id_remap[old_id],
                                 socket->direction,
                                 socket->data_type,
                                 socket->label,
                                 socket->default_value);
            }
        }

        for (const auto& socket_id : old_node->outputs)
        {
            const auto* socket = graph.find_socket(socket_id);
            if (socket != nullptr && id_remap.count(old_id) > 0)
            {
                graph.add_socket(id_remap[old_id],
                                 socket->direction,
                                 socket->data_type,
                                 socket->label,
                                 socket->default_value);
            }
        }
    }

    return new_ids;
}

auto NodeClipboard::is_valid(const ClipboardData& data) -> bool
{
    return data.valid && data.node_count > 0;
}

auto NodeClipboard::last_copied() const -> const ClipboardData&
{
    return last_copied_;
}

} // namespace markamp::node_editor
