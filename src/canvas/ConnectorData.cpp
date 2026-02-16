#include "ConnectorData.h"

#include "canvas/Board.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <sstream>

namespace markamp::canvas
{

ConnectorObject::ConnectorObject()
    : CanvasObject(CanvasObjectType::Connector)
{
}

// ── Endpoints ──────────────────────────────────────────────────

auto ConnectorObject::start_endpoint() const -> const ConnectorEndpoint&
{
    return start_;
}
auto ConnectorObject::end_endpoint() const -> const ConnectorEndpoint&
{
    return end_;
}

auto ConnectorObject::set_start_object(ObjectId obj_id, AnchorPosition anchor) -> void
{
    start_.object_id = obj_id;
    start_.anchor = anchor;
    mark_dirty();
}

auto ConnectorObject::set_end_object(ObjectId obj_id, AnchorPosition anchor) -> void
{
    end_.object_id = obj_id;
    end_.anchor = anchor;
    mark_dirty();
}

auto ConnectorObject::set_free_start(const Point2D& point) -> void
{
    start_.object_id = kInvalidObjectId;
    start_.offset = point;
    mark_dirty();
}

auto ConnectorObject::set_free_end(const Point2D& point) -> void
{
    end_.object_id = kInvalidObjectId;
    end_.offset = point;
    mark_dirty();
}

auto ConnectorObject::resolve_start(const Board& board) const -> Point2D
{
    if (!start_.is_attached())
    {
        return start_.offset;
    }

    const auto* obj = board.get_object(start_.object_id);
    if (obj == nullptr)
    {
        return start_.offset;
    }

    return resolve_anchor(obj->world_bounds(), start_.anchor);
}

auto ConnectorObject::resolve_end(const Board& board) const -> Point2D
{
    if (!end_.is_attached())
    {
        return end_.offset;
    }

    const auto* obj = board.get_object(end_.object_id);
    if (obj == nullptr)
    {
        return end_.offset;
    }

    return resolve_anchor(obj->world_bounds(), end_.anchor);
}

auto ConnectorObject::resolve_anchor(const AABB& bounds, AnchorPosition anchor) -> Point2D
{
    const auto center = bounds.center();

    switch (anchor)
    {
        case AnchorPosition::kTop:
            return Point2D{center.x, bounds.min_y};
        case AnchorPosition::kBottom:
            return Point2D{center.x, bounds.max_y};
        case AnchorPosition::kLeft:
            return Point2D{bounds.min_x, center.y};
        case AnchorPosition::kRight:
            return Point2D{bounds.max_x, center.y};
        case AnchorPosition::kCenter:
            return center;
        case AnchorPosition::kAuto:
            return center; // Auto defaults to center.
    }

    return center;
}

// ── Waypoints ──────────────────────────────────────────────────

auto ConnectorObject::waypoints() const -> const std::vector<Point2D>&
{
    return waypoints_;
}

auto ConnectorObject::add_waypoint(const Point2D& point) -> void
{
    waypoints_.push_back(point);
    mark_dirty();
}

auto ConnectorObject::clear_waypoints() -> void
{
    waypoints_.clear();
    mark_dirty();
}

// ── Line Style ─────────────────────────────────────────────────

auto ConnectorObject::line_style() const -> ConnectorLineStyle
{
    return line_style_;
}
auto ConnectorObject::set_line_style(ConnectorLineStyle style) -> void
{
    line_style_ = style;
    mark_dirty();
}
auto ConnectorObject::line_width() const -> double
{
    return line_width_;
}
auto ConnectorObject::set_line_width(double width) -> void
{
    line_width_ = width;
    mark_dirty();
}
auto ConnectorObject::line_color() const -> CanvasColor
{
    return line_color_;
}
auto ConnectorObject::set_line_color(const CanvasColor& color) -> void
{
    line_color_ = color;
    mark_dirty();
}

// ── Arrowheads ─────────────────────────────────────────────────

auto ConnectorObject::start_arrow() const -> ArrowheadStyle
{
    return start_arrow_;
}
auto ConnectorObject::set_start_arrow(ArrowheadStyle style) -> void
{
    start_arrow_ = style;
    mark_dirty();
}
auto ConnectorObject::end_arrow() const -> ArrowheadStyle
{
    return end_arrow_;
}
auto ConnectorObject::set_end_arrow(ArrowheadStyle style) -> void
{
    end_arrow_ = style;
    mark_dirty();
}

// ── Label ──────────────────────────────────────────────────────

auto ConnectorObject::label() const -> const std::string&
{
    return label_;
}
auto ConnectorObject::set_label(const std::string& label) -> void
{
    label_ = label;
    mark_dirty();
}

// ── Routing & Appearance (#7-10) ──────────────────────────────

auto ConnectorObject::routing() const -> ConnectorRouting
{
    return routing_;
}
auto ConnectorObject::set_routing(ConnectorRouting routing) -> void
{
    routing_ = routing;
    mark_dirty();
}
auto ConnectorObject::dash_pattern() const -> double
{
    return dash_pattern_;
}
auto ConnectorObject::set_dash_pattern(double length) -> void
{
    dash_pattern_ = std::max(1.0, length);
    mark_dirty();
}
auto ConnectorObject::opacity() const -> double
{
    return opacity_;
}
auto ConnectorObject::set_opacity(double opacity_val) -> void
{
    opacity_ = std::clamp(opacity_val, 0.0, 1.0);
    mark_dirty();
}

// ── Computed (#11-12) ─────────────────────────────────────────

auto ConnectorObject::is_bidirectional() const -> bool
{
    return start_arrow_ != ArrowheadStyle::kNone && end_arrow_ != ArrowheadStyle::kNone;
}

auto ConnectorObject::total_length(const Board& board) const -> double
{
    std::vector<Point2D> all_points;
    all_points.push_back(resolve_start(board));
    for (const auto& waypoint : waypoints_)
    {
        all_points.push_back(waypoint);
    }
    all_points.push_back(resolve_end(board));

    double length = 0.0;
    for (size_t idx = 1; idx < all_points.size(); ++idx)
    {
        length += all_points[idx].distance_to(all_points[idx - 1]);
    }
    return length;
}

// ── Batch 6 (#31-36) ─────────────────────────────────────────────

auto ConnectorObject::is_self_loop() const -> bool
{
    return start_.is_attached() && end_.is_attached() && start_.object_id == end_.object_id;
}

auto ConnectorObject::is_attached_to(ObjectId obj_id) const -> bool
{
    return start_.object_id == obj_id || end_.object_id == obj_id;
}

auto ConnectorObject::detach_start() -> void
{
    start_.object_id = kInvalidObjectId;
    mark_dirty();
}

auto ConnectorObject::detach_end() -> void
{
    end_.object_id = kInvalidObjectId;
    mark_dirty();
}

auto ConnectorObject::reverse_direction() -> void
{
    std::swap(start_, end_);
    std::swap(start_arrow_, end_arrow_);
    if (!waypoints_.empty())
    {
        std::reverse(waypoints_.begin(), waypoints_.end());
    }
    mark_dirty();
}

auto ConnectorObject::set_color_from_theme(const CanvasColor& themed_color) -> void
{
    line_color_ = themed_color;
    mark_dirty();
}

// ── CanvasObject overrides ─────────────────────────────────────

auto ConnectorObject::local_bounds() const -> AABB
{
    // Compute bounding box from start, end, and all waypoints.
    double min_x = start_.offset.x;
    double min_y = start_.offset.y;
    double max_x = min_x;
    double max_y = min_y;

    const auto expand = [&](const Point2D& pt)
    {
        min_x = std::min(min_x, pt.x);
        min_y = std::min(min_y, pt.y);
        max_x = std::max(max_x, pt.x);
        max_y = std::max(max_y, pt.y);
    };

    expand(end_.offset);
    for (const auto& wp : waypoints_)
    {
        expand(wp);
    }

    // Ensure minimum size.
    if (max_x - min_x < 1.0)
    {
        max_x = min_x + 1.0;
    }
    if (max_y - min_y < 1.0)
    {
        max_y = min_y + 1.0;
    }

    return AABB{min_x, min_y, max_x - min_x, max_y - min_y};
}

auto ConnectorObject::clone() const -> std::unique_ptr<CanvasObject>
{
    auto copy = std::make_unique<ConnectorObject>();
    copy->start_ = start_;
    copy->end_ = end_;
    copy->waypoints_ = waypoints_;
    copy->line_style_ = line_style_;
    copy->line_width_ = line_width_;
    copy->line_color_ = line_color_;
    copy->start_arrow_ = start_arrow_;
    copy->end_arrow_ = end_arrow_;
    copy->label_ = label_;
    copy->routing_ = routing_;
    copy->dash_pattern_ = dash_pattern_;
    copy->opacity_ = opacity_;
    copy->set_transform(transform());
    copy->set_z_index(z_index());
    copy->set_name(name());
    return copy;
}

auto ConnectorObject::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"type\":\"Connector\""
        << ",\"line_width\":" << line_width_ << ",\"line_style\":" << static_cast<int>(line_style_)
        << ",\"start_arrow\":" << static_cast<int>(start_arrow_)
        << ",\"end_arrow\":" << static_cast<int>(end_arrow_)
        << ",\"waypoint_count\":" << waypoints_.size() << "}";
    return oss.str();
}

auto ConnectorObject::from_json(const std::string& /*json*/) -> void
{
    // Stub.
}

} // namespace markamp::canvas
