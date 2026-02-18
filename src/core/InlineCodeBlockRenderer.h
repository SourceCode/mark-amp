#pragma once

/// @file InlineCodeBlockRenderer.h
/// @brief V9 Phase 45 Task 9 – Code block metadata extraction for inline rendering.
///
/// Header-only, pure logic. Parses fenced code blocks and extracts
/// language, content, and display hints.

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Information extracted from a fenced code block.
struct CodeBlockInfo
{
    std::string language;            ///< Language identifier (e.g. "python", "cpp")
    std::string content;             ///< Code content (excluding fences)
    std::string fence_info;          ///< Full first line after ``` (e.g. "python title='Example'")
    int start_line{0};               ///< Line of opening fence
    int end_line{0};                 ///< Line of closing fence
    int line_count{0};               ///< Number of content lines (excluding fences)
    bool has_syntax_highlight{true}; ///< Whether syntax highlighting is available

    /// Whether this is an empty code block.
    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return content.empty();
    }
};

/// Parses fenced code blocks and provides rendering metadata.
class InlineCodeBlockRenderer
{
public:
    /// Default collapse threshold: 20 lines.
    static constexpr int kDefaultCollapseThreshold = 20;

    InlineCodeBlockRenderer() = default;

    /// Parse a fenced code block starting at the given line index.
    /// lines: all document lines. start_line: index of the opening fence.
    /// Returns parsed CodeBlockInfo.
    [[nodiscard]] static auto parse_fenced_block(const std::vector<std::string>& lines,
                                                 int start_line) -> CodeBlockInfo
    {
        CodeBlockInfo info;
        info.start_line = start_line;

        const auto total = static_cast<int>(lines.size());
        if (start_line < 0 || start_line >= total)
        {
            return info;
        }

        const auto& opening = lines[static_cast<std::size_t>(start_line)];

        // Determine fence type and length
        char fence_char = '\0';
        int fence_length = 0;

        if (opening.size() >= 3)
        {
            if (opening[0] == '`' && opening[1] == '`' && opening[2] == '`')
            {
                fence_char = '`';
                fence_length = 3;
                // Count additional backticks
                while (static_cast<std::size_t>(fence_length) < opening.size() &&
                       opening[static_cast<std::size_t>(fence_length)] == '`')
                {
                    ++fence_length;
                }
            }
            else if (opening[0] == '~' && opening[1] == '~' && opening[2] == '~')
            {
                fence_char = '~';
                fence_length = 3;
                while (static_cast<std::size_t>(fence_length) < opening.size() &&
                       opening[static_cast<std::size_t>(fence_length)] == '~')
                {
                    ++fence_length;
                }
            }
        }

        if (fence_char == '\0')
        {
            return info; // Not a valid fence
        }

        // Extract info line after the fence characters
        if (static_cast<std::size_t>(fence_length) < opening.size())
        {
            info.fence_info = opening.substr(static_cast<std::size_t>(fence_length));
            // Trim leading whitespace from fence_info
            auto first_non_space = info.fence_info.find_first_not_of(" \t");
            if (first_non_space != std::string::npos)
            {
                info.fence_info = info.fence_info.substr(first_non_space);
            }
            info.language = detect_language(info.fence_info);
        }

        // Find closing fence
        int content_start = start_line + 1;
        int content_end = total - 1; // Default to end of document if no closing fence

        for (int scan = content_start; scan < total; ++scan)
        {
            const auto& scan_line = lines[static_cast<std::size_t>(scan)];
            if (is_closing_fence(scan_line, fence_char, fence_length))
            {
                content_end = scan - 1;
                info.end_line = scan;
                break;
            }
        }

        // If no closing fence found, end_line is the last line of the document
        if (info.end_line == 0)
        {
            info.end_line = total - 1;
            content_end = total - 1;
        }

        // Extract content
        std::string code_content;
        info.line_count = 0;
        for (int ln = content_start; ln <= content_end; ++ln)
        {
            if (ln > content_start)
            {
                code_content += '\n';
            }
            code_content += lines[static_cast<std::size_t>(ln)];
            ++info.line_count;
        }
        info.content = code_content;

        // Check syntax highlight availability
        info.has_syntax_highlight = !info.language.empty();

        return info;
    }

    /// Detect language from fence info line.
    /// "python" → "python", "python title='Ex'" → "python", "js" → "javascript"
    [[nodiscard]] static auto detect_language(const std::string& fence_info) -> std::string
    {
        if (fence_info.empty())
        {
            return "";
        }

        // Extract the first word (language identifier)
        auto end = fence_info.find_first_of(" \t{");
        std::string lang = (end != std::string::npos) ? fence_info.substr(0, end) : fence_info;

        // Normalize common aliases
        return normalize_language(lang);
    }

    /// Whether a code block should be collapsed by default (based on line count).
    [[nodiscard]] static auto should_collapse(int line_count,
                                              int threshold = kDefaultCollapseThreshold) -> bool
    {
        return line_count > threshold;
    }

    /// Check if a line is a code fence opening.
    [[nodiscard]] static auto is_fence_start(const std::string& line) -> bool
    {
        if (line.size() < 3)
        {
            return false;
        }
        return (line[0] == '`' && line[1] == '`' && line[2] == '`') ||
               (line[0] == '~' && line[1] == '~' && line[2] == '~');
    }

private:
    /// Check if a line is a closing fence matching the opening fence.
    [[nodiscard]] static auto
    is_closing_fence(const std::string& line, char fence_char, int min_length) -> bool
    {
        if (static_cast<int>(line.size()) < min_length)
        {
            return false;
        }

        int count = 0;
        for (char chr : line)
        {
            if (chr == fence_char)
            {
                ++count;
            }
            else if (chr == ' ' || chr == '\t')
            {
                continue; // Trailing whitespace ok
            }
            else
            {
                return false; // Non-fence content
            }
        }

        return count >= min_length;
    }

    /// Normalize common language aliases.
    [[nodiscard]] static auto normalize_language(const std::string& lang) -> std::string
    {
        if (lang == "js")
        {
            return "javascript";
        }
        if (lang == "ts")
        {
            return "typescript";
        }
        if (lang == "py")
        {
            return "python";
        }
        if (lang == "rb")
        {
            return "ruby";
        }
        if (lang == "sh" || lang == "bash" || lang == "zsh")
        {
            return "shellscript";
        }
        if (lang == "yml")
        {
            return "yaml";
        }
        if (lang == "md")
        {
            return "markdown";
        }
        if (lang == "c++")
        {
            return "cpp";
        }
        if (lang == "c#" || lang == "csharp")
        {
            return "csharp";
        }
        if (lang == "objc")
        {
            return "objective-c";
        }
        if (lang == "kt")
        {
            return "kotlin";
        }
        if (lang == "rs")
        {
            return "rust";
        }
        if (lang == "dockerfile")
        {
            return "docker";
        }
        return lang;
    }
};

} // namespace markamp::core
