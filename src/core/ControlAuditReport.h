/// @file ControlAuditReport.h
/// @brief V21 Phase 01 — Control audit report generator.
///
/// Generates structured control status matrices from the canonical action
/// manifest and execution traces. Produces CI-compatible JSON and
/// human-readable markdown output for release gating.
#pragma once

#include "ControlActionManifest.h"
#include "ControlExecutionTracer.h"

#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// ControlAuditEntry — single row in the audit matrix
// ============================================================================

/// One row in the control audit report.
struct ControlAuditEntry
{
    std::string action_id;
    std::string label;
    std::string category;
    ActionValidationStatus validation_status{ActionValidationStatus::kDead};
    bool has_handler{false};
    bool has_enablement{false};
    bool has_visibility{false};
    int activation_count{0};
    int success_count{0};
    int failure_count{0};
    std::vector<ControlSurface> surfaces;
};

// ============================================================================
// ControlAuditSummary — aggregate health metrics
// ============================================================================

/// Aggregate health summary for the audit report.
struct ControlAuditSummary
{
    int total_actions{0};
    int live_actions{0};
    int partial_actions{0};
    int stub_actions{0};
    int dead_actions{0};
    int gated_actions{0};
    int deprecated_actions{0};
    int duplicate_actions{0};
    int total_activations{0};
    int total_failures{0};
    int never_activated_count{0};

    /// Overall health percentage: live / total * 100.
    [[nodiscard]] auto health_pct() const noexcept -> int
    {
        if (total_actions == 0)
        {
            return 100;
        }
        return (live_actions * 100) / total_actions;
    }

    /// Whether the audit passes V21 exit criteria (100% live or gated).
    [[nodiscard]] auto passes_exit_criteria() const noexcept -> bool
    {
        return dead_actions == 0 && stub_actions == 0 && partial_actions == 0;
    }
};

// ============================================================================
// ControlAuditReport — generator
// ============================================================================

/// Generates control audit reports from a manifest and optional tracer.
class ControlAuditReport
{
public:
    ControlAuditReport() = default;

    /// Generate audit entries from a manifest and optional tracer.
    [[nodiscard]] auto generate(const ControlActionManifest& manifest,
                                const ControlExecutionTracer* tracer = nullptr) const
        -> std::vector<ControlAuditEntry>;

    /// Generate aggregate summary from audit entries.
    [[nodiscard]] auto summarize(const std::vector<ControlAuditEntry>& entries) const
        -> ControlAuditSummary;

    /// Generate a one-shot summary from manifest + optional tracer.
    [[nodiscard]] auto summarize(const ControlActionManifest& manifest,
                                 const ControlExecutionTracer* tracer = nullptr) const
        -> ControlAuditSummary;

    /// Export audit entries as JSON string.
    [[nodiscard]] auto export_json(const std::vector<ControlAuditEntry>& entries) const
        -> std::string;

    /// Export audit entries as Markdown table.
    [[nodiscard]] auto export_markdown(const std::vector<ControlAuditEntry>& entries,
                                       const ControlAuditSummary& summary) const
        -> std::string;
};

} // namespace markamp::core
