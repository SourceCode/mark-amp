#pragma once

/// @file SearchExporter.h
/// @brief V9 Phase 10 – Search result export to Markdown, CSV, and JSON.
///
/// Header-only implementation of search result export:
///   - Markdown list with snippets and file links
///   - CSV with columns: file, line, snippet, score
///   - JSON serialized SearchResult

#include "Search.h"

#include <cstdint>
#include <sstream>
#include <string>

namespace markamp::core
{

// ────────────────────────────────────────────────────────────
// SearchExportFormat
// ────────────────────────────────────────────────────────────

/// Available export formats for search results.
enum class SearchExportFormat : std::uint8_t
{
    kMarkdown, ///< Formatted markdown list
    kCsv,      ///< Comma-separated values
    kJson      ///< JSON object
};

// ────────────────────────────────────────────────────────────
// SearchExporter
// ────────────────────────────────────────────────────────────

/// Exports search results to various text formats.
class SearchExporter
{
public:
    SearchExporter() = default;

    /// Export results in the specified format.
    [[nodiscard]] auto export_results(const SearchResult& results, SearchExportFormat format) const
        -> std::string
    {
        switch (format)
        {
            case SearchExportFormat::kMarkdown:
                return export_to_markdown(results);
            case SearchExportFormat::kCsv:
                return export_to_csv(results);
            case SearchExportFormat::kJson:
                return export_to_json(results);
        }
        return {};
    }

    /// Export results as a markdown document.
    [[nodiscard]] auto export_to_markdown(const SearchResult& results) const -> std::string
    {
        std::ostringstream oss;

        oss << "# Search Results\n\n";
        oss << "**Query:** `" << results.query.query_string << "`\n";
        oss << "**Total matches:** " << results.total_count << "\n";
        oss << "**Search time:** " << results.elapsed_ms << "ms\n\n";

        if (results.hits.empty())
        {
            oss << "_No results found._\n";
            return oss.str();
        }

        oss << "---\n\n";

        int index = 1;
        for (const auto& hit : results.hits)
        {
            oss << index << ". **" << hit.doc_title << "**";
            if (!hit.block_path.empty())
            {
                oss << " (`" << hit.block_path << "`)";
            }
            oss << "\n";

            if (!hit.snippet.empty())
            {
                oss << "   > " << truncate_snippet(hit.snippet, 120) << "\n";
            }

            oss << "   - Score: " << hit.score << "\n\n";
            ++index;
        }

        return oss.str();
    }

    /// Export results as CSV.
    [[nodiscard]] auto export_to_csv(const SearchResult& results) const -> std::string
    {
        std::ostringstream oss;

        // Header
        oss << "\"File\",\"Title\",\"Snippet\",\"Score\",\"Block Type\"\n";

        for (const auto& hit : results.hits)
        {
            oss << "\"" << escape_csv(hit.block_path) << "\","
                << "\"" << escape_csv(hit.doc_title) << "\","
                << "\"" << escape_csv(truncate_snippet(hit.snippet, 200)) << "\"," << hit.score
                << ","
                << "\"" << block_type_name(hit.block_type) << "\""
                << "\n";
        }

        return oss.str();
    }

    /// Export results as JSON.
    [[nodiscard]] auto export_to_json(const SearchResult& results) const -> std::string
    {
        std::ostringstream oss;

        oss << "{\n";
        oss << "  \"query\": \"" << escape_json(results.query.query_string) << "\",\n";
        oss << "  \"total_count\": " << results.total_count << ",\n";
        oss << "  \"elapsed_ms\": " << results.elapsed_ms << ",\n";
        oss << "  \"hits\": [\n";

        for (std::size_t idx = 0; idx < results.hits.size(); ++idx)
        {
            const auto& hit = results.hits[idx];
            oss << "    {\n";
            oss << "      \"doc_title\": \"" << escape_json(hit.doc_title) << "\",\n";
            oss << "      \"block_path\": \"" << escape_json(hit.block_path) << "\",\n";
            oss << "      \"snippet\": \"" << escape_json(hit.snippet) << "\",\n";
            oss << "      \"score\": " << hit.score << ",\n";
            oss << "      \"block_type\": \"" << block_type_name(hit.block_type) << "\"\n";
            oss << "    }";

            if (idx + 1 < results.hits.size())
            {
                oss << ",";
            }
            oss << "\n";
        }

        oss << "  ]\n";
        oss << "}\n";

        return oss.str();
    }

private:
    /// Truncate a snippet to a maximum length, appending "…" if truncated.
    [[nodiscard]] static auto truncate_snippet(const std::string& snippet, std::size_t max_len)
        -> std::string
    {
        if (snippet.size() <= max_len)
        {
            return snippet;
        }
        return snippet.substr(0, max_len - 1) + "…";
    }

    /// Escape a string for CSV (double quotes).
    [[nodiscard]] static auto escape_csv(const std::string& str) -> std::string
    {
        std::string result;
        result.reserve(str.size());
        for (char chr : str)
        {
            if (chr == '"')
            {
                result += "\"\"";
            }
            else if (chr == '\n')
            {
                result += ' ';
            }
            else if (chr == '\r')
            {
                // skip
            }
            else
            {
                result += chr;
            }
        }
        return result;
    }

    /// Escape a string for JSON.
    [[nodiscard]] static auto escape_json(const std::string& str) -> std::string
    {
        std::string result;
        result.reserve(str.size());
        for (char chr : str)
        {
            switch (chr)
            {
                case '"':
                    result += "\\\"";
                    break;
                case '\\':
                    result += "\\\\";
                    break;
                case '\n':
                    result += "\\n";
                    break;
                case '\r':
                    result += "\\r";
                    break;
                case '\t':
                    result += "\\t";
                    break;
                default:
                    result += chr;
            }
        }
        return result;
    }

    /// Get a human-readable name for a block type.
    [[nodiscard]] static auto block_type_name(SearchBlockType block_type) -> std::string
    {
        switch (block_type)
        {
            case SearchBlockType::Document:
                return "Document";
            case SearchBlockType::Heading:
                return "Heading";
            case SearchBlockType::Paragraph:
                return "Paragraph";
            case SearchBlockType::MathBlock:
                return "Math";
            case SearchBlockType::Table:
                return "Table";
            case SearchBlockType::CodeBlock:
                return "Code";
            case SearchBlockType::HtmlBlock:
                return "HTML";
            case SearchBlockType::List:
                return "List";
            case SearchBlockType::ListItem:
                return "ListItem";
            case SearchBlockType::Blockquote:
                return "Blockquote";
            case SearchBlockType::SuperBlock:
                return "SuperBlock";
            case SearchBlockType::EmbedBlock:
                return "Embed";
        }
        return "Unknown";
    }
};

} // namespace markamp::core
