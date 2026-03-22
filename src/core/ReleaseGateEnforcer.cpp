/// @file ReleaseGateEnforcer.cpp
/// @brief V25 P01-T02: Release-gate enforcement implementation.
#include "core/ReleaseGateEnforcer.h"

#include <sstream>

namespace markamp::core
{

auto ReleaseGateEnforcer::enforce(const ActionReadinessGate& action_gate,
                                  const CompletionInventory& inventory,
                                  const SubsystemDoneCriteria& criteria) const
    -> ReleaseGateVerdict
{
    ReleaseGateVerdict verdict;

    // Action gate
    auto ag = action_gate.check_gate();
    verdict.dead_action_count = static_cast<int>(ag.dead_actions + ag.stub_actions + ag.orphaned_actions);
    verdict.placeholder_panel_count = static_cast<int>(ag.placeholder_panels);

    if (verdict.dead_action_count > 0) {
        verdict.blocking_reasons.push_back(
            "Dead/stub/orphaned actions: " + std::to_string(verdict.dead_action_count));
    }
    if (verdict.placeholder_panel_count > 0) {
        verdict.blocking_reasons.push_back(
            "Placeholder panels: " + std::to_string(verdict.placeholder_panel_count));
    }

    // Completion inventory
    auto summary = inventory.summary();
    verdict.critical_blocker_count = static_cast<int>(summary.blockers);
    if (verdict.critical_blocker_count > 0) {
        verdict.blocking_reasons.push_back(
            "Critical production-path blockers: " + std::to_string(verdict.critical_blocker_count));
    }

    // Subsystem criteria
    auto report = criteria.evaluate_all();
    verdict.subsystems_signed_off = report.signed_off;
    verdict.subsystems_blocked = report.blocked;
    if (report.blocked > 0) {
        verdict.blocking_reasons.push_back(
            "Blocked subsystems: " + std::to_string(report.blocked));
    }

    verdict.passes = verdict.blocking_reasons.empty();
    return verdict;
}

auto ReleaseGateEnforcer::passes(const ActionReadinessGate& action_gate,
                                 const CompletionInventory& inventory,
                                 const SubsystemDoneCriteria& criteria) const -> bool
{
    return enforce(action_gate, inventory, criteria).passes;
}

auto ReleaseGateEnforcer::export_markdown(const ReleaseGateVerdict& verdict) -> std::string
{
    std::ostringstream ss;
    ss << "# Release Gate Verdict\n\n";
    ss << "**Status:** " << (verdict.passes ? "PASS" : "BLOCKED") << "\n\n";
    ss << "| Metric | Count |\n|--------|-------|\n";
    ss << "| Dead/Stub Actions | " << verdict.dead_action_count << " |\n";
    ss << "| Placeholder Panels | " << verdict.placeholder_panel_count << " |\n";
    ss << "| Critical Blockers | " << verdict.critical_blocker_count << " |\n";
    ss << "| Subsystems Signed Off | " << verdict.subsystems_signed_off << " |\n";
    ss << "| Subsystems Blocked | " << verdict.subsystems_blocked << " |\n\n";

    if (!verdict.blocking_reasons.empty()) {
        ss << "## Blocking Reasons\n\n";
        for (const auto& reason : verdict.blocking_reasons) {
            ss << "- " << reason << "\n";
        }
    }
    return ss.str();
}

} // namespace markamp::core
