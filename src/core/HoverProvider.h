#pragma once

/// @file HoverProvider.h
/// @brief V9 Phase 19 — Hover information provider for markdown elements.
///
/// Provides contextual hover info for links, headings, wiki-links, code spans,
/// footnotes, images, and emoji shortcodes.

#include "CodeIntelligenceTypes.h"

#include <optional>
#include <string>

namespace markamp::core
{

/// Provider for hover information over markdown elements.
///
/// Given a cursor position and document content, identifies the element under
/// the cursor and returns formatted hover content.
///
/// Usage:
/// ```cpp
/// HoverProvider provider;
/// auto hover = provider.provide_hover("# Hello\n\n[link](url)", 2, 3);
/// if (hover) {
///     // hover->markdown contains formatted info
/// }
/// ```
class HoverProvider
{
public:
    HoverProvider() = default;

    /// Provide hover information at the given position.
    /// @param content Full document content
    /// @param line Cursor line (0-based)
    /// @param character Cursor character offset (0-based)
    /// @return HoverContent if an element was found under cursor, nullopt otherwise
    [[nodiscard]] auto provide_hover(const std::string& content, int line, int character) const
        -> std::optional<HoverContent>;

    /// Enable/disable link target resolution in hover.
    void set_resolve_links(bool enabled);

    /// Enable/disable image dimension hints in hover.
    void set_show_image_info(bool enabled);

private:
    bool resolve_links_{true};
    bool show_image_info_{true};

    /// Check if cursor is over a markdown link [text](url) and return hover.
    [[nodiscard]] static auto hover_link(const std::string& line_text, int character, int line)
        -> std::optional<HoverContent>;

    /// Check if cursor is over a wiki-link [[target]] and return hover.
    [[nodiscard]] static auto hover_wiki_link(const std::string& line_text, int character, int line)
        -> std::optional<HoverContent>;

    /// Check if cursor is over a heading and return hover.
    [[nodiscard]] static auto
    hover_heading(const std::string& line_text, int line, const std::string& content)
        -> std::optional<HoverContent>;

    /// Check if cursor is over an image ![alt](url) and return hover.
    [[nodiscard]] static auto hover_image(const std::string& line_text, int character, int line)
        -> std::optional<HoverContent>;

    /// Check if cursor is over a footnote reference [^id] and return hover.
    [[nodiscard]] static auto hover_footnote(const std::string& line_text,
                                             int character,
                                             int line,
                                             const std::string& content)
        -> std::optional<HoverContent>;

    /// Check if cursor is over an inline code span and return hover.
    [[nodiscard]] static auto hover_code_span(const std::string& line_text, int character, int line)
        -> std::optional<HoverContent>;

    /// Check if cursor is over an emoji shortcode :name: and return hover.
    [[nodiscard]] static auto hover_emoji(const std::string& line_text, int character, int line)
        -> std::optional<HoverContent>;

    /// Get the line text at a given line number.
    [[nodiscard]] static auto get_line_text(const std::string& content, int line) -> std::string;

    /// Count words in content below a heading until the next heading of same/higher level.
    [[nodiscard]] static auto
    count_section_words(const std::string& content, int heading_line, int heading_level) -> int;

    /// Find footnote definition text for a given footnote id.
    [[nodiscard]] static auto find_footnote_definition(const std::string& content,
                                                       const std::string& footnote_id)
        -> std::string;
};

} // namespace markamp::core
