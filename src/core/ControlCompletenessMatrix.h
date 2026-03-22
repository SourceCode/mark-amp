/// @file ControlCompletenessMatrix.h
/// @brief V21 Phase 10 — Release-gating control and handler completeness matrix.
///
/// Generates a structured report listing each control, its canonical action ID,
/// handler status, enablement source, and validation status.
/// Supports JSON and markdown output for CI artifacts.
#pragma once

#include "core/ControlActionManifest.h"
#include "core/PanelLifecycleAuditor.h"
#include "core/SettingsArchitectureAuditor.h"

#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// MatrixRow — one row in the completeness matrix
// ============================================================================

struct MatrixRow
{
    std::string control_id;
    std::string label;
    std::string surface;       ///< Where the control lives (menu, toolbar, panel, etc.)
    bool has_handler{false};
    bool has_enablement{false};
    std::string validation_status; ///< "Live", "Stub", "Dead", "Gated"
    std::string notes;
};

// ============================================================================
// MatrixSummary — aggregated stats
// ============================================================================

struct MatrixSummary
{
    std::size_t total_controls{0};
    std::size_t live{0};
    std::size_t stub{0};
    std::size_t dead{0};
    std::size_t gated{0};

    [[nodiscard]] auto pass_rate() const -> double
    {
        return total_controls > 0 ?
            static_cast<double>(live) / static_cast<double>(total_controls) * 100.0 : 0.0;
    }

    [[nodiscard]] auto passes_release_gate(double threshold = 95.0) const -> bool
    {
        return pass_rate() >= threshold && dead == 0;
    }
};

// ============================================================================
// ControlCompletenessMatrix — the matrix engine
// ============================================================================

class ControlCompletenessMatrix
{
public:
    ControlCompletenessMatrix() = default;

    // ── Build Matrix ──

    void add_row(MatrixRow row);

    /// Populate from a ControlActionManifest.
    void populate_from_manifest(const ControlActionManifest& manifest);

    // ── Query ──

    [[nodiscard]] auto all_rows() const -> std::vector<const MatrixRow*>;
    [[nodiscard]] auto rows_for_surface(const std::string& surface) const
        -> std::vector<const MatrixRow*>;
    [[nodiscard]] auto dead_rows() const -> std::vector<const MatrixRow*>;
    [[nodiscard]] auto stub_rows() const -> std::vector<const MatrixRow*>;

    // ── Summary ──

    [[nodiscard]] auto summarize() const -> MatrixSummary;

    // ── Output ──

    [[nodiscard]] auto to_markdown() const -> std::string;
    [[nodiscard]] auto row_count() const -> std::size_t;

private:
    std::vector<MatrixRow> rows_;
};

// ============================================================================
// PanelReadinessGate — release gate for panel completeness
// ============================================================================

struct PanelReadinessReport
{
    std::size_t total_panels{0};
    std::size_t ready{0};
    std::size_t placeholder{0};
    std::size_t experimental{0};
    std::size_t deprecated{0};

    [[nodiscard]] auto passes_gate() const -> bool
    {
        return placeholder == 0 && deprecated == 0;
    }
};

/// Generates a readiness gate report from panel lifecycle auditor.
class PanelReadinessGate
{
public:
    PanelReadinessGate() = default;

    void add_panel(const std::string& panel_id, PanelReadiness readiness);

    [[nodiscard]] auto generate_report() const -> PanelReadinessReport;
    [[nodiscard]] auto panel_count() const -> std::size_t;

private:
    struct Entry { std::string panel_id; PanelReadiness readiness; };
    std::vector<Entry> entries_;
};

} // namespace markamp::core
