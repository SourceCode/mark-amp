#pragma once

#include "../core/GraphTypes.h"

#include <string>
#include <tuple>

namespace markamp::ui
{

/// Mini-map overlay for graph navigation.
/// Shows a scaled-down view of the entire graph with a viewport rectangle.
class GraphMiniMap
{
public:
    GraphMiniMap() = default;

    // ── Configuration ──

    void set_size(float width, float height);
    void set_position(float pos_x, float pos_y);
    void set_enabled(bool enabled);

    // ── State Update ──

    void update_bounds(double min_x, double min_y, double max_x, double max_y);
    void update_viewport(double view_x, double view_y, double view_width, double view_height);
    void update_graph(const core::GraphData& data);

    // ── Rendering ──

    void render();

    // ── Accessors ──

    [[nodiscard]] auto is_enabled() const -> bool
    {
        return enabled_;
    }
    [[nodiscard]] auto width() const -> float
    {
        return width_;
    }
    [[nodiscard]] auto height() const -> float
    {
        return height_;
    }

private:
    float width_{150.0F};
    float height_{100.0F};
    float pos_x_{0.0F};
    float pos_y_{0.0F};
    bool enabled_{true};

    // Graph bounds
    double bounds_min_x_{0.0};
    double bounds_min_y_{0.0};
    double bounds_max_x_{0.0};
    double bounds_max_y_{0.0};

    // Current viewport in world coordinates
    double viewport_x_{0.0};
    double viewport_y_{0.0};
    double viewport_width_{0.0};
    double viewport_height_{0.0};

    core::GraphData graph_data_;
};

} // namespace markamp::ui
