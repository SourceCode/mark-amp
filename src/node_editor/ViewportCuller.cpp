#include "ViewportCuller.h"

namespace markamp::node_editor
{

ViewportCuller::ViewportCuller(float margin)
    : margin_(margin)
{
}

// ---------------------------------------------------------------------------
// Culling
// ---------------------------------------------------------------------------

auto ViewportCuller::cull_nodes(const Rect& visible_rect,
                                const std::vector<NodeLayoutResult>& layouts) const -> CullResult
{
    CullResult result;
    result.total_nodes = layouts.size();

    // Expand the viewport rect by the margin for smooth scrolling.
    const Rect expanded_rect = visible_rect.expanded(margin_);

    for (const auto& layout : layouts)
    {
        if (expanded_rect.intersects(layout.total_bounds))
        {
            result.visible_nodes.push_back(layout.node_id);
        }
        else
        {
            result.culled_nodes++;
        }
    }

    return result;
}

auto ViewportCuller::lod_for_zoom(float zoom) -> LodLevel
{
    if (zoom >= 0.4F)
    {
        return LodLevel::kFull;
    }
    if (zoom >= 0.2F)
    {
        return LodLevel::kSimplified;
    }
    if (zoom >= 0.05F)
    {
        return LodLevel::kBlock;
    }
    return LodLevel::kDot;
}

void ViewportCuller::set_lod_thresholds(float simplified, float block, float dot)
{
    threshold_simplified_ = simplified;
    threshold_block_ = block;
    threshold_dot_ = dot;
}

} // namespace markamp::node_editor
