#include "GraphPanel.h"

#include "../core/GraphService.h"

#include <algorithm>
#include <cmath>

namespace markamp::ui
{

GraphPanel::GraphPanel(core::GraphService& graph_service,
                       core::EventBus& event_bus,
                       core::Config& config)
    : graph_service_(graph_service)
    , event_bus_(event_bus)
    , config_(config)
{
}

void GraphPanel::initialize()
{
    graph_config_ = graph_service_.default_config();
    selected_nodes_.clear();
    search_highlighted_.clear();
}

void GraphPanel::layout(float pos_x, float pos_y, float width, float height)
{
    pos_x_ = pos_x;
    pos_y_ = pos_y;
    width_ = width;
    height_ = height;
}

void GraphPanel::render()
{
    // Improvement 4: Full graph rendering using layout engine positions.
    // This method is called by the host wxPanel's paint handler.
    // The host panel passes its wxDC, but since GraphPanel is a model class
    // (not a wxPanel), we prepare render data that the host can consume.

    // Compute all screen positions for nodes
    auto positions = layout_engine_.get_all_positions();

    // The host panel should call render_to_dc() instead.
    // This method now verifies the data is ready.
    render_data_ready_ = !graph_data_.nodes.empty();
}

auto GraphPanel::get_render_nodes() const -> std::vector<RenderNode>
{
    std::vector<RenderNode> nodes;
    nodes.reserve(graph_data_.nodes.size());

    for (const auto& node : graph_data_.nodes)
    {
        RenderNode render_node;
        render_node.id = node.id;
        render_node.label = node.label;

        auto [world_x, world_y] = layout_engine_.get_position(node.id);
        auto [sx, sy] = world_to_screen(world_x, world_y);
        render_node.screen_x = sx;
        render_node.screen_y = sy;
        render_node.radius = node_screen_radius(node) * zoom_;
        render_node.is_selected = selected_nodes_.contains(node.id);
        render_node.is_hovered = hovered_node_id_.has_value() && *hovered_node_id_ == node.id;
        render_node.is_search_highlighted = search_highlighted_.contains(node.id);

        nodes.push_back(std::move(render_node));
    }
    return nodes;
}

auto GraphPanel::get_render_edges() const -> std::vector<RenderEdge>
{
    std::vector<RenderEdge> edges;
    edges.reserve(graph_data_.links.size());

    for (const auto& link : graph_data_.links)
    {
        auto [src_x, src_y] = layout_engine_.get_position(link.source_id);
        auto [tgt_x, tgt_y] = layout_engine_.get_position(link.target_id);
        auto [src_sx, src_sy] = world_to_screen(src_x, src_y);
        auto [tgt_sx, tgt_sy] = world_to_screen(tgt_x, tgt_y);

        RenderEdge render_edge;
        render_edge.source_x = src_sx;
        render_edge.source_y = src_sy;
        render_edge.target_x = tgt_sx;
        render_edge.target_y = tgt_sy;
        edges.push_back(render_edge);
    }
    return edges;
}

void GraphPanel::handle_mouse_click(double screen_x, double screen_y)
{
    // Improvement 5: Mouse click handling for node selection
    auto hit = hit_test_node(screen_x, screen_y);
    if (hit.has_value())
    {
        selected_nodes_.clear();
        selected_nodes_.insert(*hit);
        if (on_node_clicked_)
        {
            on_node_clicked_(*hit);
        }
    }
    else
    {
        selected_nodes_.clear();
    }
}

void GraphPanel::handle_mouse_move(double screen_x, double screen_y)
{
    // Improvement 5: Mouse hover handling
    auto hit = hit_test_node(screen_x, screen_y);
    if (hit != hovered_node_id_)
    {
        hovered_node_id_ = hit;
        if (hit.has_value() && on_node_hovered_)
        {
            // Find the label for this node
            for (const auto& node : graph_data_.nodes)
            {
                if (node.id == *hit)
                {
                    on_node_hovered_(*hit, node.label);
                    break;
                }
            }
        }
    }
}

void GraphPanel::handle_scroll(double delta, double screen_x, double screen_y)
{
    // Improvement 5: Zoom with scroll wheel at focal point
    constexpr double kZoomFactor = 1.1;
    auto [world_x, world_y] = screen_to_world(screen_x, screen_y);

    if (delta > 0)
    {
        zoom_ *= kZoomFactor;
    }
    else
    {
        zoom_ /= kZoomFactor;
    }

    // Clamp zoom
    zoom_ = std::clamp(zoom_, 0.1, 10.0);

    // Adjust pan to keep the focal point under cursor
    auto [new_world_x, new_world_y] = screen_to_world(screen_x, screen_y);
    pan_x_ += new_world_x - world_x;
    pan_y_ += new_world_y - world_y;
}

void GraphPanel::handle_pan(double delta_x, double delta_y)
{
    // Improvement 5: Pan by dragging
    pan_x_ += delta_x / zoom_;
    pan_y_ += delta_y / zoom_;
}

void GraphPanel::show_global_graph()
{
    auto result = graph_service_.get_global_graph(graph_config_);
    if (result)
    {
        update_graph(*result);
    }
}

void GraphPanel::show_local_graph(const std::string& center_block_id, int depth)
{
    auto result = graph_service_.get_local_graph(center_block_id, depth, graph_config_);
    if (result)
    {
        update_graph(*result);
    }
}

void GraphPanel::update_graph(const core::GraphData& data)
{
    graph_data_ = data;
    layout_engine_.clear();

    // Add nodes with positions spread around origin
    double angle_step = 0.0;
    if (!data.nodes.empty())
    {
        angle_step = 2.0 * 3.14159265358979 / static_cast<double>(data.nodes.size());
    }

    for (size_t idx = 0; idx < data.nodes.size(); ++idx)
    {
        const auto& node = data.nodes[idx];
        const double angle = static_cast<double>(idx) * angle_step;
        const double radius = 200.0;
        layout_engine_.add_node(
            node.id, radius * std::cos(angle), radius * std::sin(angle), node.size_factor());
    }

    for (const auto& link : data.links)
    {
        layout_engine_.add_link(link.source_id, link.target_id, 1.0, graph_config_.link_distance);
    }

    // Run layout to convergence
    (void)layout_engine_.run_to_convergence();
}

void GraphPanel::toggle_fullscreen()
{
    fullscreen_ = !fullscreen_;
}

void GraphPanel::search_nodes(const std::string& query)
{
    search_highlighted_.clear();

    if (query.empty())
    {
        return;
    }

    // Case-insensitive search in node labels
    std::string lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

    for (const auto& node : graph_data_.nodes)
    {
        std::string lower_label = node.label;
        std::transform(lower_label.begin(), lower_label.end(), lower_label.begin(), ::tolower);

        if (lower_label.find(lower_query) != std::string::npos)
        {
            search_highlighted_.insert(node.id);
        }
    }
}

auto GraphPanel::selected_nodes() const -> const std::set<std::string>&
{
    return selected_nodes_;
}

void GraphPanel::set_on_node_clicked(OnNodeClickedCallback callback)
{
    on_node_clicked_ = std::move(callback);
}

void GraphPanel::set_on_node_hovered(OnNodeHoveredCallback callback)
{
    on_node_hovered_ = std::move(callback);
}

auto GraphPanel::world_to_screen(double world_x, double world_y) const -> std::pair<double, double>
{
    const double screen_x = (world_x + pan_x_) * zoom_ + static_cast<double>(width_) / 2.0;
    const double screen_y = (world_y + pan_y_) * zoom_ + static_cast<double>(height_) / 2.0;
    return {screen_x, screen_y};
}

auto GraphPanel::screen_to_world(double screen_x, double screen_y) const
    -> std::pair<double, double>
{
    const double world_x = (screen_x - static_cast<double>(width_) / 2.0) / zoom_ - pan_x_;
    const double world_y = (screen_y - static_cast<double>(height_) / 2.0) / zoom_ - pan_y_;
    return {world_x, world_y};
}

auto GraphPanel::hit_test_node(double screen_x, double screen_y) const -> std::optional<std::string>
{
    const auto [world_x, world_y] = screen_to_world(screen_x, screen_y);

    for (const auto& node : graph_data_.nodes)
    {
        const auto [node_x, node_y] = layout_engine_.get_position(node.id);
        const double delta_x = world_x - node_x;
        const double delta_y = world_y - node_y;
        const double dist = std::sqrt(delta_x * delta_x + delta_y * delta_y);

        if (dist <= node_screen_radius(node) / zoom_)
        {
            return node.id;
        }
    }

    return std::nullopt;
}

auto GraphPanel::node_screen_radius(const core::GraphNode& node) const -> double
{
    const double factor = node.size_factor();
    const double range = graph_config_.node_size_max - graph_config_.node_size_min;
    return graph_config_.node_size_min + factor * range / 5.0; // Normalize
}

} // namespace markamp::ui
