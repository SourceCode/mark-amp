#include "FocusManager.h"

#include <algorithm>
#include <array>

namespace markamp::ui
{

FocusManager::FocusManager()
{
    // Default canonical traversal order
    base_traversal_order_ = {
        FocusZoneId::kMenuBar,
        FocusZoneId::kActivityBar,
        FocusZoneId::kSidebar,
        FocusZoneId::kBreadcrumb,
        FocusZoneId::kEditorArea,
        FocusZoneId::kBottomPanel,
        FocusZoneId::kStatusBar,
    };

    // All zones enabled by default
    zone_enabled_.fill(true);
}

auto FocusManager::get() -> FocusManager&
{
    static FocusManager instance;
    return instance;
}

auto FocusManager::on_focus_changed(FocusChangeCallback callback) -> std::size_t
{
    auto id = ++next_listener_id_;
    listeners_.emplace_back(id, std::move(callback));
    return id;
}

void FocusManager::remove_focus_listener(std::size_t listener_id)
{
    std::erase_if(listeners_,
                  [listener_id](const auto& pair) { return pair.first == listener_id; });
}

void FocusManager::publish_focus_change()
{
    for (const auto& [id, cb] : listeners_)
    {
        if (cb)
        {
            cb(current_zone_, current_item_);
        }
    }
}

void FocusManager::set_zone(FocusZoneId zone)
{
    set_focus(zone, -1);
}

void FocusManager::set_focus(FocusZoneId zone, int item_index)
{
    if (current_zone_ != zone || current_item_ != item_index)
    {
        current_zone_ = zone;
        current_item_ = item_index;
        publish_focus_change();
    }
}

void FocusManager::set_item(int item_index)
{
    if (current_item_ != item_index)
    {
        current_item_ = item_index;
        publish_focus_change();
    }
}

void FocusManager::advance(FocusDirection direction)
{
    auto new_zone = next_enabled_zone(current_zone_, direction);
    if (new_zone != current_zone_ || current_item_ != -1)
    {
        current_zone_ = new_zone;
        current_item_ = -1;
        publish_focus_change();
    }
}

void FocusManager::push_snapshot()
{
    snapshot_stack_.push_back(FocusSnapshot{current_zone_, current_item_});
}

void FocusManager::restore()
{
    if (snapshot_stack_.empty())
    {
        return;
    }
    const auto& snap = snapshot_stack_.back();
    set_focus(snap.zone, snap.item_index);
    snapshot_stack_.pop_back();
}

void FocusManager::set_traversal_order(std::vector<FocusZoneId> order)
{
    base_traversal_order_ = std::move(order);
}

void FocusManager::push_focus_trap(std::vector<FocusZoneId> trap_zones)
{
    trap_stack_.push_back(std::move(trap_zones));
    // When a trap is pushed, typically we'd also force focus into the trap.
    // We leave that to the caller.
}

void FocusManager::pop_focus_trap()
{
    if (!trap_stack_.empty())
    {
        trap_stack_.pop_back();
    }
}

void FocusManager::set_keyboard_mode_active(bool active)
{
    if (keyboard_mode_active_ != active)
    {
        keyboard_mode_active_ = active;
        // Broadcast focus change to immediately refresh focus ring visibility
        publish_focus_change();
    }
}

auto FocusManager::arrow_behavior(FocusZoneId zone) -> ArrowKeyBehavior
{
    switch (zone)
    {
        case FocusZoneId::kMenuBar:
        case FocusZoneId::kStatusBar:
        case FocusZoneId::kBreadcrumb:
            return ArrowKeyBehavior::kHorizontalStrip;
        case FocusZoneId::kActivityBar:
        case FocusZoneId::kSidebar:
        case FocusZoneId::kBottomPanel:
            return ArrowKeyBehavior::kVerticalList;
        case FocusZoneId::kEditorArea:
        case FocusZoneId::kModalOverlay:
            return ArrowKeyBehavior::kGrid;
        case FocusZoneId::kCount:
            return ArrowKeyBehavior::kNone;
    }
    return ArrowKeyBehavior::kNone;
}

auto FocusManager::is_zone_enabled(FocusZoneId zone) const -> bool
{
    const auto zone_idx = static_cast<std::size_t>(zone);
    if (zone_idx >= kZoneCount)
    {
        return false;
    }
    return zone_enabled_.at(zone_idx);
}

void FocusManager::set_zone_enabled(FocusZoneId zone, bool enabled)
{
    const auto zone_idx = static_cast<std::size_t>(zone);
    if (zone_idx < kZoneCount)
    {
        zone_enabled_.at(zone_idx) = enabled;
    }
}

auto FocusManager::zone_order() const -> std::vector<FocusZoneId>
{
    const auto& order = trap_stack_.empty() ? base_traversal_order_ : trap_stack_.back();

    std::vector<FocusZoneId> result;
    for (auto zone : order)
    {
        if (is_zone_enabled(zone))
        {
            result.push_back(zone);
        }
    }
    return result;
}

auto FocusManager::next_enabled_zone(FocusZoneId from, FocusDirection dir) const -> FocusZoneId
{
    const auto enabled_order = zone_order();
    if (enabled_order.empty())
    {
        return from;
    }

    // Find current position in order
    auto iter = std::find(enabled_order.begin(), enabled_order.end(), from);
    if (iter == enabled_order.end())
    {
        return enabled_order.front();
    }

    auto pos = static_cast<std::size_t>(std::distance(enabled_order.begin(), iter));

    if (dir == FocusDirection::kForward)
    {
        pos = (pos + 1) % enabled_order.size();
    }
    else
    {
        pos = (pos == 0) ? (enabled_order.size() - 1) : (pos - 1);
    }

    return enabled_order.at(pos);
}

} // namespace markamp::ui
