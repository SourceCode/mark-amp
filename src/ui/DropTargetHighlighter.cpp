#include "DropTargetHighlighter.h"

#include <algorithm>

namespace markamp::ui
{

auto DropTargetState::style_name() const -> std::string
{
    switch (style)
    {
        case HighlightStyle::kNone:
            return "none";
        case HighlightStyle::kValid:
            return "valid";
        case HighlightStyle::kInvalid:
            return "invalid";
        case HighlightStyle::kNeutral:
            return "neutral";
        case HighlightStyle::kActive:
            return "active";
    }
    return "unknown";
}

auto DropTargetState::insertion_name() const -> std::string
{
    switch (insertion)
    {
        case InsertionPosition::kNone:
            return "none";
        case InsertionPosition::kBefore:
            return "before";
        case InsertionPosition::kAfter:
            return "after";
        case InsertionPosition::kInside:
            return "inside";
    }
    return "unknown";
}

void DropTargetHighlighter::add_zone(const std::string& zone_id)
{
    if (find_zone(zone_id) == nullptr)
    {
        DropTargetState state;
        state.zone_id = zone_id;
        zones_.push_back(state);
    }
}

void DropTargetHighlighter::remove_zone(const std::string& zone_id)
{
    zones_.erase(std::remove_if(zones_.begin(),
                                zones_.end(),
                                [&zone_id](const DropTargetState& state)
                                { return state.zone_id == zone_id; }),
                 zones_.end());
}

void DropTargetHighlighter::clear_zones()
{
    zones_.clear();
}

auto DropTargetHighlighter::zone_count() const -> int
{
    return static_cast<int>(zones_.size());
}

void DropTargetHighlighter::set_style(const std::string& zone_id, HighlightStyle style)
{
    auto* zone = find_zone(zone_id);
    if (zone != nullptr)
    {
        zone->style = style;
    }
}

void DropTargetHighlighter::set_insertion(const std::string& zone_id, InsertionPosition position)
{
    auto* zone = find_zone(zone_id);
    if (zone != nullptr)
    {
        zone->insertion = position;
    }
}

void DropTargetHighlighter::set_hovered(const std::string& zone_id)
{
    clear_hover();
    auto* zone = find_zone(zone_id);
    if (zone != nullptr)
    {
        zone->is_hovered = true;
    }
}

void DropTargetHighlighter::clear_hover()
{
    for (auto& zone : zones_)
    {
        zone.is_hovered = false;
    }
}

auto DropTargetHighlighter::zone_state(const std::string& zone_id) const -> DropTargetState
{
    const auto* zone = find_zone(zone_id);
    if (zone != nullptr)
    {
        return *zone;
    }
    return {};
}

auto DropTargetHighlighter::all_states() const -> std::vector<DropTargetState>
{
    return zones_;
}

auto DropTargetHighlighter::highlighted_zones() const -> std::vector<DropTargetState>
{
    std::vector<DropTargetState> result;
    for (const auto& zone : zones_)
    {
        if (zone.style != HighlightStyle::kNone)
        {
            result.push_back(zone);
        }
    }
    return result;
}

void DropTargetHighlighter::reset_all()
{
    for (auto& zone : zones_)
    {
        zone.style = HighlightStyle::kNone;
        zone.insertion = InsertionPosition::kNone;
        zone.is_hovered = false;
    }
}

void DropTargetHighlighter::highlight_valid(const std::string& zone_id, InsertionPosition insertion)
{
    auto* zone = find_zone(zone_id);
    if (zone != nullptr)
    {
        zone->style = HighlightStyle::kValid;
        zone->insertion = insertion;
    }
}

void DropTargetHighlighter::highlight_invalid(const std::string& zone_id)
{
    auto* zone = find_zone(zone_id);
    if (zone != nullptr)
    {
        zone->style = HighlightStyle::kInvalid;
        zone->insertion = InsertionPosition::kNone;
    }
}

auto DropTargetHighlighter::has_highlights() const -> bool
{
    return !highlighted_zones().empty();
}

auto DropTargetHighlighter::find_zone(const std::string& zone_id) -> DropTargetState*
{
    for (auto& zone : zones_)
    {
        if (zone.zone_id == zone_id)
        {
            return &zone;
        }
    }
    return nullptr;
}

auto DropTargetHighlighter::find_zone(const std::string& zone_id) const -> const DropTargetState*
{
    for (const auto& zone : zones_)
    {
        if (zone.zone_id == zone_id)
        {
            return &zone;
        }
    }
    return nullptr;
}

} // namespace markamp::ui
