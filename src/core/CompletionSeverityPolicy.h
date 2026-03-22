/// @file CompletionSeverityPolicy.h
/// @brief V23 Phase 01 — Severity scoring model and release-gate policy.
///
/// Defines blocker rules, waiver records, and a release gate that integrates
/// with the CompletionInventory.  Used by CI and the ReleaseGateChecker to
/// determine whether unfinished work is blocking a release.
#pragma once

#include "core/CompletionInventory.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{

// ============================================================================
// SeverityWaiver — explicit override for a known-incomplete item
// ============================================================================

struct SeverityWaiver
{
    std::string item_id;        ///< Unique identifier (file:line or feature name)
    std::string reason;         ///< Why this is waived
    std::string owner;          ///< Who owns the waiver
    CompletionSeverity original_severity{CompletionSeverity::kHigh};
    CompletionSeverity waived_severity{CompletionSeverity::kLow};
    bool is_permanent{false};   ///< True for genuinely hidden/experimental surfaces

    [[nodiscard]] auto makes_item_id() const -> std::string
    {
        return item_id;
    }
};

// ============================================================================
// GateResult — result of running the release gate
// ============================================================================

struct GateResult
{
    bool passes{false};
    std::size_t total_items{0};
    std::size_t blockers{0};
    std::size_t waived{0};
    std::size_t effective_blockers{0};  ///< blockers - waived
    std::vector<std::string> blocking_reasons;

    [[nodiscard]] auto is_clear() const noexcept -> bool
    {
        return passes && effective_blockers == 0;
    }
};

// ============================================================================
// CompletionSeverityPolicy — the policy engine
// ============================================================================

class CompletionSeverityPolicy
{
public:
    CompletionSeverityPolicy() = default;

    // ── Waiver Management ──

    void add_waiver(SeverityWaiver waiver);
    [[nodiscard]] auto waiver_count() const noexcept -> std::size_t;
    [[nodiscard]] auto has_waiver(std::string_view item_id) const -> bool;
    [[nodiscard]] auto get_waiver(std::string_view item_id) const
        -> const SeverityWaiver*;
    [[nodiscard]] auto all_waivers() const -> std::vector<const SeverityWaiver*>;
    void clear_waivers();

    // ── Severity Classification ──

    /// Apply the standard severity rules to an inventory item.
    /// Returns the effective severity after considering waivers.
    [[nodiscard]] auto effective_severity(const InventoryItem& item) const
        -> CompletionSeverity;

    /// Check whether a specific item is a release blocker after waivers.
    [[nodiscard]] auto is_effective_blocker(const InventoryItem& item) const -> bool;

    // ── Release Gate ──

    /// Run the release gate against an entire inventory.
    [[nodiscard]] auto check_release_gate(const CompletionInventory& inventory) const
        -> GateResult;

    // ── Export ──

    [[nodiscard]] auto export_gate_json(const GateResult& result) const -> std::string;
    [[nodiscard]] auto export_gate_markdown(const GateResult& result) const -> std::string;

private:
    std::vector<SeverityWaiver> waivers_;

    /// Build the waiver key for an inventory item for lookup.
    [[nodiscard]] static auto waiver_key_for(const InventoryItem& item) -> std::string;
};

} // namespace markamp::core
