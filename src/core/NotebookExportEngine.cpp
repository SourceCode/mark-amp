/// @file NotebookExportEngine.cpp
/// @brief V4 Phase 37 – Notebook Export Engine implementation.

#include "core/NotebookExportEngine.h"

#include "core/EventBus.h"
#include "core/Events.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// Constructor
// ============================================================================

NotebookExportEngine::NotebookExportEngine(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ============================================================================
// Dispatcher
// ============================================================================

auto NotebookExportEngine::export_notebook(const std::vector<ExportCell>& cells,
                                           ExportFormat format,
                                           const ExportOptions& options) -> ExportResult
{
    events::NotebookExportStartedEvent start_event;
    start_event.cell_count = static_cast<int>(cells.size());
    event_bus_.publish(start_event);

    ExportResult result;
    result.format = format;

    switch (format)
    {
        case ExportFormat::kHtml:
            result.content = export_to_html(cells, options);
            break;
        case ExportFormat::kMarkdown:
            result.content = export_to_markdown(cells, options);
            break;
        case ExportFormat::kLatex:
            result.content = export_to_latex(cells, options);
            break;
        case ExportFormat::kSlides:
            result.content = export_to_slides(cells, options);
            break;
        case ExportFormat::kPdf:
            result.error_message = "PDF export requires external renderer";
            result.success = false;
            return result;
    }

    result.success = true;

    events::NotebookExportCompletedEvent done_event;
    done_event.format = "html";
    switch (format)
    {
        case ExportFormat::kMarkdown:
            done_event.format = "markdown";
            break;
        case ExportFormat::kLatex:
            done_event.format = "latex";
            break;
        case ExportFormat::kSlides:
            done_event.format = "slides";
            break;
        default:
            break;
    }
    done_event.success = true;
    event_bus_.publish(done_event);

    return result;
}

// ============================================================================
// HTML export
// ============================================================================

auto NotebookExportEngine::export_to_html(const std::vector<ExportCell>& cells,
                                          const ExportOptions& options) -> std::string
{
    std::ostringstream oss;

    oss << "<!DOCTYPE html>\n<html>\n<head>\n";
    oss << "<meta charset=\"utf-8\">\n";

    if (!options.title.empty())
    {
        oss << "<title>" << escape_html(options.title) << "</title>\n";
    }
    else
    {
        oss << "<title>Notebook Export</title>\n";
    }

    if (!options.css_theme.empty())
    {
        oss << "<style>" << options.css_theme << "</style>\n";
    }

    oss << "</head>\n<body>\n";

    if (options.include_toc)
    {
        auto toc = generate_toc(cells);
        if (!toc.empty())
        {
            oss << "<nav class=\"toc\">\n<h2>Table of Contents</h2>\n<ul>\n";
            for (const auto& entry : toc)
            {
                oss << "<li><a href=\"#" << entry.anchor << "\">" << escape_html(entry.heading)
                    << "</a></li>\n";
            }
            oss << "</ul>\n</nav>\n";
        }
    }

    for (const auto& cell : cells)
    {
        if (cell.cell_type == "code")
        {
            oss << "<div class=\"cell code-cell\">\n";

            if (options.include_execution_counts && cell.execution_count > 0)
            {
                oss << "<span class=\"execution-count\">In [" << cell.execution_count
                    << "]:</span>\n";
            }

            oss << "<pre><code>" << escape_html(cell.source) << "</code></pre>\n";

            if (options.include_outputs && !cell.output_html.empty())
            {
                oss << "<div class=\"output\">" << cell.output_html << "</div>\n";
            }
            else if (options.include_outputs && !cell.output_text.empty())
            {
                oss << "<div class=\"output\"><pre>" << escape_html(cell.output_text)
                    << "</pre></div>\n";
            }

            oss << "</div>\n";
        }
        else
        {
            oss << "<div class=\"cell markdown-cell\">\n";
            oss << cell.source << "\n";
            oss << "</div>\n";
        }
    }

    oss << "</body>\n</html>\n";
    return oss.str();
}

// ============================================================================
// Markdown export
// ============================================================================

auto NotebookExportEngine::export_to_markdown(const std::vector<ExportCell>& cells,
                                              const ExportOptions& options) -> std::string
{
    std::ostringstream oss;

    if (!options.title.empty())
    {
        oss << "# " << options.title << "\n\n";
    }

    for (const auto& cell : cells)
    {
        if (cell.cell_type == "code")
        {
            if (options.include_execution_counts && cell.execution_count > 0)
            {
                oss << "**In [" << cell.execution_count << "]:**\n\n";
            }

            oss << "```python\n" << cell.source << "\n```\n\n";

            if (options.include_outputs && !cell.output_text.empty())
            {
                oss << "**Out:**\n\n```\n" << cell.output_text << "\n```\n\n";
            }
        }
        else
        {
            oss << cell.source << "\n\n";
        }
    }

    return oss.str();
}

// ============================================================================
// LaTeX export
// ============================================================================

auto NotebookExportEngine::export_to_latex(const std::vector<ExportCell>& cells,
                                           const ExportOptions& options) -> std::string
{
    std::ostringstream oss;

    oss << "\\documentclass{article}\n";
    oss << "\\usepackage[utf8]{inputenc}\n";
    oss << "\\usepackage{listings}\n";
    oss << "\\usepackage{color}\n\n";

    if (!options.title.empty())
    {
        oss << "\\title{" << escape_latex(options.title) << "}\n";
    }
    if (!options.author.empty())
    {
        oss << "\\author{" << escape_latex(options.author) << "}\n";
    }

    oss << "\n\\begin{document}\n";

    if (!options.title.empty())
    {
        oss << "\\maketitle\n\n";
    }

    for (const auto& cell : cells)
    {
        if (cell.cell_type == "code")
        {
            if (options.include_execution_counts && cell.execution_count > 0)
            {
                oss << "\\textbf{In [" << cell.execution_count << "]:}\n\n";
            }

            oss << "\\begin{lstlisting}[language=Python]\n";
            oss << cell.source << "\n";
            oss << "\\end{lstlisting}\n\n";

            if (options.include_outputs && !cell.output_text.empty())
            {
                oss << "\\begin{verbatim}\n" << cell.output_text << "\n\\end{verbatim}\n\n";
            }
        }
        else
        {
            // Simple markdown-to-latex: replace # headings.
            std::istringstream lines(cell.source);
            std::string line;
            while (std::getline(lines, line))
            {
                if (line.starts_with("### "))
                {
                    oss << "\\subsubsection{" << escape_latex(line.substr(4)) << "}\n";
                }
                else if (line.starts_with("## "))
                {
                    oss << "\\subsection{" << escape_latex(line.substr(3)) << "}\n";
                }
                else if (line.starts_with("# "))
                {
                    oss << "\\section{" << escape_latex(line.substr(2)) << "}\n";
                }
                else
                {
                    oss << escape_latex(line) << "\n";
                }
            }
            oss << "\n";
        }
    }

    oss << "\\end{document}\n";
    return oss.str();
}

// ============================================================================
// Slides export
// ============================================================================

auto NotebookExportEngine::export_to_slides(const std::vector<ExportCell>& cells,
                                            const ExportOptions& options) -> std::string
{
    std::ostringstream oss;

    oss << "<!DOCTYPE html>\n<html>\n<head>\n";
    oss << "<meta charset=\"utf-8\">\n";
    oss << "<title>" << escape_html(options.title.empty() ? "Slides" : options.title)
        << "</title>\n";
    oss << "<style>\n";
    oss << ".slide { page-break-after: always; padding: 2em; min-height: 80vh; }\n";
    oss << "</style>\n";
    oss << "</head>\n<body>\n";

    int slide_index = 0;
    bool in_slide = false;

    for (const auto& cell : cells)
    {
        // Check if cell starts a new slide (markdown cell starting with heading).
        bool starts_slide = false;
        if (cell.cell_type == "markdown")
        {
            starts_slide = cell.source.starts_with("# ") || cell.source.starts_with("## ");
        }

        if (starts_slide)
        {
            if (in_slide)
            {
                oss << "</div>\n"; // Close previous slide.
            }
            ++slide_index;
            oss << "<div class=\"slide\" id=\"slide-" << slide_index << "\">\n";
            in_slide = true;
        }
        else if (!in_slide)
        {
            ++slide_index;
            oss << "<div class=\"slide\" id=\"slide-" << slide_index << "\">\n";
            in_slide = true;
        }

        if (cell.cell_type == "code")
        {
            oss << "<pre><code>" << escape_html(cell.source) << "</code></pre>\n";
            if (options.include_outputs && !cell.output_text.empty())
            {
                oss << "<pre class=\"output\">" << escape_html(cell.output_text) << "</pre>\n";
            }
        }
        else
        {
            oss << "<div class=\"markdown\">" << cell.source << "</div>\n";
        }
    }

    if (in_slide)
    {
        oss << "</div>\n";
    }

    oss << "</body>\n</html>\n";
    return oss.str();
}

// ============================================================================
// TOC generation
// ============================================================================

auto NotebookExportEngine::generate_toc(const std::vector<ExportCell>& cells) const
    -> std::vector<TocEntry>
{
    std::vector<TocEntry> toc;

    for (const auto& cell : cells)
    {
        if (cell.cell_type != "markdown")
        {
            continue;
        }

        std::istringstream lines(cell.source);
        std::string line;
        while (std::getline(lines, line))
        {
            int heading_level = 0;
            size_t idx = 0;
            while (idx < line.size() && line[idx] == '#')
            {
                ++heading_level;
                ++idx;
            }

            if (heading_level > 0 && heading_level <= 6 && idx < line.size() && line[idx] == ' ')
            {
                TocEntry entry;
                entry.heading = line.substr(idx + 1);
                entry.level = heading_level;
                entry.anchor = slugify(entry.heading);
                toc.push_back(std::move(entry));
            }
        }
    }

    return toc;
}

// ============================================================================
// Strip outputs
// ============================================================================

auto NotebookExportEngine::strip_outputs(const std::vector<ExportCell>& cells) const
    -> std::vector<ExportCell>
{
    std::vector<ExportCell> stripped;
    stripped.reserve(cells.size());

    for (const auto& cell : cells)
    {
        ExportCell clean;
        clean.cell_id = cell.cell_id;
        clean.cell_type = cell.cell_type;
        clean.source = cell.source;
        clean.execution_count = 0;
        // output_text and output_html left empty.
        stripped.push_back(std::move(clean));
    }

    return stripped;
}

// ============================================================================
// Helpers
// ============================================================================

auto NotebookExportEngine::escape_html(const std::string& text) -> std::string
{
    std::string result;
    result.reserve(text.size());

    for (const char ch : text)
    {
        switch (ch)
        {
            case '&':
                result += "&amp;";
                break;
            case '<':
                result += "&lt;";
                break;
            case '>':
                result += "&gt;";
                break;
            case '"':
                result += "&quot;";
                break;
            default:
                result += ch;
                break;
        }
    }

    return result;
}

auto NotebookExportEngine::escape_latex(const std::string& text) -> std::string
{
    std::string result;
    result.reserve(text.size());

    for (const char ch : text)
    {
        switch (ch)
        {
            case '&':
                result += "\\&";
                break;
            case '%':
                result += "\\%";
                break;
            case '$':
                result += "\\$";
                break;
            case '#':
                result += "\\#";
                break;
            case '_':
                result += "\\_";
                break;
            case '{':
                result += "\\{";
                break;
            case '}':
                result += "\\}";
                break;
            case '~':
                result += "\\textasciitilde{}";
                break;
            case '^':
                result += "\\textasciicircum{}";
                break;
            case '\\':
                result += "\\textbackslash{}";
                break;
            default:
                result += ch;
                break;
        }
    }

    return result;
}

auto NotebookExportEngine::slugify(const std::string& text) -> std::string
{
    std::string slug;
    slug.reserve(text.size());

    for (const char ch : text)
    {
        if (std::isalnum(static_cast<unsigned char>(ch)))
        {
            slug += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        }
        else if (ch == ' ' || ch == '-')
        {
            slug += '-';
        }
    }

    return slug;
}

} // namespace markamp::core
