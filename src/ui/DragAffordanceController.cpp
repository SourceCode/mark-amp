#include "DragAffordanceController.h"

namespace markamp::ui
{

// ── DragIntent ──────────────────────────────────────────────────────

auto DragIntent::meets_threshold(int min_distance, int min_hold_ms) const -> bool
{
    return distance_px >= min_distance && hold_time_ms >= min_hold_ms;
}

// ── DragAffordanceController ────────────────────────────────────────

void DragAffordanceController::set_distance_threshold(int pixels)
{
    distance_threshold_ = pixels;
    model_.set_drag_threshold(pixels);
}

auto DragAffordanceController::distance_threshold() const -> int
{
    return distance_threshold_;
}

void DragAffordanceController::set_hold_threshold(int ms)
{
    hold_threshold_ms_ = ms;
}

auto DragAffordanceController::hold_threshold() const -> int
{
    return hold_threshold_ms_;
}

auto DragAffordanceController::evaluate_intent(const DragIntent& intent) const -> bool
{
    return intent.meets_threshold(distance_threshold_, hold_threshold_ms_);
}

void DragAffordanceController::begin(const std::string& source_id)
{
    model_.begin_drag(source_id);
    last_error_ = {};
}

void DragAffordanceController::update(int distance_px)
{
    model_.update_distance(distance_px);
}

auto DragAffordanceController::complete(const std::string& zone_id) -> bool
{
    auto validity = model_.zone_validity(zone_id);
    if (validity == DropValidity::kInvalid)
    {
        last_error_ = make_error(zone_id, "This drop zone does not accept the dragged item");
        model_.cancel();
        return false;
    }
    model_.complete(zone_id);
    return true;
}

void DragAffordanceController::cancel()
{
    model_.cancel();
}

void DragAffordanceController::reset()
{
    model_.reset();
    last_error_ = {};
    copy_mode_ = false;
}

auto DragAffordanceController::state() const -> DragState
{
    return model_.state();
}

auto DragAffordanceController::cursor_state(const std::string& hover_zone_id) const
    -> DragCursorState
{
    switch (model_.state())
    {
        case DragState::kIdle:
            return DragCursorState::kDefault;
        case DragState::kPending:
            return DragCursorState::kGrab;
        case DragState::kDragging:
        {
            if (!hover_zone_id.empty())
            {
                auto validity = model_.zone_validity(hover_zone_id);
                if (validity == DropValidity::kInvalid)
                {
                    return DragCursorState::kNoDrop;
                }
                return copy_mode_ ? DragCursorState::kCopy : DragCursorState::kMove;
            }
            return DragCursorState::kGrabbing;
        }
        case DragState::kCancelled:
        case DragState::kCompleted:
            return DragCursorState::kDefault;
    }
    return DragCursorState::kDefault;
}

auto DragAffordanceController::is_dragging() const -> bool
{
    return model_.state() == DragState::kDragging;
}

auto DragAffordanceController::source_id() const -> const std::string&
{
    return model_.source_id();
}

void DragAffordanceController::set_drop_zones(std::vector<DropZone> zones)
{
    model_.set_drop_zones(std::move(zones));
}

auto DragAffordanceController::drop_zones() const -> const std::vector<DropZone>&
{
    return model_.drop_zones();
}

auto DragAffordanceController::zone_validity(const std::string& zone_id) const -> DropValidity
{
    return model_.zone_validity(zone_id);
}

void DragAffordanceController::set_copy_mode(bool enabled)
{
    copy_mode_ = enabled;
}

auto DragAffordanceController::is_copy_mode() const -> bool
{
    return copy_mode_;
}

auto DragAffordanceController::last_error() const -> DropError
{
    return last_error_;
}

auto DragAffordanceController::make_error(const std::string& zone_id, const std::string& reason)
    -> DropError
{
    DropError error;
    error.zone_id = zone_id;
    error.reason = reason;
    error.suggestion = "Try dropping on a compatible target or press Escape to cancel";
    return error;
}

} // namespace markamp::ui
