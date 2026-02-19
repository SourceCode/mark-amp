#include "DragDropModel.h"

namespace markamp::ui
{

void DragDropModel::set_drag_threshold(int pixels)
{
    drag_threshold_ = (pixels < 1) ? 1 : pixels;
}
auto DragDropModel::drag_threshold() const -> int
{
    return drag_threshold_;
}

auto DragDropModel::state() const -> DragState
{
    return state_;
}

void DragDropModel::begin_drag(const std::string& source_id)
{
    if (state_ != DragState::kIdle)
    {
        return;
    }
    source_id_ = source_id;
    accumulated_distance_ = 0;
    state_ = DragState::kPending;
    last_error_.clear();
}

void DragDropModel::update_distance(int pixels)
{
    if (state_ != DragState::kPending)
    {
        return;
    }
    accumulated_distance_ += pixels;
    if (accumulated_distance_ >= drag_threshold_)
    {
        state_ = DragState::kDragging;
    }
}

void DragDropModel::complete(const std::string& zone_id)
{
    if (state_ != DragState::kDragging)
    {
        return;
    }
    const auto validity = zone_validity(zone_id);
    if (validity == DropValidity::kInvalid)
    {
        last_error_ = "Cannot drop here: invalid target \"" + zone_id + "\"";
        cancel();
        return;
    }
    state_ = DragState::kCompleted;
}

void DragDropModel::cancel()
{
    if (state_ == DragState::kPending || state_ == DragState::kDragging)
    {
        state_ = DragState::kCancelled;
    }
}

void DragDropModel::reset()
{
    state_ = DragState::kIdle;
    source_id_.clear();
    accumulated_distance_ = 0;
    last_error_.clear();
}

auto DragDropModel::source_id() const -> const std::string&
{
    return source_id_;
}

void DragDropModel::set_drop_zones(std::vector<DropZone> zones)
{
    zones_ = std::move(zones);
}

auto DragDropModel::drop_zones() const -> const std::vector<DropZone>&
{
    return zones_;
}

auto DragDropModel::zone_validity(const std::string& zone_id) const -> DropValidity
{
    for (const auto& zone : zones_)
    {
        if (zone.zone_id == zone_id)
        {
            return zone.validity;
        }
    }
    return DropValidity::kUnknown;
}

auto DragDropModel::last_error() const -> const std::string&
{
    return last_error_;
}

} // namespace markamp::ui
