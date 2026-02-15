# Phase 08 -- Graph View Interactive UI

## Objective

Implement the interactive Graph View panel: a zoomable, pannable, custom-drawn wxPanel that visualizes the knowledge graph computed by GraphEngine (Phase 07). Supports mouse interaction (click to select, drag to move nodes, hover for tooltips), zoom/pan with mouse wheel and trackpad, node coloring by cluster or tag, search-to-highlight, and animated layout.

## Prerequisites

- Phase 07 (GraphEngine -- graph data and layout computation)
- Existing ThemeEngine, EventBus, MainFrame sidebar infrastructure

## Feature References (PRD)

- PRD #2: Graph View (interactive network graph)
- PRD #10: Customizable UI (graph view as a pane)

## Data Structures to Implement

### File: `src/ui/GraphViewPanel.h`

```cpp
#pragma once

#include "core/GraphEngine.h"

#include <wx/panel.h>
#include <wx/timer.h>

#include <optional>
#include <string>

namespace markamp::core { class EventBus; class ThemeEngine; class VaultService; }

namespace markamp::ui
{

struct GraphViewState
{
    double zoom{1.0};
    double pan_x{0.0};
    double pan_y{0.0};
    std::optional<int> selected_node;       // Index into graph.nodes
    std::optional<int> hovered_node;
    std::optional<int> dragging_node;
    bool is_panning{false};
    double drag_start_x{0.0};
    double drag_start_y{0.0};
};

struct GraphViewConfig
{
    bool animate_layout{true};
    int animation_fps{30};
    bool show_labels{true};
    bool show_edge_labels{false};
    double label_min_zoom{0.5};     // Hide labels below this zoom
    bool color_by_cluster{true};
    bool color_by_tag{false};
    std::string highlight_tag;
    double zoom_min{0.1};
    double zoom_max{5.0};
};

class GraphViewPanel : public wxPanel
{
public:
    GraphViewPanel(wxWindow* parent,
                   markamp::core::EventBus& event_bus,
                   markamp::core::GraphEngine& graph_engine,
                   markamp::core::VaultService& vault_service,
                   markamp::core::ThemeEngine& theme_engine);
    ~GraphViewPanel() override;

    /// Refresh the graph view with current vault data.
    auto refresh_graph() -> void;

    /// Center the view on a specific document node.
    auto focus_on_document(const std::string& document_id) -> void;

    /// Highlight nodes matching a search term.
    auto highlight_search(const std::string& search_term) -> void;

    /// Clear highlights.
    auto clear_highlight() -> void;

    /// Get/set the layout configuration.
    [[nodiscard]] auto layout_config() const -> const markamp::core::GraphLayoutConfig&;
    auto set_layout_config(const markamp::core::GraphLayoutConfig& config) -> void;

    /// Get/set the view configuration.
    [[nodiscard]] auto view_config() const -> const GraphViewConfig&;
    auto set_view_config(const GraphViewConfig& config) -> void;

private:
    markamp::core::EventBus& event_bus_;
    markamp::core::GraphEngine& graph_engine_;
    markamp::core::VaultService& vault_service_;
    markamp::core::ThemeEngine& theme_engine_;

    markamp::core::GraphData graph_data_;
    markamp::core::GraphLayoutConfig layout_config_;
    GraphViewConfig view_config_;
    GraphViewState state_;

    wxTimer animation_timer_;
    bool layout_running_{false};

    // Rendering
    auto on_paint(wxPaintEvent& event) -> void;
    auto draw_edges(wxDC& dc) -> void;
    auto draw_nodes(wxDC& dc) -> void;
    auto draw_labels(wxDC& dc) -> void;
    auto draw_selection_ring(wxDC& dc, int node_index) -> void;
    auto draw_hover_tooltip(wxDC& dc, int node_index) -> void;

    // Coordinate transforms
    [[nodiscard]] auto world_to_screen(double wx, double wy) const -> std::pair<double, double>;
    [[nodiscard]] auto screen_to_world(double sx, double sy) const -> std::pair<double, double>;

    // Input handling
    auto on_mouse_down(wxMouseEvent& event) -> void;
    auto on_mouse_up(wxMouseEvent& event) -> void;
    auto on_mouse_move(wxMouseEvent& event) -> void;
    auto on_mouse_wheel(wxMouseEvent& event) -> void;
    auto on_key_down(wxKeyEvent& event) -> void;

    // Hit testing
    [[nodiscard]] auto hit_test_node(double screen_x, double screen_y) const
        -> std::optional<int>;

    // Animation
    auto on_animation_tick(wxTimerEvent& event) -> void;
    auto start_animation() -> void;
    auto stop_animation() -> void;

    // Node colors
    [[nodiscard]] auto node_color(const markamp::core::GraphNode& node) const -> wxColour;
    [[nodiscard]] auto edge_color(const markamp::core::GraphEdge& edge) const -> wxColour;

    auto apply_theme() -> void;

    markamp::core::Subscription vault_changed_sub_;
    markamp::core::Subscription theme_changed_sub_;
    markamp::core::Subscription graph_computed_sub_;
};

} // namespace markamp::ui
```

## Key Functions to Implement

1. **`refresh_graph()`** -- Call graph_engine_.build_graph(layout_config_). Store result in graph_data_. If animate_layout, start animation timer. Else, run full layout synchronously. Refresh().

2. **`on_paint(event)`** -- Double-buffered paint. Apply zoom/pan transform. Call draw_edges(), draw_nodes(), draw_labels(). If hovered_node, draw tooltip. If selected_node, draw selection ring.

3. **`draw_nodes(dc)`** -- For each node: transform to screen coords. Draw filled circle with node_color(). Size = node.radius * zoom. Skip if off-screen (frustum culling).

4. **`draw_edges(dc)`** -- For each edge: draw line from source to target node. Use edge_color(). Alpha based on zoom level. Skip if both endpoints off-screen.

5. **`draw_labels(dc)`** -- If zoom > label_min_zoom: draw node labels. Truncate long labels. Use contrast color against node fill.

6. **`on_mouse_down(event)`** -- Hit test for nodes. If node hit: start dragging (pin node position). If no node: start panning.

7. **`on_mouse_move(event)`** -- If dragging: update node position. If panning: update pan_x/pan_y. Always: update hovered_node via hit_test.

8. **`on_mouse_wheel(event)`** -- Zoom in/out centered on cursor position. Clamp to zoom_min/zoom_max.

9. **`on_animation_tick(event)`** -- Call graph_engine_.step_layout(graph_data_, layout_config_). If converged, stop animation. Refresh().

10. **`focus_on_document(document_id)`** -- Find node by ID. Set pan to center on that node. Flash/highlight the node briefly.

11. **`highlight_search(search_term)`** -- Find nodes whose labels match. Dim non-matching nodes. Bright-fill matching nodes.

12. **`node_color(node)`** -- If color_by_cluster: assign palette color by cluster_id. If color_by_tag and node has highlight_tag: use accent color.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphNodeSelectedEvent)
std::string document_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphNodeDoubleClickedEvent)
std::string document_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphViewZoomChangedEvent)
double zoom_level{1.0};
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_graph_view.cpp`

1. **Graph renders without crash** -- Build graph with 10 nodes. Trigger paint. No crash.
2. **Zoom in/out** -- Simulate wheel events. Verify zoom changes within bounds.
3. **Pan** -- Simulate drag on empty space. Verify pan offset changes.
4. **Node hit test** -- Place node at known position. Hit test at that position. Verify node found.
5. **Node selection** -- Click on node. Verify GraphNodeSelectedEvent published.
6. **Node double-click navigation** -- Double-click node. Verify GraphNodeDoubleClickedEvent published.
7. **Node drag** -- Click and drag node. Verify node position updates.
8. **Focus on document** -- Call focus_on_document(). Verify view centered on target.
9. **Search highlight** -- highlight_search("test"). Verify matching nodes flagged.
10. **Theme colors** -- Change theme. Verify node and edge colors update.

## Acceptance Criteria

- [ ] Graph renders all nodes and edges with correct positions
- [ ] Zoom and pan work smoothly with mouse wheel and drag
- [ ] Node click selects and publishes event; double-click navigates
- [ ] Node drag moves the node in the layout
- [ ] Labels display at sufficient zoom levels
- [ ] Animated layout shows simulation converging
- [ ] Search highlighting dims non-matching nodes
- [ ] Cluster coloring assigns distinct colors to disconnected groups
- [ ] Frustum culling prevents rendering off-screen elements
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/ui/GraphViewPanel.h` | GraphViewPanel, state/config structs |
| CREATE | `src/ui/GraphViewPanel.cpp` | Full rendering and interaction |
| MODIFY | `src/ui/MainFrame.cpp` | Register GraphViewPanel as a sidebar tab |
| MODIFY | `src/core/Events.h` | Add 3 graph view events |
| MODIFY | `src/CMakeLists.txt` | Add GraphViewPanel.cpp |
| CREATE | `tests/unit/test_graph_view.cpp` | 10 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_graph_view target |

## Architecture Notes

- Uses double-buffered wxPaintDC for flicker-free rendering
- Animation timer runs at 30fps during layout simulation
- Large graphs (>500 nodes) should batch render with LOD (skip labels, simplify edges)
- Constructor injection for all services
- The panel can be placed in the sidebar or opened as a full tab

## Estimated Complexity

**XL** -- Custom painting, coordinate transforms, mouse interaction state machine, animated layout, frustum culling, 10 tests.
