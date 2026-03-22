/// @file SidebarStateContract.cpp
/// @brief V21 Phase 05 — SidebarStateContract implementation.

#include "SidebarStateContract.h"

#include <algorithm>

namespace markamp::core
{

// ── Current State Tracking ──

void SidebarStateContract::set_active_panel(const std::string& area,
                                             const std::string& panel_id)
{
    active_panels_[area] = panel_id;
}

auto SidebarStateContract::active_panel(const std::string& area) const -> std::string
{
    auto iter = active_panels_.find(area);
    return iter != active_panels_.end() ? iter->second : "";
}

void SidebarStateContract::set_panel_expanded(const std::string& panel_id, bool expanded)
{
    panel_states_[panel_id].panel_id = panel_id;
    panel_states_[panel_id].is_expanded = expanded;
}

auto SidebarStateContract::is_panel_expanded(const std::string& panel_id) const -> bool
{
    auto iter = panel_states_.find(panel_id);
    return iter != panel_states_.end() ? iter->second.is_expanded : true;
}

void SidebarStateContract::set_panel_visible(const std::string& panel_id, bool visible)
{
    panel_states_[panel_id].panel_id = panel_id;
    panel_states_[panel_id].is_visible = visible;
}

auto SidebarStateContract::is_panel_visible(const std::string& panel_id) const -> bool
{
    auto iter = panel_states_.find(panel_id);
    return iter != panel_states_.end() ? iter->second.is_visible : true;
}

void SidebarStateContract::set_sidebar_width(const std::string& area, int width)
{
    sidebar_widths_[area] = width;
}

auto SidebarStateContract::sidebar_width(const std::string& area) const -> int
{
    auto iter = sidebar_widths_.find(area);
    return iter != sidebar_widths_.end() ? iter->second : 250;
}

// ── Snapshot Operations ──

auto SidebarStateContract::take_snapshot(const std::string& area) const -> SidebarSnapshot
{
    SidebarSnapshot snapshot;
    snapshot.sidebar_area = area;
    snapshot.active_panel_id = active_panel(area);
    snapshot.sidebar_width = sidebar_width(area);

    auto area_iter = area_panels_.find(area);
    if (area_iter != area_panels_.end())
    {
        for (const auto& panel_id : area_iter->second)
        {
            auto state_iter = panel_states_.find(panel_id);
            if (state_iter != panel_states_.end())
            {
                snapshot.panel_states.push_back(state_iter->second);
            }
            else
            {
                SidebarPanelState default_state;
                default_state.panel_id = panel_id;
                snapshot.panel_states.push_back(std::move(default_state));
            }
        }
    }

    return snapshot;
}

void SidebarStateContract::restore_snapshot(const SidebarSnapshot& snapshot)
{
    active_panels_[snapshot.sidebar_area] = snapshot.active_panel_id;
    sidebar_widths_[snapshot.sidebar_area] = snapshot.sidebar_width;

    for (const auto& state : snapshot.panel_states)
    {
        panel_states_[state.panel_id] = state;
    }
}

void SidebarStateContract::save_named_snapshot(const std::string& name,
                                                const SidebarSnapshot& snapshot)
{
    named_snapshots_[name] = snapshot;
}

auto SidebarStateContract::get_named_snapshot(const std::string& name) const
    -> const SidebarSnapshot*
{
    auto iter = named_snapshots_.find(name);
    return iter != named_snapshots_.end() ? &iter->second : nullptr;
}

auto SidebarStateContract::has_snapshot(const std::string& name) const -> bool
{
    return named_snapshots_.contains(name);
}

auto SidebarStateContract::snapshot_names() const -> std::vector<std::string>
{
    std::vector<std::string> names;
    names.reserve(named_snapshots_.size());
    for (const auto& [name, snap] : named_snapshots_)
    {
        names.push_back(name);
    }
    std::sort(names.begin(), names.end());
    return names;
}

// ── Registration ──

void SidebarStateContract::register_panel(const std::string& panel_id,
                                           const std::string& area)
{
    auto& panels = area_panels_[area];
    if (std::find(panels.begin(), panels.end(), panel_id) == panels.end())
    {
        panels.push_back(panel_id);
    }

    // Initialize default state if not present
    if (!panel_states_.contains(panel_id))
    {
        SidebarPanelState state;
        state.panel_id = panel_id;
        panel_states_[panel_id] = std::move(state);
    }
}

auto SidebarStateContract::panels_for_area(const std::string& area) const
    -> std::vector<std::string>
{
    auto iter = area_panels_.find(area);
    return iter != area_panels_.end() ? iter->second : std::vector<std::string>{};
}

auto SidebarStateContract::tracked_panel_count() const -> std::size_t
{
    return panel_states_.size();
}

} // namespace markamp::core
