#include "CameraModel.h"

#include <algorithm>

namespace markamp::canvas
{

void CameraModel::set_zoom(double zoom)
{
    zoom_ = std::clamp(zoom, kMinZoom, kMaxZoom);
}
auto CameraModel::zoom() const -> double
{
    return zoom_;
}

void CameraModel::zoom_at(double delta, double cursor_x, double cursor_y)
{
    anchor_x_ = cursor_x;
    anchor_y_ = cursor_y;
    zoom_ = std::clamp(zoom_ + delta, kMinZoom, kMaxZoom);
}

auto CameraModel::anchor_x() const -> double
{
    return anchor_x_;
}
auto CameraModel::anchor_y() const -> double
{
    return anchor_y_;
}

void CameraModel::apply_preset(ZoomPreset preset)
{
    switch (preset)
    {
        case ZoomPreset::kFiftyPercent:
            zoom_ = 0.5;
            break;
        case ZoomPreset::kHundredPercent:
            zoom_ = 1.0;
            break;
        case ZoomPreset::kTwoHundredPercent:
            zoom_ = 2.0;
            break;
        case ZoomPreset::kFitBoard:
            zoom_ = 1.0;
            pan_x_ = 0.0;
            pan_y_ = 0.0;
            break;
        case ZoomPreset::kFitSelection:
            zoom_ = 1.0;
            break;
    }
}

void CameraModel::set_pan(double pan_x, double pan_y)
{
    pan_x_ = pan_x;
    pan_y_ = pan_y;
}
auto CameraModel::pan_x() const -> double
{
    return pan_x_;
}
auto CameraModel::pan_y() const -> double
{
    return pan_y_;
}

void CameraModel::set_pan_velocity(double velocity)
{
    pan_velocity_ = std::clamp(velocity, 0.1, 5.0);
}
auto CameraModel::pan_velocity() const -> double
{
    return pan_velocity_;
}

void CameraModel::set_bounds(double min_x, double min_y, double max_x, double max_y)
{
    bound_min_x_ = min_x;
    bound_min_y_ = min_y;
    bound_max_x_ = max_x;
    bound_max_y_ = max_y;
}

void CameraModel::enforce_bounds()
{
    pan_x_ = std::clamp(pan_x_, bound_min_x_, bound_max_x_);
    pan_y_ = std::clamp(pan_y_, bound_min_y_, bound_max_y_);
}

void CameraModel::recenter()
{
    pan_x_ = (bound_min_x_ + bound_max_x_) / 2.0;
    pan_y_ = (bound_min_y_ + bound_max_y_) / 2.0;
    zoom_ = 1.0;
}

auto CameraModel::is_out_of_bounds() const -> bool
{
    return pan_x_ < bound_min_x_ || pan_x_ > bound_max_x_ || pan_y_ < bound_min_y_ ||
           pan_y_ > bound_max_y_;
}

} // namespace markamp::canvas
