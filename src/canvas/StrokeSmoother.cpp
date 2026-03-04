#include "StrokeSmoother.h"

#include <algorithm>
#include <cmath>

namespace markamp::canvas
{

auto StrokePoint::distance_to(const StrokePoint& other) const -> double
{
    double dx = pos_x - other.pos_x;
    double dy = pos_y - other.pos_y;
    return std::sqrt(dx * dx + dy * dy);
}

auto StrokePoint::speed_to(const StrokePoint& other) const -> double
{
    int dt = std::abs(timestamp_ms - other.timestamp_ms);
    if (dt == 0)
    {
        return 0.0;
    }
    return distance_to(other) / static_cast<double>(dt);
}

void StrokeSmoother::add_point(const StrokePoint& point)
{
    points_.push_back(point);
}

auto StrokeSmoother::raw_count() const -> int
{
    return static_cast<int>(points_.size());
}

auto StrokeSmoother::raw_points() const -> const std::vector<StrokePoint>&
{
    return points_;
}

auto StrokeSmoother::smooth() const -> std::vector<StrokePoint>
{
    if (points_.size() < 3)
    {
        return points_;
    }

    std::vector<StrokePoint> result;
    result.reserve(points_.size());

    // Keep first point
    result.push_back(points_[0]);

    for (size_t i = 1; i < points_.size() - 1; ++i)
    {
        const auto& prev = points_[i - 1];
        const auto& curr = points_[i];
        const auto& next = points_[i + 1];

        // Adaptive: use speed to determine smoothing weight
        double speed = curr.speed_to(prev);
        double weight = (speed > speed_threshold_) ? smoothing_factor_ : smoothing_factor_ * 0.3;

        StrokePoint smoothed;
        smoothed.pos_x = curr.pos_x + weight * ((prev.pos_x + next.pos_x) / 2.0 - curr.pos_x);
        smoothed.pos_y = curr.pos_y + weight * ((prev.pos_y + next.pos_y) / 2.0 - curr.pos_y);
        smoothed.pressure = curr.pressure;
        smoothed.timestamp_ms = curr.timestamp_ms;

        result.push_back(smoothed);
    }

    // Keep last point
    result.push_back(points_.back());

    return result;
}

void StrokeSmoother::set_smoothing_factor(double factor)
{
    smoothing_factor_ = std::clamp(factor, 0.0, 1.0);
}

auto StrokeSmoother::smoothing_factor() const -> double
{
    return smoothing_factor_;
}

auto StrokeSmoother::reduce(double tolerance) const -> std::vector<StrokePoint>
{
    if (points_.size() < 3)
    {
        return points_;
    }

    // Simple distance-based reduction
    std::vector<StrokePoint> result;
    result.push_back(points_[0]);

    for (size_t i = 1; i < points_.size(); ++i)
    {
        if (points_[i].distance_to(result.back()) >= tolerance)
        {
            result.push_back(points_[i]);
        }
    }

    // Always keep last point
    if (result.back().distance_to(points_.back()) > 0.001)
    {
        result.push_back(points_.back());
    }

    return result;
}

auto StrokeSmoother::pressure_width(double pressure, double base_width) -> double
{
    return base_width * (0.3 + 0.7 * pressure); // Min 30% width at zero pressure
}

void StrokeSmoother::set_speed_threshold(double threshold)
{
    speed_threshold_ = threshold;
}

auto StrokeSmoother::speed_threshold() const -> double
{
    return speed_threshold_;
}

void StrokeSmoother::clear()
{
    points_.clear();
}

} // namespace markamp::canvas
