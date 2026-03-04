#include "PointerEventRouter.h"

#include <algorithm>

namespace markamp::canvas
{

auto PointerEvent::type_name() const -> std::string
{
    switch (type)
    {
        case PointerEventType::kDown:
            return "down";
        case PointerEventType::kMove:
            return "move";
        case PointerEventType::kUp:
            return "up";
        case PointerEventType::kCancel:
            return "cancel";
    }
    return "unknown";
}

auto HitTarget::contains(double px, double py) const -> bool
{
    return px >= x && px <= x + width && py >= y && py <= y + height;
}

void PointerEventRouter::register_target(const HitTarget& target)
{
    targets_.push_back(target);
}

void PointerEventRouter::remove_target(const std::string& target_id)
{
    targets_.erase(std::remove_if(targets_.begin(),
                                  targets_.end(),
                                  [&target_id](const HitTarget& target)
                                  { return target.target_id == target_id; }),
                   targets_.end());
}

auto PointerEventRouter::target_count() const -> int
{
    return static_cast<int>(targets_.size());
}

auto PointerEventRouter::hit_test(double x, double y) const -> const HitTarget*
{
    const HitTarget* best = nullptr;
    for (const auto& target : targets_)
    {
        if (target.contains(x, y))
        {
            if (best == nullptr || target.z_order > best->z_order)
            {
                best = &target;
            }
        }
    }
    return best;
}

void PointerEventRouter::set_capture(const std::string& target_id)
{
    capture_target_id_ = target_id;
    has_capture_ = true;
}

void PointerEventRouter::release_capture()
{
    capture_target_id_.clear();
    has_capture_ = false;
}

auto PointerEventRouter::has_capture() const -> bool
{
    return has_capture_;
}

auto PointerEventRouter::capture_target_id() const -> const std::string&
{
    return capture_target_id_;
}

auto PointerEventRouter::dispatch(const PointerEvent& event) -> std::string
{
    ++dispatch_count_;

    // Cancel events release capture
    if (event.type == PointerEventType::kCancel)
    {
        std::string target = capture_target_id_;
        release_capture();
        return target;
    }

    // If captured, route to capture target
    if (has_capture_)
    {
        // Up releases capture
        if (event.type == PointerEventType::kUp)
        {
            std::string target = capture_target_id_;
            release_capture();
            return target;
        }
        return capture_target_id_;
    }

    // Hit test
    const auto* target = hit_test(event.x, event.y);
    if (target != nullptr)
    {
        // Down sets capture
        if (event.type == PointerEventType::kDown)
        {
            set_capture(target->target_id);
        }
        return target->target_id;
    }

    return "";
}

auto PointerEventRouter::dispatch_count() const -> int
{
    return dispatch_count_;
}

} // namespace markamp::canvas
