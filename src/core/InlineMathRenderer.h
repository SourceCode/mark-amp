#pragma once

/// @file InlineMathRenderer.h
/// @brief V9 Phase 45 Task 12 – Math block detection for inline and display math.
///
/// Header-only, pure logic. Parses inline math ($...$) and display math ($$...$$)
/// expressions from markdown content.

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// A single inline math span within a line.
struct MathSpan
{
    std::string expression; ///< The math expression (without delimiters)
    int start_col{0};       ///< Start column in the source line
    int end_col{0};         ///< End column in the source line (exclusive)
    int line_number{0};     ///< Source line number
    bool is_valid{false};   ///< Whether this is a valid math expression
};

/// Information about a display math block ($$...$$).
struct MathBlockInfo
{
    std::string expression;     ///< The math expression (without delimiters)
    int start_line{0};          ///< Line of opening $$
    int end_line{0};            ///< Line of closing $$
    bool is_display_math{true}; ///< Always true for display blocks
    bool is_valid{false};       ///< Whether this is a valid math block

    /// Number of lines in the expression.
    [[nodiscard]] auto line_count() const noexcept -> int
    {
        return end_line - start_line + 1;
    }
};

/// Parses inline and display math expressions from markdown.
class InlineMathRenderer
{
public:
    InlineMathRenderer() = default;

    /// Parse inline math expressions ($...$) from a single line.
    /// Does NOT match display math ($$) — those are handled by parse_display_math.
    [[nodiscard]] static auto parse_inline_math(const std::string& line, int line_number = 0)
        -> std::vector<MathSpan>
    {
        std::vector<MathSpan> spans;

        if (line.empty())
        {
            return spans;
        }

        std::size_t pos = 0;
        while (pos < line.size())
        {
            // Find opening $
            auto dollar_pos = line.find('$', pos);
            if (dollar_pos == std::string::npos)
            {
                break;
            }

            // Skip $$ (display math delimiter)
            if (dollar_pos + 1 < line.size() && line[dollar_pos + 1] == '$')
            {
                pos = dollar_pos + 2;
                continue;
            }

            // Skip escaped \$
            if (dollar_pos > 0 && line[dollar_pos - 1] == '\\')
            {
                pos = dollar_pos + 1;
                continue;
            }

            // Find closing $ (not $$, not escaped)
            auto close_pos = find_close_dollar(line, dollar_pos + 1);
            if (close_pos == std::string::npos)
            {
                pos = dollar_pos + 1;
                continue;
            }

            // Extract expression
            MathSpan span;
            span.expression = line.substr(dollar_pos + 1, close_pos - dollar_pos - 1);
            span.start_col = static_cast<int>(dollar_pos);
            span.end_col = static_cast<int>(close_pos + 1);
            span.line_number = line_number;

            // Validate: must not be empty, must not be only whitespace
            if (!span.expression.empty() &&
                span.expression.find_first_not_of(' ') != std::string::npos)
            {
                span.is_valid = true;
                spans.push_back(std::move(span));
            }

            pos = close_pos + 1;
        }

        return spans;
    }

    /// Parse a display math block ($$...$$) starting at the given line index.
    [[nodiscard]] static auto parse_display_math(const std::vector<std::string>& lines,
                                                 int start_line) -> MathBlockInfo
    {
        MathBlockInfo info;
        info.start_line = start_line;
        info.is_display_math = true;

        const auto total = static_cast<int>(lines.size());
        if (start_line < 0 || start_line >= total)
        {
            return info;
        }

        const auto& first = lines[static_cast<std::size_t>(start_line)];

        // Must start with $$
        if (first.size() < 2 || first[0] != '$' || first[1] != '$')
        {
            return info;
        }

        // Check for single-line display math: $$ expression $$
        if (first.size() > 4)
        {
            auto close = first.find("$$", 2);
            if (close != std::string::npos && close > 2)
            {
                info.expression = first.substr(2, close - 2);
                // Trim leading/trailing whitespace
                trim_whitespace(info.expression);
                info.end_line = start_line;
                info.is_valid = !info.expression.empty();
                return info;
            }
        }

        // Multi-line display math: find closing $$
        std::string expression;
        for (int idx = start_line + 1; idx < total; ++idx)
        {
            const auto& current = lines[static_cast<std::size_t>(idx)];

            // Check for closing $$
            if (current.size() >= 2 && current[0] == '$' && current[1] == '$')
            {
                info.end_line = idx;
                info.expression = expression;
                trim_whitespace(info.expression);
                info.is_valid = !info.expression.empty();
                return info;
            }

            if (!expression.empty())
            {
                expression += '\n';
            }
            expression += current;
        }

        // No closing $$ found: treat rest of document as the block
        info.end_line = total - 1;
        info.expression = expression;
        trim_whitespace(info.expression);
        info.is_valid = !info.expression.empty();
        return info;
    }

    /// Check if a line starts a display math block.
    [[nodiscard]] static auto is_display_math_start(const std::string& line) -> bool
    {
        auto trimmed = line;
        while (!trimmed.empty() && (trimmed[0] == ' ' || trimmed[0] == '\t'))
        {
            trimmed.erase(trimmed.begin());
        }
        return trimmed.size() >= 2 && trimmed[0] == '$' && trimmed[1] == '$';
    }

    /// Check if a line contains inline math.
    [[nodiscard]] static auto has_inline_math(const std::string& line) -> bool
    {
        return !parse_inline_math(line).empty();
    }

private:
    /// Find the closing $ for an inline math expression.
    [[nodiscard]] static auto find_close_dollar(const std::string& line, std::size_t start_pos)
        -> std::size_t
    {
        for (std::size_t pos = start_pos; pos < line.size(); ++pos)
        {
            if (line[pos] == '$')
            {
                // Must not be escaped
                if (pos > 0 && line[pos - 1] == '\\')
                {
                    continue;
                }
                // Must not be $$ (display math)
                if (pos + 1 < line.size() && line[pos + 1] == '$')
                {
                    return std::string::npos; // Hit display math delimiter
                }
                return pos;
            }
        }
        return std::string::npos;
    }

    /// Trim leading and trailing whitespace from a string in-place.
    static void trim_whitespace(std::string& str)
    {
        auto start = str.find_first_not_of(" \t\n\r");
        if (start == std::string::npos)
        {
            str.clear();
            return;
        }
        auto end_pos = str.find_last_not_of(" \t\n\r");
        str = str.substr(start, end_pos - start + 1);
    }
};

} // namespace markamp::core
