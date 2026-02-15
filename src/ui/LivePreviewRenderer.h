#pragma once

/// @file LivePreviewRenderer.h
/// @brief V4 Phase 13 – Live Preview / WYSIWYG inline rendering.

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{
class ThemeEngine;
}

namespace markamp::ui
{

/// Describes how a line should be rendered in Live Preview mode.
struct LivePreviewLine
{
    int line_number{0};
    bool is_cursor_line{false}; ///< Show raw markdown on this line
    std::string raw_text;       ///< Original markdown
    std::string display_text;   ///< Rendered display text (stripped markers)

    /// Inline style runs for rendering.
    struct StyleRun
    {
        int start{0};
        int length{0};
        enum class Style : uint8_t
        {
            Normal,
            Bold,
            Italic,
            BoldItalic,
            Code,
            Strikethrough,
            Link,
            LinkUrl,
            Heading1,
            Heading2,
            Heading3,
            Heading4,
            Heading5,
            Heading6,
            BlockquoteMarker,
            ListMarker,
            CheckboxUnchecked,
            CheckboxChecked,
            Tag,
            WikiLink,
            MathInline,
            Highlight,
            InternalLink
        } style{Style::Normal};
    };
    std::vector<StyleRun> styles;

    /// Folding: hide markdown syntax characters.
    struct FoldedRange
    {
        int raw_start{0};  ///< Position in raw_text
        int raw_length{0}; ///< Characters to hide
    };
    std::vector<FoldedRange> folded_ranges;
};

/// Converts markdown lines into live preview rendering instructions.
/// This is a pure transform: markdown string → rendering instructions.
/// No dependency on VaultService or BacklinkIndex.
class LivePreviewRenderer
{
public:
    LivePreviewRenderer() = default;

    /// Process a full document and generate live preview lines.
    [[nodiscard]] auto process(const std::string& markdown, int cursor_line) const
        -> std::vector<LivePreviewLine>;

    /// Process a single line (for incremental updates).
    [[nodiscard]] auto process_line(const std::string& line,
                                    int line_number,
                                    bool is_cursor_line) const -> LivePreviewLine;

    /// Check if a character position is inside a folded range.
    [[nodiscard]] static auto is_folded(const LivePreviewLine& line, int position) -> bool;

    /// Detect heading level (1-6). Returns 0 if not a heading.
    [[nodiscard]] static auto detect_heading_level(const std::string& text) -> int;

private:
    [[nodiscard]] auto detect_inline_styles(const std::string& text) const
        -> std::vector<LivePreviewLine::StyleRun>;

    [[nodiscard]] auto compute_folded_ranges(const std::string& text) const
        -> std::vector<LivePreviewLine::FoldedRange>;

    [[nodiscard]] auto
    build_display_text(const std::string& raw,
                       const std::vector<LivePreviewLine::FoldedRange>& folds) const -> std::string;

    [[nodiscard]] auto detect_cursor_line_styles(const std::string& text) const
        -> std::vector<LivePreviewLine::StyleRun>;
};

} // namespace markamp::ui
