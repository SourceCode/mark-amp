#pragma once

/// @file FoldingRangeProvider.h
/// @brief V9 Phase 19 — Folding range provider for markdown documents.
///
/// Detects foldable regions: heading sections, code fences, YAML frontmatter,
/// HTML blocks, nested lists, multi-line tables, and collapsible sections.

#include "CodeIntelligenceTypes.h"

#include <sstream>
#include <string>
#include <vector>

namespace markamp::core
{

/// Header-only provider for folding ranges in markdown documents.
///
/// Usage:
/// ```cpp
/// FoldingRangeProvider provider;
/// auto ranges = provider.provide_ranges("# Section\ntext\n## Sub\nmore");
/// // ranges[0] = heading fold from line 0 to line 3
/// ```
class FoldingRangeProvider
{
public:
    FoldingRangeProvider() = default;

    /// Provide folding ranges for the entire document.
    /// @param content Full document content
    /// @return Vector of FoldingRange entries
    [[nodiscard]] static auto provide_ranges(const std::string& content)
        -> std::vector<FoldingRange>
    {
        std::vector<FoldingRange> ranges;
        std::vector<std::string> lines;

        // Parse all lines
        std::istringstream stream(content);
        std::string line_text;
        while (std::getline(stream, line_text))
        {
            lines.push_back(line_text);
        }

        auto total_lines = static_cast<int>(lines.size());
        if (total_lines == 0)
        {
            return ranges;
        }

        // 1. YAML frontmatter folding
        detect_frontmatter(lines, total_lines, ranges);

        // 2. Heading-based folding
        detect_heading_folds(lines, total_lines, ranges);

        // 3. Code fence folding
        detect_code_fences(lines, total_lines, ranges);

        // 4. HTML block folding (including <details>)
        detect_html_blocks(lines, total_lines, ranges);

        // 5. Multi-line table folding
        detect_tables(lines, total_lines, ranges);

        // 6. Nested list folding
        detect_nested_lists(lines, total_lines, ranges);

        return ranges;
    }

private:
    /// Detect YAML frontmatter (--- ... ---).
    static void detect_frontmatter(const std::vector<std::string>& lines,
                                   int total_lines,
                                   std::vector<FoldingRange>& ranges)
    {
        if (total_lines > 1 && lines[0] == "---")
        {
            for (int end_line_idx = 1; end_line_idx < total_lines; ++end_line_idx)
            {
                if (lines[static_cast<std::vector<std::string>::size_type>(end_line_idx)] == "---")
                {
                    ranges.push_back({0, end_line_idx, FoldingRangeKind::kFrontmatter, "---"});
                    break;
                }
            }
        }
    }

    /// Detect heading-based folds.
    static void detect_heading_folds(const std::vector<std::string>& lines,
                                     int total_lines,
                                     std::vector<FoldingRange>& ranges)
    {
        struct HeadingInfo
        {
            int level;
            int line;
        };

        std::vector<HeadingInfo> headings;
        for (int idx = 0; idx < total_lines; ++idx)
        {
            const auto& current_line = lines[static_cast<std::vector<std::string>::size_type>(idx)];
            if (!current_line.empty() && current_line[0] == '#')
            {
                int level = 0;
                while (level < static_cast<int>(current_line.size()) &&
                       current_line[static_cast<std::string::size_type>(level)] == '#')
                {
                    ++level;
                }
                if (level <= 6 && level < static_cast<int>(current_line.size()) &&
                    current_line[static_cast<std::string::size_type>(level)] == ' ')
                {
                    headings.push_back({level, idx});
                }
            }
        }

        for (std::vector<HeadingInfo>::size_type idx = 0; idx < headings.size(); ++idx)
        {
            const auto& heading = headings[idx];
            int fold_end = total_lines - 1; // Default: fold to end of document

            // Find the next heading at the same or higher level
            for (auto next = idx + 1; next < headings.size(); ++next)
            {
                if (headings[next].level <= heading.level)
                {
                    fold_end = headings[next].line - 1;
                    break;
                }
            }

            // Skip empty folds
            if (fold_end > heading.line)
            {
                // Trim trailing empty lines
                while (fold_end > heading.line &&
                       lines[static_cast<std::vector<std::string>::size_type>(fold_end)].empty())
                {
                    --fold_end;
                }

                if (fold_end > heading.line)
                {
                    const auto& heading_text =
                        lines[static_cast<std::vector<std::string>::size_type>(heading.line)];
                    ranges.push_back(
                        {heading.line, fold_end, FoldingRangeKind::kHeading, heading_text});
                }
            }
        }
    }

    /// Detect code fence folds (``` ... ```).
    static void detect_code_fences(const std::vector<std::string>& lines,
                                   int total_lines,
                                   std::vector<FoldingRange>& ranges)
    {
        int fence_start = -1;
        for (int idx = 0; idx < total_lines; ++idx)
        {
            const auto& current_line = lines[static_cast<std::vector<std::string>::size_type>(idx)];
            if (current_line.size() >= 3 && current_line.substr(0, 3) == "```")
            {
                if (fence_start < 0)
                {
                    fence_start = idx;
                }
                else
                {
                    ranges.push_back({fence_start, idx, FoldingRangeKind::kCodeFence, "```"});
                    fence_start = -1;
                }
            }
        }
    }

    /// Detect HTML blocks and <details> sections.
    static void detect_html_blocks(const std::vector<std::string>& lines,
                                   int total_lines,
                                   std::vector<FoldingRange>& ranges)
    {
        for (int idx = 0; idx < total_lines; ++idx)
        {
            const auto& current_line = lines[static_cast<std::vector<std::string>::size_type>(idx)];
            auto trimmed = trim_whitespace(current_line);

            // <details> ... </details>
            if (trimmed.find("<details") == 0)
            {
                for (int end_idx = idx + 1; end_idx < total_lines; ++end_idx)
                {
                    auto end_trimmed = trim_whitespace(
                        lines[static_cast<std::vector<std::string>::size_type>(end_idx)]);
                    if (end_trimmed.find("</details>") != std::string::npos)
                    {
                        // Extract summary text if available
                        std::string collapsed_text = "<details>";
                        for (int summary_idx = idx; summary_idx <= end_idx; ++summary_idx)
                        {
                            auto summary_line = trim_whitespace(
                                lines[static_cast<std::vector<std::string>::size_type>(
                                    summary_idx)]);
                            auto summary_start = summary_line.find("<summary>");
                            if (summary_start != std::string::npos)
                            {
                                auto summary_end = summary_line.find("</summary>");
                                if (summary_end != std::string::npos)
                                {
                                    collapsed_text = summary_line.substr(
                                        summary_start + 9, summary_end - summary_start - 9);
                                }
                                break;
                            }
                        }

                        ranges.push_back(
                            {idx, end_idx, FoldingRangeKind::kCollapsible, collapsed_text});
                        break;
                    }
                }
            }
        }
    }

    /// Detect multi-line tables.
    static void detect_tables(const std::vector<std::string>& lines,
                              int total_lines,
                              std::vector<FoldingRange>& ranges)
    {
        int table_start = -1;
        for (int idx = 0; idx < total_lines; ++idx)
        {
            const auto& current_line = lines[static_cast<std::vector<std::string>::size_type>(idx)];
            const bool is_table_line = !current_line.empty() && current_line[0] == '|';

            if (is_table_line && table_start < 0)
            {
                table_start = idx;
            }
            else if (!is_table_line && table_start >= 0)
            {
                // Table ends — only fold if 4+ lines (header + separator + 2+ rows)
                if (idx - table_start >= 4)
                {
                    ranges.push_back({table_start, idx - 1, FoldingRangeKind::kTable, "Table"});
                }
                table_start = -1;
            }
        }

        // Handle table at end of document
        if (table_start >= 0 && total_lines - table_start >= 4)
        {
            ranges.push_back({table_start, total_lines - 1, FoldingRangeKind::kTable, "Table"});
        }
    }

    /// Detect nested lists (fold deeply indented sublists).
    static void detect_nested_lists(const std::vector<std::string>& lines,
                                    int total_lines,
                                    std::vector<FoldingRange>& ranges)
    {
        int list_start = -1;
        int base_indent = 0;

        for (int idx = 0; idx < total_lines; ++idx)
        {
            const auto& current_line = lines[static_cast<std::vector<std::string>::size_type>(idx)];
            if (current_line.empty())
            {
                if (list_start >= 0 && idx - list_start >= 3)
                {
                    ranges.push_back({list_start, idx - 1, FoldingRangeKind::kList, "…"});
                }
                list_start = -1;
                continue;
            }

            auto indent = get_indent_level(current_line);
            auto first_non_space = current_line.find_first_not_of(" \t");
            if (first_non_space == std::string::npos)
            {
                continue;
            }

            const bool is_list_item =
                (current_line[first_non_space] == '-' || current_line[first_non_space] == '*' ||
                 current_line[first_non_space] == '+');
            const bool is_numbered =
                std::isdigit(static_cast<unsigned char>(current_line[first_non_space])) != 0;

            if ((is_list_item || is_numbered) && indent > 0)
            {
                if (list_start < 0)
                {
                    list_start = idx;
                    base_indent = indent;
                }
            }
            else if (indent <= base_indent && list_start >= 0)
            {
                if (idx - list_start >= 3)
                {
                    ranges.push_back({list_start, idx - 1, FoldingRangeKind::kList, "…"});
                }
                list_start = -1;
            }
        }
    }

    /// Get the indentation level (number of leading spaces/tabs).
    [[nodiscard]] static auto get_indent_level(const std::string& line_text) -> int
    {
        int indent = 0;
        for (const char chr : line_text)
        {
            if (chr == ' ')
            {
                ++indent;
            }
            else if (chr == '\t')
            {
                indent += 4;
            }
            else
            {
                break;
            }
        }
        return indent;
    }

    /// Trim leading/trailing whitespace.
    [[nodiscard]] static auto trim_whitespace(const std::string& text) -> std::string
    {
        auto start = text.find_first_not_of(" \t\r\n");
        if (start == std::string::npos)
        {
            return "";
        }
        auto end = text.find_last_not_of(" \t\r\n");
        return text.substr(start, end - start + 1);
    }
};

} // namespace markamp::core
