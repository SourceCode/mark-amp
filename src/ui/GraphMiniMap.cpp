#include "GraphMiniMap.h"

namespace markamp::ui
{

void GraphMiniMap::set_size(float width, float height)
{
    width_ = width;
    height_ = height;
}

void GraphMiniMap::set_position(float pos_x, float pos_y)
{
    pos_x_ = pos_x;
    pos_y_ = pos_y;
}

void GraphMiniMap::set_enabled(bool enabled)
{
    enabled_ = enabled;
}

void GraphMiniMap::update_bounds(double min_x, double min_y, double max_x, double max_y)
{
    bounds_min_x_ = min_x;
    bounds_min_y_ = min_y;
    bounds_max_x_ = max_x;
    bounds_max_y_ = max_y;
}

void GraphMiniMap::update_viewport(double view_x,
                                   double view_y,
                                   double view_width,
                                   double view_height)
{
    viewport_x_ = view_x;
    viewport_y_ = view_y;
    viewport_width_ = view_width;
    viewport_height_ = view_height;
}

void GraphMiniMap::update_graph(const core::GraphData& data)
{
    graph_data_ = data;
}

void GraphMiniMap::render()
{
    if (!enabled_)
    {
        return;
    }

    // Stub: actual rendering deferred to UI integration phase
    // Would draw scaled dots for nodes and a viewport rectangle
}

} // namespace markamp::ui
