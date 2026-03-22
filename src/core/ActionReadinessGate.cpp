/// @file ActionReadinessGate.cpp
/// @brief V23 Phase 02 — ActionReadinessGate implementation.

#include "ActionReadinessGate.h"

#include <algorithm>
#include <set>
#include <sstream>

namespace markamp::core
{

// ── Action Registration ──

void ActionReadinessGate::add_action(ActionReadinessItem item)
{
    actions_.push_back(std::move(item));
}

void ActionReadinessGate::add_actions(std::vector<ActionReadinessItem> items)
{
    for (auto& item : items) actions_.push_back(std::move(item));
}

// ── Panel Registration ──

void ActionReadinessGate::add_panel(PanelReadinessItem item)
{
    panels_.push_back(std::move(item));
}

void ActionReadinessGate::add_panels(std::vector<PanelReadinessItem> items)
{
    for (auto& item : items) panels_.push_back(std::move(item));
}

// ── Action Queries ──

auto ActionReadinessGate::action_count() const noexcept -> std::size_t
{
    return actions_.size();
}

auto ActionReadinessGate::actions_by_surface(ActionSurfaceKind surface) const
    -> std::vector<const ActionReadinessItem*>
{
    std::vector<const ActionReadinessItem*> result;
    for (const auto& item : actions_)
        if (item.surface == surface) result.push_back(&item);
    return result;
}

auto ActionReadinessGate::actions_by_status(ActionBindingStatus status) const
    -> std::vector<const ActionReadinessItem*>
{
    std::vector<const ActionReadinessItem*> result;
    for (const auto& item : actions_)
        if (item.status == status) result.push_back(&item);
    return result;
}

auto ActionReadinessGate::blocking_actions() const
    -> std::vector<const ActionReadinessItem*>
{
    std::vector<const ActionReadinessItem*> result;
    for (const auto& item : actions_)
        if (item.is_gate_blocker()) result.push_back(&item);
    return result;
}

auto ActionReadinessGate::all_surfaces() const -> std::vector<ActionSurfaceKind>
{
    std::set<ActionSurfaceKind> surfaces;
    for (const auto& item : actions_)
        surfaces.insert(item.surface);
    return {surfaces.begin(), surfaces.end()};
}

// ── Panel Queries ──

auto ActionReadinessGate::panel_count() const noexcept -> std::size_t
{
    return panels_.size();
}

auto ActionReadinessGate::ready_panels() const
    -> std::vector<const PanelReadinessItem*>
{
    std::vector<const PanelReadinessItem*> result;
    for (const auto& item : panels_)
        if (item.has_factory && !item.is_placeholder) result.push_back(&item);
    return result;
}

auto ActionReadinessGate::placeholder_panels() const
    -> std::vector<const PanelReadinessItem*>
{
    std::vector<const PanelReadinessItem*> result;
    for (const auto& item : panels_)
        if (item.is_placeholder) result.push_back(&item);
    return result;
}

auto ActionReadinessGate::blocking_panels() const
    -> std::vector<const PanelReadinessItem*>
{
    std::vector<const PanelReadinessItem*> result;
    for (const auto& item : panels_)
        if (item.is_gate_blocker()) result.push_back(&item);
    return result;
}

// ── Gate Check ──

auto ActionReadinessGate::check_gate() const -> ActionGateResult
{
    ActionGateResult result;
    result.total_actions = actions_.size();
    result.total_panels = panels_.size();

    for (const auto& action : actions_)
    {
        switch (action.status)
        {
        case ActionBindingStatus::kLive:     ++result.live_actions; break;
        case ActionBindingStatus::kStub:     ++result.stub_actions; break;
        case ActionBindingStatus::kDead:     ++result.dead_actions; break;
        case ActionBindingStatus::kGated:    ++result.gated_actions; break;
        case ActionBindingStatus::kOrphaned: ++result.orphaned_actions; break;
        }

        if (action.is_gate_blocker())
        {
            result.blocking_reasons.push_back(
                "Action " + action.action_id + " on " +
                action_surface_label(action.surface) + ": " +
                binding_status_label(action.status));
        }
    }

    for (const auto& panel : panels_)
    {
        if (panel.is_gated)
            ++result.gated_panels;
        else if (panel.has_factory && !panel.is_placeholder)
            ++result.ready_panels;

        if (panel.is_placeholder)
            ++result.placeholder_panels;

        if (panel.is_gate_blocker())
        {
            result.blocking_reasons.push_back(
                "Panel " + panel.panel_id + ": " +
                (panel.is_placeholder ? "placeholder" : "missing factory"));
        }
    }

    result.passes = result.blocking_reasons.empty();
    return result;
}

// ── Clear ──

void ActionReadinessGate::clear()
{
    actions_.clear();
    panels_.clear();
}

// ── Export ──

auto ActionReadinessGate::export_json() const -> std::string
{
    auto gate = check_gate();
    std::ostringstream ss;
    ss << "{\n"
       << "  \"passes\": " << (gate.passes ? "true" : "false") << ",\n"
       << "  \"actions\": {\n"
       << "    \"total\": " << gate.total_actions << ",\n"
       << "    \"live\": " << gate.live_actions << ",\n"
       << "    \"stub\": " << gate.stub_actions << ",\n"
       << "    \"dead\": " << gate.dead_actions << ",\n"
       << "    \"gated\": " << gate.gated_actions << ",\n"
       << "    \"orphaned\": " << gate.orphaned_actions << "\n"
       << "  },\n"
       << "  \"panels\": {\n"
       << "    \"total\": " << gate.total_panels << ",\n"
       << "    \"ready\": " << gate.ready_panels << ",\n"
       << "    \"placeholder\": " << gate.placeholder_panels << ",\n"
       << "    \"gated\": " << gate.gated_panels << "\n"
       << "  },\n"
       << "  \"blocking_reasons\": [\n";
    for (std::size_t i = 0; i < gate.blocking_reasons.size(); ++i)
    {
        ss << "    \"" << gate.blocking_reasons[i] << "\"";
        if (i + 1 < gate.blocking_reasons.size()) ss << ",";
        ss << "\n";
    }
    ss << "  ]\n}\n";
    return ss.str();
}

auto ActionReadinessGate::export_markdown() const -> std::string
{
    auto gate = check_gate();
    std::ostringstream ss;
    ss << "# V23 Action & Panel Readiness Gate\n\n"
       << "**Result:** " << (gate.passes ? "✅ PASS" : "❌ FAIL") << "\n\n"
       << "## Actions\n\n"
       << "| Metric | Count |\n|---|---|\n"
       << "| Live | " << gate.live_actions << " |\n"
       << "| Stub | " << gate.stub_actions << " |\n"
       << "| Dead | " << gate.dead_actions << " |\n"
       << "| Gated | " << gate.gated_actions << " |\n"
       << "| Orphaned | " << gate.orphaned_actions << " |\n\n"
       << "## Panels\n\n"
       << "| Metric | Count |\n|---|---|\n"
       << "| Ready | " << gate.ready_panels << " |\n"
       << "| Placeholder | " << gate.placeholder_panels << " |\n"
       << "| Gated | " << gate.gated_panels << " |\n";

    if (!gate.blocking_reasons.empty())
    {
        ss << "\n## Blocking Reasons\n\n";
        for (const auto& reason : gate.blocking_reasons)
            ss << "- " << reason << "\n";
    }

    return ss.str();
}

} // namespace markamp::core
