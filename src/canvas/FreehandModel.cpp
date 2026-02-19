#include "FreehandModel.h"

#include <algorithm>

namespace markamp::canvas
{

void FreehandModel::begin_stroke()
{
    points_.clear();
    recording_ = true;
}

void FreehandModel::add_point(StrokePoint point)
{
    if (recording_)
    {
        points_.push_back(point);
    }
}

void FreehandModel::end_stroke()
{
    recording_ = false;
}

auto FreehandModel::points() const -> const std::vector<StrokePoint>&
{
    return points_;
}
auto FreehandModel::point_count() const -> int
{
    return static_cast<int>(points_.size());
}

void FreehandModel::set_smoothing(double factor)
{
    smoothing_ = std::clamp(factor, 0.0, 1.0);
}

auto FreehandModel::smoothing() const -> double
{
    return smoothing_;
}

auto FreehandModel::smoothed_points() const -> std::vector<StrokePoint>
{
    if (points_.size() < 3 || smoothing_ <= 0.0)
    {
        return points_;
    }

    std::vector<StrokePoint> result = points_;
    const int window = static_cast<int>(1 + smoothing_ * 4); // 1–5

    for (int i = 1; i < static_cast<int>(result.size()) - 1; ++i)
    {
        double avg_x = 0.0;
        double avg_y = 0.0;
        int count = 0;
        for (int j = std::max(0, i - window);
             j <= std::min(static_cast<int>(result.size()) - 1, i + window);
             ++j)
        {
            avg_x += points_[static_cast<size_t>(j)].x;
            avg_y += points_[static_cast<size_t>(j)].y;
            ++count;
        }
        result[static_cast<size_t>(i)].x = avg_x / count;
        result[static_cast<size_t>(i)].y = avg_y / count;
    }
    return result;
}

auto FreehandModel::supports_pressure() const -> bool
{
    return pressure_support_;
}
void FreehandModel::set_pressure_support(bool supported)
{
    pressure_support_ = supported;
}

auto FreehandModel::width_at(int index, double base_width) const -> double
{
    if (index < 0 || index >= static_cast<int>(points_.size()))
    {
        return base_width;
    }
    if (!pressure_support_)
    {
        return base_width;
    }
    return base_width * points_[static_cast<size_t>(index)].pressure;
}

void FreehandModel::set_preset(StrokePreset preset)
{
    preset_ = preset;
}
auto FreehandModel::preset() const -> StrokePreset
{
    return preset_;
}

void FreehandModel::set_eraser_mode(EraserMode mode)
{
    eraser_mode_ = mode;
}
auto FreehandModel::eraser_mode() const -> EraserMode
{
    return eraser_mode_;
}

} // namespace markamp::canvas
