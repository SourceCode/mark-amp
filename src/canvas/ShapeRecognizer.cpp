#include "ShapeRecognizer.h"

#include "canvas/ShapeData.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace markamp::canvas
{

// ── Helpers ─────────────────────────────────────────────────────

auto ShapeRecognizer::centroid(const std::vector<Point2D>& points) -> Point2D
{
    if (points.empty())
    {
        return {0.0, 0.0};
    }

    double sum_x = 0.0;
    double sum_y = 0.0;
    for (const auto& pt : points)
    {
        sum_x += pt.x;
        sum_y += pt.y;
    }

    const auto count = static_cast<double>(points.size());
    return {sum_x / count, sum_y / count};
}

auto ShapeRecognizer::compute_bounds(const std::vector<Point2D>& points) -> AABB
{
    if (points.empty())
    {
        return {0.0, 0.0, 0.0, 0.0};
    }

    double min_x_val = points[0].x;
    double min_y_val = points[0].y;
    double max_x_val = points[0].x;
    double max_y_val = points[0].y;

    for (const auto& pt : points)
    {
        min_x_val = std::min(min_x_val, pt.x);
        min_y_val = std::min(min_y_val, pt.y);
        max_x_val = std::max(max_x_val, pt.x);
        max_y_val = std::max(max_y_val, pt.y);
    }

    return {min_x_val, min_y_val, max_x_val, max_y_val};
}

auto ShapeRecognizer::is_closed(const std::vector<Point2D>& points, double threshold_ratio) -> bool
{
    if (points.size() < 3)
    {
        return false;
    }

    const double total_len = path_length(points);
    if (total_len < 1e-6)
    {
        return true;
    }

    const double gap = points.front().distance_to(points.back());
    return (gap / total_len) < threshold_ratio;
}

auto ShapeRecognizer::path_length(const std::vector<Point2D>& points) -> double
{
    double total = 0.0;
    for (size_t idx = 1; idx < points.size(); ++idx)
    {
        total += points[idx - 1].distance_to(points[idx]);
    }
    return total;
}

auto ShapeRecognizer::count_corners(const std::vector<Point2D>& points, double angle_threshold_deg)
    -> size_t
{
    if (points.size() < 3)
    {
        return 0;
    }

    const double threshold_rad = angle_threshold_deg * M_PI / 180.0;
    size_t corner_count = 0;

    // Sample points at roughly 10% intervals to reduce noise.
    const size_t step = std::max(size_t{1}, points.size() / 20);

    for (size_t idx = step; idx + step < points.size(); idx += step)
    {
        const auto& prev_pt = points[idx - step];
        const auto& curr_pt = points[idx];
        const auto& next_pt = points[idx + step];

        const double dx1 = curr_pt.x - prev_pt.x;
        const double dy1 = curr_pt.y - prev_pt.y;
        const double dx2 = next_pt.x - curr_pt.x;
        const double dy2 = next_pt.y - curr_pt.y;

        const double len1 = std::sqrt(dx1 * dx1 + dy1 * dy1);
        const double len2 = std::sqrt(dx2 * dx2 + dy2 * dy2);

        if (len1 < 1e-6 || len2 < 1e-6)
        {
            continue;
        }

        const double dot = (dx1 * dx2 + dy1 * dy2) / (len1 * len2);
        const double angle = std::acos(std::clamp(dot, -1.0, 1.0));

        if (angle > threshold_rad)
        {
            ++corner_count;
        }
    }

    return corner_count;
}

// ── Detection: Line ─────────────────────────────────────────────

auto ShapeRecognizer::detect_line(const std::vector<Point2D>& points, RecognitionResult& result)
    -> double
{
    if (points.size() < 2)
    {
        return 0.0;
    }

    const auto& start_pt = points.front();
    const auto& end_pt = points.back();
    const double line_len = start_pt.distance_to(end_pt);

    if (line_len < 5.0)
    {
        return 0.0;
    }

    // Compute max perpendicular distance from the line.
    double max_dev = 0.0;
    const double dx_line = end_pt.x - start_pt.x;
    const double dy_line = end_pt.y - start_pt.y;

    for (const auto& pt : points)
    {
        const double cross =
            std::abs(dx_line * (start_pt.y - pt.y) - (start_pt.x - pt.x) * dy_line);
        const double dist = cross / line_len;
        max_dev = std::max(max_dev, dist);
    }

    // Score: better if deviation is small relative to line length.
    const double ratio = max_dev / line_len;
    const double confidence = std::max(0.0, 1.0 - ratio * 10.0);

    if (confidence > 0.0)
    {
        result.shape = RecognizedShape::kLine;
        result.bounds = compute_bounds(points);
        result.angle = std::atan2(dy_line, dx_line);
    }

    return confidence;
}

// ── Detection: Rectangle ────────────────────────────────────────

auto ShapeRecognizer::detect_rectangle(const std::vector<Point2D>& points,
                                       RecognitionResult& result) -> double
{
    if (points.size() < 4 || !is_closed(points))
    {
        return 0.0;
    }

    const size_t corners = count_corners(points, 50.0);
    if (corners < 3 || corners > 6)
    {
        return 0.0;
    }

    // Check how well the points fit the bounding rectangle.
    const AABB bbox = compute_bounds(points);
    const double rect_perimeter = 2.0 * (bbox.width() + bbox.height());
    const double actual_perimeter = path_length(points);

    if (rect_perimeter < 1e-6)
    {
        return 0.0;
    }

    const double perimeter_ratio = actual_perimeter / rect_perimeter;
    // Ideal ratio is ~1.0 for a perfect rectangle.
    const double confidence = std::max(0.0, 1.0 - std::abs(perimeter_ratio - 1.0) * 3.0);

    if (confidence > 0.0)
    {
        result.shape = RecognizedShape::kRectangle;
        result.bounds = bbox;
    }

    return confidence * (corners >= 3 ? 1.0 : 0.5);
}

// ── Detection: Ellipse ──────────────────────────────────────────

auto ShapeRecognizer::detect_ellipse(const std::vector<Point2D>& points, RecognitionResult& result)
    -> double
{
    if (points.size() < 6 || !is_closed(points))
    {
        return 0.0;
    }

    const Point2D center_pt = centroid(points);
    const AABB bbox = compute_bounds(points);

    const double semi_major = bbox.width() / 2.0;
    const double semi_minor = bbox.height() / 2.0;

    if (semi_major < 3.0 || semi_minor < 3.0)
    {
        return 0.0;
    }

    // Check how uniformly the points are distributed around the centroid
    // relative to an ideal ellipse. For each point, compute the expected
    // distance on the ellipse at that angle and compare.
    double total_deviation = 0.0;
    size_t sample_count = 0;

    for (const auto& pt : points)
    {
        const double rel_x = pt.x - center_pt.x;
        const double rel_y = pt.y - center_pt.y;

        // Normalized ellipse equation: (x/a)^2 + (y/b)^2 should be ~1.0
        const double norm_val = (rel_x * rel_x) / (semi_major * semi_major) +
                                (rel_y * rel_y) / (semi_minor * semi_minor);

        total_deviation += std::abs(norm_val - 1.0);
        ++sample_count;
    }

    if (sample_count == 0)
    {
        return 0.0;
    }

    const double avg_deviation = total_deviation / static_cast<double>(sample_count);
    const double confidence = std::max(0.0, 1.0 - avg_deviation * 2.0);

    if (confidence > 0.0)
    {
        result.shape = RecognizedShape::kEllipse;
        result.bounds = bbox;
    }

    return confidence;
}

// ── Detection: Triangle ─────────────────────────────────────────

auto ShapeRecognizer::detect_triangle(const std::vector<Point2D>& points, RecognitionResult& result)
    -> double
{
    if (points.size() < 3 || !is_closed(points))
    {
        return 0.0;
    }

    const size_t corners = count_corners(points, 30.0);
    if (corners < 2 || corners > 5)
    {
        return 0.0;
    }

    // Triangles should have exactly 3 dominant corners.
    const double corner_score = (corners == 3) ? 1.0 : (corners == 2 || corners == 4) ? 0.7 : 0.3;

    // Check perimeter ratio against triangle-like path.
    const AABB bbox = compute_bounds(points);
    const double diag = std::sqrt(bbox.width() * bbox.width() + bbox.height() * bbox.height());
    const double actual_len = path_length(points);

    if (diag < 1e-6)
    {
        return 0.0;
    }

    // Triangle perimeter is roughly 2.5–3.5x the diagonal of the bounding box.
    const double ratio = actual_len / diag;
    const double perimeter_score = std::max(0.0, 1.0 - std::abs(ratio - 3.0) * 0.5);

    const double confidence = corner_score * 0.6 + perimeter_score * 0.4;

    if (confidence > 0.0)
    {
        result.shape = RecognizedShape::kTriangle;
        result.bounds = bbox;
    }

    return confidence;
}

// ── Detection: Arrow ────────────────────────────────────────────

auto ShapeRecognizer::detect_arrow(const std::vector<Point2D>& points, RecognitionResult& result)
    -> double
{
    if (points.size() < 5)
    {
        return 0.0;
    }

    // An arrow is essentially a line with a branching end.
    // First check that the overall shape is somewhat linear.
    RecognitionResult line_result;
    const double line_conf = detect_line(points, line_result);

    if (line_conf < 0.3)
    {
        return 0.0; // Not line-like enough.
    }

    // Look for a fork/branching pattern near the end.
    // Check if multiple corners exist near the end of the path.
    const size_t tail_start = points.size() * 3 / 4;
    std::vector<Point2D> tail_points(points.begin() + static_cast<long>(tail_start), points.end());

    const size_t tail_corners = count_corners(tail_points, 30.0);

    if (tail_corners < 1)
    {
        return 0.0;
    }

    const double confidence =
        std::min(1.0, line_conf * 0.5 + static_cast<double>(tail_corners) * 0.25);

    if (confidence > 0.0)
    {
        result.shape = RecognizedShape::kArrow;
        result.bounds = compute_bounds(points);
        result.angle = line_result.angle;
    }

    return confidence;
}

// ── Main Recognition ────────────────────────────────────────────

auto ShapeRecognizer::recognize(const std::vector<Point2D>& points) -> RecognitionResult
{
    if (points.size() < 2)
    {
        return {};
    }

    RecognitionResult best;
    double best_conf = 0.0;

    // Try each detector and keep the best match.
    RecognitionResult line_result;
    const double line_conf = detect_line(points, line_result);
    if (line_conf > best_conf)
    {
        best = line_result;
        best_conf = line_conf;
        best.confidence = line_conf;
    }

    RecognitionResult rect_result;
    const double rect_conf = detect_rectangle(points, rect_result);
    if (rect_conf > best_conf)
    {
        best = rect_result;
        best_conf = rect_conf;
        best.confidence = rect_conf;
    }

    RecognitionResult ellipse_result;
    const double ellipse_conf = detect_ellipse(points, ellipse_result);
    if (ellipse_conf > best_conf)
    {
        best = ellipse_result;
        best_conf = ellipse_conf;
        best.confidence = ellipse_conf;
    }

    RecognitionResult tri_result;
    const double tri_conf = detect_triangle(points, tri_result);
    if (tri_conf > best_conf)
    {
        best = tri_result;
        best_conf = tri_conf;
        best.confidence = tri_conf;
    }

    RecognitionResult arrow_result;
    const double arrow_conf = detect_arrow(points, arrow_result);
    if (arrow_conf > best_conf)
    {
        best = arrow_result;
        best_conf = arrow_conf;
        best.confidence = arrow_conf;
    }

    return best;
}

// ── Conversion to ShapeObject ───────────────────────────────────

auto ShapeRecognizer::to_shape_object(const RecognitionResult& result)
    -> std::unique_ptr<ShapeObject>
{
    if (result.shape == RecognizedShape::kNone || result.confidence < kMinConfidence)
    {
        return nullptr;
    }

    auto shape = std::make_unique<ShapeObject>();

    switch (result.shape)
    {
        case RecognizedShape::kRectangle:
            shape->set_shape_type(ShapeType::kRectangle);
            break;
        case RecognizedShape::kEllipse:
            shape->set_shape_type(ShapeType::kEllipse);
            break;
        case RecognizedShape::kTriangle:
            shape->set_shape_type(ShapeType::kTriangle);
            break;
        case RecognizedShape::kLine:
        case RecognizedShape::kArrow:
            shape->set_shape_type(ShapeType::kRectangle); // Simplified: lines as thin rects.
            break;
        case RecognizedShape::kNone:
            return nullptr;
    }

    // Position the shape at the detected bounds.
    shape->resize(result.bounds.width(), result.bounds.height());
    shape->set_position(result.bounds.min_x, result.bounds.min_y);

    return shape;
}

} // namespace markamp::canvas
