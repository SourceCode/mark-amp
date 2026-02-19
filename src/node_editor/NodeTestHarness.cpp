#include "NodeTestHarness.h"

#include <random>

namespace markamp::node_editor
{

// ---------------------------------------------------------------------------
// Graph generators
// ---------------------------------------------------------------------------

auto NodeTestHarness::create_chain_graph(std::size_t count) -> NodeGraph
{
    NodeGraph graph;

    NodeId prev_id;
    SocketId prev_output;
    for (std::size_t idx = 0; idx < count; ++idx)
    {
        auto nid = graph.add_node("test_node", Vec2{static_cast<float>(idx) * 200.0F, 0.0F});

        // add_socket(NodeId, SocketDirection, SocketDataType, label)
        auto in_sock = graph.add_socket(nid, SocketDirection::Input, SocketDataType::Float, "in");
        auto out_sock =
            graph.add_socket(nid, SocketDirection::Output, SocketDataType::Float, "out");

        // Chain: connect previous output to this input.
        if (prev_id.is_valid() && idx > 0)
        {
            graph.add_link(prev_output, in_sock);
        }

        prev_id = nid;
        prev_output = out_sock;
    }

    return graph;
}

auto NodeTestHarness::create_diamond_graph(std::size_t width, std::size_t depth) -> NodeGraph
{
    NodeGraph graph;

    // Layer 0: single source node.
    auto src_id = graph.add_node("source", Vec2{0.0F, 0.0F});
    auto src_out = graph.add_socket(src_id, SocketDirection::Output, SocketDataType::Float, "out");

    // Fan-out layers.
    std::vector<NodeId> prev_layer = {src_id};
    std::vector<SocketId> prev_outputs = {src_out};

    for (std::size_t layer = 0; layer < depth; ++layer)
    {
        std::vector<NodeId> current_layer;
        std::vector<SocketId> current_outputs;

        auto layer_w = (layer < depth / 2) ? width : std::max(width / 2, std::size_t{1});

        for (std::size_t col = 0; col < layer_w; ++col)
        {
            auto position =
                Vec2{static_cast<float>(col) * 200.0F, static_cast<float>(layer + 1) * 150.0F};
            auto nid = graph.add_node("mid_" + std::to_string(layer), position);
            auto in_sock =
                graph.add_socket(nid, SocketDirection::Input, SocketDataType::Float, "in");
            auto out_sock =
                graph.add_socket(nid, SocketDirection::Output, SocketDataType::Float, "out");

            // Connect from previous layer.
            if (!prev_outputs.empty())
            {
                auto src_index = col % prev_outputs.size();
                graph.add_link(prev_outputs[src_index], in_sock);
            }

            current_layer.push_back(nid);
            current_outputs.push_back(out_sock);
        }

        prev_layer = current_layer;
        prev_outputs = current_outputs;
    }

    // Fan-in: single sink node.
    auto snk_id = graph.add_node("sink", Vec2{0.0F, static_cast<float>(depth + 1) * 150.0F});
    auto snk_in = graph.add_socket(snk_id, SocketDirection::Input, SocketDataType::Float, "in");

    if (!prev_outputs.empty())
    {
        graph.add_link(prev_outputs[0], snk_in);
    }

    return graph;
}

auto NodeTestHarness::create_random_graph(std::size_t node_count, float link_density) -> NodeGraph
{
    NodeGraph graph;
    std::mt19937 rng{
        42}; // NOLINT(cert-msc32-c,cert-msc51-cpp) — deterministic for reproducibility.

    struct NodeSockets
    {
        NodeId id;
        SocketId input;
        SocketId output;
    };

    std::vector<NodeSockets> nodes;
    nodes.reserve(node_count);

    // Create nodes.
    for (std::size_t idx = 0; idx < node_count; ++idx)
    {
        std::uniform_real_distribution<float> pos_dist(0.0F, 2000.0F);
        auto position = Vec2{pos_dist(rng), pos_dist(rng)};
        auto nid = graph.add_node("rand_" + std::to_string(idx), position);
        auto in_sock = graph.add_socket(nid, SocketDirection::Input, SocketDataType::Float, "in");
        auto out_sock =
            graph.add_socket(nid, SocketDirection::Output, SocketDataType::Float, "out");
        nodes.push_back({nid, in_sock, out_sock});
    }

    // Create random links.
    auto max_link_count = static_cast<std::size_t>(static_cast<float>(node_count) * link_density);
    std::uniform_int_distribution<std::size_t> node_dist(0, node_count - 1);

    for (std::size_t link_idx = 0; link_idx < max_link_count; ++link_idx)
    {
        auto src = node_dist(rng);
        auto dst = node_dist(rng);
        if (src != dst)
        {
            graph.add_link(nodes[src].output, nodes[dst].input);
        }
    }

    return graph;
}

auto NodeTestHarness::create_stress_graph(std::size_t count) -> NodeGraph
{
    return create_random_graph(count, 0.5F);
}

// ---------------------------------------------------------------------------
// Benchmark utilities
// ---------------------------------------------------------------------------

auto NodeTestHarness::benchmark(const std::string& name,
                                std::size_t iterations,
                                const std::function<void()>& func,
                                float threshold_ms) -> BenchmarkResult
{
    auto time_start = std::chrono::steady_clock::now();

    for (std::size_t iter = 0; iter < iterations; ++iter)
    {
        func();
    }

    auto time_end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start);
    auto total_ms = static_cast<float>(elapsed.count()) / 1000.0F;

    BenchmarkResult result;
    result.name = name;
    result.duration_ms = total_ms;
    result.iterations = iterations;
    result.per_op_ms = total_ms / static_cast<float>(iterations);
    result.threshold_ms = threshold_ms;
    result.passed = (threshold_ms <= 0.0F) || (total_ms <= threshold_ms);

    return result;
}

auto NodeTestHarness::measure(const std::string& name, const std::function<void()>& func)
    -> BenchmarkResult
{
    return benchmark(name, 1, func);
}

// ---------------------------------------------------------------------------
// Graph verification
// ---------------------------------------------------------------------------

auto NodeTestHarness::verify_integrity(const NodeGraph& graph) -> bool
{
    auto link_ids = graph.all_link_ids();
    for (const auto& link_id : link_ids)
    {
        const auto* link = graph.find_link(link_id);
        if (link == nullptr)
        {
            return false;
        }
        const auto* src = graph.find_socket(link->source);
        const auto* dst = graph.find_socket(link->target);
        if (src == nullptr || dst == nullptr)
        {
            return false;
        }
        if (src->direction != SocketDirection::Output)
        {
            return false;
        }
        if (dst->direction != SocketDirection::Input)
        {
            return false;
        }
    }
    return true;
}

auto NodeTestHarness::link_count(const NodeGraph& graph) -> std::size_t
{
    return graph.link_count();
}

} // namespace markamp::node_editor
