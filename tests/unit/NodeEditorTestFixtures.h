#pragma once

// V11 Phase 10: Core Test Fixtures & Golden Scenarios
// Reusable test graph builders for canonical graph topologies.

#include "node_editor/NodeGraph.h"

namespace markamp::node_editor::test_fixtures
{

/// Build a linear chain: A → B → C → ... with Float sockets.
/// Returns graph with `count` nodes connected in series.
inline auto make_linear_chain(std::size_t count) -> NodeGraph
{
    NodeGraph graph;
    graph.set_name("Linear Chain");

    if (count == 0)
    {
        return graph;
    }

    std::vector<NodeId> node_ids;
    node_ids.reserve(count);
    std::vector<SocketId> outputs;
    outputs.reserve(count);

    for (std::size_t idx = 0; idx < count; ++idx)
    {
        const auto nid = graph.add_node("chain.node", Vec2{static_cast<float>(idx) * 200.0F, 0});
        node_ids.push_back(nid);

        if (idx > 0)
        {
            graph.add_socket(nid, SocketDirection::Input, SocketDataType::Float, "in", "");
        }

        const auto out =
            graph.add_socket(nid, SocketDirection::Output, SocketDataType::Float, "out", "");
        outputs.push_back(out);
    }

    // Wire sequential links
    for (std::size_t idx = 1; idx < count; ++idx)
    {
        const auto* node = graph.find_node(node_ids[idx]);
        if (node != nullptr && !node->inputs.empty())
        {
            graph.add_link(outputs[idx - 1], node->inputs[0]);
        }
    }

    return graph;
}

/// Build a diamond graph: A → (B, C) → D with Float sockets.
///
///   A ──┬── B ──┐
///       └── C ──┴── D
inline auto make_diamond_graph() -> NodeGraph
{
    NodeGraph graph;
    graph.set_name("Diamond Graph");

    const auto nid_a = graph.add_node("diamond.source", Vec2{0, 100});
    const auto nid_b = graph.add_node("diamond.left", Vec2{200, 0});
    const auto nid_c = graph.add_node("diamond.right", Vec2{200, 200});
    const auto nid_d = graph.add_node("diamond.merge", Vec2{400, 100});

    // A outputs
    const auto out_a1 =
        graph.add_socket(nid_a, SocketDirection::Output, SocketDataType::Float, "out1", "");
    const auto out_a2 =
        graph.add_socket(nid_a, SocketDirection::Output, SocketDataType::Float, "out2", "");

    // B input/output
    const auto in_b =
        graph.add_socket(nid_b, SocketDirection::Input, SocketDataType::Float, "in", "");
    const auto out_b =
        graph.add_socket(nid_b, SocketDirection::Output, SocketDataType::Float, "out", "");

    // C input/output
    const auto in_c =
        graph.add_socket(nid_c, SocketDirection::Input, SocketDataType::Float, "in", "");
    const auto out_c =
        graph.add_socket(nid_c, SocketDirection::Output, SocketDataType::Float, "out", "");

    // D inputs
    const auto in_d1 =
        graph.add_socket(nid_d, SocketDirection::Input, SocketDataType::Float, "in1", "");
    const auto in_d2 =
        graph.add_socket(nid_d, SocketDirection::Input, SocketDataType::Float, "in2", "");
    graph.add_socket(nid_d, SocketDirection::Output, SocketDataType::Float, "out", "");

    // Wire: A→B, A→C, B→D, C→D
    graph.add_link(out_a1, in_b);
    graph.add_link(out_a2, in_c);
    graph.add_link(out_b, in_d1);
    graph.add_link(out_c, in_d2);

    return graph;
}

/// Build a disconnected graph with 3 isolated nodes (no links).
inline auto make_disconnected_graph() -> NodeGraph
{
    NodeGraph graph;
    graph.set_name("Disconnected Graph");

    const auto nid_a = graph.add_node("isolated.a", Vec2{0, 0});
    const auto nid_b = graph.add_node("isolated.b", Vec2{200, 0});
    const auto nid_c = graph.add_node("isolated.c", Vec2{400, 0});

    graph.add_socket(nid_a, SocketDirection::Output, SocketDataType::Float, "out", "1.0");
    graph.add_socket(nid_b, SocketDirection::Input, SocketDataType::Float, "in", "");
    graph.add_socket(nid_b, SocketDirection::Output, SocketDataType::Float, "out", "");
    graph.add_socket(nid_c, SocketDirection::Input, SocketDataType::Float, "in", "");

    return graph;
}

/// Build a complex multi-type graph with numeric coercion paths.
///
///   FloatSource ──(Float→Int)──► IntProcessor ──(Int→Float)──► FloatDisplay
///                                     └──(Int→Bool)──► BoolConsumer
inline auto make_complex_math_graph() -> NodeGraph
{
    NodeGraph graph;
    graph.set_name("Complex Math Graph");

    const auto nid_src = graph.add_node("math.float_source", Vec2{0, 0});
    const auto nid_proc = graph.add_node("math.int_processor", Vec2{200, 0});
    const auto nid_disp = graph.add_node("math.float_display", Vec2{400, 0});
    const auto nid_bool = graph.add_node("math.bool_consumer", Vec2{400, 200});

    // FloatSource: output Float
    const auto out_src =
        graph.add_socket(nid_src, SocketDirection::Output, SocketDataType::Float, "value", "");

    // IntProcessor: input Int, output Int
    const auto in_proc =
        graph.add_socket(nid_proc, SocketDirection::Input, SocketDataType::Int, "in", "");
    const auto out_proc =
        graph.add_socket(nid_proc, SocketDirection::Output, SocketDataType::Int, "out", "");

    // FloatDisplay: input Float
    const auto in_disp =
        graph.add_socket(nid_disp, SocketDirection::Input, SocketDataType::Float, "value", "");

    // BoolConsumer: input Bool
    const auto in_bool =
        graph.add_socket(nid_bool, SocketDirection::Input, SocketDataType::Bool, "flag", "");

    // Wire with coercion: Float→Int, Int→Float, Int→Bool
    graph.add_link(out_src, in_proc);  // Float→Int (numeric coercion)
    graph.add_link(out_proc, in_disp); // Int→Float (numeric coercion)
    graph.add_link(out_proc, in_bool); // Int→Bool (widening)

    return graph;
}

} // namespace markamp::node_editor::test_fixtures
