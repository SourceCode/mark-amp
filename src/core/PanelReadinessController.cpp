/// @file PanelReadinessController.cpp
/// @brief V25 P05: Panel readiness and settings host implementation.
#include "core/PanelReadinessController.h"

#include <algorithm>

namespace markamp::core
{

// ── PanelReadinessController ──

void PanelReadinessController::classify_panel(PanelClassification entry)
{
    panels_.push_back(std::move(entry));
}

auto PanelReadinessController::get_classification(const std::string& panel_id) const
    -> const PanelClassification*
{
    auto it = std::find_if(panels_.begin(), panels_.end(),
        [&](const PanelClassification& p) { return p.panel_id == panel_id; });
    return it != panels_.end() ? &(*it) : nullptr;
}

auto PanelReadinessController::gated_panels() const -> std::vector<const PanelClassification*>
{
    std::vector<const PanelClassification*> result;
    for (const auto& p : panels_) {
        if (p.scope == PanelScope::kGated) result.push_back(&p);
    }
    return result;
}

auto PanelReadinessController::blocking_panels() const -> std::vector<const PanelClassification*>
{
    std::vector<const PanelClassification*> result;
    for (const auto& p : panels_) {
        if (p.blocks_release()) result.push_back(&p);
    }
    return result;
}

void PanelReadinessController::clear() { panels_.clear(); }

// ── SettingsHostController ──

void SettingsHostController::stage_change(const std::string& key, const std::string& value)
{
    staged_changes_.emplace_back(key, value);
    state_.has_pending_changes = true;
    state_.pending_count = static_cast<int>(staged_changes_.size());
}

auto SettingsHostController::apply_staged() -> bool
{
    if (staged_changes_.empty()) return false;
    staged_changes_.clear();
    state_.has_pending_changes = false;
    state_.pending_count = 0;
    return true;
}

auto SettingsHostController::cancel_staged() -> bool
{
    if (staged_changes_.empty()) return false;
    staged_changes_.clear();
    state_.has_pending_changes = false;
    state_.pending_count = 0;
    return true;
}

void SettingsHostController::navigate_to_section(const std::string& section)
{
    state_.active_section = section;
}

void SettingsHostController::activate_deep_link(const std::string& /*link*/)
{
    state_.deep_link_active = true;
}

} // namespace markamp::core
