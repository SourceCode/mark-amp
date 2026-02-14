# Phase 28 -- Knowledge Graph Visualization Panel

## Objective

Implement the interactive knowledge graph visualization panel that renders the GraphData computed in Phase 27 as an interactive force-directed node-link diagram. SiYuan uses vis.js for browser-based rendering; MarkAmp implements this natively using wxGraphicsContext for hardware-accelerated 2D rendering with a custom force-directed layout engine. The panel supports zoom, pan, node selection, hover tooltips showing block content previews, click-to-navigate, and real-time filtering controls.

The visualization consists of three interconnected systems: (1) a ForceDirectedLayout physics simulation that computes node positions using repulsive forces between all nodes (Coulomb's law) and attractive forces along links (Hooke's spring law), with velocity damping and convergence detection; (2) a GraphRenderer that draws nodes as circles sized by reference count and colored by type or notebook, draws links as lines or curves with optional arrowheads, and renders labels; (3) an interaction layer that handles mouse events for zoom (scroll wheel), pan (click-drag on background), node selection (click), multi-select (shift-click), hover detection (mouse move), and context menus (right-click). The layout simulation runs on a timer at 60fps during active layout and switches to idle when the simulation converges (total kinetic energy below threshold).

The panel also includes a mini-map in the corner for navigation in large graphs, a search bar that highlights matching nodes, and a filter sidebar with controls for block type, notebook, minimum reference count, and layout parameter sliders (force strength, link distance). A fullscreen toggle expands the graph panel to fill the entire window. Right-click context menus on nodes provide actions: Open Document, Show Local Graph, Copy Block Reference, Pin Node Position.

## Prerequisites

- Phase 27 (Knowledge Graph Data Computation) -- provides GraphService, GraphData, GraphNode, GraphLink, GraphConfig, and the adjacency list computation

## SiYuan Source Reference

- `app/src/layout/dock/Graph.ts` (~782 lines) -- vis.js Network configuration, node/edge styling, event handlers, search, filtering, fullscreen
- `app/src/layout/dock/Graph.ts:initNetwork()` -- vis.js physics configuration: gravitational constant, spring length, spring constant, damping
- `app/src/layout/dock/Graph.ts:updateGraph()` -- fetch graph data via API, update vis.js network
- SiYuan graph styling: nodes sized by ref_count, colored by notebook, links as lines with type-based colors

## MarkAmp Integration Points

### New Files to Create

| File | Namespace | Purpose |
|------|-----------|---------|
| `src/core/ForceDirectedLayout.h` | `markamp::core` | Physics simulation for node positioning |
| `src/core/ForceDirectedLayout.cpp` | `markamp::core` | ForceDirectedLayout implementation |
| `src/ui/GraphPanel.h` | `markamp::ui` | Interactive graph visualization panel |
| `src/ui/GraphPanel.cpp` | `markamp::ui` | GraphPanel rendering and interaction |
| `src/ui/GraphMiniMap.h` | `markamp::ui` | Mini-map overlay for graph navigation |
| `src/ui/GraphMiniMap.cpp` | `markamp::ui` | GraphMiniMap implementation |
| `src/ui/GraphFilterBar.h` | `markamp::ui` | Filter controls sidebar |
| `src/ui/GraphFilterBar.cpp` | `markamp::ui` | GraphFilterBar implementation |
| `tests/unit/test_force_layout.cpp` | (test) | Catch2 test suite for layout physics |

### Existing Files to Modify

| File | Change |
|------|--------|
| `src/core/Events.h` | Add graph interaction events |
| `src/ui/MainFrame.h` | Add graph panel management and menu entries |
| `src/ui/MainFrame.cpp` | Wire graph panel activation and toolbar buttons |
| `src/ui/LayoutManager.h` | Register graph panel as a dock panel |
| `src/CMakeLists.txt` | Add new source files |
| `tests/CMakeLists.txt` | Add test_force_layout target |
| `resources/config_defaults.json` | Add graph visualization config defaults |

## Data Structures to Implement

```cpp
// ============================================================================
// File: src/core/ForceDirectedLayout.h
// ============================================================================
#pragma once

#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// 2D position with velocity for physics simulation.
struct LayoutNode
{
    std::string id;
    double x{0.0};
    double y{0.0};
    double vx{0.0};       // Velocity X
    double vy{0.0};       // Velocity Y
    double mass{1.0};     // Node mass (derived from ref_count)
    bool pinned{false};   // If true, position is fixed

    [[nodiscard]] auto kinetic_energy() const -> double
    {
        return 0.5 * mass * (vx * vx + vy * vy);
    }

    [[nodiscard]] auto distance_to(const LayoutNode& other) const -> double
    {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};

/// A link in the physics simulation.
struct LayoutLink
{
    std::string source_id;
    std::string target_id;
    double strength{1.0};       // Spring constant
    double ideal_length{100.0}; // Rest length of the spring
};

/// Parameters for the force-directed simulation.
struct LayoutParams
{
    double repulsion_strength{-300.0};   // Coulomb constant (negative = repulsive)
    double spring_constant{0.01};         // Hooke's spring constant
    double damping{0.9};                  // Velocity damping per step (0-1)
    double max_velocity{50.0};            // Velocity cap per axis
    double convergence_threshold{0.5};    // Total KE below this = converged
    double center_gravity{0.05};          // Pull toward center (prevents drift)
    double time_step{1.0};               // Simulation time step
    int max_iterations{500};             // Stop after this many steps
};

/// Force-directed layout engine using Fruchterman-Reingold style forces.
/// Computes 2D positions for nodes connected by links.
class ForceDirectedLayout
{
public:
    ForceDirectedLayout();
    explicit ForceDirectedLayout(const LayoutParams& params);

    /// Add a node to the simulation. Initial position is random if not specified.
    void add_node(const std::string& id, double x, double y, double mass = 1.0);

    /// Add a link (spring) between two nodes.
    void add_link(const std::string& source_id, const std::string& target_id,
                  double strength = 1.0, double ideal_length = 100.0);

    /// Remove all nodes and links.
    void clear();

    /// Perform one simulation step. Returns false when the layout has converged
    /// (total kinetic energy below convergence_threshold).
    [[nodiscard]] auto step() -> bool;

    /// Run the simulation until convergence or max_iterations.
    /// Returns the number of iterations performed.
    [[nodiscard]] auto run_to_convergence() -> int;

    /// Get the current position of a node.
    [[nodiscard]] auto get_position(const std::string& id) const
        -> std::pair<double, double>;

    /// Get all node positions.
    [[nodiscard]] auto get_all_positions() const
        -> std::unordered_map<std::string, std::pair<double, double>>;

    /// Pin a node at its current position (prevents movement).
    void pin_node(const std::string& id);

    /// Unpin a node (allows movement again).
    void unpin_node(const std::string& id);

    /// Move a node to a specific position (e.g., during drag).
    void set_position(const std::string& id, double x, double y);

    /// Get the total kinetic energy (used for convergence check).
    [[nodiscard]] auto total_kinetic_energy() const -> double;

    /// Get the bounding box of all nodes: {min_x, min_y, max_x, max_y}.
    [[nodiscard]] auto bounding_box() const
        -> std::tuple<double, double, double, double>;

    /// Update simulation parameters.
    void set_params(const LayoutParams& params);

    /// Get current simulation parameters.
    [[nodiscard]] auto params() const -> const LayoutParams&
    {
        return params_;
    }

    /// Whether the simulation has converged.
    [[nodiscard]] auto is_converged() const -> bool
    {
        return converged_;
    }

    /// Current iteration count.
    [[nodiscard]] auto iteration_count() const -> int
    {
        return iteration_;
    }

private:
    LayoutParams params_;
    std::vector<LayoutNode> nodes_;
    std::vector<LayoutLink> links_;
    std::unordered_map<std::string, size_t> node_index_; // id -> index in nodes_
    bool converged_{false};
    int iteration_{0};

    /// Apply repulsive forces between all node pairs (O(n^2) Coulomb).
    void apply_repulsion();

    /// Apply attractive forces along links (Hooke's spring).
    void apply_attraction();

    /// Apply gravity toward center to prevent drift.
    void apply_center_gravity();

    /// Update velocities and positions with damping.
    void integrate();

    /// Clamp velocity to max_velocity.
    void clamp_velocities();

    /// Check convergence based on total kinetic energy.
    void check_convergence();
};

} // namespace markamp::core
```

```cpp
// ============================================================================
// File: src/ui/GraphPanel.h (partial -- key declarations)
// ============================================================================
#pragma once

#include "core/ForceDirectedLayout.h"
#include "core/GraphTypes.h"

#include <wx/panel.h>
#include <wx/timer.h>

#include <memory>
#include <optional>
#include <set>
#include <string>

namespace markamp::core
{
class GraphService;
class EventBus;
class Config;
} // namespace markamp::core

namespace markamp::ui
{

/// Interactive knowledge graph visualization panel.
/// Renders nodes and links using wxGraphicsContext with force-directed layout.
class GraphPanel : public wxPanel
{
public:
    GraphPanel(wxWindow* parent,
               core::GraphService& graph_service,
               core::EventBus& event_bus,
               core::Config& config);
    ~GraphPanel() override;

    /// Load and display the global graph.
    void show_global_graph();

    /// Load and display a local graph centered on a block.
    void show_local_graph(const std::string& center_block_id, int depth = 3);

    /// Update the graph with new data (e.g., after filter change).
    void update_graph(const core::GraphData& data);

    /// Toggle fullscreen mode.
    void toggle_fullscreen();

    /// Search and highlight nodes matching a query.
    void search_nodes(const std::string& query);

    /// Get the currently selected node IDs.
    [[nodiscard]] auto selected_nodes() const -> const std::set<std::string>&;

private:
    // ── Rendering ──
    void OnPaint(wxPaintEvent& event);
    void render_nodes(wxGraphicsContext* gc);
    void render_links(wxGraphicsContext* gc);
    void render_labels(wxGraphicsContext* gc);
    void render_selection_highlights(wxGraphicsContext* gc);
    void render_search_highlights(wxGraphicsContext* gc);

    // ── Interaction ──
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnRightClick(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    // ── Layout Animation ──
    void OnLayoutTimer(wxTimerEvent& event);
    void start_layout_animation();
    void stop_layout_animation();

    // ── Coordinate Transforms ──
    [[nodiscard]] auto world_to_screen(double wx, double wy) const
        -> std::pair<double, double>;
    [[nodiscard]] auto screen_to_world(double sx, double sy) const
        -> std::pair<double, double>;
    [[nodiscard]] auto hit_test_node(double sx, double sy) const
        -> std::optional<std::string>;
    [[nodiscard]] auto node_screen_radius(const core::GraphNode& node) const -> double;

    // ── Node Styling ──
    [[nodiscard]] auto node_color(const core::GraphNode& node) const -> wxColour;
    [[nodiscard]] auto link_color(const core::GraphLink& link) const -> wxColour;

    // ── Context Menu ──
    void show_context_menu(const std::string& node_id, wxPoint pos);

    // ── Members ──
    core::GraphService& graph_service_;
    core::EventBus& event_bus_;
    core::Config& config_;

    core::GraphData graph_data_;
    core::ForceDirectedLayout layout_;
    core::GraphConfig graph_config_;

    wxTimer layout_timer_;
    static constexpr int kLayoutTimerIntervalMs = 16; // ~60fps

    // View transform
    double zoom_{1.0};
    double pan_x_{0.0};
    double pan_y_{0.0};

    // Interaction state
    bool dragging_node_{false};
    bool panning_{false};
    std::string dragged_node_id_;
    wxPoint drag_start_;
    std::set<std::string> selected_nodes_;
    std::optional<std::string> hovered_node_id_;
    std::set<std::string> search_highlighted_;
    bool fullscreen_{false};

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui
```

## Key Functions to Implement

1. **`ForceDirectedLayout::step()`** -- Execute one simulation step: (a) clear all forces, (b) call `apply_repulsion()` to compute pairwise Coulomb forces, (c) call `apply_attraction()` to compute spring forces along links, (d) call `apply_center_gravity()`, (e) call `integrate()` to update velocities and positions, (f) call `clamp_velocities()`, (g) call `check_convergence()`. Increment `iteration_`. Return `!converged_`.

2. **`ForceDirectedLayout::apply_repulsion()`** -- For each pair of nodes (i, j), compute the repulsive force: `F = repulsion_strength / distance^2`. Apply the force as acceleration to both nodes in opposite directions along the line connecting them. Guard against zero distance by clamping to a minimum (e.g., 0.1).

3. **`ForceDirectedLayout::apply_attraction()`** -- For each link, compute the spring force: `F = spring_constant * (distance - ideal_length)`. Apply the force to both endpoints. Positive distance delta pulls nodes together; negative pushes apart.

4. **`ForceDirectedLayout::integrate()`** -- For each unpinned node, update velocity: `vx = (vx + ax * time_step) * damping`, same for vy. Update position: `x += vx * time_step`. Skip pinned nodes.

5. **`GraphPanel::OnPaint(event)`** -- Create a wxGraphicsContext from the paint DC. Clear background. Apply the zoom/pan transform. Call `render_links()`, then `render_nodes()`, then `render_labels()`, then `render_selection_highlights()` and `render_search_highlights()`. Render the mini-map overlay in the corner.

6. **`GraphPanel::render_nodes(gc)`** -- For each GraphNode in `graph_data_`, look up its position from the layout. Compute screen-space position via `world_to_screen()`. Compute radius from `node.size_factor()` scaled to `[node_size_min, node_size_max]` and multiplied by zoom. Set fill color from `node_color()`. Draw a filled circle. If the node is hovered, draw an additional highlight ring.

7. **`GraphPanel::render_links(gc)`** -- For each GraphLink, look up source and target positions. Draw a line from source to target using `link_color()`. Optionally draw an arrowhead at the target end for directed refs (BlockRef, DocLink). Use semi-transparent colors for links.

8. **`GraphPanel::OnMouseWheel(event)`** -- Compute zoom delta from scroll amount. Zoom toward the mouse cursor position: adjust `pan_x_` and `pan_y_` so the world point under the cursor stays fixed. Clamp zoom to [0.1, 10.0]. Trigger Refresh().

9. **`GraphPanel::hit_test_node(sx, sy)`** -- Convert screen coordinates to world coordinates. For each node, compute distance from the world point to the node's position. If distance is less than the node's visual radius, return the node ID. Check in reverse z-order so topmost node is returned first.

10. **`GraphPanel::OnMouseDown(event)`** -- Hit test for a node. If a node is hit: if shift is held, toggle the node in `selected_nodes_`; otherwise, set `selected_nodes_` to just that node, begin `dragging_node_` mode, pin the node in the layout. If no node is hit: begin `panning_` mode, clear selection.

11. **`GraphPanel::OnLayoutTimer(event)`** -- Call `layout_.step()`. If the layout returns false (converged), stop the timer. Call `Refresh()` to repaint.

12. **`GraphPanel::show_context_menu(node_id, pos)`** -- Create a wxMenu with items: "Open Document", "Show Local Graph", "Copy Block Reference", "Pin/Unpin Node Position". Bind handlers. Call `PopupMenu()`.

13. **`GraphPanel::search_nodes(query)`** -- Iterate all nodes. For each node whose label contains the query (case-insensitive), add its ID to `search_highlighted_`. Pan the view to center on the first match. Refresh.

14. **`GraphMiniMap::render(gc)`** -- Compute the bounding box of all nodes. Scale it to fit in the mini-map rectangle (e.g., 150x100 pixels in the bottom-right corner). Draw dots for nodes and lines for links at reduced scale. Draw a viewport rectangle showing the current visible region.

15. **`GraphFilterBar::OnFilterChanged()`** -- Read filter control values (min_refs slider, type checkboxes, show_daily_notes toggle). Construct a new GraphConfig. Publish `GraphFilterChangedEvent`. Trigger graph recomputation via `graph_service_.get_global_graph(new_config)`.

## Events to Add

Add the following to `src/core/Events.h`:

```cpp
// ============================================================================
// Graph visualization events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphNodeClickedEvent)
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphNodeHoveredEvent)
std::string block_id;
std::string label;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(GraphLayoutCompleteEvent);

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ShowLocalGraphRequestEvent)
std::string center_block_id;
int depth{3};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(ShowGlobalGraphRequestEvent);
```

## Config Keys to Add

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `knowledgebase.graph.node_size_min` | double | 4.0 | Minimum node radius in pixels |
| `knowledgebase.graph.node_size_max` | double | 24.0 | Maximum node radius in pixels |
| `knowledgebase.graph.show_labels` | bool | true | Show node labels on the graph |
| `knowledgebase.graph.show_arrows` | bool | true | Show directional arrows on links |
| `knowledgebase.graph.animation_fps` | int | 60 | Layout animation frame rate |
| `knowledgebase.graph.minimap_enabled` | bool | true | Show the mini-map overlay |
| `knowledgebase.graph.background_color` | string | "#1a1a2e" | Graph panel background color |

## Test Cases

All tests in `tests/unit/test_force_layout.cpp` using Catch2.

1. **"ForceDirectedLayout converges for two connected nodes"** -- Add 2 nodes at random positions and 1 link between them. Run `run_to_convergence()`. Verify the distance between nodes is approximately equal to the ideal link length (within 20% tolerance). Verify `is_converged()` returns true.

2. **"ForceDirectedLayout separates unconnected nodes"** -- Add 3 nodes at the same position (0,0) with no links. Run to convergence. Verify all three nodes have moved apart (pairwise distances > 0). Repulsion should spread them.

3. **"pinned nodes do not move"** -- Add 2 linked nodes. Pin node A at position (0,0). Run to convergence. Verify node A's position is still (0,0). Node B should have moved to approximately `ideal_length` distance away.

4. **"step returns false when converged"** -- Create a simple 2-node graph. Run `step()` in a loop. Verify it eventually returns false. Verify `iteration_count()` is less than `max_iterations`.

5. **"bounding_box computes correct extents"** -- Add nodes at known positions: (-10, -5), (20, 15), (0, 0). Verify bounding_box returns (-10, -5, 20, 15).

6. **"set_position moves a node and resets velocity"** -- Add a node with non-zero velocity. Call `set_position(id, 100, 200)`. Verify the position is updated. Verify velocity is zeroed.

7. **"layout handles single node without crashing"** -- Add 1 node, no links. Run `step()`. Verify it does not crash. Node should stay near center due to gravity.

8. **"layout handles disconnected components"** -- Add two clusters: nodes A-B-C connected, and nodes D-E connected, with no links between clusters. Run to convergence. Verify both clusters are internally compact but separated from each other.

9. **"total_kinetic_energy decreases over time"** -- Add 5 nodes with random velocities. Record KE after step 1 and step 50. Verify KE at step 50 is less than KE at step 1 (damping reduces energy).

10. **"large graph (100 nodes) converges within max_iterations"** -- Add 100 nodes with random links (each node linked to 2-3 others). Run `run_to_convergence()`. Verify it returns before `max_iterations`. Verify no node positions are NaN or Inf.

## Acceptance Criteria

- [ ] ForceDirectedLayout produces stable, non-overlapping positions for connected graphs
- [ ] Physics simulation converges (kinetic energy drops below threshold) within reasonable iterations
- [ ] Pinned nodes maintain their position during simulation
- [ ] GraphPanel renders nodes as sized, colored circles with optional labels
- [ ] GraphPanel renders links as lines with optional arrowheads
- [ ] Zoom (mouse wheel) works with cursor-centered zooming
- [ ] Pan (click-drag background) scrolls the view
- [ ] Node click publishes GraphNodeClickedEvent for navigation
- [ ] Node hover shows a tooltip with block content preview
- [ ] Context menu offers Open Document, Show Local Graph, Copy Block Ref, Pin Node

## Files to Create/Modify

```
CREATE  src/core/ForceDirectedLayout.h
CREATE  src/core/ForceDirectedLayout.cpp
CREATE  src/ui/GraphPanel.h
CREATE  src/ui/GraphPanel.cpp
CREATE  src/ui/GraphMiniMap.h
CREATE  src/ui/GraphMiniMap.cpp
CREATE  src/ui/GraphFilterBar.h
CREATE  src/ui/GraphFilterBar.cpp
CREATE  tests/unit/test_force_layout.cpp
MODIFY  src/core/Events.h              -- add graph visualization events
MODIFY  src/ui/MainFrame.h             -- add graph panel management
MODIFY  src/ui/MainFrame.cpp           -- wire graph panel activation
MODIFY  src/ui/LayoutManager.h         -- register graph panel as dock panel
MODIFY  src/CMakeLists.txt              -- add new source files
MODIFY  tests/CMakeLists.txt            -- add test_force_layout target
MODIFY  resources/config_defaults.json  -- add graph visualization defaults
```
