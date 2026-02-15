/// @file GraphViewPanel.cpp
/// @brief V4 Phase 08 – Graph View Interactive UI controller implementation.

#include "ui/GraphViewPanel.h"

#include "core/Events.h"
#include "core/VaultService.h"

#include <algorithm>
#include <cmath>

namespace markamp::core
{

// ============================================================================
// GraphViewPanelController
// ============================================================================

GraphViewPanelController::GraphViewPanelController(EventBus& event_bus,
                                                   GraphEngine& graph_engine,
                                                   VaultService& vault_service)
    : event_bus_(event_bus)
    , graph_engine_(graph_engine)
    , vault_service_(vault_service)
{
    vault_changed_sub_ = event_bus_.subscribe<events::VaultOpenedEvent>(
        [this](const events::VaultOpenedEvent&) { build_full_graph(); });
}

// --- Graph Management ---

auto GraphViewPanelController::build_full_graph() -> void
{
    graph_ = graph_engine_.build_graph(layout_config_);
    graph_engine_.detect_clusters(graph_);

    selected_node_index_ = -1;
    hovered_node_index_ = -1;

    auto event = events::GraphComputedEvent{};
    event.node_count = graph_.node_count();
    event.edge_count = graph_.edge_count();
    event_bus_.publish(event);
}

auto GraphViewPanelController::build_local_graph(const std::string& document_id, int depth) -> void
{
    graph_ = graph_engine_.build_local_graph(document_id, depth, layout_config_);

    selected_node_index_ = -1;
    hovered_node_index_ = -1;

    auto event = events::GraphComputedEvent{};
    event.node_count = graph_.node_count();
    event.edge_count = graph_.edge_count();
    event_bus_.publish(event);
}

auto GraphViewPanelController::graph_data() const -> const GraphData&
{
    return graph_;
}

auto GraphViewPanelController::layout_config() const -> const GraphLayoutConfig&
{
    return layout_config_;
}

auto GraphViewPanelController::set_layout_config(const GraphLayoutConfig& config) -> void
{
    layout_config_ = config;
    build_full_graph();
}

// --- Viewport ---

auto GraphViewPanelController::viewport() const -> const GraphViewport&
{
    return viewport_;
}

auto GraphViewPanelController::set_viewport_size(int width, int height) -> void
{
    viewport_.viewport_width = width;
    viewport_.viewport_height = height;
}

auto GraphViewPanelController::zoom_at(double delta, double pixel_x, double pixel_y) -> void
{
    // Get graph position under cursor before zoom
    auto [graph_x, graph_y] = screen_to_graph(pixel_x, pixel_y);

    // Apply zoom factor
    const double zoom_factor = 1.0 + delta * 0.1;
    const double new_zoom = viewport_.zoom * zoom_factor;
    viewport_.zoom = std::clamp(new_zoom, viewport_.min_zoom, viewport_.max_zoom);

    // Adjust center so the graph point stays under cursor
    const double half_w = static_cast<double>(viewport_.viewport_width) * 0.5;
    const double half_h = static_cast<double>(viewport_.viewport_height) * 0.5;
    viewport_.center_x = graph_x - (pixel_x - half_w) / viewport_.zoom;
    viewport_.center_y = graph_y - (pixel_y - half_h) / viewport_.zoom;

    auto event = events::GraphViewZoomChangedEvent{};
    event.zoom_level = viewport_.zoom;
    event_bus_.publish(event);
}

auto GraphViewPanelController::zoom_to_fit() -> void
{
    if (graph_.nodes.empty())
    {
        return;
    }

    // Find bounding box
    double min_x = graph_.bounds_x;
    double min_y = graph_.bounds_y;
    double max_x = graph_.bounds_x + graph_.bounds_width;
    double max_y = graph_.bounds_y + graph_.bounds_height;

    const double graph_width = max_x - min_x;
    const double graph_height = max_y - min_y;

    if (graph_width <= 0.0 || graph_height <= 0.0)
    {
        return;
    }

    // Compute zoom to fit with padding
    constexpr double kPadding = 50.0;
    const double avail_w = static_cast<double>(viewport_.viewport_width) - 2.0 * kPadding;
    const double avail_h = static_cast<double>(viewport_.viewport_height) - 2.0 * kPadding;

    const double zoom_x = avail_w / graph_width;
    const double zoom_y = avail_h / graph_height;
    viewport_.zoom = std::clamp(std::min(zoom_x, zoom_y), viewport_.min_zoom, viewport_.max_zoom);

    // Center on the graph
    viewport_.center_x = min_x + graph_width * 0.5;
    viewport_.center_y = min_y + graph_height * 0.5;

    auto event = events::GraphViewZoomChangedEvent{};
    event.zoom_level = viewport_.zoom;
    event_bus_.publish(event);
}

auto GraphViewPanelController::reset_view() -> void
{
    viewport_.zoom = 1.0;
    viewport_.center_x = 0.0;
    viewport_.center_y = 0.0;
    zoom_to_fit();
}

// --- Coordinate Transforms ---

auto GraphViewPanelController::graph_to_screen(double graph_x, double graph_y) const
    -> std::pair<double, double>
{
    const double half_w = static_cast<double>(viewport_.viewport_width) * 0.5;
    const double half_h = static_cast<double>(viewport_.viewport_height) * 0.5;

    const double screen_x = (graph_x - viewport_.center_x) * viewport_.zoom + half_w;
    const double screen_y = (graph_y - viewport_.center_y) * viewport_.zoom + half_h;

    return {screen_x, screen_y};
}

auto GraphViewPanelController::screen_to_graph(double pixel_x, double pixel_y) const
    -> std::pair<double, double>
{
    const double half_w = static_cast<double>(viewport_.viewport_width) * 0.5;
    const double half_h = static_cast<double>(viewport_.viewport_height) * 0.5;

    const double graph_x = (pixel_x - half_w) / viewport_.zoom + viewport_.center_x;
    const double graph_y = (pixel_y - half_h) / viewport_.zoom + viewport_.center_y;

    return {graph_x, graph_y};
}

// --- Hit Testing ---

auto GraphViewPanelController::hit_test(double pixel_x, double pixel_y) const -> HitTestResult
{
    HitTestResult result;

    auto [graph_x, graph_y] = screen_to_graph(pixel_x, pixel_y);

    // Find closest node within its radius
    for (int idx = 0; idx < static_cast<int>(graph_.nodes.size()); ++idx)
    {
        const auto& node = graph_.nodes[static_cast<size_t>(idx)];
        const double delta_x = node.x - graph_x;
        const double delta_y = node.y - graph_y;
        const double dist_sq = delta_x * delta_x + delta_y * delta_y;

        // Use scaled radius for hit testing
        const double hit_radius = node.radius * display_config_.hover_scale;
        if (dist_sq <= hit_radius * hit_radius)
        {
            result.hit = true;
            result.node_index = idx;
            result.node_id = node.id;
            return result; // Return first hit (nodes rendered front-to-back)
        }
    }

    return result;
}

// --- Selection ---

auto GraphViewPanelController::selected_node_index() const -> int
{
    return selected_node_index_;
}

auto GraphViewPanelController::select_node(int node_index) -> void
{
    selected_node_index_ = node_index;

    if (node_index >= 0 && node_index < static_cast<int>(graph_.nodes.size()))
    {
        auto event = events::GraphNodeSelectedEvent{};
        event.document_id = graph_.nodes[static_cast<size_t>(node_index)].id;
        event_bus_.publish(event);
    }
}

auto GraphViewPanelController::clear_selection() -> void
{
    selected_node_index_ = -1;
}

// --- Hover ---

auto GraphViewPanelController::hovered_node_index() const -> int
{
    return hovered_node_index_;
}

auto GraphViewPanelController::update_hover(double pixel_x, double pixel_y) -> void
{
    auto result = hit_test(pixel_x, pixel_y);
    hovered_node_index_ = result.hit ? result.node_index : -1;
}

auto GraphViewPanelController::clear_hover() -> void
{
    hovered_node_index_ = -1;
}

// --- Drag ---

auto GraphViewPanelController::begin_drag(double pixel_x, double pixel_y) -> void
{
    auto result = hit_test(pixel_x, pixel_y);

    drag_state_.start_x = pixel_x;
    drag_state_.start_y = pixel_y;

    if (result.hit)
    {
        // Dragging a node
        drag_state_.is_dragging = true;
        drag_state_.is_panning = false;
        drag_state_.dragged_node_index = result.node_index;

        const auto& node = graph_.nodes[static_cast<size_t>(result.node_index)];
        drag_state_.node_start_x = node.x;
        drag_state_.node_start_y = node.y;
    }
    else
    {
        // Panning the viewport
        drag_state_.is_dragging = false;
        drag_state_.is_panning = true;
        drag_state_.dragged_node_index = -1;
    }
}

auto GraphViewPanelController::update_drag(double pixel_x, double pixel_y) -> void
{
    if (drag_state_.is_dragging && drag_state_.dragged_node_index >= 0)
    {
        // Move the node
        const double dx_screen = pixel_x - drag_state_.start_x;
        const double dy_screen = pixel_y - drag_state_.start_y;

        auto& node = graph_.nodes[static_cast<size_t>(drag_state_.dragged_node_index)];
        node.x = drag_state_.node_start_x + dx_screen / viewport_.zoom;
        node.y = drag_state_.node_start_y + dy_screen / viewport_.zoom;
    }
    else if (drag_state_.is_panning)
    {
        // Pan the viewport
        const double dx_screen = pixel_x - drag_state_.start_x;
        const double dy_screen = pixel_y - drag_state_.start_y;

        viewport_.center_x -= dx_screen / viewport_.zoom;
        viewport_.center_y -= dy_screen / viewport_.zoom;

        drag_state_.start_x = pixel_x;
        drag_state_.start_y = pixel_y;
    }
}

auto GraphViewPanelController::end_drag() -> void
{
    if (drag_state_.is_dragging && drag_state_.dragged_node_index >= 0)
    {
        // Node drag complete — velocity reset to keep it in place
        auto& node = graph_.nodes[static_cast<size_t>(drag_state_.dragged_node_index)];
        node.vx = 0.0;
        node.vy = 0.0;
    }

    drag_state_.is_dragging = false;
    drag_state_.is_panning = false;
    drag_state_.dragged_node_index = -1;
}

auto GraphViewPanelController::drag_state() const -> const DragState&
{
    return drag_state_;
}

// --- Double Click ---

auto GraphViewPanelController::handle_double_click(double pixel_x, double pixel_y) -> void
{
    auto result = hit_test(pixel_x, pixel_y);
    if (result.hit)
    {
        auto event = events::GraphNodeDoubleClickedEvent{};
        event.document_id = result.node_id;
        event_bus_.publish(event);

        // Also publish a file open request
        auto open_event = events::FileOpenRequestEvent{};
        open_event.file_path = result.node_id;
        event_bus_.publish(open_event);
    }
}

// --- Display Config ---

auto GraphViewPanelController::display_config() const -> const GraphDisplayConfig&
{
    return display_config_;
}

auto GraphViewPanelController::set_display_config(const GraphDisplayConfig& config) -> void
{
    display_config_ = config;
}

} // namespace markamp::core
