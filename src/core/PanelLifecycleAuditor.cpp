/// @file PanelLifecycleAuditor.cpp
/// @brief V21 Phase 05 — PanelLifecycleAuditor implementation.

#include "PanelLifecycleAuditor.h"

#include <algorithm>

namespace markamp::core
{

// ── Panel Registration ──

void PanelLifecycleAuditor::register_panel(PanelRegistryEntry entry)
{
    const auto id = entry.panel_id;
    if (!panels_.contains(id))
    {
        panel_order_.push_back(id);
    }
    panels_[id] = std::move(entry);
}

auto PanelLifecycleAuditor::get_panel(const std::string& panel_id) const
    -> const PanelRegistryEntry*
{
    auto iter = panels_.find(panel_id);
    return iter != panels_.end() ? &iter->second : nullptr;
}

auto PanelLifecycleAuditor::all_panels() const -> std::vector<const PanelRegistryEntry*>
{
    std::vector<const PanelRegistryEntry*> result;
    result.reserve(panel_order_.size());
    for (const auto& id : panel_order_)
    {
        auto iter = panels_.find(id);
        if (iter != panels_.end()) result.push_back(&iter->second);
    }
    return result;
}

auto PanelLifecycleAuditor::panels_for_area(const std::string& area) const
    -> std::vector<const PanelRegistryEntry*>
{
    std::vector<const PanelRegistryEntry*> result;
    for (const auto& id : panel_order_)
    {
        auto iter = panels_.find(id);
        if (iter != panels_.end() && iter->second.area == area)
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

auto PanelLifecycleAuditor::production_panels() const
    -> std::vector<const PanelRegistryEntry*>
{
    std::vector<const PanelRegistryEntry*> result;
    for (const auto& id : panel_order_)
    {
        auto iter = panels_.find(id);
        if (iter != panels_.end() && iter->second.is_production_visible())
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

auto PanelLifecycleAuditor::placeholder_panels() const
    -> std::vector<const PanelRegistryEntry*>
{
    std::vector<const PanelRegistryEntry*> result;
    for (const auto& id : panel_order_)
    {
        auto iter = panels_.find(id);
        if (iter != panels_.end() &&
            (iter->second.readiness == PanelReadiness::kPlaceholder ||
             iter->second.readiness == PanelReadiness::kExperimental))
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

auto PanelLifecycleAuditor::panel_count() const -> std::size_t
{
    return panels_.size();
}

// ── Lifecycle State Transitions ──

auto PanelLifecycleAuditor::set_lifecycle(const std::string& panel_id,
                                            PanelLifecycleState state) -> bool
{
    auto iter = panels_.find(panel_id);
    if (iter == panels_.end()) return false;
    iter->second.lifecycle = state;
    return true;
}

auto PanelLifecycleAuditor::mark_stale(const std::string& panel_id) -> bool
{
    return set_lifecycle(panel_id, PanelLifecycleState::kStale);
}

auto PanelLifecycleAuditor::panels_in_state(PanelLifecycleState state) const
    -> std::vector<const PanelRegistryEntry*>
{
    std::vector<const PanelRegistryEntry*> result;
    for (const auto& id : panel_order_)
    {
        auto iter = panels_.find(id);
        if (iter != panels_.end() && iter->second.lifecycle == state)
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

// ── Header Actions ──

void PanelLifecycleAuditor::register_header_action(PanelHeaderAction action)
{
    header_actions_.push_back(std::move(action));
}

auto PanelLifecycleAuditor::header_actions(const std::string& panel_id) const
    -> std::vector<const PanelHeaderAction*>
{
    std::vector<const PanelHeaderAction*> result;
    for (const auto& action : header_actions_)
    {
        if (action.panel_id == panel_id)
        {
            result.push_back(&action);
        }
    }
    return result;
}

void PanelLifecycleAuditor::refresh_header_actions(const ControlActionManifest& manifest)
{
    for (auto& action : header_actions_)
    {
        const auto* entry = manifest.get_action(action.action_id);
        if (entry != nullptr)
        {
            action.is_bound = entry->has_handler();
            action.is_enabled = entry->has_handler();
        }
        else
        {
            action.is_bound = false;
            action.is_enabled = false;
        }
    }
}

// ── Explorer Sections ──

void PanelLifecycleAuditor::register_explorer_section(ExplorerSection section)
{
    explorer_sections_.push_back(std::move(section));
}

auto PanelLifecycleAuditor::explorer_sections() const
    -> std::vector<const ExplorerSection*>
{
    std::vector<const ExplorerSection*> result;
    for (const auto& section : explorer_sections_)
    {
        result.push_back(&section);
    }
    return result;
}

auto PanelLifecycleAuditor::incomplete_explorer_sections() const
    -> std::vector<const ExplorerSection*>
{
    std::vector<const ExplorerSection*> result;
    for (const auto& section : explorer_sections_)
    {
        if (!section.is_implemented)
        {
            result.push_back(&section);
        }
    }
    return result;
}

// ── Diagnostics ──

auto PanelLifecycleAuditor::diagnose() const -> std::vector<PanelDiagnostic>
{
    std::vector<PanelDiagnostic> diagnostics;

    // Check panels
    for (const auto& id : panel_order_)
    {
        auto iter = panels_.find(id);
        if (iter == panels_.end()) continue;

        const auto& panel = iter->second;

        if (panel.readiness == PanelReadiness::kPlaceholder)
        {
            PanelDiagnostic diag;
            diag.panel_id = panel.panel_id;
            diag.issue = "Panel is a placeholder — should be gated from production";
            diag.is_placeholder = true;
            diagnostics.push_back(std::move(diag));
        }

        if (!panel.has_factory || !panel.has_real_content)
        {
            PanelDiagnostic diag;
            diag.panel_id = panel.panel_id;
            diag.issue = "Panel has no factory or no real content";
            diag.is_dead = true;
            diagnostics.push_back(std::move(diag));
        }

        if (panel.lifecycle == PanelLifecycleState::kStale)
        {
            PanelDiagnostic diag;
            diag.panel_id = panel.panel_id;
            diag.issue = "Panel is stale and needs refresh";
            diag.is_stale = true;
            diagnostics.push_back(std::move(diag));
        }
    }

    // Check explorer sections
    for (const auto& section : explorer_sections_)
    {
        if (!section.is_implemented)
        {
            PanelDiagnostic diag;
            diag.panel_id = "explorer." + section.section_id;
            diag.issue = "Explorer section '" + section.label + "' not implemented";
            diag.is_incomplete_section = true;
            diagnostics.push_back(std::move(diag));
        }
    }

    return diagnostics;
}

auto PanelLifecycleAuditor::production_count() const -> std::size_t
{
    return production_panels().size();
}

auto PanelLifecycleAuditor::non_production_count() const -> std::size_t
{
    return panel_count() - production_count();
}

} // namespace markamp::core
