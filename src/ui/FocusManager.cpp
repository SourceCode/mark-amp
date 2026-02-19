#include "FocusManager.h"

#include <algorithm>
#include <array>

namespace markamp::ui
{

FocusManager::FocusManager()
{
    // All zones enabled by default
    zone_enabled_.fill(true);
}

auto FocusManager::get() -> FocusManager&
{
    static FocusManager instance;
    return instance;
}

void FocusManager::set_zone(FocusZoneId zone)
{
    current_zone_ = zone;
}

void FocusManager::advance(FocusDirection direction)
{
    current_zone_ = next_enabled_zone(current_zone_, direction);
}

void FocusManager::push_snapshot()
{
    snapshot_stack_.push_back(FocusSnapshot{current_zone_, -1});
}

void FocusManager::restore()
{
    if (snapshot_stack_.empty())
    {
        return;
    }
    const auto& snap = snapshot_stack_.back();
    current_zone_ = snap.zone;
    snapshot_stack_.pop_back();
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
    // Canonical traversal order
    static constexpr std::array<FocusZoneId, 7> kTraversalOrder = {{
        FocusZoneId::kMenuBar,
        FocusZoneId::kActivityBar,
        FocusZoneId::kSidebar,
        FocusZoneId::kBreadcrumb,
        FocusZoneId::kEditorArea,
        FocusZoneId::kBottomPanel,
        FocusZoneId::kStatusBar,
    }};

    std::vector<FocusZoneId> result;
    for (auto zone : kTraversalOrder)
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
