#pragma once

#include "NodeEditorTypes.h"
#include "NodeGraph.h"

#include <chrono>
#include <cstddef>
#include <functional>
#include <string>

namespace markamp::node_editor
{

// ---------------------------------------------------------------------------
// BenchmarkResult — timing result for a single benchmark
// ---------------------------------------------------------------------------

struct BenchmarkResult
{
    std::string name;
    float duration_ms{0.0F};
    std::size_t iterations{1};
    float per_op_ms{0.0F};    ///< duration_ms / iterations
    bool passed{true};        ///< True if within threshold
    float threshold_ms{0.0F}; ///< Max acceptable time
};

// ---------------------------------------------------------------------------
// NodeTestHarness — test fixture factory and benchmark utilities
// ---------------------------------------------------------------------------

class NodeTestHarness
{
public:
    NodeTestHarness() = default;

    // --- Graph generators -------------------------------------------------

    /// Create a chain graph: N nodes connected sequentially.
    [[nodiscard]] static auto create_chain_graph(std::size_t count) -> NodeGraph;

    /// Create a diamond graph: fan-out then fan-in pattern.
    [[nodiscard]] static auto create_diamond_graph(std::size_t width, std::size_t depth)
        -> NodeGraph;

    /// Create a random graph with specified node count and link density (0.0–1.0).
    [[nodiscard]] static auto create_random_graph(std::size_t node_count, float link_density)
        -> NodeGraph;

    /// Create a stress-test graph (1000+ nodes).
    [[nodiscard]] static auto create_stress_graph(std::size_t count = 1000) -> NodeGraph;

    // --- Benchmark utilities ----------------------------------------------

    /// Measure time to execute a function N times.
    [[nodiscard]] static auto benchmark(const std::string& name,
                                        std::size_t iterations,
                                        const std::function<void()>& func,
                                        float threshold_ms = 0.0F) -> BenchmarkResult;

    /// Quick single-iteration measurement.
    [[nodiscard]] static auto measure(const std::string& name, const std::function<void()>& func)
        -> BenchmarkResult;

    // --- Graph verification -----------------------------------------------

    /// Verify graph integrity (all links point to valid sockets).
    [[nodiscard]] static auto verify_integrity(const NodeGraph& graph) -> bool;

    /// Count total links in a graph.
    [[nodiscard]] static auto link_count(const NodeGraph& graph) -> std::size_t;
};

} // namespace markamp::node_editor
