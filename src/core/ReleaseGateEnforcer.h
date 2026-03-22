/// @file ReleaseGateEnforcer.h
/// @brief V25 P01-T02: Aggregated release-gate enforcement.
///
/// Combines ActionReadinessGate, CompletionInventory, and subsystem
/// signoff into a single blocking release verdict.
#pragma once

#include "ActionReadinessGate.h"
#include "CompletionInventory.h"
#include "SubsystemDoneCriteria.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Result of a full release-gate enforcement check.
struct ReleaseGateVerdict
{
    bool passes{false};
    int dead_action_count{0};
    int placeholder_panel_count{0};
    int critical_blocker_count{0};
    int subsystems_blocked{0};
    int subsystems_signed_off{0};
    std::vector<std::string> blocking_reasons;

    [[nodiscard]] auto is_clear() const noexcept -> bool
    {
        return blocking_reasons.empty();
    }

    [[nodiscard]] auto total_blockers() const noexcept -> int
    {
        return dead_action_count + placeholder_panel_count + critical_blocker_count;
    }
};

/// Aggregates all release-gate sources into one verdict.
class ReleaseGateEnforcer
{
public:
    ReleaseGateEnforcer() = default;

    /// Run a full gate check from the provided sources.
    [[nodiscard]] auto enforce(const ActionReadinessGate& action_gate,
                               const CompletionInventory& inventory,
                               const SubsystemDoneCriteria& criteria) const
        -> ReleaseGateVerdict;

    /// Quick check: does it pass?
    [[nodiscard]] auto passes(const ActionReadinessGate& action_gate,
                              const CompletionInventory& inventory,
                              const SubsystemDoneCriteria& criteria) const -> bool;

    /// Export verdict as Markdown.
    [[nodiscard]] static auto export_markdown(const ReleaseGateVerdict& verdict) -> std::string;
};

} // namespace markamp::core
