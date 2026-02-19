// V11 Phase 03: Graph Data Model & Identity System
// Implementation of NodeGraph — core graph container.

#include "node_editor/NodeGraph.h"

#include <algorithm>
#include <queue>
#include <unordered_set>

namespace markamp::node_editor
{

NodeGraph::NodeGraph() = default;

NodeGraph::NodeGraph(GraphId graph_id)
    : graph_id_(graph_id)
{
}

auto NodeGraph::id() const -> GraphId
{
    return graph_id_;
}

void NodeGraph::set_id(GraphId graph_id)
{
    graph_id_ = graph_id;
}

auto NodeGraph::name() const -> const std::string&
{
    return name_;
}

void NodeGraph::set_name(const std::string& graph_name)
{
    name_ = graph_name;
}

auto NodeGraph::mode() const -> GraphMode
{
    return mode_;
}

void NodeGraph::set_mode(GraphMode graph_mode)
{
    mode_ = graph_mode;
}

// ============================================================================
// Node operations
// ============================================================================

auto NodeGraph::add_node(const std::string& type_name, Vec2 position) -> NodeId
{
    auto nid = generate_node_id();
    GraphNode node;
    node.id = nid;
    node.type_name = type_name;
    node.label = type_name; // Default label = type name
    node.position = position;
    nodes_.emplace(nid, std::move(node));
    return nid;
}

auto NodeGraph::remove_node(NodeId node_id) -> bool
{
    auto node_it = nodes_.find(node_id);
    if (node_it == nodes_.end())
    {
        return false;
    }

    // Collect all sockets owned by this node
    std::vector<SocketId> owned_sockets;
    owned_sockets.reserve(node_it->second.inputs.size() + node_it->second.outputs.size());
    for (const auto& sid : node_it->second.inputs)
    {
        owned_sockets.push_back(sid);
    }
    for (const auto& sid : node_it->second.outputs)
    {
        owned_sockets.push_back(sid);
    }

    // Remove all links connected to owned sockets
    for (const auto& sid : owned_sockets)
    {
        auto sock_it = sockets_.find(sid);
        if (sock_it != sockets_.end())
        {
            // Copy link IDs — the list will be modified during removal
            auto link_ids = sock_it->second.links;
            for (const auto& lid : link_ids)
            {
                remove_link(lid);
            }
        }
    }

    // Remove owned sockets
    for (const auto& sid : owned_sockets)
    {
        sockets_.erase(sid);
    }

    nodes_.erase(node_it);
    return true;
}

auto NodeGraph::find_node(NodeId node_id) const -> const GraphNode*
{
    auto iter = nodes_.find(node_id);
    return iter != nodes_.end() ? &iter->second : nullptr;
}

auto NodeGraph::find_node_mut(NodeId node_id) -> GraphNode*
{
    auto iter = nodes_.find(node_id);
    return iter != nodes_.end() ? &iter->second : nullptr;
}

auto NodeGraph::node_count() const -> std::size_t
{
    return nodes_.size();
}

auto NodeGraph::all_node_ids() const -> std::vector<NodeId>
{
    std::vector<NodeId> ids;
    ids.reserve(nodes_.size());
    for (const auto& [nid, node] : nodes_)
    {
        ids.push_back(nid);
    }
    return ids;
}

// ============================================================================
// Socket operations
// ============================================================================

auto NodeGraph::add_socket(NodeId node_id,
                           SocketDirection direction,
                           SocketDataType data_type,
                           const std::string& label,
                           const std::string& default_value) -> SocketId
{
    auto node_it = nodes_.find(node_id);
    if (node_it == nodes_.end())
    {
        return SocketId{}; // Invalid — node doesn't exist
    }

    auto sid = generate_socket_id();
    GraphSocket socket;
    socket.id = sid;
    socket.owner_node = node_id;
    socket.direction = direction;
    socket.data_type = data_type;
    socket.label = label;
    socket.default_value = default_value;

    sockets_.emplace(sid, std::move(socket));

    if (direction == SocketDirection::Input)
    {
        node_it->second.inputs.push_back(sid);
    }
    else
    {
        node_it->second.outputs.push_back(sid);
    }

    return sid;
}

auto NodeGraph::find_socket(SocketId socket_id) const -> const GraphSocket*
{
    auto iter = sockets_.find(socket_id);
    return iter != sockets_.end() ? &iter->second : nullptr;
}

auto NodeGraph::find_socket_mut(SocketId socket_id) -> GraphSocket*
{
    auto iter = sockets_.find(socket_id);
    return iter != sockets_.end() ? &iter->second : nullptr;
}

// ============================================================================
// Link operations
// ============================================================================

auto NodeGraph::add_link(SocketId source, SocketId target) -> LinkId
{
    // Validate source and target exist
    auto* src_socket = find_socket_mut(source);
    auto* tgt_socket = find_socket_mut(target);
    if (src_socket == nullptr || tgt_socket == nullptr)
    {
        return LinkId{}; // Invalid sockets
    }

    // Source must be Output, target must be Input
    if (src_socket->direction != SocketDirection::Output ||
        tgt_socket->direction != SocketDirection::Input)
    {
        return LinkId{};
    }

    // Prevent self-loops (same node)
    if (src_socket->owner_node == tgt_socket->owner_node)
    {
        return LinkId{};
    }

    // Check type compatibility
    if (!are_types_compatible(src_socket->data_type, tgt_socket->data_type))
    {
        return LinkId{};
    }

    // Check for duplicate links
    for (const auto& lid : src_socket->links)
    {
        auto* existing = find_link(lid);
        if (existing != nullptr && existing->source == source && existing->target == target)
        {
            return LinkId{}; // Duplicate
        }
    }

    auto lid = generate_link_id();
    GraphLink link;
    link.id = lid;
    link.source = source;
    link.target = target;
    link.valid = true;

    links_.emplace(lid, link);
    src_socket->links.push_back(lid);
    tgt_socket->links.push_back(lid);

    return lid;
}

auto NodeGraph::remove_link(LinkId link_id) -> bool
{
    auto link_it = links_.find(link_id);
    if (link_it == links_.end())
    {
        return false;
    }

    // Remove link reference from source and target sockets
    auto* src_socket = find_socket_mut(link_it->second.source);
    if (src_socket != nullptr)
    {
        auto& src_links = src_socket->links;
        src_links.erase(std::remove(src_links.begin(), src_links.end(), link_id), src_links.end());
    }

    auto* tgt_socket = find_socket_mut(link_it->second.target);
    if (tgt_socket != nullptr)
    {
        auto& tgt_links = tgt_socket->links;
        tgt_links.erase(std::remove(tgt_links.begin(), tgt_links.end(), link_id), tgt_links.end());
    }

    links_.erase(link_it);
    return true;
}

auto NodeGraph::find_link(LinkId link_id) const -> const GraphLink*
{
    auto iter = links_.find(link_id);
    return iter != links_.end() ? &iter->second : nullptr;
}

auto NodeGraph::link_count() const -> std::size_t
{
    return links_.size();
}

auto NodeGraph::all_link_ids() const -> std::vector<LinkId>
{
    std::vector<LinkId> ids;
    ids.reserve(links_.size());
    for (const auto& [lid, link] : links_)
    {
        ids.push_back(lid);
    }
    return ids;
}

auto NodeGraph::links_for_socket(SocketId socket_id) const -> std::vector<LinkId>
{
    auto* socket = find_socket(socket_id);
    if (socket == nullptr)
    {
        return {};
    }
    return socket->links;
}

// ============================================================================
// Graph analysis
// ============================================================================

auto NodeGraph::topological_sort() const -> std::vector<NodeId>
{
    // Kahn's algorithm for topological sort
    // Build in-degree map based on links between nodes
    std::unordered_map<NodeId, int> in_degree;
    std::unordered_map<NodeId, std::vector<NodeId>> adjacency;

    for (const auto& [nid, node] : nodes_)
    {
        in_degree[nid] = 0;
    }

    for (const auto& [lid, link] : links_)
    {
        if (!link.valid)
        {
            continue;
        }
        const auto* src = find_socket(link.source);
        const auto* tgt = find_socket(link.target);
        if (src == nullptr || tgt == nullptr)
        {
            continue;
        }
        adjacency[src->owner_node].push_back(tgt->owner_node);
        in_degree[tgt->owner_node]++;
    }

    std::queue<NodeId> queue;
    for (const auto& [nid, degree] : in_degree)
    {
        if (degree == 0)
        {
            queue.push(nid);
        }
    }

    std::vector<NodeId> sorted;
    sorted.reserve(nodes_.size());

    while (!queue.empty())
    {
        auto current = queue.front();
        queue.pop();
        sorted.push_back(current);

        if (adjacency.contains(current))
        {
            for (const auto& neighbor : adjacency.at(current))
            {
                in_degree[neighbor]--;
                if (in_degree[neighbor] == 0)
                {
                    queue.push(neighbor);
                }
            }
        }
    }

    // If sorted doesn't contain all nodes, there is a cycle
    if (sorted.size() != nodes_.size())
    {
        return {}; // Cycle detected
    }

    return sorted;
}

auto NodeGraph::has_cycles() const -> bool
{
    return !nodes_.empty() && topological_sort().empty();
}

auto NodeGraph::validate() const -> ValidationResult
{
    ValidationResult result;

    // Check all links reference valid sockets
    for (const auto& [lid, link] : links_)
    {
        if (find_socket(link.source) == nullptr)
        {
            result.valid = false;
            result.errors.push_back("Link " + std::to_string(lid.value) +
                                    " references invalid source socket " +
                                    std::to_string(link.source.value));
        }
        if (find_socket(link.target) == nullptr)
        {
            result.valid = false;
            result.errors.push_back("Link " + std::to_string(lid.value) +
                                    " references invalid target socket " +
                                    std::to_string(link.target.value));
        }
    }

    // Check all sockets reference valid owner nodes
    for (const auto& [sid, socket_entry] : sockets_)
    {
        if (find_node(socket_entry.owner_node) == nullptr)
        {
            result.valid = false;
            result.errors.emplace_back("Socket " + std::to_string(sid.value) +
                                       " references invalid owner node " +
                                       std::to_string(socket_entry.owner_node.value));
        }
    }

    // Check for cycles
    if (has_cycles())
    {
        result.warnings.push_back("Graph contains cycles — some evaluation orders may be invalid");
    }

    // Check for unconnected input sockets without defaults
    for (const auto& [sid, sock] : sockets_)
    {
        if (sock.direction == SocketDirection::Input && !sock.is_connected() &&
            sock.default_value.empty())
        {
            result.warnings.emplace_back("Input socket '" + sock.label +
                                         "' (id=" + std::to_string(sid.value) +
                                         ") has no connection and no default value");
        }
    }

    return result;
}

// ============================================================================
// Bulk operations
// ============================================================================

void NodeGraph::clear()
{
    nodes_.clear();
    sockets_.clear();
    links_.clear();
    next_node_id_ = 1;
    next_socket_id_ = 1;
    next_link_id_ = 1;
}

// ============================================================================
// Private helpers
// ============================================================================

auto NodeGraph::generate_node_id() -> NodeId
{
    return NodeId{next_node_id_++};
}

auto NodeGraph::generate_socket_id() -> SocketId
{
    return SocketId{next_socket_id_++};
}

auto NodeGraph::generate_link_id() -> LinkId
{
    return LinkId{next_link_id_++};
}

auto NodeGraph::are_types_compatible(SocketDataType source, SocketDataType target) -> bool
{
    // Same type is always compatible
    if (source == target)
    {
        return true;
    }

    // Numeric coercion: Float <-> Int
    if ((source == SocketDataType::Float && target == SocketDataType::Int) ||
        (source == SocketDataType::Int && target == SocketDataType::Float))
    {
        return true;
    }

    // Vector promotion: Float -> Vector2/3/4
    if (source == SocketDataType::Float &&
        (target == SocketDataType::Vector2 || target == SocketDataType::Vector3 ||
         target == SocketDataType::Vector4))
    {
        return true;
    }

    // Color <-> Vector4 (same underlying repr)
    if ((source == SocketDataType::Color && target == SocketDataType::Vector4) ||
        (source == SocketDataType::Vector4 && target == SocketDataType::Color))
    {
        return true;
    }

    // Bool -> Int/Float (widening)
    if (source == SocketDataType::Bool &&
        (target == SocketDataType::Int || target == SocketDataType::Float))
    {
        return true;
    }

    return false;
}

} // namespace markamp::node_editor
