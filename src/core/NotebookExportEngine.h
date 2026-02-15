/// @file NotebookExportEngine.h
/// @brief V4 Phase 37 – Notebook Export Engine.

#pragma once

#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;

// ============================================================================
// Data structures
// ============================================================================

/// Export format enumeration.
enum class ExportFormat
{
    kHtml,
    kMarkdown,
    kLatex,
    kSlides,
    kPdf
};

/// Cell data for export (mirrors NotebookCell but decoupled).
struct ExportCell
{
    std::string cell_id;
    std::string cell_type; ///< "code" or "markdown"
    std::string source;
    std::string output_text;
    std::string output_html;
    int execution_count{0};
};

/// Export options.
struct ExportOptions
{
    bool include_outputs{true};
    bool include_execution_counts{true};
    bool include_toc{false};
    std::string title;
    std::string author;
    std::string css_theme; ///< Optional CSS for HTML export.
};

/// Export result.
struct ExportResult
{
    bool success{false};
    std::string content;
    ExportFormat format{ExportFormat::kHtml};
    std::string error_message;
};

/// Table of contents entry.
struct TocEntry
{
    std::string heading;
    int level{1};
    std::string anchor;
};

// ============================================================================
// NotebookExportEngine
// ============================================================================

class NotebookExportEngine
{
public:
    explicit NotebookExportEngine(EventBus& event_bus);

    /// Export cells to a specific format.
    [[nodiscard]] auto export_notebook(const std::vector<ExportCell>& cells,
                                       ExportFormat format,
                                       const ExportOptions& options = {}) -> ExportResult;

    /// Export to HTML.
    [[nodiscard]] auto export_to_html(const std::vector<ExportCell>& cells,
                                      const ExportOptions& options = {}) -> std::string;

    /// Export to Markdown.
    [[nodiscard]] auto export_to_markdown(const std::vector<ExportCell>& cells,
                                          const ExportOptions& options = {}) -> std::string;

    /// Export to LaTeX.
    [[nodiscard]] auto export_to_latex(const std::vector<ExportCell>& cells,
                                       const ExportOptions& options = {}) -> std::string;

    /// Export to HTML slides (heading-based slide breaks).
    [[nodiscard]] auto export_to_slides(const std::vector<ExportCell>& cells,
                                        const ExportOptions& options = {}) -> std::string;

    /// Generate table of contents from markdown headings.
    [[nodiscard]] auto generate_toc(const std::vector<ExportCell>& cells) const
        -> std::vector<TocEntry>;

    /// Return cells with outputs removed.
    [[nodiscard]] auto strip_outputs(const std::vector<ExportCell>& cells) const
        -> std::vector<ExportCell>;

private:
    EventBus& event_bus_;

    /// Escape HTML entities.
    [[nodiscard]] static auto escape_html(const std::string& text) -> std::string;

    /// Escape LaTeX special characters.
    [[nodiscard]] static auto escape_latex(const std::string& text) -> std::string;

    /// Generate a slug from a heading for use as an anchor.
    [[nodiscard]] static auto slugify(const std::string& text) -> std::string;
};

} // namespace markamp::core
