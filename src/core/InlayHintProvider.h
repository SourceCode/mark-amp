#pragma once

/// @file InlayHintProvider.h
/// @brief V9 Phase 19 — Inlay hints for markdown documents.
///
/// Provides inline hints: word count after headings, link target hints,
/// image dimension hints, and table column count hints.

#include "CodeIntelligenceTypes.h"

#include <sstream>
#include <string>
#include <vector>

namespace markamp::core
{

/// Header-only provider for inlay hints in markdown documents.
///
/// Usage:
/// ```cpp
/// InlayHintProvider provider;
/// auto hints = provider.provide_hints("# Section\n\nSome text here.");
/// // hints[0] = word count hint after heading
/// ```
class InlayHintProvider
{
public:
    InlayHintProvider() = default;

    /// Enable/disable individual hint kinds.
    void set_word_count_enabled(bool enabled)
    {
        word_count_enabled_ = enabled;
    }
    void set_link_target_enabled(bool enabled)
    {
        link_target_enabled_ = enabled;
    }
    void set_image_info_enabled(bool enabled)
    {
        image_info_enabled_ = enabled;
    }
    void set_table_info_enabled(bool enabled)
    {
        table_info_enabled_ = enabled;
    }

    /// Provide inlay hints for the document or a range.
    /// @param content Full document content
    /// @param start_line Start of range (0-based, default 0)
    /// @param end_line End of range (-1 = end of document)
    /// @return Vector of InlayHintInfo entries
    [[nodiscard]] auto provide_hints(const std::string& content,
                                     int start_line = 0,
                                     int end_line = -1) const -> std::vector<InlayHintInfo>
    {
        std::vector<InlayHintInfo> hints;
        std::istringstream stream(content);
        std::string line_text;
        int line_num = 0;
        bool in_table = false;
        int table_start_line = 0;
        int table_column_count = 0;

        while (std::getline(stream, line_text))
        {
            if (end_line >= 0 && line_num > end_line)
            {
                break;
            }

            if (line_num >= start_line)
            {
                // Heading word count hints
                if (word_count_enabled_ && !line_text.empty() && line_text[0] == '#')
                {
                    auto heading_hint = make_heading_hint(line_text, line_num, content);
                    if (!heading_hint.label.empty())
                    {
                        hints.push_back(std::move(heading_hint));
                    }
                }

                // Link target hints
                if (link_target_enabled_)
                {
                    auto link_hints = make_link_hints(line_text, line_num);
                    for (auto& hint : link_hints)
                    {
                        hints.push_back(std::move(hint));
                    }
                }

                // Image info hints
                if (image_info_enabled_)
                {
                    auto image_hints = make_image_hints(line_text, line_num);
                    for (auto& hint : image_hints)
                    {
                        hints.push_back(std::move(hint));
                    }
                }

                // Table detection and column count
                if (table_info_enabled_)
                {
                    const bool is_table_line = !line_text.empty() && line_text[0] == '|';
                    if (is_table_line && !in_table)
                    {
                        in_table = true;
                        table_start_line = line_num;
                        table_column_count = count_table_columns(line_text);
                    }
                    else if (!is_table_line && in_table)
                    {
                        in_table = false;
                        InlayHintInfo hint;
                        hint.line = table_start_line;
                        hint.character =
                            static_cast<int>(get_line_at(content, table_start_line).size());
                        hint.label = " " + std::to_string(table_column_count) + " cols";
                        hint.kind = InlayHintKind::kTableColumns;
                        hint.padding_left = true;
                        hints.push_back(std::move(hint));
                    }
                }
            }

            ++line_num;
        }

        // Handle table at end of document
        if (in_table && table_info_enabled_)
        {
            InlayHintInfo hint;
            hint.line = table_start_line;
            hint.character = static_cast<int>(get_line_at(content, table_start_line).size());
            hint.label = " " + std::to_string(table_column_count) + " cols";
            hint.kind = InlayHintKind::kTableColumns;
            hint.padding_left = true;
            hints.push_back(std::move(hint));
        }

        return hints;
    }

private:
    bool word_count_enabled_{true};
    bool link_target_enabled_{true};
    bool image_info_enabled_{true};
    bool table_info_enabled_{true};

    /// Create a word count hint for a heading line.
    [[nodiscard]] static auto make_heading_hint(const std::string& line_text,
                                                int line_num,
                                                const std::string& content) -> InlayHintInfo
    {
        int level = 0;
        while (level < static_cast<int>(line_text.size()) &&
               line_text[static_cast<std::string::size_type>(level)] == '#')
        {
            ++level;
        }

        if (level > 6 || level >= static_cast<int>(line_text.size()) ||
            line_text[static_cast<std::string::size_type>(level)] != ' ')
        {
            return {};
        }

        auto word_count = count_section_words(content, line_num, level);

        InlayHintInfo hint;
        hint.line = line_num;
        hint.character = static_cast<int>(line_text.size());
        hint.label = " (" + std::to_string(word_count) + " words)";
        hint.kind = InlayHintKind::kWordCount;
        hint.padding_left = true;
        return hint;
    }

    /// Create link target hints for links in a line.
    [[nodiscard]] static auto make_link_hints(const std::string& line_text, int line_num)
        -> std::vector<InlayHintInfo>
    {
        std::vector<InlayHintInfo> hints;
        std::string::size_type pos = 0;

        while (pos < line_text.size())
        {
            // Find [text](url) — skip images
            if (line_text[pos] == '[' && (pos == 0 || line_text[pos - 1] != '!'))
            {
                auto close_bracket = line_text.find(']', pos + 1);
                if (close_bracket != std::string::npos && close_bracket + 1 < line_text.size() &&
                    line_text[close_bracket + 1] == '(')
                {
                    auto close_paren = line_text.find(')', close_bracket + 2);
                    if (close_paren != std::string::npos)
                    {
                        auto url =
                            line_text.substr(close_bracket + 2, close_paren - close_bracket - 2);
                        // Shorten long URLs
                        auto shortened = shorten_url(url);
                        if (shortened != url)
                        {
                            InlayHintInfo hint;
                            hint.line = line_num;
                            hint.character = static_cast<int>(close_paren + 1);
                            hint.label = " → " + shortened;
                            hint.kind = InlayHintKind::kLinkTarget;
                            hint.padding_left = true;
                            hints.push_back(std::move(hint));
                        }
                        pos = close_paren + 1;
                        continue;
                    }
                }
            }
            ++pos;
        }

        return hints;
    }

    /// Create image info hints on a line.
    [[nodiscard]] static auto make_image_hints(const std::string& line_text, int line_num)
        -> std::vector<InlayHintInfo>
    {
        std::vector<InlayHintInfo> hints;
        std::string::size_type pos = 0;

        while (pos < line_text.size())
        {
            if (pos + 1 < line_text.size() && line_text[pos] == '!' && line_text[pos + 1] == '[')
            {
                auto close_bracket = line_text.find(']', pos + 2);
                if (close_bracket != std::string::npos && close_bracket + 1 < line_text.size() &&
                    line_text[close_bracket + 1] == '(')
                {
                    auto close_paren = line_text.find(')', close_bracket + 2);
                    if (close_paren != std::string::npos)
                    {
                        auto alt_text = line_text.substr(pos + 2, close_bracket - pos - 2);
                        auto url =
                            line_text.substr(close_bracket + 2, close_paren - close_bracket - 2);

                        // Extract file extension for the hint
                        auto ext = get_file_extension(url);

                        InlayHintInfo hint;
                        hint.line = line_num;
                        hint.character = static_cast<int>(close_paren + 1);
                        hint.label = " [" + ext + "]";
                        if (!alt_text.empty())
                        {
                            hint.label += " alt: " + alt_text;
                        }
                        hint.kind = InlayHintKind::kImageSize;
                        hint.padding_left = true;
                        hints.push_back(std::move(hint));
                        pos = close_paren + 1;
                        continue;
                    }
                }
            }
            ++pos;
        }

        return hints;
    }

    /// Count words in a section until the next heading of same or higher level.
    [[nodiscard]] static auto
    count_section_words(const std::string& content, int heading_line, int heading_level) -> int
    {
        std::istringstream stream(content);
        std::string current_line;
        int current = 0;
        int word_count = 0;
        bool in_section = false;

        while (std::getline(stream, current_line))
        {
            if (current == heading_line)
            {
                in_section = true;
                ++current;
                continue;
            }

            if (in_section)
            {
                if (!current_line.empty() && current_line[0] == '#')
                {
                    int level = 0;
                    while (level < static_cast<int>(current_line.size()) &&
                           current_line[static_cast<std::string::size_type>(level)] == '#')
                    {
                        ++level;
                    }
                    if (level <= heading_level)
                    {
                        break;
                    }
                }

                std::istringstream word_stream(current_line);
                std::string word;
                while (word_stream >> word)
                {
                    ++word_count;
                }
            }
            ++current;
        }

        return word_count;
    }

    /// Count pipe-delimited columns in a table line.
    [[nodiscard]] static auto count_table_columns(const std::string& line_text) -> int
    {
        int count = 0;
        for (const char chr : line_text)
        {
            if (chr == '|')
            {
                ++count;
            }
        }
        // Pipes at start and end don't count as extra columns
        return (count > 1) ? count - 1 : 1;
    }

    /// Shorten a URL for display (e.g., "https://example.com/path/to/page" → "example.com/…/page")
    [[nodiscard]] static auto shorten_url(const std::string& url) -> std::string
    {
        if (url.size() <= 40)
        {
            return url;
        }

        // Remove protocol
        auto protocol_end = url.find("://");
        std::string::size_type start = 0;
        if (protocol_end != std::string::npos)
        {
            start = protocol_end + 3;
        }

        auto remaining = url.substr(start);
        if (remaining.size() <= 35)
        {
            return remaining;
        }

        // Show domain + last path segment
        auto first_slash = remaining.find('/');
        if (first_slash == std::string::npos)
        {
            return remaining.substr(0, 35) + "…";
        }

        auto domain = remaining.substr(0, first_slash);
        auto last_slash = remaining.rfind('/');
        if (last_slash != std::string::npos && last_slash > first_slash)
        {
            return domain + "/…" + remaining.substr(last_slash);
        }

        return remaining.substr(0, 35) + "…";
    }

    /// Get file extension from a URL/path.
    [[nodiscard]] static auto get_file_extension(const std::string& url) -> std::string
    {
        auto query_pos = url.find('?');
        auto clean_url = (query_pos != std::string::npos) ? url.substr(0, query_pos) : url;

        auto dot_pos = clean_url.rfind('.');
        if (dot_pos == std::string::npos || dot_pos == clean_url.size() - 1)
        {
            return "img";
        }

        return clean_url.substr(dot_pos + 1);
    }

    /// Get line text at a specific line number.
    [[nodiscard]] static auto get_line_at(const std::string& content, int line) -> std::string
    {
        std::istringstream stream(content);
        std::string current_line;
        int current = 0;
        while (std::getline(stream, current_line))
        {
            if (current == line)
            {
                return current_line;
            }
            ++current;
        }
        return "";
    }
};

} // namespace markamp::core
