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
    // Stub: actual rendering (wxGraphicsContext or similar) deferred
    // to UI integration phase. Layout engine positions are available
    // via layout_engine_.get_all_positions().
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
