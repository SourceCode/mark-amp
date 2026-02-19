// V11 Phase 07: Command Stack & Undo/Redo Transactions
// Concrete command implementations.

#include "node_editor/NodeCommand.h"

#include <algorithm>

namespace markamp::node_editor
{

// ============================================================================
// AddNodeCommand
// ============================================================================

AddNodeCommand::AddNodeCommand(std::string type_name, Vec2 position)
    : type_name_(std::move(type_name))
    , position_(position)
{
}

void AddNodeCommand::execute(NodeGraph& graph)
{
    created_id_ = graph.add_node(type_name_, position_);
}

void AddNodeCommand::undo(NodeGraph& graph)
{
    if (created_id_.is_valid())
    {
        graph.remove_node(created_id_);
        created_id_ = NodeId{};
    }
}

auto AddNodeCommand::description() const -> std::string
{
    return "Add node '" + type_name_ + "'";
}

// ============================================================================
// RemoveNodeCommand
// ============================================================================

RemoveNodeCommand::RemoveNodeCommand(NodeId node_id)
    : node_id_(node_id)
{
}

void RemoveNodeCommand::execute(NodeGraph& graph)
{
    // Stash the node state for undo
    const auto* node = graph.find_node(node_id_);
    if (node == nullptr)
    {
        return;
    }
    stashed_node_ = *node;

    // Stash owned sockets
    stashed_sockets_.clear();
    auto all_sids = node->inputs;
    all_sids.insert(all_sids.end(), node->outputs.begin(), node->outputs.end());
    for (const auto& sid : all_sids)
    {
        const auto* socket = graph.find_socket(sid);
        if (socket != nullptr)
        {
            stashed_sockets_.push_back(*socket);
        }
    }

    // Stash connected links
    stashed_links_.clear();
    for (const auto& socket : stashed_sockets_)
    {
        for (const auto& lid : socket.links)
        {
            const auto* link = graph.find_link(lid);
            if (link != nullptr)
            {
                // Avoid duplicate stashing
                const bool already_stashed = std::any_of(stashed_links_.begin(),
                                                         stashed_links_.end(),
                                                         [&lid](const GraphLink& stashed_link)
                                                         { return stashed_link.id == lid; });
                if (!already_stashed)
                {
                    stashed_links_.push_back(*link);
                }
            }
        }
    }

    graph.remove_node(node_id_);
}

void RemoveNodeCommand::undo(NodeGraph& graph)
{
    // Re-add the node
    auto new_nid = graph.add_node(stashed_node_.type_name, stashed_node_.position);
    auto* node = graph.find_node_mut(new_nid);
    if (node != nullptr)
    {
        node->label = stashed_node_.label;
        node->dimensions = stashed_node_.dimensions;
        node->flags = stashed_node_.flags;
    }

    // Re-add sockets
    for (const auto& socket : stashed_sockets_)
    {
        graph.add_socket(
            new_nid, socket.direction, socket.data_type, socket.label, socket.default_value);
    }

    // Re-add links (best effort — socket IDs may have changed)
    // Note: Full undo fidelity requires ID persistence, which is handled
    // by the transaction layer in later phases.
    node_id_ = new_nid;
}

auto RemoveNodeCommand::description() const -> std::string
{
    return "Remove node " + std::to_string(node_id_.value);
}

// ============================================================================
// MoveNodeCommand
// ============================================================================

MoveNodeCommand::MoveNodeCommand(NodeId node_id, Vec2 new_position)
    : node_id_(node_id)
    , new_position_(new_position)
{
}

void MoveNodeCommand::execute(NodeGraph& graph)
{
    auto* node = graph.find_node_mut(node_id_);
    if (node != nullptr)
    {
        old_position_ = node->position;
        node->position = new_position_;
    }
}

void MoveNodeCommand::undo(NodeGraph& graph)
{
    auto* node = graph.find_node_mut(node_id_);
    if (node != nullptr)
    {
        node->position = old_position_;
    }
}

auto MoveNodeCommand::description() const -> std::string
{
    return "Move node " + std::to_string(node_id_.value);
}

// ============================================================================
// AddLinkCommand
// ============================================================================

AddLinkCommand::AddLinkCommand(SocketId source, SocketId target)
    : source_(source)
    , target_(target)
{
}

void AddLinkCommand::execute(NodeGraph& graph)
{
    created_id_ = graph.add_link(source_, target_);
}

void AddLinkCommand::undo(NodeGraph& graph)
{
    if (created_id_.is_valid())
    {
        graph.remove_link(created_id_);
        created_id_ = LinkId{};
    }
}

auto AddLinkCommand::description() const -> std::string
{
    return "Add link";
}

// ============================================================================
// RemoveLinkCommand
// ============================================================================

RemoveLinkCommand::RemoveLinkCommand(LinkId link_id)
    : link_id_(link_id)
{
}

void RemoveLinkCommand::execute(NodeGraph& graph)
{
    const auto* link = graph.find_link(link_id_);
    if (link != nullptr)
    {
        source_ = link->source;
        target_ = link->target;
        graph.remove_link(link_id_);
    }
}

void RemoveLinkCommand::undo(NodeGraph& graph)
{
    link_id_ = graph.add_link(source_, target_);
}

auto RemoveLinkCommand::description() const -> std::string
{
    return "Remove link " + std::to_string(link_id_.value);
}

} // namespace markamp::node_editor
