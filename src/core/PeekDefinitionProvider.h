/// @file PeekDefinitionProvider.h
/// @brief V9 Phase 17 Task 13 — Inline preview of link targets without navigation.
///
/// Header-only. Extracts short content previews of link targets (headings,
/// block refs, markdown sections) to show in a popup/tooltip.

#pragma once

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// PeekContentType
// ============================================================================

/// What kind of content is being peeked.
enum class PeekContentType : uint8_t
{
    kMarkdown, ///< Regular markdown paragraph
    kCode,     ///< Code block
    kHeading,  ///< Heading section
    kBlockRef  ///< A ^block-id reference
};

// ============================================================================
// PeekContent
// ============================================================================

/// Content preview for a peek-definition popup.
struct PeekContent
{
    std::string title;        ///< Short title for the preview
    std::string preview_text; ///< Extracted content (up to ~10 lines)
    int line_count{0};        ///< Number of lines in the preview
    std::string target_uri;   ///< URI of the full target
    int target_line{0};       ///< Line number in the target document
    PeekContentType content_type{PeekContentType::kMarkdown};
};

// ============================================================================
// PeekDefinitionProvider
// ============================================================================

/// Provides inline previews of link targets for peek-definition UI.
///
/// Given cursor position in a document, identifies the link under cursor
/// and extracts a short content snippet from the target.
///
/// Usage:
/// ```cpp
/// PeekDefinitionProvider provider;
/// auto peek = provider.peek(content, line, character);
/// ```
class PeekDefinitionProvider
{
public:
    PeekDefinitionProvider() = default;

    /// Maximum lines to include in a preview.
    static constexpr int kMaxPreviewLines = 10;

    /// Peek at the link under the cursor and return a content preview.
    [[nodiscard]] auto peek(const std::string& content, int line, int character) const
        -> std::optional<PeekContent>
    {
        auto line_text = get_line_at(content, line);
        if (line_text.empty() || character < 0 || character >= static_cast<int>(line_text.size()))
        {
            return std::nullopt;
        }

        // Try wikilink [[target]]
        auto wiki_peek = try_wikilink_peek(line_text, character, content);
        if (wiki_peek.has_value())
        {
            return wiki_peek;
        }

        // Try heading ref [text](#heading)
        auto heading_peek = try_heading_peek(line_text, character, content);
        if (heading_peek.has_value())
        {
            return heading_peek;
        }

        // Try markdown link [text](url)
        auto md_peek = try_markdown_link_peek(line_text, character);
        if (md_peek.has_value())
        {
            return md_peek;
        }

        return std::nullopt;
    }

    /// Get a content preview starting from a specific heading.
    [[nodiscard]] static auto peek_heading(const std::string& content,
                                           const std::string& heading_text)
        -> std::optional<PeekContent>
    {
        auto lines = split_lines(content);
        const auto target_slug = slugify(heading_text);

        for (std::size_t idx = 0; idx < lines.size(); ++idx)
        {
            auto trimmed = trim_left(lines[idx]);
            if (!trimmed.empty() && trimmed[0] == '#')
            {
                auto slug = extract_heading_slug(trimmed);
                if (slug == target_slug)
                {
                    PeekContent peek_result;
                    peek_result.title = heading_text;
                    peek_result.target_line = static_cast<int>(idx);
                    peek_result.content_type = PeekContentType::kHeading;
                    peek_result.preview_text = extract_lines(lines, idx, kMaxPreviewLines);
                    peek_result.line_count =
                        std::min(kMaxPreviewLines, static_cast<int>(lines.size() - idx));
                    return peek_result;
                }
            }
        }
        return std::nullopt;
    }

    /// Get a content preview at a block reference.
    [[nodiscard]] static auto peek_block(const std::string& content, const std::string& block_id)
        -> std::optional<PeekContent>
    {
        auto lines = split_lines(content);
        const auto marker = " ^" + block_id;

        for (std::size_t idx = 0; idx < lines.size(); ++idx)
        {
            if (lines[idx].size() >= marker.size())
            {
                auto suffix = lines[idx].substr(lines[idx].size() - marker.size());
                if (suffix == marker)
                {
                    PeekContent peek_result;
                    peek_result.title = "^" + block_id;
                    peek_result.target_line = static_cast<int>(idx);
                    peek_result.content_type = PeekContentType::kBlockRef;

                    // Show context: 2 lines before + the block line + lines after
                    auto start = (idx >= 2) ? idx - 2 : 0;
                    peek_result.preview_text = extract_lines(lines, start, kMaxPreviewLines);
                    peek_result.line_count =
                        std::min(kMaxPreviewLines, static_cast<int>(lines.size() - start));
                    return peek_result;
                }
            }
        }
        return std::nullopt;
    }

private:
    /// Try to peek a [[wikilink]] under the cursor.
    [[nodiscard]] static auto
    try_wikilink_peek(const std::string& line, int character, const std::string& content)
        -> std::optional<PeekContent>
    {
        auto open_bracket = line.rfind("[[", static_cast<std::size_t>(character));
        if (open_bracket == std::string::npos)
        {
            return std::nullopt;
        }

        auto close_bracket = line.find("]]", open_bracket + 2);
        if (close_bracket == std::string::npos || static_cast<int>(close_bracket) < character)
        {
            return std::nullopt;
        }

        auto link_content = line.substr(open_bracket + 2, close_bracket - open_bracket - 2);

        // Parse target
        auto pipe_pos = link_content.find('|');
        auto target =
            (pipe_pos != std::string::npos) ? link_content.substr(0, pipe_pos) : link_content;

        // Check for heading ref
        auto hash_pos = target.find('#');
        if (hash_pos != std::string::npos)
        {
            auto heading = target.substr(hash_pos + 1);
            auto heading_peek = peek_heading(content, heading);
            if (heading_peek.has_value())
            {
                heading_peek->target_uri = target.substr(0, hash_pos);
                return heading_peek;
            }
        }

        // Check for block ref
        auto caret_pos = target.find('^');
        if (caret_pos != std::string::npos)
        {
            auto block_id = target.substr(caret_pos + 1);
            auto block_peek = peek_block(content, block_id);
            if (block_peek.has_value())
            {
                block_peek->target_uri = target.substr(0, caret_pos);
                return block_peek;
            }
        }

        // Just a document link — preview first 10 lines
        PeekContent peek_result;
        peek_result.title = target;
        peek_result.content_type = PeekContentType::kMarkdown;
        peek_result.target_uri = target;
        peek_result.preview_text = "(Link target: " + target + ")";
        peek_result.line_count = 1;
        return peek_result;
    }

    /// Try to peek a [text](#heading) under the cursor.
    [[nodiscard]] static auto
    try_heading_peek(const std::string& line, int character, const std::string& content)
        -> std::optional<PeekContent>
    {
        // Look for ](#heading) pattern around cursor
        auto pattern = line.rfind("](#", static_cast<std::size_t>(character));
        if (pattern == std::string::npos)
        {
            return std::nullopt;
        }

        auto paren_close = line.find(')', pattern + 3);
        if (paren_close == std::string::npos || static_cast<int>(paren_close) < character)
        {
            return std::nullopt;
        }

        auto heading_slug = line.substr(pattern + 3, paren_close - pattern - 3);
        return peek_heading(content, heading_slug);
    }

    /// Try to peek a [text](url) markdown link under the cursor.
    [[nodiscard]] static auto try_markdown_link_peek(const std::string& line, int character)
        -> std::optional<PeekContent>
    {
        auto bracket_close = line.rfind("](", static_cast<std::size_t>(character));
        if (bracket_close == std::string::npos)
        {
            return std::nullopt;
        }

        auto paren_close = line.find(')', bracket_close + 2);
        if (paren_close == std::string::npos || static_cast<int>(paren_close) < character)
        {
            return std::nullopt;
        }

        auto url = line.substr(bracket_close + 2, paren_close - bracket_close - 2);

        PeekContent peek_result;
        peek_result.title = url;
        peek_result.target_uri = url;
        peek_result.content_type = (url.find("://") != std::string::npos)
                                       ? PeekContentType::kMarkdown
                                       : PeekContentType::kCode;
        peek_result.preview_text = "(Link: " + url + ")";
        peek_result.line_count = 1;
        return peek_result;
    }

    /// Get a line at a specific 0-based index.
    [[nodiscard]] static auto get_line_at(const std::string& content, int line) -> std::string
    {
        int current_line = 0;
        std::size_t pos = 0;

        while (pos < content.size())
        {
            auto eol = content.find('\n', pos);
            if (eol == std::string::npos)
            {
                eol = content.size();
            }

            if (current_line == line)
            {
                return content.substr(pos, eol - pos);
            }

            pos = eol + 1;
            ++current_line;
        }
        return {};
    }

    /// Split content into lines.
    [[nodiscard]] static auto split_lines(const std::string& content) -> std::vector<std::string>
    {
        std::vector<std::string> lines;
        std::size_t pos = 0;
        while (pos < content.size())
        {
            auto eol = content.find('\n', pos);
            if (eol == std::string::npos)
            {
                lines.push_back(content.substr(pos));
                break;
            }
            lines.push_back(content.substr(pos, eol - pos));
            pos = eol + 1;
        }
        return lines;
    }

    /// Extract N lines starting from an index.
    [[nodiscard]] static auto extract_lines(const std::vector<std::string>& lines,
                                            std::size_t start,
                                            int count) -> std::string
    {
        std::string result;
        auto end_idx = std::min(start + static_cast<std::size_t>(count), lines.size());
        for (auto idx = start; idx < end_idx; ++idx)
        {
            if (idx > start)
            {
                result += '\n';
            }
            result += lines[idx];
        }
        return result;
    }

    /// Slugify heading text.
    [[nodiscard]] static auto slugify(const std::string& text) -> std::string
    {
        std::string slug;
        bool prev_was_dash = true;
        for (const char ch : text)
        {
            if (std::isalnum(static_cast<unsigned char>(ch)) != 0)
            {
                slug += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
                prev_was_dash = false;
            }
            else if (!prev_was_dash && (ch == ' ' || ch == '-' || ch == '_'))
            {
                slug += '-';
                prev_was_dash = true;
            }
        }
        if (!slug.empty() && slug.back() == '-')
        {
            slug.pop_back();
        }
        return slug;
    }

    /// Extract heading slug from a heading line.
    [[nodiscard]] static auto extract_heading_slug(const std::string& trimmed_line) -> std::string
    {
        std::size_t idx = 0;
        while (idx < trimmed_line.size() && trimmed_line[idx] == '#')
        {
            ++idx;
        }
        if (idx >= trimmed_line.size() || trimmed_line[idx] != ' ')
        {
            return {};
        }
        ++idx;
        auto heading_text = trimmed_line.substr(idx);
        while (!heading_text.empty() && (heading_text.back() == '#' || heading_text.back() == ' ' ||
                                         heading_text.back() == '\t'))
        {
            heading_text.pop_back();
        }
        return slugify(heading_text);
    }

    /// Trim leading whitespace.
    [[nodiscard]] static auto trim_left(const std::string& str) -> std::string
    {
        auto start = str.find_first_not_of(" \t");
        if (start == std::string::npos)
        {
            return {};
        }
        return str.substr(start);
    }
};

} // namespace markamp::core
