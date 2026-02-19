#pragma once

// V11 Phase 30: Group Subgraph And NodeTree Interface Editing
// Group node support: nested graphs with interface sockets.

#include "NodeEditorTypes.h"
#include "NodeGraph.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::node_editor
{

/// Interface socket definition for a group I/O boundary.
struct GroupIOSocket
{
    std::string name;
    SocketDataType data_type{};
    SocketDirection direction{};
};

/// A group node: wraps a nested graph with defined I/O interface.
struct NodeGroup
{
    NodeId group_node_id;                      // The node ID in the parent graph
    std::unique_ptr<NodeGraph> inner_graph;    // The nested subgraph
    std::vector<GroupIOSocket> input_sockets;  // Group input interface
    std::vector<GroupIOSocket> output_sockets; // Group output interface
    std::string name{"Group"};
    bool expanded{false}; // Whether the subgraph is visible in the editor
};

/// Manages group nodes and subgraph navigation.
class NodeGroupManager
{
public:
    NodeGroupManager() = default;

    // --- Group CRUD ---
    auto create_group(const std::string& name, NodeId parent_node_id) -> NodeId;
    auto remove_group(NodeId group_node_id) -> bool;

    // --- Queries ---
    [[nodiscard]] auto find_group(NodeId group_node_id) const -> const NodeGroup*;
    auto find_group_mut(NodeId group_node_id) -> NodeGroup*;
    [[nodiscard]] auto is_group(NodeId node_id) const -> bool;
    [[nodiscard]] auto group_count() const -> std::size_t;
    [[nodiscard]] auto all_group_ids() const -> std::vector<NodeId>;

    // --- I/O interface editing ---
    void add_group_input(NodeId group_node_id, const std::string& name, SocketDataType data_type);
    void add_group_output(NodeId group_node_id, const std::string& name, SocketDataType data_type);

    // --- Navigation ---
    void push_into_group(NodeId group_node_id);
    void pop_to_parent();
    [[nodiscard]] auto navigation_depth() const -> std::size_t;
    [[nodiscard]] auto current_group() const -> NodeId;
    [[nodiscard]] auto breadcrumb_path() const -> std::vector<NodeId>;

    // --- Bulk ---
    void clear();

private:
    std::unordered_map<NodeId, std::unique_ptr<NodeGroup>> groups_;
    std::vector<NodeId> navigation_stack_;
    uint64_t next_inner_graph_id_{1000};
};

} // namespace markamp::node_editor
