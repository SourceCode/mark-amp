#pragma once

// V11 Phase 03: Graph Data Model & Identity System
// Core graph container with node, socket, and link management.

#include "node_editor/NodeEditorTypes.h"

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::node_editor
{

// ============================================================================
// Graph Socket
// ============================================================================

/// A socket (port) on a node. Sockets are owned by their parent node.
struct GraphSocket
{
    SocketId id;
    NodeId owner_node;           // Parent node
    SocketDirection direction{}; // Input or Output
    SocketDataType data_type{};  // Semantic type
    std::string label;           // Display name
    std::string default_value;   // Default value as string (type-interpreted)
    std::vector<LinkId> links;   // Connected links

    [[nodiscard]] auto is_connected() const -> bool
    {
        return !links.empty();
    }
};

// ============================================================================
// Graph Node
// ============================================================================

/// A node within the graph. Nodes own their sockets.
struct GraphNode
{
    NodeId id;
    std::string type_name;      // Matches NodeDefinition type string
    std::string label;          // User-visible label
    Vec2 position;              // Canvas position
    Vec2 dimensions{180.0F, 0}; // Width, height (height auto-computed)
    NodeFlags flags{NodeFlags::None};

    std::vector<SocketId> inputs;  // Input socket IDs (ordered)
    std::vector<SocketId> outputs; // Output socket IDs (ordered)
};

// ============================================================================
// Graph Link
// ============================================================================

/// A connection between an output socket and an input socket.
struct GraphLink
{
    LinkId id;
    SocketId source; // Output socket
    SocketId target; // Input socket
    bool valid{true};
};

// ============================================================================
// Node Graph
// ============================================================================

/// Core graph container implementing an adjacency-list graph model.
///
/// Provides node/socket/link CRUD operations, ID generation, topological
/// ordering, and graph validation. Thread-safety is NOT provided — callers
/// must synchronize externally (typically via the command stack on the UI thread).
class NodeGraph
{
public:
    NodeGraph();
    explicit NodeGraph(GraphId graph_id);

    // -- Identity --
    [[nodiscard]] auto id() const -> GraphId;
    void set_id(GraphId graph_id);

    [[nodiscard]] auto name() const -> const std::string&;
    void set_name(const std::string& graph_name);

    [[nodiscard]] auto mode() const -> GraphMode;
    void set_mode(GraphMode graph_mode);

    // -- Node operations --
    auto add_node(const std::string& type_name, Vec2 position = {}) -> NodeId;
    auto remove_node(NodeId node_id) -> bool;
    [[nodiscard]] auto find_node(NodeId node_id) const -> const GraphNode*;
    auto find_node_mut(NodeId node_id) -> GraphNode*;
    [[nodiscard]] auto node_count() const -> std::size_t;
    [[nodiscard]] auto all_node_ids() const -> std::vector<NodeId>;

    // -- Socket operations --
    auto add_socket(NodeId node_id,
                    SocketDirection direction,
                    SocketDataType data_type,
                    const std::string& label,
                    const std::string& default_value = {}) -> SocketId;
    [[nodiscard]] auto find_socket(SocketId socket_id) const -> const GraphSocket*;
    auto find_socket_mut(SocketId socket_id) -> GraphSocket*;

    // -- Link operations --
    /// Create a link from an output socket to an input socket.
    /// Returns invalid LinkId if validation fails (type mismatch, self-loop, duplicate).
    auto add_link(SocketId source, SocketId target) -> LinkId;
    auto remove_link(LinkId link_id) -> bool;
    [[nodiscard]] auto find_link(LinkId link_id) const -> const GraphLink*;
    [[nodiscard]] auto link_count() const -> std::size_t;
    [[nodiscard]] auto all_link_ids() const -> std::vector<LinkId>;

    /// Get all links connected to a specific socket.
    [[nodiscard]] auto links_for_socket(SocketId socket_id) const -> std::vector<LinkId>;

    // -- Graph analysis --
    /// Returns nodes in topological order (sources first).
    /// Returns empty vector if the graph has cycles.
    [[nodiscard]] auto topological_sort() const -> std::vector<NodeId>;

    /// Check if the graph contains cycles.
    [[nodiscard]] auto has_cycles() const -> bool;

    /// Validate entire graph: checks link validity, dangling sockets, etc.
    struct ValidationResult
    {
        bool valid{true};
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    [[nodiscard]] auto validate() const -> ValidationResult;

    // -- Bulk operations --
    void clear();

    /// Schema version for serialization forward-compat.
    static constexpr int kSchemaVersion = 1;

private:
    GraphId graph_id_;
    std::string name_{"Untitled"};
    GraphMode mode_{GraphMode::Graphics};

    uint64_t next_node_id_{1};
    uint64_t next_socket_id_{1};
    uint64_t next_link_id_{1};

    std::unordered_map<NodeId, GraphNode> nodes_;
    std::unordered_map<SocketId, GraphSocket> sockets_;
    std::unordered_map<LinkId, GraphLink> links_;

    [[nodiscard]] auto generate_node_id() -> NodeId;
    [[nodiscard]] auto generate_socket_id() -> SocketId;
    [[nodiscard]] auto generate_link_id() -> LinkId;

    /// Check type compatibility between source and target sockets.
    [[nodiscard]] static auto are_types_compatible(SocketDataType source, SocketDataType target)
        -> bool;
};

} // namespace markamp::node_editor
