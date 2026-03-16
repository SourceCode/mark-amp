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

    // Improvement 8: Compute scaled minimap rendering data.
    // The host wxPanel calls get_minimap_nodes() and get_viewport_rect()
    // to draw the minimap overlay.

    // Compute the scale from world bounds to minimap pixel dimensions
    const double world_width = bounds_max_x_ - bounds_min_x_;
    const double world_height = bounds_max_y_ - bounds_min_y_;

    if (world_width <= 0.0 || world_height <= 0.0)
    {
        return;
    }

    // Use uniform scaling to preserve aspect ratio
    const double kScaleX = static_cast<double>(width_) / world_width;
    const double kScaleY = static_cast<double>(height_) / world_height;
    const double kUniformScale = std::min(kScaleX, kScaleY);
    minimap_scale_x_ = kUniformScale;
    minimap_scale_y_ = kUniformScale;

    // Compute viewport rectangle in minimap space
    viewport_rect_x_ = (viewport_x_ - bounds_min_x_) * minimap_scale_x_;
    viewport_rect_y_ = (viewport_y_ - bounds_min_y_) * minimap_scale_y_;
    viewport_rect_w_ = viewport_width_ * minimap_scale_x_;
    viewport_rect_h_ = viewport_height_ * minimap_scale_y_;

    // Clamp viewport rectangle to minimap bounds
    viewport_rect_x_ = std::max(0.0, std::min(viewport_rect_x_, static_cast<double>(width_)));
    viewport_rect_y_ = std::max(0.0, std::min(viewport_rect_y_, static_cast<double>(height_)));
    viewport_rect_w_ = std::min(viewport_rect_w_, static_cast<double>(width_) - viewport_rect_x_);
    viewport_rect_h_ = std::min(viewport_rect_h_, static_cast<double>(height_) - viewport_rect_y_);
}

} // namespace markamp::ui
