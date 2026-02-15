#pragma once

#include "../core/ForceDirectedLayout.h"
#include "../core/GraphTypes.h"

#include <functional>
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
/// Renders nodes and links with force-directed layout.
class GraphPanel
{
public:
    using OnNodeClickedCallback = std::function<void(const std::string& node_id)>;
    using OnNodeHoveredCallback =
        std::function<void(const std::string& node_id, const std::string& label)>;

    GraphPanel(core::GraphService& graph_service, core::EventBus& event_bus, core::Config& config);

    // ── Panel lifecycle ──

    void initialize();
    void layout(float pos_x, float pos_y, float width, float height);
    void render();

    // ── Graph display ──

    void show_global_graph();
    void show_local_graph(const std::string& center_block_id, int depth = 3);
    void update_graph(const core::GraphData& data);
    void toggle_fullscreen();
    void search_nodes(const std::string& query);

    // ── Selection ──

    [[nodiscard]] auto selected_nodes() const -> const std::set<std::string>&;

    // ── Callbacks ──

    void set_on_node_clicked(OnNodeClickedCallback callback);
    void set_on_node_hovered(OnNodeHoveredCallback callback);

private:
    core::GraphService& graph_service_;
    [[maybe_unused]] core::EventBus& event_bus_;
    [[maybe_unused]] core::Config& config_;

    float pos_x_{0}, pos_y_{0}, width_{0}, height_{0};

    core::GraphData graph_data_;
    core::ForceDirectedLayout layout_engine_;
    core::GraphConfig graph_config_;

    // View transform
    double zoom_{1.0};
    double pan_x_{0.0};
    double pan_y_{0.0};

    // Interaction state
    [[maybe_unused]] bool dragging_node_{false};
    [[maybe_unused]] bool panning_{false};
    std::string dragged_node_id_;
    std::set<std::string> selected_nodes_;
    std::optional<std::string> hovered_node_id_;
    std::set<std::string> search_highlighted_;
    bool fullscreen_{false};

    OnNodeClickedCallback on_node_clicked_;
    OnNodeHoveredCallback on_node_hovered_;

    // ── Coordinate Transforms ──

    [[nodiscard]] auto world_to_screen(double world_x, double world_y) const
        -> std::pair<double, double>;
    [[nodiscard]] auto screen_to_world(double screen_x, double screen_y) const
        -> std::pair<double, double>;
    [[nodiscard]] auto hit_test_node(double screen_x, double screen_y) const
        -> std::optional<std::string>;
    [[nodiscard]] auto node_screen_radius(const core::GraphNode& node) const -> double;
};

} // namespace markamp::ui
