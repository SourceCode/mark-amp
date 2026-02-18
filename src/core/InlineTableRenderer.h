#pragma once

/// @file InlineTableRenderer.h
/// @brief V9 Phase 45 Task 14 – Table rendering metadata extraction.
///
/// Header-only, pure logic. Parses markdown pipe tables and extracts
/// headers, rows, alignments, and column width hints for inline rendering.
/// Complements TableEditorOverlay for WYSIWYG table display.

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Column alignment in a markdown table.
enum class TableAlignment : std::uint8_t
{
    kLeft = 0,
    kCenter = 1,
    kRight = 2,
    kDefault = 3
};

/// Rendering metadata for a markdown pipe table.
struct TableRenderInfo
{
    std::vector<std::string> headers;           ///< Header cells
    std::vector<std::vector<std::string>> rows; ///< Data rows (row × col)
    std::vector<TableAlignment> alignments;     ///< Per-column alignment
    int column_count{0};                        ///< Number of columns
    int row_count{0};      ///< Number of data rows (excluding header + separator)
    int start_line{0};     ///< First line of table
    int end_line{0};       ///< Last line of table
    bool has_header{true}; ///< Whether the table has a header row
    bool is_valid{false};  ///< Whether this is a valid table

    /// Total lines including header and separator.
    [[nodiscard]] auto total_lines() const noexcept -> int
    {
        return end_line - start_line + 1;
    }
};

/// Parses markdown pipe tables and extracts rendering metadata.
class InlineTableRenderer
{
public:
    InlineTableRenderer() = default;

    /// Check if a line starts a table (pipe-delimited row).
    [[nodiscard]] static auto is_table_start(const std::string& line) -> bool
    {
        auto trimmed = trim(line);
        return !trimmed.empty() && trimmed[0] == '|';
    }

    /// Parse a table from document lines starting at a given index.
    [[nodiscard]] static auto parse_table(const std::vector<std::string>& lines, int start_line)
        -> TableRenderInfo
    {
        TableRenderInfo info;
        info.start_line = start_line;

        const auto total = static_cast<int>(lines.size());
        if (start_line < 0 || start_line >= total)
        {
            return info;
        }

        // Parse header row
        auto header_cells = parse_row(lines[static_cast<std::size_t>(start_line)]);
        if (header_cells.empty())
        {
            return info;
        }

        info.headers = header_cells;
        info.column_count = static_cast<int>(header_cells.size());

        int next_line = start_line + 1;

        // Parse separator row (if present)
        if (next_line < total && is_separator_row(lines[static_cast<std::size_t>(next_line)]))
        {
            info.alignments = parse_alignments(lines[static_cast<std::size_t>(next_line)]);
            // Pad alignments to match column count
            while (static_cast<int>(info.alignments.size()) < info.column_count)
            {
                info.alignments.push_back(TableAlignment::kDefault);
            }
            ++next_line;
        }
        else
        {
            // No separator: fill with defaults
            info.alignments.resize(static_cast<std::size_t>(info.column_count),
                                   TableAlignment::kDefault);
        }

        // Parse data rows
        while (next_line < total)
        {
            const auto& row_line = lines[static_cast<std::size_t>(next_line)];
            if (!is_table_start(row_line))
            {
                break;
            }

            auto cells = parse_row(row_line);
            // Pad or truncate to match column count
            cells.resize(static_cast<std::size_t>(info.column_count));
            info.rows.push_back(std::move(cells));
            ++next_line;
        }

        info.end_line = next_line - 1;
        info.row_count = static_cast<int>(info.rows.size());
        info.is_valid = true;

        return info;
    }

    /// Compute column widths for rendering, given a maximum total width.
    /// Returns pixel widths proportional to content length.
    [[nodiscard]] static auto compute_column_widths(const TableRenderInfo& info,
                                                    int max_total_width) -> std::vector<int>
    {
        if (info.column_count == 0)
        {
            return {};
        }

        // Compute max content length per column
        std::vector<int> max_lengths(static_cast<std::size_t>(info.column_count), 0);

        for (int col = 0; col < info.column_count; ++col)
        {
            auto col_idx = static_cast<std::size_t>(col);
            max_lengths[col_idx] =
                static_cast<int>(col_idx < info.headers.size() ? info.headers[col_idx].size() : 0);

            for (const auto& row : info.rows)
            {
                if (col_idx < row.size())
                {
                    max_lengths[col_idx] =
                        std::max(max_lengths[col_idx], static_cast<int>(row[col_idx].size()));
                }
            }

            // Minimum width
            max_lengths[col_idx] = std::max(max_lengths[col_idx], 3);
        }

        // Compute proportional widths
        int total_content = 0;
        for (auto len : max_lengths)
        {
            total_content += len;
        }

        if (total_content == 0)
        {
            total_content = 1;
        }

        static constexpr int kPaddingPerColumn = 16;
        int available = max_total_width - (info.column_count * kPaddingPerColumn);
        available = std::max(available, info.column_count * 30); // Min 30px per column

        std::vector<int> widths(static_cast<std::size_t>(info.column_count));
        for (int col = 0; col < info.column_count; ++col)
        {
            widths[static_cast<std::size_t>(col)] =
                (max_lengths[static_cast<std::size_t>(col)] * available) / total_content;
            widths[static_cast<std::size_t>(col)] =
                std::max(widths[static_cast<std::size_t>(col)], 30);
        }

        return widths;
    }

private:
    /// Parse a pipe-delimited row into cells.
    [[nodiscard]] static auto parse_row(const std::string& line) -> std::vector<std::string>
    {
        std::vector<std::string> cells;
        auto trimmed = trim(line);

        if (trimmed.empty() || trimmed[0] != '|')
        {
            return cells;
        }

        // Remove leading and trailing pipes
        if (!trimmed.empty() && trimmed[0] == '|')
        {
            trimmed = trimmed.substr(1);
        }
        if (!trimmed.empty() && trimmed.back() == '|')
        {
            trimmed.pop_back();
        }

        // Split by pipe
        std::size_t pos = 0;
        while (pos <= trimmed.size())
        {
            auto pipe = trimmed.find('|', pos);
            std::string cell;
            if (pipe != std::string::npos)
            {
                cell = trimmed.substr(pos, pipe - pos);
                pos = pipe + 1;
            }
            else
            {
                cell = trimmed.substr(pos);
                pos = trimmed.size() + 1;
            }
            cells.push_back(trim(cell));
        }

        return cells;
    }

    /// Check if a line is a separator row (e.g. |---|:---|---:|).
    [[nodiscard]] static auto is_separator_row(const std::string& line) -> bool
    {
        auto trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] != '|')
        {
            return false;
        }

        for (char chr : trimmed)
        {
            if (chr != '|' && chr != '-' && chr != ':' && chr != ' ')
            {
                return false;
            }
        }

        return trimmed.find('-') != std::string::npos;
    }

    /// Parse alignment indicators from a separator row.
    [[nodiscard]] static auto parse_alignments(const std::string& line)
        -> std::vector<TableAlignment>
    {
        auto cells = parse_row(line);
        std::vector<TableAlignment> alignments;

        for (const auto& cell : cells)
        {
            auto trimmed = trim(cell);
            if (trimmed.empty())
            {
                alignments.push_back(TableAlignment::kDefault);
                continue;
            }

            bool left_colon = trimmed[0] == ':';
            bool right_colon = trimmed.back() == ':';

            if (left_colon && right_colon)
            {
                alignments.push_back(TableAlignment::kCenter);
            }
            else if (right_colon)
            {
                alignments.push_back(TableAlignment::kRight);
            }
            else if (left_colon)
            {
                alignments.push_back(TableAlignment::kLeft);
            }
            else
            {
                alignments.push_back(TableAlignment::kDefault);
            }
        }

        return alignments;
    }

    /// Trim leading and trailing whitespace.
    [[nodiscard]] static auto trim(const std::string& str) -> std::string
    {
        auto start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos)
        {
            return "";
        }
        auto end_pos = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end_pos - start + 1);
    }
};

} // namespace markamp::core
