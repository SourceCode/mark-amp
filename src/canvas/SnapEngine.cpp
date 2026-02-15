#include "SnapEngine.h"

#include <cmath>

namespace markamp::canvas
{

// ── Grid Snap ──────────────────────────────────────────────────

auto SnapEngine::snap_to_grid(const Point2D& point) const -> Point2D
{
    if (!config_.grid_enabled || config_.grid_spacing <= 0.0)
    {
        return point;
    }

    return Point2D{std::round(point.x / config_.grid_spacing) * config_.grid_spacing,
                   std::round(point.y / config_.grid_spacing) * config_.grid_spacing};
}

// ── Object Snap ────────────────────────────────────────────────

auto SnapEngine::snap_to_objects(const AABB& moving, const std::vector<AABB>& others) const
    -> SnapResult
{
    SnapResult result;
    result.snapped_x = moving.min_x;
    result.snapped_y = moving.min_y;
    result.snapped = false;

    if (!config_.object_snap_enabled || others.empty())
    {
        return result;
    }

    const double threshold = config_.snap_threshold;
    const auto moving_center = moving.center();
    const double moving_right = moving.max_x;
    const double moving_bottom = moving.max_y;

    bool snapped_h = false;
    bool snapped_v = false;

    for (const auto& other : others)
    {
        const auto other_center = other.center();
        const double other_right = other.max_x;
        const double other_bottom = other.max_y;

        // ── Vertical alignment (snap X) ────────────────────────

        if (!snapped_h)
        {
            // Center-to-center vertical.
            if (std::abs(moving_center.x - other_center.x) < threshold)
            {
                result.snapped_x = other_center.x - moving.width() / 2.0;
                result.snapped = true;
                snapped_h = true;
                result.guide_lines.push_back(
                    GuideLine{Point2D{other_center.x, std::min(moving.min_y, other.min_y)},
                              Point2D{other_center.x, std::max(moving_bottom, other_bottom)},
                              GuideLineType::kCenterV});
            }
            // Left edge to left edge.
            else if (std::abs(moving.min_x - other.min_x) < threshold)
            {
                result.snapped_x = other.min_x;
                result.snapped = true;
                snapped_h = true;
                result.guide_lines.push_back(
                    GuideLine{Point2D{other.min_x, std::min(moving.min_y, other.min_y)},
                              Point2D{other.min_x, std::max(moving_bottom, other_bottom)},
                              GuideLineType::kEdgeLeft});
            }
            // Right edge to right edge.
            else if (std::abs(moving_right - other_right) < threshold)
            {
                result.snapped_x = other_right - moving.width();
                result.snapped = true;
                snapped_h = true;
                result.guide_lines.push_back(
                    GuideLine{Point2D{other_right, std::min(moving.min_y, other.min_y)},
                              Point2D{other_right, std::max(moving_bottom, other_bottom)},
                              GuideLineType::kEdgeRight});
            }
        }

        // ── Horizontal alignment (snap Y) ──────────────────────

        if (!snapped_v)
        {
            // Center-to-center horizontal.
            if (std::abs(moving_center.y - other_center.y) < threshold)
            {
                result.snapped_y = other_center.y - moving.height() / 2.0;
                result.snapped = true;
                snapped_v = true;
                result.guide_lines.push_back(
                    GuideLine{Point2D{std::min(moving.min_x, other.min_x), other_center.y},
                              Point2D{std::max(moving_right, other_right), other_center.y},
                              GuideLineType::kCenterH});
            }
            // Top edge to top edge.
            else if (std::abs(moving.min_y - other.min_y) < threshold)
            {
                result.snapped_y = other.min_y;
                result.snapped = true;
                snapped_v = true;
                result.guide_lines.push_back(
                    GuideLine{Point2D{std::min(moving.min_x, other.min_x), other.min_y},
                              Point2D{std::max(moving_right, other_right), other.min_y},
                              GuideLineType::kEdgeTop});
            }
            // Bottom edge to bottom edge.
            else if (std::abs(moving_bottom - other_bottom) < threshold)
            {
                result.snapped_y = other_bottom - moving.height();
                result.snapped = true;
                snapped_v = true;
                result.guide_lines.push_back(
                    GuideLine{Point2D{std::min(moving.min_x, other.min_x), other_bottom},
                              Point2D{std::max(moving_right, other_right), other_bottom},
                              GuideLineType::kEdgeBottom});
            }
        }

        if (snapped_h && snapped_v)
        {
            break;
        }
    }

    return result;
}

// ── Combined Snap ──────────────────────────────────────────────

auto SnapEngine::snap_point(const Point2D& proposed,
                            const AABB& moving_bounds,
                            const std::vector<AABB>& other_bounds,
                            const ViewportTransform& /*viewport*/) const -> SnapResult
{
    // Try object snapping first (higher priority).
    if (config_.object_snap_enabled && !other_bounds.empty())
    {
        auto obj_result = snap_to_objects(moving_bounds, other_bounds);
        if (obj_result.snapped)
        {
            return obj_result;
        }
    }

    // Fall back to grid snapping.
    if (config_.grid_enabled)
    {
        const auto grid_pt = snap_to_grid(proposed);
        SnapResult result;
        result.snapped_x = grid_pt.x;
        result.snapped_y = grid_pt.y;
        result.snapped = (grid_pt.x != proposed.x || grid_pt.y != proposed.y);
        return result;
    }

    // No snapping.
    SnapResult result;
    result.snapped_x = proposed.x;
    result.snapped_y = proposed.y;
    result.snapped = false;
    return result;
}

// ── Configuration ──────────────────────────────────────────────

auto SnapEngine::config() const -> const SnapConfig&
{
    return config_;
}
auto SnapEngine::set_config(const SnapConfig& cfg) -> void
{
    config_ = cfg;
}

} // namespace markamp::canvas
