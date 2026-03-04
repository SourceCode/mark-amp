#include "ConnectorRouteEngine.h"

#include <algorithm>

namespace markamp::canvas
{

void ConnectorRouteEngine::set_mode(RouteMode mode)
{
    mode_ = mode;
}

auto ConnectorRouteEngine::mode() const -> RouteMode
{
    return mode_;
}

auto ConnectorRouteEngine::mode_name(RouteMode mode) -> std::string
{
    switch (mode)
    {
        case RouteMode::kStraight:
            return "straight";
        case RouteMode::kOrthogonal:
            return "orthogonal";
        case RouteMode::kCurved:
            return "curved";
    }
    return "unknown";
}

void ConnectorRouteEngine::compute_route(double src_x, double src_y, double dst_x, double dst_y)
{
    // Keep user-defined bends, rebuild auto points
    std::vector<RoutePoint> user_bends;
    for (const auto& pt : points_)
    {
        if (pt.is_user_defined)
        {
            user_bends.push_back(pt);
        }
    }

    points_.clear();

    // Source
    points_.push_back({src_x, src_y, false});

    if (mode_ == RouteMode::kOrthogonal)
    {
        // Add midpoint for orthogonal routing
        double mid_x = (src_x + dst_x) / 2.0;
        points_.push_back({mid_x, src_y, false});
        points_.push_back({mid_x, dst_y, false});
    }
    else if (mode_ == RouteMode::kCurved)
    {
        // Add control points for curve
        double dx = (dst_x - src_x) * 0.4;
        points_.push_back({src_x + dx, src_y, false});
        points_.push_back({dst_x - dx, dst_y, false});
    }

    // Re-insert user bends
    for (const auto& bend : user_bends)
    {
        points_.push_back(bend);
    }

    // Destination
    points_.push_back({dst_x, dst_y, false});
}

auto ConnectorRouteEngine::route_points() const -> const std::vector<RoutePoint>&
{
    return points_;
}

auto ConnectorRouteEngine::point_count() const -> int
{
    return static_cast<int>(points_.size());
}

void ConnectorRouteEngine::add_bend(int index, double pos_x, double pos_y)
{
    if (index >= 0 && index <= static_cast<int>(points_.size()))
    {
        points_.insert(points_.begin() + index, {pos_x, pos_y, true});
    }
}

void ConnectorRouteEngine::remove_bend(int index)
{
    if (index >= 0 && index < static_cast<int>(points_.size()) &&
        points_[static_cast<size_t>(index)].is_user_defined)
    {
        points_.erase(points_.begin() + index);
    }
}

void ConnectorRouteEngine::move_bend(int index, double pos_x, double pos_y)
{
    if (index >= 0 && index < static_cast<int>(points_.size()) &&
        points_[static_cast<size_t>(index)].is_user_defined)
    {
        points_[static_cast<size_t>(index)].pos_x = pos_x;
        points_[static_cast<size_t>(index)].pos_y = pos_y;
    }
}

auto ConnectorRouteEngine::bend_count() const -> int
{
    int count = 0;
    for (const auto& pt : points_)
    {
        if (pt.is_user_defined)
        {
            ++count;
        }
    }
    return count;
}

void ConnectorRouteEngine::set_label(const ConnectorLabel& label)
{
    label_ = label;
    has_label_ = true;
}

auto ConnectorRouteEngine::label() const -> const ConnectorLabel&
{
    return label_;
}

auto ConnectorRouteEngine::has_label() const -> bool
{
    return has_label_;
}

auto ConnectorRouteEngine::compute_label_position() const -> LabelPosition
{
    if (points_.size() < 2)
    {
        return {0.0, 0.0};
    }

    // Interpolate along route based on offset
    const auto& first = points_.front();
    const auto& last = points_.back();

    LabelPosition pos;
    pos.pos_x = first.pos_x + (last.pos_x - first.pos_x) * label_.offset + label_.lateral_offset;
    pos.pos_y = first.pos_y + (last.pos_y - first.pos_y) * label_.offset;

    return pos;
}

} // namespace markamp::canvas
