// ============================================================================
// File: src/core/IncrementalParsePolicy.cpp
// Phase 30: Performance Optimization — Incremental markdown parsing policy
// ============================================================================

#include "IncrementalParsePolicy.h"

#include <algorithm>

namespace markamp::core
{

IncrementalParsePolicy::IncrementalParsePolicy() = default;

auto IncrementalParsePolicy::compute_reparse_range(const EditRegion& edit,
                                                   int32_t total_lines,
                                                   const std::vector<std::string>& lines) const
    -> ReparseRange
{
    if (total_lines <= 0)
    {
        return ReparseRange{0, 0, true};
    }

    // Check if a full reparse is needed
    if (needs_full_reparse(edit, lines))
    {
        return ReparseRange{0, total_lines - 1, true};
    }

    // Basic range: edit region + context
    const int32_t range_start = std::max(0, edit.start_line - context_lines_);
    int32_t range_end = std::min(total_lines - 1, edit.end_line + context_lines_);

    // For multi-line edits, expand further
    const int32_t edit_span = edit.end_line - edit.start_line;
    if (edit_span > 0)
    {
        // Add extra context proportional to edit size, capped
        const int32_t extra = std::min(edit_span, kMaxContextLines - context_lines_);
        range_end = std::min(total_lines - 1, range_end + extra);
    }

    // Expand to block boundaries if needed
    return expand_to_block_boundary(range_start, range_end, total_lines, lines);
}

auto IncrementalParsePolicy::needs_full_reparse(const EditRegion& edit,
                                                const std::vector<std::string>& lines) const -> bool
{
    // Large structural changes always need full reparse
    if (edit.lines_inserted > 50 || edit.lines_deleted > 50)
    {
        return true;
    }

    // Check for unclosed block boundaries
    return crosses_unclosed_block(edit, lines);
}

auto IncrementalParsePolicy::detect_boundary(std::string_view line) -> BlockBoundaryKind
{
    // Trim leading whitespace (up to 3 spaces for markdown)
    size_t indent = 0;
    while (indent < line.size() && indent < 4 && line[indent] == ' ')
    {
        ++indent;
    }

    if (indent >= line.size())
    {
        return BlockBoundaryKind::kNone;
    }

    const auto trimmed = line.substr(indent);

    // Code fence: ``` or ~~~
    if (trimmed.size() >= 3)
    {
        if (trimmed.substr(0, 3) == "```" || trimmed.substr(0, 3) == "~~~")
        {
            return BlockBoundaryKind::kCodeFence;
        }
    }

    // Front matter / thematic break: ---
    if (trimmed.size() >= 3 && indent == 0)
    {
        bool all_dash = true;
        bool all_star = true;
        bool all_under = true;
        for (size_t pos = 0; pos < trimmed.size(); ++pos)
        {
            const char cur_char = trimmed[pos];
            if (cur_char == ' ')
            {
                continue; // spaces allowed in thematic breaks
            }
            if (cur_char != '-')
            {
                all_dash = false;
            }
            if (cur_char != '*')
            {
                all_star = false;
            }
            if (cur_char != '_')
            {
                all_under = false;
            }
        }
        // Count non-space characters
        size_t non_space = 0;
        for (const char current_char : trimmed)
        {
            if (current_char != ' ')
            {
                ++non_space;
            }
        }
        if (non_space >= 3 && (all_dash || all_star || all_under))
        {
            return all_dash ? BlockBoundaryKind::kFrontMatter : BlockBoundaryKind::kThematicBreak;
        }
    }

    // Math block: $$
    if (trimmed.size() >= 2 && trimmed.substr(0, 2) == "$$")
    {
        return BlockBoundaryKind::kMathBlock;
    }

    // HTML block
    if (!trimmed.empty() && trimmed[0] == '<')
    {
        // Check for common block-level HTML tags
        auto lower = std::string(trimmed.substr(0, std::min(trimmed.size(), size_t{20})));
        for (char& cur_char : lower)
        {
            cur_char = static_cast<char>(std::tolower(static_cast<unsigned char>(cur_char)));
        }
        if (lower.find("<div") == 0 || lower.find("<table") == 0 || lower.find("<pre") == 0 ||
            lower.find("<script") == 0 || lower.find("</div") == 0 || lower.find("</table") == 0 ||
            lower.find("</pre") == 0 || lower.find("<section") == 0 || lower.find("</section") == 0)
        {
            return BlockBoundaryKind::kHTMLBlock;
        }
    }

    // Block quote
    if (!trimmed.empty() && trimmed[0] == '>')
    {
        return BlockBoundaryKind::kBlockQuote;
    }

    // List items
    if (!trimmed.empty())
    {
        if (trimmed[0] == '-' || trimmed[0] == '*' || trimmed[0] == '+')
        {
            if (trimmed.size() >= 2 && trimmed[1] == ' ')
            {
                return BlockBoundaryKind::kListItem;
            }
        }
        // Ordered list: digit(s) followed by . or )
        if (trimmed[0] >= '0' && trimmed[0] <= '9')
        {
            size_t pos = 0;
            while (pos < trimmed.size() && trimmed[pos] >= '0' && trimmed[pos] <= '9')
            {
                ++pos;
            }
            if (pos < trimmed.size() && (trimmed[pos] == '.' || trimmed[pos] == ')'))
            {
                if (pos + 1 < trimmed.size() && trimmed[pos + 1] == ' ')
                {
                    return BlockBoundaryKind::kListItem;
                }
            }
        }
    }

    return BlockBoundaryKind::kNone;
}

auto IncrementalParsePolicy::context_lines() const noexcept -> int32_t
{
    return context_lines_;
}

void IncrementalParsePolicy::set_context_lines(int32_t lines)
{
    context_lines_ = std::clamp(lines, int32_t{0}, kMaxContextLines);
}

auto IncrementalParsePolicy::crosses_unclosed_block(const EditRegion& edit,
                                                    const std::vector<std::string>& lines) const
    -> bool
{
    // Count code fences and math blocks in the edit region
    // If any are unclosed (odd count), we have a structural change
    int fence_count = 0;
    int math_count = 0;

    const int32_t check_start = std::max(0, edit.start_line);
    const int32_t check_end = std::min(static_cast<int32_t>(lines.size()) - 1, edit.end_line);

    for (int32_t idx = check_start; idx <= check_end; ++idx)
    {
        const auto boundary = detect_boundary(lines.at(static_cast<size_t>(idx)));
        if (boundary == BlockBoundaryKind::kCodeFence)
        {
            ++fence_count;
        }
        if (boundary == BlockBoundaryKind::kMathBlock)
        {
            ++math_count;
        }
    }

    // Odd number of fences or math delimiters means unclosed block
    return (fence_count % 2 != 0) || (math_count % 2 != 0);
}

auto IncrementalParsePolicy::expand_to_block_boundary(int32_t start,
                                                      int32_t end_val,
                                                      int32_t total_lines,
                                                      const std::vector<std::string>& lines) const
    -> ReparseRange
{
    int32_t result_start = start;
    int32_t result_end = end_val;

    // Scan backward from start to find enclosing block start
    for (int32_t idx = start; idx >= 0; --idx)
    {
        const auto boundary = detect_boundary(lines.at(static_cast<size_t>(idx)));
        if (boundary == BlockBoundaryKind::kCodeFence || boundary == BlockBoundaryKind::kMathBlock)
        {
            result_start = idx;
            break;
        }
    }

    // Scan forward from end to find enclosing block end
    for (int32_t idx = end_val; idx < total_lines; ++idx)
    {
        const auto boundary = detect_boundary(lines.at(static_cast<size_t>(idx)));
        if (boundary == BlockBoundaryKind::kCodeFence || boundary == BlockBoundaryKind::kMathBlock)
        {
            result_end = idx;
            break;
        }
    }

    return ReparseRange{result_start, result_end, false};
}

} // namespace markamp::core
