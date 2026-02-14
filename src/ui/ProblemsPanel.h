#pragma once

#include "core/DiagnosticsService.h"

#include <wx/listctrl.h>
#include <wx/panel.h>
#include <wx/stattext.h>

#include <string>
#include <vector>

namespace markamp::ui
{

/// Display item in the problems panel.
struct ProblemItem
{
    std::string file_uri;
    std::string message;
    core::DiagnosticSeverity severity;
    int line{0};
    int character{0};
    std::string source;
};

/// Diagnostics list panel with severity icons (#45).
/// Displays file path, line number, message, filterable by severity.
/// Supports click-to-navigate. Mirrors VS Code's "Problems" panel.
class ProblemsPanel : public wxPanel
{
public:
    /// Data-only constructor (for tests).
    ProblemsPanel();

    /// UI constructor with rendering support.
    ProblemsPanel(wxWindow* parent, core::DiagnosticsService* service);

    /// Set the diagnostics service to read from.
    void set_service(core::DiagnosticsService* service);

    /// Get all problems, optionally filtered by minimum severity.
    [[nodiscard]] auto
    problems(core::DiagnosticSeverity min_severity = core::DiagnosticSeverity::kHint) const
        -> std::vector<ProblemItem>;

    /// Count problems by severity.
    [[nodiscard]] auto error_count() const -> std::size_t;
    [[nodiscard]] auto warning_count() const -> std::size_t;
    [[nodiscard]] auto info_count() const -> std::size_t;

    /// Filter controls.
    void set_severity_filter(core::DiagnosticSeverity min_severity);
    [[nodiscard]] auto severity_filter() const -> core::DiagnosticSeverity;

    /// Refresh displayed content from the service.
    void RefreshContent();

    /// Apply theme colors.
    void ApplyTheme(const wxColour& bg_colour, const wxColour& fg_colour);

private:
    void CreateLayout();

    core::DiagnosticsService* service_{nullptr};
    core::DiagnosticSeverity filter_{core::DiagnosticSeverity::kHint};

    // UI controls (null in data-only / test mode)
    wxListCtrl* list_ctrl_{nullptr};
    wxStaticText* summary_label_{nullptr};
};

} // namespace markamp::ui
