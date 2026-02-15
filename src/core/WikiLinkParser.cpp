#include "core/WikiLinkParser.h"

#include <algorithm>
#include <ranges>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// Code range detection helpers
// ============================================================================

namespace
{

/// Scan past a fenced code block starting at `pos`.
/// Returns the position after the closing fence, or `content.size()` if unclosed.
auto scan_fenced_block(std::string_view content,
                       std::size_t pos,
                       char fence_char,
                       std::size_t fence_len,
                       std::vector<std::pair<std::size_t, std::size_t>>& ranges) -> std::size_t
{
    const std::size_t block_start = pos - fence_len;
    const auto content_len = content.size();

    // Skip info string (rest of opening fence line)
    while (pos < content_len && content[pos] != '\n')
    {
        ++pos;
    }
    if (pos < content_len)
    {
        ++pos; // skip newline
    }

    // Find closing fence
    while (pos < content_len)
    {
        if (content[pos] == fence_char)
        {
            std::size_t close_len = 0;
            while (pos < content_len && content[pos] == fence_char)
            {
                ++close_len;
                ++pos;
            }
            if (close_len >= fence_len)
            {
                ranges.emplace_back(block_start, pos);
                return pos;
            }
        }
        else
        {
            ++pos;
        }
    }

    // Unclosed — treat rest as code
    ranges.emplace_back(block_start, content_len);
    return content_len;
}

/// Scan past an inline code span starting at `pos`.
/// Returns the position after the closing backticks, or `original_pos + backtick_len` on failure.
auto scan_inline_code(std::string_view content,
                      std::size_t pos,
                      std::size_t backtick_len,
                      std::size_t code_start,
                      std::vector<std::pair<std::size_t, std::size_t>>& ranges) -> std::size_t
{
    const auto content_len = content.size();

    while (pos < content_len)
    {
        if (content[pos] == '`')
        {
            std::size_t close_len = 0;
            while (pos < content_len && content[pos] == '`')
            {
                ++close_len;
                ++pos;
            }
            if (close_len == backtick_len)
            {
                ranges.emplace_back(code_start, pos);
                return pos;
            }
        }
        else
        {
            ++pos;
        }
    }

    // Unclosed — treat backticks as literal
    return code_start + backtick_len;
}

} // anonymous namespace

auto WikiLinkParser::find_code_ranges(std::string_view content)
    -> std::vector<std::pair<std::size_t, std::size_t>>
{
    std::vector<std::pair<std::size_t, std::size_t>> ranges;
    std::size_t pos = 0;
    const auto content_len = content.size();

    while (pos < content_len)
    {
        if (content[pos] == '`' || content[pos] == '~')
        {
            const char fence_char = content[pos];
            const std::size_t fence_start = pos;
            std::size_t fence_len = 0;
            while (pos < content_len && content[pos] == fence_char)
            {
                ++fence_len;
                ++pos;
            }

            if (fence_len >= 3)
            {
                pos = scan_fenced_block(content, pos, fence_char, fence_len, ranges);
            }
            else if (fence_char == '`' && fence_len > 0)
            {
                pos = scan_inline_code(content, pos, fence_len, fence_start, ranges);
            }
        }
        else
        {
            ++pos;
        }
    }

    return ranges;
}

auto WikiLinkParser::is_in_code_block(
    std::string_view /*content*/,
    std::size_t pos,
    const std::vector<std::pair<std::size_t, std::size_t>>& code_ranges) -> bool
{
    return std::ranges::any_of(
        code_ranges, [pos](const auto& range) { return pos >= range.first && pos < range.second; });
}

// ============================================================================
// WikiLink parsing
// ============================================================================

auto WikiLinkParser::parse_links(std::string_view content) const -> std::vector<WikiLink>
{
    std::vector<WikiLink> links;
    const auto code_ranges = find_code_ranges(content);
    const auto content_len = content.size();

    int current_line = 1;
    std::size_t line_start = 0;

    for (std::size_t idx = 0; idx < content_len; ++idx)
    {
        if (content[idx] == '\n')
        {
            ++current_line;
            line_start = idx + 1;
            continue;
        }

        if (is_in_code_block(content, idx, code_ranges))
        {
            continue;
        }

        // Check for ![[embed]] or [[link]]
        bool is_embed = false;
        if (content[idx] == '!' && idx + 2 < content_len && content[idx + 1] == '[' &&
            content[idx + 2] == '[')
        {
            is_embed = true;
        }
        else if (content[idx] == '[' && idx + 1 < content_len && content[idx + 1] == '[')
        {
            if (idx > 0 && content[idx - 1] == '!')
            {
                continue; // Already handled as embed
            }
        }
        else
        {
            continue;
        }

        const std::size_t link_text_start = is_embed ? idx + 3 : idx + 2;

        // Find closing ]]
        std::size_t scan = link_text_start;
        bool found_close = false;
        while (scan + 1 < content_len)
        {
            if (content[scan] == ']' && content[scan + 1] == ']')
            {
                found_close = true;
                break;
            }
            if (content[scan] == '\n')
            {
                break;
            }
            ++scan;
        }

        if (!found_close)
        {
            continue;
        }

        const std::string link_content(content.substr(link_text_start, scan - link_text_start));

        WikiLink link;
        link.is_embed = is_embed;
        link.source_line = current_line;
        link.source_col = static_cast<int>(idx - line_start);

        // Parse: target|display_text
        const std::size_t pipe_pos = link_content.find('|');
        std::string target_part;
        if (pipe_pos != std::string::npos)
        {
            target_part = link_content.substr(0, pipe_pos);
            link.display_text = link_content.substr(pipe_pos + 1);
        }
        else
        {
            target_part = link_content;
        }

        // Parse: target#heading^block_ref
        const std::size_t caret_pos = target_part.find('^');
        if (caret_pos != std::string::npos)
        {
            link.block_ref = target_part.substr(caret_pos + 1);
            target_part = target_part.substr(0, caret_pos);
        }

        const std::size_t hash_pos = target_part.find('#');
        if (hash_pos != std::string::npos)
        {
            link.heading = target_part.substr(hash_pos + 1);
            target_part = target_part.substr(0, hash_pos);
        }

        link.target = target_part;
        links.push_back(std::move(link));

        idx = scan + 1; // Loop will increment
    }

    return links;
}

// ============================================================================
// Tag parsing
// ============================================================================

auto WikiLinkParser::parse_tags(std::string_view content) const -> std::vector<ParsedTag>
{
    std::vector<ParsedTag> parsed_tags;
    const auto code_ranges = find_code_ranges(content);
    const auto content_len = content.size();

    int current_line = 1;
    std::size_t line_start = 0;

    for (std::size_t idx = 0; idx < content_len; ++idx)
    {
        if (content[idx] == '\n')
        {
            ++current_line;
            line_start = idx + 1;
            continue;
        }

        if (content[idx] != '#')
        {
            continue;
        }

        if (is_in_code_block(content, idx, code_ranges))
        {
            continue;
        }

        // Tags must be preceded by whitespace or start of content
        if (idx > 0 && content[idx - 1] != ' ' && content[idx - 1] != '\t' &&
            content[idx - 1] != '\n' && content[idx - 1] != '\r')
        {
            continue;
        }

        if (idx + 1 >= content_len)
        {
            continue;
        }

        const char next_char = content[idx + 1];
        if (std::isalnum(static_cast<unsigned char>(next_char)) == 0 && next_char != '_' &&
            next_char != '-')
        {
            continue;
        }

        // Collect tag text
        const std::size_t tag_start = idx + 1;
        std::size_t scan = tag_start;
        while (scan < content_len)
        {
            const char tag_ch = content[scan];
            if (std::isalnum(static_cast<unsigned char>(tag_ch)) != 0 || tag_ch == '_' ||
                tag_ch == '-' || tag_ch == '/')
            {
                ++scan;
            }
            else
            {
                break;
            }
        }

        if (scan == tag_start)
        {
            continue;
        }

        std::string tag_text(content.substr(tag_start, scan - tag_start));

        // Don't include trailing slashes
        while (!tag_text.empty() && tag_text.back() == '/')
        {
            tag_text.pop_back();
        }

        if (tag_text.empty())
        {
            continue;
        }

        ParsedTag tag;
        tag.full_tag = tag_text;
        tag.source_line = current_line;
        tag.source_col = static_cast<int>(idx - line_start);

        // Split on '/' for hierarchy
        std::istringstream stream(tag_text);
        std::string part;
        while (std::getline(stream, part, '/'))
        {
            if (!part.empty())
            {
                tag.parts.push_back(std::move(part));
            }
        }

        parsed_tags.push_back(std::move(tag));

        idx = scan - 1; // Loop will increment
    }

    return parsed_tags;
}

// ============================================================================
// Frontmatter parsing
// ============================================================================

auto WikiLinkParser::parse_frontmatter(std::string_view content) const -> std::optional<Frontmatter>
{
    // Frontmatter must start at the very beginning of the document
    if (content.size() < 4 || content.substr(0, 3) != "---")
    {
        return std::nullopt;
    }

    // Check that --- is followed by newline
    std::size_t pos = 3;
    if (pos < content.size() && content[pos] == '\r')
    {
        ++pos;
    }
    if (pos >= content.size() || content[pos] != '\n')
    {
        return std::nullopt;
    }
    ++pos; // skip newline

    // Find closing ---
    const std::size_t fm_start = pos;
    std::size_t close_pos = std::string_view::npos;

    while (pos < content.size())
    {
        if (content.substr(pos, 3) == "---")
        {
            const std::size_t after = pos + 3;
            if (after >= content.size() || content[after] == '\n' || content[after] == '\r')
            {
                close_pos = pos;
                break;
            }
        }
        while (pos < content.size() && content[pos] != '\n')
        {
            ++pos;
        }
        if (pos < content.size())
        {
            ++pos;
        }
    }

    if (close_pos == std::string_view::npos)
    {
        return std::nullopt;
    }

    // Parse YAML content
    const std::string yaml_content(content.substr(fm_start, close_pos - fm_start));

    Frontmatter frontmatter;

    std::istringstream yaml_stream(yaml_content);
    std::string line;
    std::string current_key;
    bool in_list = false;

    while (std::getline(yaml_stream, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        // Check for list item
        std::size_t indent = 0;
        while (indent < line.size() && line[indent] == ' ')
        {
            ++indent;
        }

        if (indent >= 2 && indent < line.size() && line[indent] == '-' &&
            indent + 1 < line.size() && line[indent + 1] == ' ')
        {
            if (in_list)
            {
                std::string item = line.substr(indent + 2);
                while (!item.empty() && (item.back() == ' ' || item.back() == '\t'))
                {
                    item.pop_back();
                }
                if (current_key == "tags")
                {
                    frontmatter.tags.push_back(item);
                }
                else if (current_key == "aliases")
                {
                    frontmatter.aliases.push_back(item);
                }
            }
            continue;
        }

        in_list = false;

        const auto colon_pos = line.find(':');
        if (colon_pos == std::string::npos)
        {
            continue;
        }

        std::string key = line.substr(0, colon_pos);
        while (!key.empty() && (key.front() == ' ' || key.front() == '\t'))
        {
            key.erase(key.begin());
        }
        while (!key.empty() && (key.back() == ' ' || key.back() == '\t'))
        {
            key.pop_back();
        }

        std::string value;
        if (colon_pos + 1 < line.size())
        {
            value = line.substr(colon_pos + 1);
            while (!value.empty() && (value.front() == ' ' || value.front() == '\t'))
            {
                value.erase(value.begin());
            }
            while (!value.empty() && (value.back() == ' ' || value.back() == '\t'))
            {
                value.pop_back();
            }
        }

        current_key = key;

        if (value.empty() && (key == "tags" || key == "aliases"))
        {
            in_list = true;
            continue;
        }

        // Handle inline array syntax: [tag1, tag2]
        if (!value.empty() && value.front() == '[' && value.back() == ']')
        {
            const std::string inner = value.substr(1, value.size() - 2);
            std::istringstream item_stream(inner);
            std::string item;
            while (std::getline(item_stream, item, ','))
            {
                while (!item.empty() && (item.front() == ' ' || item.front() == '\t'))
                {
                    item.erase(item.begin());
                }
                while (!item.empty() && (item.back() == ' ' || item.back() == '\t'))
                {
                    item.pop_back();
                }
                if (!item.empty())
                {
                    if (key == "tags")
                    {
                        frontmatter.tags.push_back(item);
                    }
                    else if (key == "aliases")
                    {
                        frontmatter.aliases.push_back(item);
                    }
                }
            }
            continue;
        }

        // Assign known fields
        if (key == "title")
        {
            frontmatter.title = value;
        }
        else if (key == "date")
        {
            frontmatter.date = value;
        }
        else if (key == "cssclass")
        {
            frontmatter.css_class = value;
        }
        else if (key == "publish")
        {
            frontmatter.publish = (value == "true" || value == "1" || value == "yes");
        }
        else if (key != "tags" && key != "aliases")
        {
            frontmatter.string_fields[key] = value;
        }
    }

    return frontmatter;
}

// ============================================================================
// Link resolution
// ============================================================================

auto WikiLinkParser::resolve_link(const WikiLink& link,
                                  const std::vector<std::string>& vault_paths) const
    -> std::optional<std::string>
{
    if (link.target.empty())
    {
        return std::nullopt;
    }

    const std::string target_lower = [&]()
    {
        std::string lower = link.target;
        std::transform(lower.begin(),
                       lower.end(),
                       lower.begin(),
                       [](unsigned char chr) { return std::tolower(chr); });
        return lower;
    }();

    // Try exact filename match first (shortest path)
    for (const auto& path : vault_paths)
    {
        std::string filename = path;
        const auto last_slash = filename.rfind('/');
        if (last_slash != std::string::npos)
        {
            filename = filename.substr(last_slash + 1);
        }
        const auto dot_pos = filename.rfind('.');
        if (dot_pos != std::string::npos)
        {
            filename = filename.substr(0, dot_pos);
        }

        std::string filename_lower = filename;
        std::transform(filename_lower.begin(),
                       filename_lower.end(),
                       filename_lower.begin(),
                       [](unsigned char chr) { return std::tolower(chr); });

        if (filename_lower == target_lower)
        {
            return path;
        }
    }

    // Try path-based match (target includes partial path)
    for (const auto& path : vault_paths)
    {
        std::string path_no_ext = path;
        const auto dot_pos = path_no_ext.rfind('.');
        if (dot_pos != std::string::npos)
        {
            path_no_ext = path_no_ext.substr(0, dot_pos);
        }

        std::string path_lower = path_no_ext;
        std::transform(path_lower.begin(),
                       path_lower.end(),
                       path_lower.begin(),
                       [](unsigned char chr) { return std::tolower(chr); });

        if (path_lower.ends_with(target_lower))
        {
            if (path_lower.size() == target_lower.size() ||
                path_lower[path_lower.size() - target_lower.size() - 1] == '/')
            {
                return path;
            }
        }
    }

    return std::nullopt;
}

} // namespace markamp::core
