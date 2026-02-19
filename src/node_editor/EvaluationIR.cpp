// V11 Phase 09: Evaluation IR & Execution Planning

#include "node_editor/EvaluationIR.h"

#include "node_editor/NodeGraph.h"

#include <algorithm>

namespace markamp::node_editor
{

auto EvaluationIR::build_from_graph(const NodeGraph& graph) -> EvaluationIR
{
    EvaluationIR ir;

    // Check for cycles — if present, IR is invalid
    if (graph.has_cycles())
    {
        ir.valid_ = false;
        return ir;
    }

    // Get topological order
    const auto topo_order = graph.topological_sort();
    if (topo_order.empty() && graph.node_count() > 0)
    {
        ir.valid_ = false;
        return ir;
    }

    // Build IR nodes in topological order
    ir.nodes_.reserve(topo_order.size());
    for (const auto& nid : topo_order)
    {
        const auto* graph_node = graph.find_node(nid);
        if (graph_node == nullptr)
        {
            continue;
        }

        IRNode ir_node;
        ir_node.node_id = nid;
        ir_node.type_name = graph_node->type_name;
        ir_node.input_slots =
            std::vector<SocketId>(graph_node->inputs.begin(), graph_node->inputs.end());
        ir_node.output_slots =
            std::vector<SocketId>(graph_node->outputs.begin(), graph_node->outputs.end());
        ir.nodes_.push_back(std::move(ir_node));
    }

    // Build IR links
    const auto all_links = graph.all_link_ids();
    ir.links_.reserve(all_links.size());
    for (const auto& lid : all_links)
    {
        const auto* graph_link = graph.find_link(lid);
        if (graph_link == nullptr)
        {
            continue;
        }

        const auto* source_socket = graph.find_socket(graph_link->source);
        const auto* target_socket = graph.find_socket(graph_link->target);
        if (source_socket == nullptr || target_socket == nullptr)
        {
            continue;
        }

        IRLink ir_link;
        ir_link.link_id = lid;
        ir_link.source_socket = graph_link->source;
        ir_link.target_socket = graph_link->target;
        ir_link.source_node = source_socket->owner_node;
        ir_link.target_node = target_socket->owner_node;
        ir.links_.push_back(std::move(ir_link));
    }

    ir.valid_ = true;
    return ir;
}

auto EvaluationIR::nodes() const -> const std::vector<IRNode>&
{
    return nodes_;
}

auto EvaluationIR::links() const -> const std::vector<IRLink>&
{
    return links_;
}

auto EvaluationIR::node_count() const -> std::size_t
{
    return nodes_.size();
}

auto EvaluationIR::link_count() const -> std::size_t
{
    return links_.size();
}

auto EvaluationIR::is_valid() const -> bool
{
    return valid_;
}

auto EvaluationIR::find_ir_node(NodeId nid) const -> const IRNode*
{
    const auto it = std::find_if(
        nodes_.begin(), nodes_.end(), [nid](const IRNode& node) { return node.node_id == nid; });
    if (it == nodes_.end())
    {
        return nullptr;
    }
    return &(*it);
}

} // namespace markamp::node_editor
