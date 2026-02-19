#pragma once

// V11 Phase 09: Evaluation IR & Execution Planning
// Intermediate representation for graph execution — flattened, ordered instructions.

#include "node_editor/NodeEditorTypes.h"

#include <string>
#include <vector>

namespace markamp::node_editor
{

// Forward declaration
class NodeGraph;

// ============================================================================
// IR Node
// ============================================================================

/// A flattened node reference in the IR with resolved input/output slot indices.
struct IRNode
{
    NodeId node_id;
    std::string type_name;
    std::vector<SocketId> input_slots;
    std::vector<SocketId> output_slots;
};

// ============================================================================
// IR Link
// ============================================================================

/// A resolved connection in the IR between two slot indices.
struct IRLink
{
    LinkId link_id;
    SocketId source_socket;
    SocketId target_socket;
    NodeId source_node;
    NodeId target_node;
};

// ============================================================================
// Evaluation IR
// ============================================================================

/// Intermediate representation built from a NodeGraph.
///
/// Flattens the graph into a list of IR nodes and links suitable for
/// execution planning. The IR is immutable once constructed.
class EvaluationIR
{
public:
    EvaluationIR() = default;

    /// Factory: build IR from a NodeGraph. Returns empty IR if graph has cycles.
    [[nodiscard]] static auto build_from_graph(const NodeGraph& graph) -> EvaluationIR;

    // -- Accessors --
    [[nodiscard]] auto nodes() const -> const std::vector<IRNode>&;
    [[nodiscard]] auto links() const -> const std::vector<IRLink>&;
    [[nodiscard]] auto node_count() const -> std::size_t;
    [[nodiscard]] auto link_count() const -> std::size_t;
    [[nodiscard]] auto is_valid() const -> bool;

    /// Find IR node by NodeId. Returns nullptr if not found.
    [[nodiscard]] auto find_ir_node(NodeId nid) const -> const IRNode*;

private:
    std::vector<IRNode> nodes_;
    std::vector<IRLink> links_;
    bool valid_{false};
};

} // namespace markamp::node_editor
