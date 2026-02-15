#pragma once

/// @file GraphViewPanel.h
/// @brief V4 Phase 08 – Graph View Interactive UI controller and view state.

#include "../core/EventBus.h"
#include "../core/GraphEngine.h"

#include <optional>
#include <string>

namespace markamp::core
{

class BacklinkIndex;
class TagService;
class VaultService;

// ============================================================================
// View State
// ============================================================================

/// Camera/viewport state for the graph view.
struct GraphViewport
{
    double center_x{0.0};
    double center_y{0.0};
    double zoom{1.0};
    double min_zoom{0.1};
    double max_zoom{5.0};
    int viewport_width{800};
    int viewport_height{600};
};

/// Result of a hit-test: which node (if any) is under a pixel coordinate.
struct HitTestResult
{
    bool hit{false};
    int node_index{-1};
    std::string node_id;
};

/// State of interactive dragging.
struct DragState
{
    bool is_dragging{false};
    bool is_panning{false};
    int dragged_node_index{-1};
    double start_x{0.0};
    double start_y{0.0};
    double node_start_x{0.0};
    double node_start_y{0.0};
};

/// Minimap rendering state.
struct MinimapState
{
    bool visible{true};
    double minimap_x{0.0};
    double minimap_y{0.0};
    double minimap_width{150.0};
    double minimap_height{100.0};
};

/// Display preferences for graph rendering.
struct GraphDisplayConfig
{
    bool show_labels{true};
    bool show_minimap{true};
    bool animate_transitions{true};
    bool curved_edges{false};
    double label_min_zoom{0.4}; // Min zoom to show labels
    double edge_opacity{0.4};
    double node_border_width{2.0};
    double edge_width{1.5};
    double hover_scale{1.3};    // Scale factor for hovered nodes
    double selected_scale{1.2}; // Scale factor for selected nodes
};

// ============================================================================
// Graph View Panel Controller
// ============================================================================

/// Non-UI controller for the interactive graph view.
/// Manages viewport, selection, hover, drag, zoom/pan, and hit-testing.
/// Delegates graph computation to GraphEngine.
class GraphViewPanelController
{
public:
    GraphViewPanelController(EventBus& event_bus,
                             GraphEngine& graph_engine,
                             VaultService& vault_service);

    // --- Graph Management ---

    /// Build the full graph and run layout.
    auto build_full_graph() -> void;

    /// Build a local graph centered on a document.
    auto build_local_graph(const std::string& document_id, int depth = 2) -> void;

    /// Get the current graph data (read-only).
    [[nodiscard]] auto graph_data() const -> const GraphData&;

    /// Get the current layout config.
    [[nodiscard]] auto layout_config() const -> const GraphLayoutConfig&;

    /// Update the layout config and rebuild.
    auto set_layout_config(const GraphLayoutConfig& config) -> void;

    // --- Viewport ---

    /// Get the current viewport.
    [[nodiscard]] auto viewport() const -> const GraphViewport&;

    /// Set viewport dimensions (from window resize).
    auto set_viewport_size(int width, int height) -> void;

    /// Zoom by delta at a focal pixel point.
    auto zoom_at(double delta, double pixel_x, double pixel_y) -> void;

    /// Zoom to fit all nodes in the viewport.
    auto zoom_to_fit() -> void;

    /// Reset zoom to 1.0 and center the view.
    auto reset_view() -> void;

    // --- Coordinate Transforms ---

    /// Convert graph coordinates to screen pixel coordinates.
    [[nodiscard]] auto graph_to_screen(double graph_x, double graph_y) const
        -> std::pair<double, double>;

    /// Convert screen pixel coordinates to graph coordinates.
    [[nodiscard]] auto screen_to_graph(double pixel_x, double pixel_y) const
        -> std::pair<double, double>;

    // --- Hit Testing ---

    /// Hit-test at pixel coordinates.
    [[nodiscard]] auto hit_test(double pixel_x, double pixel_y) const -> HitTestResult;

    // --- Selection ---

    /// Get the currently selected node index (-1 if none).
    [[nodiscard]] auto selected_node_index() const -> int;

    /// Select a node by index. Publishes GraphNodeSelectedEvent.
    auto select_node(int node_index) -> void;

    /// Clear selection.
    auto clear_selection() -> void;

    // --- Hover ---

    /// Get the currently hovered node index (-1 if none).
    [[nodiscard]] auto hovered_node_index() const -> int;

    /// Update hover state from mouse position (pixel coords).
    auto update_hover(double pixel_x, double pixel_y) -> void;

    /// Clear hover.
    auto clear_hover() -> void;

    // --- Drag (node or pan) ---

    /// Begin drag at pixel position.
    auto begin_drag(double pixel_x, double pixel_y) -> void;

    /// Continue drag to new pixel position.
    auto update_drag(double pixel_x, double pixel_y) -> void;

    /// End drag.
    auto end_drag() -> void;

    /// Get the current drag state.
    [[nodiscard]] auto drag_state() const -> const DragState&;

    // --- Double Click ---

    /// Handle double-click at pixel position (opens document).
    auto handle_double_click(double pixel_x, double pixel_y) -> void;

    // --- Display Config ---

    [[nodiscard]] auto display_config() const -> const GraphDisplayConfig&;
    auto set_display_config(const GraphDisplayConfig& config) -> void;

private:
    EventBus& event_bus_;
    GraphEngine& graph_engine_;
    VaultService& vault_service_;

    GraphData graph_;
    GraphLayoutConfig layout_config_;
    GraphViewport viewport_;
    GraphDisplayConfig display_config_;
    DragState drag_state_;

    int selected_node_index_{-1};
    int hovered_node_index_{-1};

    Subscription vault_changed_sub_;
};

} // namespace markamp::core
