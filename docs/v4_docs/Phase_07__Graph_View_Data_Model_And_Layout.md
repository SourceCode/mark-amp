# Phase 07 -- Graph View Data Model and Layout Engine

## Objective

Implement the knowledge graph data computation and force-directed layout engine. This phase builds the data layer that computes node positions for the global graph view: a full vault visualization where documents are nodes and wikilinks are edges. Includes the force-directed simulation (spring-electric model), node sizing by reference count, and clustering detection. Phase 08 adds the interactive UI.

## Prerequisites

- Phase 03 (BacklinkIndex -- link data)
- Phase 02 (VaultService -- document listing)
- Phase 04 (TagService -- tag connections)

## Feature References (PRD)

- PRD #2: Graph View
- PRD #36: Backlink Visualization Per Note

## Data Structures to Implement

### File: `src/core/GraphEngine.h`

```cpp
#pragma once

#include <atomic>
#include <cmath>
#include <functional>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class VaultService;
class BacklinkIndex;
class TagService;

struct GraphNode
{
    std::string id;
    std::string label;
    std::string notebook_id;
    double x{0.0};
    double y{0.0};
    double vx{0.0};     // velocity x
    double vy{0.0};     // velocity y
    double radius{6.0};
    int link_count{0};   // total incoming + outgoing
    bool is_daily_note{false};
    bool is_orphan{false};
    int cluster_id{-1};  // -1 = unassigned

    enum class NodeType : uint8_t { Document, Tag, Heading } type{NodeType::Document};
};

struct GraphEdge
{
    int source_index{0};  // Index into nodes vector
    int target_index{0};
    double strength{1.0};

    enum class EdgeType : uint8_t { WikiLink, Embed, Tag } type{EdgeType::WikiLink};
};

struct GraphLayoutConfig
{
    // Force simulation parameters
    double repulsion_strength{-400.0};
    double attraction_strength{0.01};
    double damping{0.9};
    double min_distance{20.0};
    double max_velocity{50.0};
    double convergence_threshold{0.01};
    int max_iterations{500};

    // Node sizing
    double node_size_min{4.0};
    double node_size_max{28.0};
    bool scale_by_links{true};

    // Filtering
    int min_links{0};
    bool show_orphans{true};
    bool show_daily_notes{true};
    bool show_tags{false};
    std::set<std::string> highlight_tags;  // Tags to highlight
};

struct GraphData
{
    std::vector<GraphNode> nodes;
    std::vector<GraphEdge> edges;
    double bounds_x{0.0};
    double bounds_y{0.0};
    double bounds_width{1000.0};
    double bounds_height{1000.0};
    int cluster_count{0};

    [[nodiscard]] auto node_count() const -> int;
    [[nodiscard]] auto edge_count() const -> int;
    [[nodiscard]] auto find_node(const std::string& id) const -> const GraphNode*;
    [[nodiscard]] auto find_node_index(const std::string& id) const -> std::optional<int>;
};

/// Computes graph data from the vault and runs force-directed layout.
class GraphEngine
{
public:
    GraphEngine(EventBus& event_bus,
                VaultService& vault_service,
                BacklinkIndex& backlink_index,
                TagService& tag_service);

    /// Build graph data from the current vault state.
    [[nodiscard]] auto build_graph(const GraphLayoutConfig& config) -> GraphData;

    /// Build a local subgraph centered on a document with BFS to depth.
    [[nodiscard]] auto build_local_graph(const std::string& center_id,
                                          int depth,
                                          const GraphLayoutConfig& config) -> GraphData;

    /// Run the force-directed layout simulation on existing graph data.
    /// Modifies node positions in-place. Returns iteration count.
    auto run_layout(GraphData& graph, const GraphLayoutConfig& config) -> int;

    /// Run a single layout iteration (for animated layout).
    /// Returns the total kinetic energy (for convergence check).
    [[nodiscard]] auto step_layout(GraphData& graph,
                                    const GraphLayoutConfig& config) -> double;

    /// Detect clusters using connected component analysis.
    auto detect_clusters(GraphData& graph) -> int;

    /// Cancel a running layout simulation.
    auto cancel_layout() -> void;

    /// Invalidate cached graph data.
    auto invalidate() -> void;

private:
    EventBus& event_bus_;
    VaultService& vault_service_;
    BacklinkIndex& backlink_index_;
    TagService& tag_service_;

    std::atomic<bool> cancel_requested_{false};
    mutable std::mutex graph_mutex_;

    // Force calculation helpers
    auto apply_repulsion(GraphData& graph, const GraphLayoutConfig& config) -> void;
    auto apply_attraction(GraphData& graph, const GraphLayoutConfig& config) -> void;
    auto apply_centering(GraphData& graph) -> void;
    auto apply_damping(GraphData& graph, double damping) -> void;
    auto clamp_velocity(GraphData& graph, double max_vel) -> void;
    auto update_positions(GraphData& graph) -> void;
    [[nodiscard]] auto total_kinetic_energy(const GraphData& graph) const -> double;

    // Graph construction
    auto collect_nodes(const GraphLayoutConfig& config) -> std::vector<GraphNode>;
    auto collect_edges(const std::vector<GraphNode>& nodes) -> std::vector<GraphEdge>;
    auto compute_node_sizes(GraphData& graph, const GraphLayoutConfig& config) -> void;
    auto initialize_positions(GraphData& graph) -> void;  // Random initial placement

    // Local graph
    auto bfs_neighbors(const std::string& center_id, int depth) const
        -> std::set<std::string>;

    Subscription vault_changed_sub_;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`build_graph(config)`** -- Collect nodes from VaultService (one per document, optionally one per tag). Collect edges from BacklinkIndex (wikilinks). Apply filters (min_links, show_orphans, etc.). Initialize random positions. Run layout simulation. Detect clusters. Compute bounds. Return GraphData.

2. **`run_layout(graph, config)`** -- Iterate: apply_repulsion, apply_attraction, apply_centering, apply_damping, clamp_velocity, update_positions. Check convergence (total_kinetic_energy < threshold). Respect cancel_requested_. Return iteration count.

3. **`apply_repulsion(graph, config)`** -- For every pair of nodes, compute repulsive force: F = -strength / distance^2 (Coulomb's law). Apply force to both nodes' velocities. Use Barnes-Hut approximation for large graphs (>500 nodes).

4. **`apply_attraction(graph, config)`** -- For each edge, compute attractive force: F = strength * distance (Hooke's law). Pull source and target toward each other.

5. **`step_layout(graph, config)`** -- Execute one iteration of the force simulation. Return kinetic energy for animated rendering.

6. **`detect_clusters(graph)`** -- Union-Find on edges. Assign cluster_id to each node. Return number of clusters.

7. **`build_local_graph(center_id, depth, config)`** -- BFS from center node to depth. Collect only reachable nodes and their edges. Run layout with center pinned.

8. **`compute_node_sizes(graph, config)`** -- Map link_count to radius using logarithmic scaling between node_size_min and node_size_max.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphComputedEvent)
int node_count{0};
int edge_count{0};
int cluster_count{0};
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphLayoutIterationEvent)
int iteration{0};
double kinetic_energy{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(GraphInvalidatedEvent);
```

## Test Cases

File: `tests/unit/test_graph_engine.cpp`

1. **Build graph -- node count** -- 5 documents in vault. Verify graph has 5 nodes.
2. **Build graph -- edge count** -- 3 wikilinks. Verify 3 edges.
3. **Filter orphans** -- config.show_orphans=false. Verify isolated nodes excluded.
4. **Filter min_links** -- config.min_links=2. Verify only well-connected nodes shown.
5. **Force layout converges** -- Run layout. Verify kinetic_energy drops below threshold.
6. **Repulsion separates nodes** -- 2 overlapping nodes, no edges. After layout, distance > min_distance.
7. **Attraction connects edges** -- 2 nodes with edge. After layout, distance shorter than unconnected pair.
8. **Cluster detection** -- 2 disconnected subgraphs. Verify cluster_count == 2.
9. **Local graph BFS** -- Chain A->B->C->D. Local graph at B, depth=1. Verify {A,B,C} included.
10. **Node sizing** -- Node with 10 links is larger than node with 1 link.
11. **Cancel layout** -- Start layout, cancel after 10 iterations. Verify returns early.
12. **Bounds computation** -- After layout, bounds contain all nodes.

## Acceptance Criteria

- [ ] Graph correctly represents vault documents and wikilinks
- [ ] Force-directed layout converges to stable positions
- [ ] Repulsion prevents node overlap; attraction keeps edges short
- [ ] Cluster detection identifies disconnected subgraphs
- [ ] Local graph BFS returns correct neighborhood
- [ ] Node sizes scale with link count
- [ ] Layout cancellation works mid-simulation
- [ ] Filters (orphans, min_links, daily notes) correctly exclude nodes
- [ ] All 12 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/GraphEngine.h` | GraphNode, GraphEdge, GraphData, GraphEngine |
| CREATE | `src/core/GraphEngine.cpp` | Force simulation and graph building |
| MODIFY | `src/core/Events.h` | Add 3 graph events |
| MODIFY | `src/core/PluginContext.h` | Add `GraphEngine* graph_engine{nullptr};` |
| MODIFY | `src/CMakeLists.txt` | Add GraphEngine.cpp |
| CREATE | `tests/unit/test_graph_engine.cpp` | 12 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_graph_engine target |

## Architecture Notes

- GraphEngine uses BacklinkIndex for link data (not raw file scanning)
- Force simulation is CPU-intensive -- large graphs (>1000 nodes) should run on a background thread
- step_layout() enables animated rendering where the UI calls it each frame
- Barnes-Hut tree for O(n log n) repulsion on large graphs
- Constructor injection: GraphEngine(EventBus&, VaultService&, BacklinkIndex&, TagService&)

## Estimated Complexity

**XL** -- Force-directed simulation with multiple force types, BFS, cluster detection, Barnes-Hut approximation, 12 tests.
