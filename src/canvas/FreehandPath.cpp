#include "FreehandPath.h"

#include <algorithm>
#include <cmath>
#include <sstream>

namespace markamp::canvas
{

FreehandPath::FreehandPath()
    : CanvasObject(CanvasObjectType::FreehandPath)
{
}

// ── Points ──────────────────────────────────────────────────────

auto FreehandPath::add_point(Point2D point) -> void
{
    points_.push_back(point);
    mark_dirty();
}

auto FreehandPath::points() const -> const std::vector<Point2D>&
{
    return points_;
}

auto FreehandPath::point_count() const -> size_t
{
    return points_.size();
}

// ── Stroke ──────────────────────────────────────────────────────

auto FreehandPath::stroke_color() const -> CanvasColor
{
    return stroke_color_;
}

auto FreehandPath::set_stroke_color(CanvasColor color) -> void
{
    stroke_color_ = color;
    mark_dirty();
}

auto FreehandPath::stroke_width() const -> double
{
    return stroke_width_;
}

auto FreehandPath::set_stroke_width(double width) -> void
{
    stroke_width_ = std::max(0.5, width);
    mark_dirty();
}

// ── Smoothing factor ────────────────────────────────────────────

auto FreehandPath::smoothing_factor() const -> double
{
    return smoothing_factor_;
}

auto FreehandPath::set_smoothing_factor(double factor) -> void
{
    smoothing_factor_ = std::clamp(factor, 0.0, 1.0);
}

// ── Chaikin Corner-Cutting Smoothing ────────────────────────────

auto FreehandPath::smooth() -> void
{
    if (points_.size() < 3)
    {
        return;
    }

    // Number of passes scales with smoothing factor (0→0, 0.5→2, 1.0→4).
    const int kPasses = static_cast<int>(smoothing_factor_ * 4.0);
    if (kPasses <= 0)
    {
        return;
    }

    for (int pass = 0; pass < kPasses; ++pass)
    {
        std::vector<Point2D> smoothed;
        smoothed.reserve(points_.size() * 2);

        // Keep the first point.
        smoothed.push_back(points_.front());

        for (size_t idx = 0; idx + 1 < points_.size(); ++idx)
        {
            const auto& current_pt = points_[idx];
            const auto& next_pt = points_[idx + 1];

            // Q = 3/4 * P_i + 1/4 * P_{i+1}
            const Point2D quarter_pt{0.75 * current_pt.x + 0.25 * next_pt.x,
                                     0.75 * current_pt.y + 0.25 * next_pt.y};

            // R = 1/4 * P_i + 3/4 * P_{i+1}
            const Point2D three_quarter_pt{0.25 * current_pt.x + 0.75 * next_pt.x,
                                           0.25 * current_pt.y + 0.75 * next_pt.y};

            smoothed.push_back(quarter_pt);
            smoothed.push_back(three_quarter_pt);
        }

        // Keep the last point.
        smoothed.push_back(points_.back());
        points_ = std::move(smoothed);
    }

    mark_dirty();
}

// ── Ramer-Douglas-Peucker Simplification ────────────────────────

auto FreehandPath::perpendicular_distance(const Point2D& point,
                                          const Point2D& line_start,
                                          const Point2D& line_end) -> double
{
    const double line_dx = line_end.x - line_start.x;
    const double line_dy = line_end.y - line_start.y;
    const double line_length_sq = line_dx * line_dx + line_dy * line_dy;

    if (line_length_sq < 1e-12)
    {
        // Degenerate line segment (start == end).
        return point.distance_to(line_start);
    }

    // Perpendicular distance via cross product.
    const double cross = std::abs((line_end.x - line_start.x) * (line_start.y - point.y) -
                                  (line_start.x - point.x) * (line_end.y - line_start.y));

    return cross / std::sqrt(line_length_sq);
}

auto FreehandPath::rdp_simplify(const std::vector<Point2D>& points,
                                double tolerance,
                                size_t start_idx,
                                size_t end_idx,
                                std::vector<bool>& keep) -> void
{
    if (end_idx <= start_idx + 1)
    {
        return;
    }

    double max_dist = 0.0;
    size_t max_idx = start_idx;

    for (size_t idx = start_idx + 1; idx < end_idx; ++idx)
    {
        const double dist = perpendicular_distance(points[idx], points[start_idx], points[end_idx]);

        if (dist > max_dist)
        {
            max_dist = dist;
            max_idx = idx;
        }
    }

    if (max_dist > tolerance)
    {
        keep[max_idx] = true;
        rdp_simplify(points, tolerance, start_idx, max_idx, keep);
        rdp_simplify(points, tolerance, max_idx, end_idx, keep);
    }
}

auto FreehandPath::simplify(double tolerance) -> void
{
    if (points_.size() < 3)
    {
        return;
    }

    std::vector<bool> keep(points_.size(), false);
    keep.front() = true;
    keep.back() = true;

    rdp_simplify(points_, tolerance, 0, points_.size() - 1, keep);

    std::vector<Point2D> simplified;
    simplified.reserve(points_.size());
    for (size_t idx = 0; idx < points_.size(); ++idx)
    {
        if (keep[idx])
        {
            simplified.push_back(points_[idx]);
        }
    }

    points_ = std::move(simplified);
    mark_dirty();
}

// ── Path Shape (#13-14) ────────────────────────────────────────

auto FreehandPath::is_closed() const -> bool
{
    return closed_;
}
auto FreehandPath::set_closed(bool closed) -> void
{
    closed_ = closed;
    mark_dirty();
}
auto FreehandPath::fill_color() const -> CanvasColor
{
    return fill_color_;
}
auto FreehandPath::set_fill_color(CanvasColor color) -> void
{
    fill_color_ = color;
    mark_dirty();
}

// ── Computed & Operations (#15-18) ─────────────────────────────

auto FreehandPath::total_length() const -> double
{
    double length = 0.0;
    for (size_t idx = 1; idx < points_.size(); ++idx)
    {
        length += points_[idx].distance_to(points_[idx - 1]);
    }
    return length;
}

auto FreehandPath::bounding_center() const -> Point2D
{
    const auto bounds = local_bounds();
    return bounds.center();
}

auto FreehandPath::reverse() -> void
{
    std::reverse(points_.begin(), points_.end());
    mark_dirty();
}

auto FreehandPath::erase_points_in(const AABB& region) -> void
{
    points_.erase(std::remove_if(points_.begin(),
                                 points_.end(),
                                 [&region](const Point2D& point)
                                 {
                                     return point.x >= region.min_x && point.x <= region.max_x &&
                                            point.y >= region.min_y && point.y <= region.max_y;
                                 }),
                  points_.end());
    mark_dirty();
}

// --- Batch 7 (#37-39) ---

auto FreehandPath::is_empty() const -> bool
{
    return points_.empty();
}

auto FreehandPath::clear_points() -> void
{
    points_.clear();
    mark_dirty();
}

auto FreehandPath::subsample(size_t max_points) -> void
{
    if (points_.size() <= max_points || max_points < 2)
    {
        return;
    }

    std::vector<Point2D> sampled;
    sampled.reserve(max_points);

    const double step =
        static_cast<double>(points_.size() - 1) / static_cast<double>(max_points - 1);
    for (size_t idx = 0; idx < max_points; ++idx)
    {
        const size_t src_idx = static_cast<size_t>(static_cast<double>(idx) * step);
        sampled.push_back(points_[std::min(src_idx, points_.size() - 1)]);
    }

    points_ = std::move(sampled);
    mark_dirty();
}

// ── CanvasObject overrides ──────────────────────────────────────

auto FreehandPath::local_bounds() const -> AABB
{
    if (points_.empty())
    {
        return AABB{0.0, 0.0, 0.0, 0.0};
    }

    double min_x_val = points_[0].x;
    double min_y_val = points_[0].y;
    double max_x_val = points_[0].x;
    double max_y_val = points_[0].y;

    for (const auto& point : points_)
    {
        min_x_val = std::min(min_x_val, point.x);
        min_y_val = std::min(min_y_val, point.y);
        max_x_val = std::max(max_x_val, point.x);
        max_y_val = std::max(max_y_val, point.y);
    }

    // Expand by stroke width so the stroked path is fully contained.
    const double half_stroke = stroke_width_ / 2.0;
    return AABB{min_x_val - half_stroke,
                min_y_val - half_stroke,
                max_x_val + half_stroke,
                max_y_val + half_stroke};
}

auto FreehandPath::clone() const -> std::unique_ptr<CanvasObject>
{
    auto copy = std::make_unique<FreehandPath>();
    copy->points_ = points_;
    copy->stroke_color_ = stroke_color_;
    copy->stroke_width_ = stroke_width_;
    copy->smoothing_factor_ = smoothing_factor_;
    copy->closed_ = closed_;
    copy->fill_color_ = fill_color_;
    copy->set_transform(transform());
    copy->set_z_index(z_index());
    copy->set_name(name());
    return copy;
}

auto FreehandPath::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"type\":\"FreehandPath\""
        << ",\"stroke_width\":" << stroke_width_ << ",\"stroke_color\":\"" << stroke_color_.to_hex()
        << "\""
        << ",\"smoothing\":" << smoothing_factor_ << ",\"points\":[";

    for (size_t idx = 0; idx < points_.size(); ++idx)
    {
        if (idx > 0)
        {
            oss << ",";
        }
        oss << "{\"x\":" << points_[idx].x << ",\"y\":" << points_[idx].y << "}";
    }

    oss << "]}";
    return oss.str();
}

auto FreehandPath::from_json(const std::string& json) -> void
{
    auto extract_number = [&](const std::string& key) -> double
    {
        const std::string needle = "\"" + key + "\":";
        const auto pos = json.find(needle);
        if (pos == std::string::npos)
        {
            return 0.0;
        }
        const auto val_start = pos + needle.size();
        return std::stod(json.substr(val_start));
    };

    auto extract_string = [&](const std::string& key) -> std::string
    {
        const std::string needle = "\"" + key + "\":\"";
        const auto pos = json.find(needle);
        if (pos == std::string::npos)
        {
            return {};
        }
        const auto val_start = pos + needle.size();
        const auto val_end = json.find('"', val_start);
        if (val_end == std::string::npos)
        {
            return {};
        }
        return json.substr(val_start, val_end - val_start);
    };

    set_stroke_width(extract_number("stroke_width"));
    set_smoothing_factor(extract_number("smoothing"));

    const std::string hex_color = extract_string("stroke_color");
    if (!hex_color.empty())
    {
        set_stroke_color(CanvasColor::from_hex(hex_color));
    }

    // Parse points array: "points":[{"x":1,"y":2},...]
    points_.clear();
    const std::string points_key = "\"points\":[";
    auto pts_pos = json.find(points_key);
    if (pts_pos != std::string::npos)
    {
        auto cursor = pts_pos + points_key.size();
        while (cursor < json.size())
        {
            const auto x_pos = json.find("\"x\":", cursor);
            if (x_pos == std::string::npos)
            {
                break;
            }
            const double px = std::stod(json.substr(x_pos + 4));
            const auto y_pos = json.find("\"y\":", x_pos);
            if (y_pos == std::string::npos)
            {
                break;
            }
            const double py = std::stod(json.substr(y_pos + 4));
            points_.push_back({px, py});

            // Advance past the closing brace of this point object.
            const auto brace = json.find('}', y_pos);
            if (brace == std::string::npos)
            {
                break;
            }
            cursor = brace + 1;
        }
    }

    mark_dirty();
}

} // namespace markamp::canvas
