// ============================================================================
// File: src/canvas/DragAutoscrollModel.cpp
// Phase 02 W16: Drag Autoscroll model implementation
// ============================================================================

#include "canvas/DragAutoscrollModel.h"

namespace markamp::canvas
{

void DragAutoscrollModel::set_edge_margin(double pixels)
{
    edge_margin_ = pixels;
}

auto DragAutoscrollModel::edge_margin() const -> double
{
    return edge_margin_;
}

void DragAutoscrollModel::set_speed(double pixels_per_tick)
{
    speed_ = pixels_per_tick;
}

auto DragAutoscrollModel::speed() const -> double
{
    return speed_;
}

void DragAutoscrollModel::start(AutoscrollDirection dir)
{
    direction_ = dir;
}

void DragAutoscrollModel::stop()
{
    direction_ = AutoscrollDirection::kNone;
}

auto DragAutoscrollModel::direction() const -> AutoscrollDirection
{
    return direction_;
}

auto DragAutoscrollModel::direction_label(AutoscrollDirection dir) -> std::string
{
    switch (dir)
    {
        case AutoscrollDirection::kNone: return "none";
        case AutoscrollDirection::kUp: return "up";
        case AutoscrollDirection::kDown: return "down";
        case AutoscrollDirection::kLeft: return "left";
        case AutoscrollDirection::kRight: return "right";
        case AutoscrollDirection::kUpLeft: return "up_left";
        case AutoscrollDirection::kUpRight: return "up_right";
        case AutoscrollDirection::kDownLeft: return "down_left";
        case AutoscrollDirection::kDownRight: return "down_right";
    }
    return "unknown";
}

} // namespace markamp::canvas
