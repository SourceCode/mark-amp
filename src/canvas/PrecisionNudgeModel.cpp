// ============================================================================
// File: src/canvas/PrecisionNudgeModel.cpp
// Phase 02 W14: Precision Nudge model implementation
// ============================================================================

#include "canvas/PrecisionNudgeModel.h"

namespace markamp::canvas
{

void PrecisionNudgeModel::set_small_step(double pixels)
{
    small_step_ = pixels;
}

auto PrecisionNudgeModel::small_step() const -> double
{
    return small_step_;
}

void PrecisionNudgeModel::set_big_step(double pixels)
{
    big_step_ = pixels;
}

auto PrecisionNudgeModel::big_step() const -> double
{
    return big_step_;
}

auto PrecisionNudgeModel::step_for(bool big) const -> double
{
    return big ? big_step_ : small_step_;
}

auto PrecisionNudgeModel::delta_x(NudgeDirection dir, bool big) const -> double
{
    const auto step = step_for(big);
    switch (dir)
    {
        case NudgeDirection::kLeft: return -step;
        case NudgeDirection::kRight: return step;
        default: return 0.0;
    }
}

auto PrecisionNudgeModel::delta_y(NudgeDirection dir, bool big) const -> double
{
    const auto step = step_for(big);
    switch (dir)
    {
        case NudgeDirection::kUp: return -step;
        case NudgeDirection::kDown: return step;
        default: return 0.0;
    }
}

auto PrecisionNudgeModel::direction_label(NudgeDirection dir) -> std::string
{
    switch (dir)
    {
        case NudgeDirection::kUp: return "up";
        case NudgeDirection::kDown: return "down";
        case NudgeDirection::kLeft: return "left";
        case NudgeDirection::kRight: return "right";
    }
    return "unknown";
}

} // namespace markamp::canvas
