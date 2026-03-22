/// @file ControlCompletenessMatrix.cpp
/// @brief V21 Phase 10 — ControlCompletenessMatrix & PanelReadinessGate implementation.

#include "ControlCompletenessMatrix.h"
#include <sstream>

namespace markamp::core
{

// ── ControlCompletenessMatrix ──

void ControlCompletenessMatrix::add_row(MatrixRow row) { rows_.push_back(std::move(row)); }

void ControlCompletenessMatrix::populate_from_manifest(const ControlActionManifest& manifest) {
    for (const auto* action : manifest.all_actions()) {
        MatrixRow row;
        row.control_id = action->action_id;
        row.label = action->label;
        row.has_handler = action->has_handler();
        row.has_enablement = true;

        switch (action->validation_status) {
        case ActionValidationStatus::kLive: row.validation_status = "Live"; break;
        case ActionValidationStatus::kStub: row.validation_status = "Stub"; break;
        case ActionValidationStatus::kDead: row.validation_status = "Dead"; break;
        case ActionValidationStatus::kGated: row.validation_status = "Gated"; break;
        default: row.validation_status = "Unknown"; break;
        }
        rows_.push_back(std::move(row));
    }
}

auto ControlCompletenessMatrix::all_rows() const -> std::vector<const MatrixRow*> {
    std::vector<const MatrixRow*> result;
    for (const auto& r : rows_) result.push_back(&r);
    return result;
}

auto ControlCompletenessMatrix::rows_for_surface(const std::string& surface) const
    -> std::vector<const MatrixRow*> {
    std::vector<const MatrixRow*> result;
    for (const auto& r : rows_) if (r.surface == surface) result.push_back(&r);
    return result;
}

auto ControlCompletenessMatrix::dead_rows() const -> std::vector<const MatrixRow*> {
    std::vector<const MatrixRow*> result;
    for (const auto& r : rows_) if (r.validation_status == "Dead") result.push_back(&r);
    return result;
}

auto ControlCompletenessMatrix::stub_rows() const -> std::vector<const MatrixRow*> {
    std::vector<const MatrixRow*> result;
    for (const auto& r : rows_) if (r.validation_status == "Stub") result.push_back(&r);
    return result;
}

auto ControlCompletenessMatrix::summarize() const -> MatrixSummary {
    MatrixSummary s;
    s.total_controls = rows_.size();
    for (const auto& r : rows_) {
        if (r.validation_status == "Live") ++s.live;
        else if (r.validation_status == "Stub") ++s.stub;
        else if (r.validation_status == "Dead") ++s.dead;
        else if (r.validation_status == "Gated") ++s.gated;
    }
    return s;
}

auto ControlCompletenessMatrix::to_markdown() const -> std::string {
    std::ostringstream ss;
    ss << "# Control Completeness Matrix\n\n";
    auto summary = summarize();
    ss << "**Total:** " << summary.total_controls
       << " | **Live:** " << summary.live
       << " | **Stub:** " << summary.stub
       << " | **Dead:** " << summary.dead
       << " | **Gated:** " << summary.gated
       << " | **Pass Rate:** " << summary.pass_rate() << "%\n\n";
    ss << "| Control ID | Label | Handler | Status |\n";
    ss << "|---|---|---|---|\n";
    for (const auto& r : rows_) {
        ss << "| " << r.control_id << " | " << r.label
           << " | " << (r.has_handler ? "✅" : "❌")
           << " | " << r.validation_status << " |\n";
    }
    return ss.str();
}

auto ControlCompletenessMatrix::row_count() const -> std::size_t { return rows_.size(); }

// ── PanelReadinessGate ──

void PanelReadinessGate::add_panel(const std::string& panel_id, PanelReadiness readiness) {
    entries_.push_back({.panel_id = panel_id, .readiness = readiness});
}

auto PanelReadinessGate::generate_report() const -> PanelReadinessReport {
    PanelReadinessReport report;
    report.total_panels = entries_.size();
    for (const auto& e : entries_) {
        switch (e.readiness) {
        case PanelReadiness::kReady: ++report.ready; break;
        case PanelReadiness::kPlaceholder: ++report.placeholder; break;
        case PanelReadiness::kExperimental: ++report.experimental; break;
        case PanelReadiness::kDeprecated: ++report.deprecated; break;
        }
    }
    return report;
}

auto PanelReadinessGate::panel_count() const -> std::size_t { return entries_.size(); }

} // namespace markamp::core
