#pragma once

#include "core/DiagnosticsService.h"

#include <algorithm>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Sort mode for the problems tree.
enum class ProblemSortMode
{
    kSeverity,
    kFile,
    kPosition,
};

/// A file node in the problems tree — groups diagnostics by file URI.
struct ProblemFileNode
{
    std::string file_uri;
    std::string display_name; ///< Basename for display
    std::vector<core::Diagnostic> diagnostics;
    bool collapsed{false};

    [[nodiscard]] auto error_count() const -> std::size_t
    {
        return static_cast<std::size_t>(
            std::count_if(diagnostics.begin(),
                          diagnostics.end(),
                          [](const core::Diagnostic& diag)
                          { return diag.severity == core::DiagnosticSeverity::kError; }));
    }

    [[nodiscard]] auto warning_count() const -> std::size_t
    {
        return static_cast<std::size_t>(
            std::count_if(diagnostics.begin(),
                          diagnostics.end(),
                          [](const core::Diagnostic& diag)
                          { return diag.severity == core::DiagnosticSeverity::kWarning; }));
    }

    [[nodiscard]] auto info_count() const -> std::size_t
    {
        return static_cast<std::size_t>(
            std::count_if(diagnostics.begin(),
                          diagnostics.end(),
                          [](const core::Diagnostic& diag)
                          { return diag.severity == core::DiagnosticSeverity::kInformation; }));
    }

    [[nodiscard]] auto max_severity() const -> core::DiagnosticSeverity
    {
        auto sev = core::DiagnosticSeverity::kHint;
        for (const auto& diag : diagnostics)
        {
            if (diag.severity < sev) // Lower numeric = higher severity
            {
                sev = diag.severity;
            }
        }
        return sev;
    }
};

/// Tree model for the Problems Panel V2.
/// Transforms flat diagnostics from DiagnosticsService into a navigable,
/// filterable, sortable tree grouped by file.
class ProblemsTreeModel
{
public:
    ProblemsTreeModel() = default;

    /// Rebuild the tree from the diagnostics service.
    void rebuild(const core::DiagnosticsService& service);

    /// Get all file nodes (after filtering and sorting).
    [[nodiscard]] auto file_nodes() const -> const std::vector<ProblemFileNode>&;

    /// Total counts across all files.
    [[nodiscard]] auto total_error_count() const -> std::size_t;
    [[nodiscard]] auto total_warning_count() const -> std::size_t;
    [[nodiscard]] auto total_info_count() const -> std::size_t;
    [[nodiscard]] auto total_diagnostic_count() const -> std::size_t;

    // ── Filtering ──

    /// Set minimum severity filter.
    void set_severity_filter(core::DiagnosticSeverity min_severity);
    [[nodiscard]] auto severity_filter() const -> core::DiagnosticSeverity;

    /// Set source filter (e.g., "gcc", "clang").
    void set_source_filter(const std::string& source);
    [[nodiscard]] auto source_filter() const -> const std::string&;

    /// Set text search filter.
    void set_text_filter(const std::string& text);
    [[nodiscard]] auto text_filter() const -> const std::string&;

    // ── Sorting ──

    void set_sort_mode(ProblemSortMode mode);
    [[nodiscard]] auto sort_mode() const -> ProblemSortMode;

    // ── Collapse/Expand ──

    void collapse_all();
    void expand_all();
    void toggle_file(const std::string& file_uri);

private:
    std::vector<ProblemFileNode> nodes_;
    std::vector<ProblemFileNode> filtered_nodes_;
    core::DiagnosticSeverity severity_filter_{core::DiagnosticSeverity::kHint};
    std::string source_filter_;
    std::string text_filter_;
    ProblemSortMode sort_mode_{ProblemSortMode::kSeverity};

    void apply_filters();
    void apply_sort();

    [[nodiscard]] static auto extract_basename(const std::string& uri) -> std::string;
};

} // namespace markamp::ui
