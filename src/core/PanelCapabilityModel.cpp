/// @file PanelCapabilityModel.cpp
/// @brief V21 Phase 06 — PanelCapabilityModel implementation.

#include "PanelCapabilityModel.h"

#include <algorithm>

namespace markamp::core
{

// ── Registration ──

void PanelCapabilityModel::register_panel(PanelCapabilityEntry entry)
{
    const auto id = entry.panel_id;
    if (!panels_.contains(id)) panel_order_.push_back(id);
    panels_[id] = std::move(entry);
}

auto PanelCapabilityModel::get_panel(const std::string& panel_id) const
    -> const PanelCapabilityEntry*
{
    auto it = panels_.find(panel_id);
    return it != panels_.end() ? &it->second : nullptr;
}

auto PanelCapabilityModel::all_panels() const -> std::vector<const PanelCapabilityEntry*>
{
    std::vector<const PanelCapabilityEntry*> result;
    result.reserve(panel_order_.size());
    for (const auto& id : panel_order_)
    {
        auto it = panels_.find(id);
        if (it != panels_.end()) result.push_back(&it->second);
    }
    return result;
}

auto PanelCapabilityModel::panels_for_area(PanelHostArea area) const
    -> std::vector<const PanelCapabilityEntry*>
{
    std::vector<const PanelCapabilityEntry*> result;
    for (const auto& id : panel_order_)
    {
        auto it = panels_.find(id);
        if (it != panels_.end() && it->second.host_area == area)
            result.push_back(&it->second);
    }
    return result;
}

auto PanelCapabilityModel::stub_panels() const -> std::vector<const PanelCapabilityEntry*>
{
    std::vector<const PanelCapabilityEntry*> result;
    for (const auto& id : panel_order_)
    {
        auto it = panels_.find(id);
        if (it != panels_.end() && it->second.is_stub)
            result.push_back(&it->second);
    }
    return result;
}

auto PanelCapabilityModel::panel_count() const -> std::size_t { return panels_.size(); }

// ── Toggle Commands ──

void PanelCapabilityModel::register_toggle(PanelToggleCommand toggle)
{
    toggles_[toggle.panel_id] = std::move(toggle);
}

auto PanelCapabilityModel::get_toggle(const std::string& panel_id) const
    -> const PanelToggleCommand*
{
    auto it = toggles_.find(panel_id);
    return it != toggles_.end() ? &it->second : nullptr;
}

auto PanelCapabilityModel::dispatch_toggle(const std::string& panel_id,
                                            ControlActionManifest& manifest) -> bool
{
    auto it = toggles_.find(panel_id);
    if (it == toggles_.end()) return false;
    return manifest.execute_action(it->second.action_id);
}

void PanelCapabilityModel::refresh_toggles(const ControlActionManifest& manifest)
{
    for (auto& [id, toggle] : toggles_)
    {
        const auto* action = manifest.get_action(toggle.action_id);
        toggle.is_bound = action != nullptr && action->has_handler();
    }
}

// ── Layout Snapshots ──

void PanelCapabilityModel::set_layout_state(const std::string& panel_id,
                                             bool visible, int width, int height)
{
    auto& state = layout_states_[panel_id];
    state.panel_id = panel_id;
    state.is_visible = visible;
    state.width = width;
    state.height = height;
}

void PanelCapabilityModel::set_active_bottom(const std::string& panel_id)
{
    active_bottom_ = panel_id;
}

void PanelCapabilityModel::set_active_secondary(const std::string& panel_id)
{
    active_secondary_ = panel_id;
}

void PanelCapabilityModel::set_bottom_height(int height) { bottom_height_ = height; }
void PanelCapabilityModel::set_secondary_width(int width) { secondary_width_ = width; }
void PanelCapabilityModel::set_bottom_visible(bool visible) { bottom_visible_ = visible; }
void PanelCapabilityModel::set_secondary_visible(bool visible) { secondary_visible_ = visible; }

auto PanelCapabilityModel::take_layout_snapshot() const -> PanelLayoutSnapshot
{
    PanelLayoutSnapshot snapshot;
    snapshot.active_bottom_panel = active_bottom_;
    snapshot.active_secondary_panel = active_secondary_;
    snapshot.bottom_panel_height = bottom_height_;
    snapshot.secondary_sidebar_width = secondary_width_;
    snapshot.bottom_visible = bottom_visible_;
    snapshot.secondary_visible = secondary_visible_;

    for (const auto& [id, state] : layout_states_)
        snapshot.panel_states.push_back(state);

    return snapshot;
}

void PanelCapabilityModel::restore_layout_snapshot(const PanelLayoutSnapshot& snapshot)
{
    active_bottom_ = snapshot.active_bottom_panel;
    active_secondary_ = snapshot.active_secondary_panel;
    bottom_height_ = snapshot.bottom_panel_height;
    secondary_width_ = snapshot.secondary_sidebar_width;
    bottom_visible_ = snapshot.bottom_visible;
    secondary_visible_ = snapshot.secondary_visible;

    for (const auto& state : snapshot.panel_states)
        layout_states_[state.panel_id] = state;
}

// ── Diagnostics ──

auto PanelCapabilityModel::diagnose(const ControlActionManifest& manifest) const
    -> std::vector<PanelCapabilityDiagnostic>
{
    std::vector<PanelCapabilityDiagnostic> diags;

    for (const auto& id : panel_order_)
    {
        auto pit = panels_.find(id);
        if (pit == panels_.end()) continue;
        const auto& panel = pit->second;

        if (panel.is_stub)
        {
            diags.push_back({.panel_id = id,
                             .issue = "Panel is a stub — should be gated or removed",
                             .is_stub = true});
        }

        auto tit = toggles_.find(id);
        if (tit == toggles_.end())
        {
            diags.push_back({.panel_id = id,
                             .issue = "Panel has no toggle command",
                             .is_missing_toggle = true});
        }
        else
        {
            const auto* action = manifest.get_action(tit->second.action_id);
            if (action == nullptr || !action->has_handler())
            {
                diags.push_back({.panel_id = id,
                                 .issue = "Panel toggle command has no handler",
                                 .is_missing_handler = true});
            }
        }
    }

    return diags;
}

auto PanelCapabilityModel::live_panel_count() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& [id, p] : panels_)
        if (!p.is_stub && !p.is_gated) ++count;
    return count;
}

auto PanelCapabilityModel::stub_count() const -> std::size_t
{
    return stub_panels().size();
}

} // namespace markamp::core
