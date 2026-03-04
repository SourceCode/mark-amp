#include "DragController.h"

#include <algorithm>
#include <cmath>

namespace markamp::ui
{

void DragControllerModel::begin_potential_drag(int pos_x,
                                               int pos_y,
                                               DragPayloadType type,
                                               DragPayload payload)
{
    phase_ = DragPhase::kPending;
    type_ = type;
    payload_ = std::move(payload);
    start_x_ = pos_x;
    start_y_ = pos_y;
    current_x_ = pos_x;
    current_y_ = pos_y;
    active_zone_id_.clear();
}

auto DragControllerModel::update_position(int pos_x, int pos_y) -> bool
{
    current_x_ = pos_x;
    current_y_ = pos_y;

    if (phase_ == DragPhase::kPending && has_exceeded_threshold())
    {
        phase_ = DragPhase::kDragging;
        return true;
    }
    return false;
}

void DragControllerModel::complete_drop()
{
    if (phase_ == DragPhase::kDragging)
    {
        phase_ = DragPhase::kDropping;
    }
}

void DragControllerModel::cancel()
{
    if (phase_ == DragPhase::kDragging || phase_ == DragPhase::kPending)
    {
        phase_ = DragPhase::kCancelled;
    }
}

void DragControllerModel::reset()
{
    phase_ = DragPhase::kIdle;
    active_zone_id_.clear();
}

auto DragControllerModel::phase() const -> DragPhase
{
    return phase_;
}
auto DragControllerModel::payload_type() const -> DragPayloadType
{
    return type_;
}
auto DragControllerModel::payload() const -> const DragPayload&
{
    return payload_;
}
auto DragControllerModel::start_x() const -> int
{
    return start_x_;
}
auto DragControllerModel::start_y() const -> int
{
    return start_y_;
}
auto DragControllerModel::current_x() const -> int
{
    return current_x_;
}
auto DragControllerModel::current_y() const -> int
{
    return current_y_;
}

auto DragControllerModel::drag_distance_squared() const -> int
{
    const int delta_x = current_x_ - start_x_;
    const int delta_y = current_y_ - start_y_;
    return delta_x * delta_x + delta_y * delta_y;
}

auto DragControllerModel::has_exceeded_threshold() const -> bool
{
    return drag_distance_squared() > distance_threshold_ * distance_threshold_;
}

void DragControllerModel::register_drop_zone(DropZone zone)
{
    zones_.push_back(std::move(zone));
}

void DragControllerModel::clear_drop_zones()
{
    zones_.clear();
    active_zone_id_.clear();
}

auto DragControllerModel::active_drop_zone() const -> const DropZone*
{
    if (active_zone_id_.empty())
    {
        return nullptr;
    }
    for (const auto& zone : zones_)
    {
        if (zone.zone_id == active_zone_id_)
        {
            return &zone;
        }
    }
    return nullptr;
}

void DragControllerModel::set_active_zone(const std::string& zone_id)
{
    active_zone_id_ = zone_id;
    for (auto& zone : zones_)
    {
        zone.is_active = (zone.zone_id == zone_id);
    }
}

void DragControllerModel::clear_active_zone()
{
    active_zone_id_.clear();
    for (auto& zone : zones_)
    {
        zone.is_active = false;
    }
}

auto DragControllerModel::needs_auto_scroll() const -> bool
{
    if (phase_ != DragPhase::kDragging)
    {
        return false;
    }
    return current_y_ < auto_scroll_margin_ ||
           (viewport_height_ > 0 && current_y_ > viewport_height_ - auto_scroll_margin_);
}

auto DragControllerModel::auto_scroll_direction() const -> int
{
    if (phase_ != DragPhase::kDragging)
    {
        return 0;
    }
    if (current_y_ < auto_scroll_margin_)
    {
        return -1;
    }
    if (viewport_height_ > 0 && current_y_ > viewport_height_ - auto_scroll_margin_)
    {
        return 1;
    }
    return 0;
}

void DragControllerModel::set_scroll_edge_margin(int margin_px)
{
    auto_scroll_margin_ = margin_px > 0 ? margin_px : kDefaultAutoScrollMargin;
}

void DragControllerModel::set_distance_threshold(int threshold_px)
{
    distance_threshold_ = threshold_px > 0 ? threshold_px : kDefaultThreshold;
}

auto DragControllerModel::distance_threshold() const -> int
{
    return distance_threshold_;
}

} // namespace markamp::ui
