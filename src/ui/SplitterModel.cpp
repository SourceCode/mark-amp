#include "SplitterModel.h"

#include <algorithm>
#include <cmath>

namespace markamp::ui
{

void SplitterModel::set_constraints(int min_pos, int max_pos, int default_pos)
{
    min_position_ = min_pos;
    max_position_ = max_pos;
    default_position_ = default_pos;
    position_ = clamp(default_pos);
}

void SplitterModel::set_snap_points(std::vector<SplitterSnapPoint> snap_points)
{
    snap_points_ = std::move(snap_points);
}

void SplitterModel::set_keyboard_step(int step)
{
    keyboard_step_ = step;
}

auto SplitterModel::position() const -> int
{
    return position_;
}

void SplitterModel::set_position(int raw_position)
{
    position_ = snap(clamp(raw_position));
}

void SplitterModel::reset_to_default()
{
    position_ = clamp(default_position_);
}

void SplitterModel::resize_decrease()
{
    set_position(position_ - keyboard_step_);
}

void SplitterModel::resize_increase()
{
    set_position(position_ + keyboard_step_);
}

void SplitterModel::begin_drag()
{
    state_ = SplitterState::kDragging;
}

void SplitterModel::update_drag(int raw_position)
{
    if (state_ == SplitterState::kDragging)
    {
        set_position(raw_position);
    }
}

void SplitterModel::end_drag()
{
    state_ = SplitterState::kIdle;
}

auto SplitterModel::state() const -> SplitterState
{
    return state_;
}

auto SplitterModel::min_position() const -> int
{
    return min_position_;
}

auto SplitterModel::max_position() const -> int
{
    return max_position_;
}

auto SplitterModel::default_position() const -> int
{
    return default_position_;
}

auto SplitterModel::clamp(int raw_position) const -> int
{
    return std::clamp(raw_position, min_position_, max_position_);
}

auto SplitterModel::snap(int raw_position) const -> int
{
    for (const auto& snap_point : snap_points_)
    {
        if (std::abs(raw_position - snap_point.position) <= snap_point.tolerance)
        {
            return snap_point.position;
        }
    }
    return raw_position;
}

} // namespace markamp::ui
