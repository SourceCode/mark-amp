#include "GeometryHandleController.h"

#include <cmath>

namespace markamp::canvas
{

auto GeometryHandle::type_name() const -> std::string
{
    switch (type)
    {
        case HandleType::kCorner:
            return "corner";
        case HandleType::kEdge:
            return "edge";
        case HandleType::kRotation:
            return "rotation";
        case HandleType::kRadius:
            return "radius";
    }
    return "unknown";
}

auto GeometryHandle::position_name() const -> std::string
{
    switch (position)
    {
        case HandlePosition::kTopLeft:
            return "top_left";
        case HandlePosition::kTopRight:
            return "top_right";
        case HandlePosition::kBottomLeft:
            return "bottom_left";
        case HandlePosition::kBottomRight:
            return "bottom_right";
        case HandlePosition::kTop:
            return "top";
        case HandlePosition::kBottom:
            return "bottom";
        case HandlePosition::kLeft:
            return "left";
        case HandlePosition::kRight:
            return "right";
        case HandlePosition::kRotate:
            return "rotate";
        case HandlePosition::kRadiusControl:
            return "radius_control";
    }
    return "unknown";
}

void GeometryHandleController::generate_handles(double pos_x,
                                                double pos_y,
                                                double width,
                                                double height)
{
    handles_.clear();
    handles_.reserve(10);

    // 4 corners
    handles_.push_back({HandleType::kCorner, HandlePosition::kTopLeft, pos_x, pos_y});
    handles_.push_back({HandleType::kCorner, HandlePosition::kTopRight, pos_x + width, pos_y});
    handles_.push_back({HandleType::kCorner, HandlePosition::kBottomLeft, pos_x, pos_y + height});
    handles_.push_back(
        {HandleType::kCorner, HandlePosition::kBottomRight, pos_x + width, pos_y + height});

    // 4 edges (midpoints)
    handles_.push_back({HandleType::kEdge, HandlePosition::kTop, pos_x + width / 2.0, pos_y});
    handles_.push_back(
        {HandleType::kEdge, HandlePosition::kBottom, pos_x + width / 2.0, pos_y + height});
    handles_.push_back({HandleType::kEdge, HandlePosition::kLeft, pos_x, pos_y + height / 2.0});
    handles_.push_back(
        {HandleType::kEdge, HandlePosition::kRight, pos_x + width, pos_y + height / 2.0});

    // Rotation handle (above top center)
    handles_.push_back(
        {HandleType::kRotation, HandlePosition::kRotate, pos_x + width / 2.0, pos_y - 20.0});

    // Radius handle (top-left corner offset)
    handles_.push_back(
        {HandleType::kRadius, HandlePosition::kRadiusControl, pos_x + 10.0, pos_y + 10.0});
}

auto GeometryHandleController::handle_count() const -> int
{
    return static_cast<int>(handles_.size());
}

auto GeometryHandleController::handles() const -> const std::vector<GeometryHandle>&
{
    return handles_;
}

auto GeometryHandleController::hit_handle(double pos_x, double pos_y, double threshold) const
    -> const GeometryHandle*
{
    for (const auto& handle : handles_)
    {
        double dist_x = pos_x - handle.pos_x;
        double dist_y = pos_y - handle.pos_y;
        double dist = std::sqrt(dist_x * dist_x + dist_y * dist_y);
        if (dist <= threshold)
        {
            return &handle;
        }
    }
    return nullptr;
}

void GeometryHandleController::set_constraint(ConstraintMode mode)
{
    constraint_ = mode;
}

auto GeometryHandleController::constraint() const -> ConstraintMode
{
    return constraint_;
}

auto GeometryHandleController::constrained_resize(HandlePosition handle,
                                                  double delta_x,
                                                  double delta_y,
                                                  const Bounds& original) const -> Bounds
{
    Bounds result = original;

    // Apply delta based on handle position
    switch (handle)
    {
        case HandlePosition::kBottomRight:
            result.width += delta_x;
            result.height += delta_y;
            break;
        case HandlePosition::kTopLeft:
            result.pos_x += delta_x;
            result.pos_y += delta_y;
            result.width -= delta_x;
            result.height -= delta_y;
            break;
        case HandlePosition::kTopRight:
            result.pos_y += delta_y;
            result.width += delta_x;
            result.height -= delta_y;
            break;
        case HandlePosition::kBottomLeft:
            result.pos_x += delta_x;
            result.width -= delta_x;
            result.height += delta_y;
            break;
        case HandlePosition::kRight:
            result.width += delta_x;
            break;
        case HandlePosition::kBottom:
            result.height += delta_y;
            break;
        case HandlePosition::kLeft:
            result.pos_x += delta_x;
            result.width -= delta_x;
            break;
        case HandlePosition::kTop:
            result.pos_y += delta_y;
            result.height -= delta_y;
            break;
        default:
            break;
    }

    // Apply proportional constraint
    if (constraint_ == ConstraintMode::kProportional || constraint_ == ConstraintMode::kBoth)
    {
        if (original.height > 0.0)
        {
            double aspect = original.width / original.height;
            result.height = result.width / aspect;
        }
    }

    // Apply center-anchored constraint
    if (constraint_ == ConstraintMode::kCenterAnchored || constraint_ == ConstraintMode::kBoth)
    {
        double center_x = original.pos_x + original.width / 2.0;
        double center_y = original.pos_y + original.height / 2.0;
        result.pos_x = center_x - result.width / 2.0;
        result.pos_y = center_y - result.height / 2.0;
    }

    return result;
}

void GeometryHandleController::set_snap_grid(double grid_size)
{
    snap_grid_ = grid_size;
}

auto GeometryHandleController::snap_grid() const -> double
{
    return snap_grid_;
}

auto GeometryHandleController::snap_value(double value) const -> double
{
    if (snap_grid_ <= 0.0)
    {
        return value;
    }
    return std::round(value / snap_grid_) * snap_grid_;
}

} // namespace markamp::canvas
