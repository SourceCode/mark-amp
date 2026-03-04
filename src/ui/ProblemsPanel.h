#pragma once

/// @file ProblemsPanel.h
/// @brief Problems Panel V2 — file-grouped diagnostic tree with severity filters.
///
/// Features:
///   - File-grouped tree model (ProblemsTreeModel)
///   - Severity icons (⊘/⚠/ℹ/💡) and filter toggles
///   - Source filter, text search, sort controls (severity/file)
///   - Collapse/expand all, keyboard navigation
///   - Quick fix API, autofix all, file decorations for FileTreeCtrl
///   - Editor gutter marker integration
///   - Navigate-to-problem (double-click), context menu
///   - Batch diagnostic operations (clear by source/severity)
///   - Related information links on diagnostics
///
/// @see DiagnosticsService, ProblemsTreeModel, ProblemFileNode

#include "core/DiagnosticsService.h"
#include "ui/ProblemsTreeModel.h"

#include <wx/button.h>
#include <wx/clipbrd.h>
#include <wx/listctrl.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/srchctrl.h>
#include <wx/stattext.h>

#include <string>
#include <vector>

namespace markamp::ui
{

class PanelContainer;

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

/// A quick-fix action for a diagnostic.
struct QuickFixAction
{
    std::string title;
    std::string command_id;
    core::DiagnosticRange range;
};

/// A gutter marker for editor integration.
struct GutterMarker
{
    int line{0};
    core::DiagnosticSeverity severity{core::DiagnosticSeverity::kError};
    std::string tooltip;
};

/// Problems Panel V2 — file-grouped tree view with severity filters.
/// Features: severity filter toggles, source filter, text search, sort controls,
/// collapse/expand, click-to-navigate, ProblemsTreeModel backing.
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

    // ── V2 API ──

    /// Sort mode.
    void set_sort_mode(ProblemSortMode mode);
    [[nodiscard]] auto sort_mode() const -> ProblemSortMode;

    /// Text search filter.
    void set_text_filter(const std::string& text);
    [[nodiscard]] auto text_filter() const -> const std::string&;

    /// Source filter.
    void set_source_filter(const std::string& source);
    [[nodiscard]] auto source_filter() const -> const std::string&;

    /// Collapse/expand controls.
    void collapse_all();
    void expand_all();

    /// Access the tree model.
    [[nodiscard]] auto tree_model() -> ProblemsTreeModel&;
    [[nodiscard]] auto tree_model() const -> const ProblemsTreeModel&;

    /// Refresh displayed content from the service.
    void RefreshContent();

    /// Apply theme colors.
    void ApplyTheme(const wxColour& bg_colour, const wxColour& fg_colour);

    // ── Quick Fix (Phase 23 Task 7) ──

    /// Get available quick fixes for a diagnostic at the given index.
    [[nodiscard]] auto available_quick_fixes(std::size_t diag_index) const
        -> std::vector<QuickFixAction>;

    /// Apply a quick fix action.
    void apply_quick_fix(const QuickFixAction& action);

    // ── Autofix All (Phase 23 Task 15) ──

    /// Apply all available autofixes.
    void autofix_all();

    // ── File Decorations (Phase 23 Task 10) ──

    /// Get file decoration severity for a URI (for FileTreeCtrl badges).
    [[nodiscard]] auto file_decoration_for_uri(const std::string& uri) const
        -> core::DiagnosticSeverity;

    // ── Editor Gutter Integration (Phase 23 Task 21) ──

    /// Get gutter markers for a file URI.
    [[nodiscard]] auto gutter_markers_for_uri(const std::string& uri) const
        -> std::vector<GutterMarker>;

private:
    void CreateLayout(wxWindow* parent);
    void RenderTreeToList();
    void OnContextMenu(wxContextMenuEvent& event);
    void OnKeyDown(wxListEvent& event);
    void NavigateToSelectedProblem();
    void CopySelectedToClipboard();

    core::DiagnosticsService* service_{nullptr};
    core::DiagnosticSeverity filter_{core::DiagnosticSeverity::kHint};
    ProblemsTreeModel tree_model_;

    // UI controls (null in data-only / test mode)
    wxWindow* toolbar_{nullptr};
    wxListCtrl* list_ctrl_{nullptr};
    wxStaticText* summary_label_{nullptr};
    wxSearchCtrl* search_ctrl_{nullptr};
};

} // namespace markamp::ui
