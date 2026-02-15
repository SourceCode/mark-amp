/// @file LivePreviewRenderer.cpp
/// @brief V4 Phase 13 – Live Preview / WYSIWYG inline rendering implementation.

#include "ui/LivePreviewRenderer.h"

#include <algorithm>
#include <array>
#include <regex>
#include <sstream>

namespace markamp::ui
{

// ============================================================================
// Public API
// ============================================================================

auto LivePreviewRenderer::process(const std::string& markdown, int cursor_line) const
    -> std::vector<LivePreviewLine>
{
    std::vector<LivePreviewLine> result;
    std::istringstream stream(markdown);
    std::string line;
    int line_num = 0;
    bool in_code_block = false;

    while (std::getline(stream, line))
    {
        // Track fenced code blocks — don't process lines inside them
        if (line.size() >= 3 && line.substr(0, 3) == "```")
        {
            in_code_block = !in_code_block;
            LivePreviewLine lpl;
            lpl.line_number = line_num;
            lpl.is_cursor_line = (line_num == cursor_line);
            lpl.raw_text = line;
            lpl.display_text = line;
            // Code fence lines are shown as-is
            result.push_back(std::move(lpl));
            ++line_num;
            continue;
        }

        if (in_code_block)
        {
            // Lines inside code blocks get no processing
            LivePreviewLine lpl;
            lpl.line_number = line_num;
            lpl.is_cursor_line = (line_num == cursor_line);
            lpl.raw_text = line;
            lpl.display_text = line;
            LivePreviewLine::StyleRun run;
            run.start = 0;
            run.length = static_cast<int>(line.size());
            run.style = LivePreviewLine::StyleRun::Style::Code;
            lpl.styles.push_back(run);
            result.push_back(std::move(lpl));
            ++line_num;
            continue;
        }

        result.push_back(process_line(line, line_num, line_num == cursor_line));
        ++line_num;
    }

    return result;
}

auto LivePreviewRenderer::process_line(const std::string& line,
                                       int line_number,
                                       bool is_cursor_line) const -> LivePreviewLine
{
    LivePreviewLine result;
    result.line_number = line_number;
    result.is_cursor_line = is_cursor_line;
    result.raw_text = line;

    if (is_cursor_line)
    {
        // Cursor line: show raw markdown with syntax highlighting
        result.display_text = line;
        result.styles = detect_cursor_line_styles(line);
        // No folded ranges on cursor line
        return result;
    }

    // Non-cursor line: fold markers, compute display text, apply styles
    auto folds = compute_folded_ranges(line);
    result.folded_ranges = folds;
    result.display_text = build_display_text(line, folds);

    // Detect styles on display text but use raw text for heading check
    // (heading markers get folded, so display_text won't start with #)
    const int heading = detect_heading_level(line);
    if (heading > 0)
    {
        LivePreviewLine::StyleRun run;
        run.start = 0;
        run.length = static_cast<int>(result.display_text.size());
        const std::array<LivePreviewLine::StyleRun::Style, 6> hs = {
            LivePreviewLine::StyleRun::Style::Heading1,
            LivePreviewLine::StyleRun::Style::Heading2,
            LivePreviewLine::StyleRun::Style::Heading3,
            LivePreviewLine::StyleRun::Style::Heading4,
            LivePreviewLine::StyleRun::Style::Heading5,
            LivePreviewLine::StyleRun::Style::Heading6,
        };
        run.style = hs[static_cast<size_t>(heading - 1)];
        result.styles.push_back(run);
    }
    else
    {
        // Inline styles are detected on display text (markers already removed
        // from display_text by folding, so we detect on raw text's markers)
        result.styles = detect_inline_styles(line);
    }

    return result;
}

auto LivePreviewRenderer::is_folded(const LivePreviewLine& line, int position) -> bool
{
    for (const auto& fold : line.folded_ranges)
    {
        if (position >= fold.raw_start && position < fold.raw_start + fold.raw_length)
        {
            return true;
        }
    }
    return false;
}

auto LivePreviewRenderer::detect_heading_level(const std::string& text) -> int
{
    if (text.empty() || text[0] != '#')
    {
        return 0;
    }

    int level = 0;
    for (char character : text)
    {
        if (character == '#')
        {
            ++level;
        }
        else
        {
            break;
        }
    }

    // Must have a space after the # characters, or be just '#' chars
    if (level > 6 || level == 0)
    {
        return 0;
    }

    if (static_cast<size_t>(level) < text.size() && text[static_cast<size_t>(level)] != ' ')
    {
        return 0;
    }

    return level;
}

// ============================================================================
// Private: Inline style detection
// ============================================================================

auto LivePreviewRenderer::detect_inline_styles(const std::string& text) const
    -> std::vector<LivePreviewLine::StyleRun>
{
    std::vector<LivePreviewLine::StyleRun> runs;

    if (text.empty())
    {
        return runs;
    }

    // Check if it's a heading → whole line gets heading style
    const int heading_level = detect_heading_level(text);
    if (heading_level > 0)
    {
        LivePreviewLine::StyleRun run;
        run.start = 0;
        run.length = static_cast<int>(text.size());
        // Map heading levels to styles
        const std::array<LivePreviewLine::StyleRun::Style, 6> heading_styles = {
            LivePreviewLine::StyleRun::Style::Heading1,
            LivePreviewLine::StyleRun::Style::Heading2,
            LivePreviewLine::StyleRun::Style::Heading3,
            LivePreviewLine::StyleRun::Style::Heading4,
            LivePreviewLine::StyleRun::Style::Heading5,
            LivePreviewLine::StyleRun::Style::Heading6,
        };
        run.style = heading_styles[static_cast<size_t>(heading_level - 1)];
        runs.push_back(run);
        return runs;
    }

    // Detect bold: text surrounded by **
    try
    {
        // Bold detection on display text (already folded)
        static const std::regex bold_re(R"(\*\*(.+?)\*\*)");
        std::sregex_iterator bold_it(text.begin(), text.end(), bold_re);
        std::sregex_iterator end_it;
        for (; bold_it != end_it; ++bold_it)
        {
            LivePreviewLine::StyleRun run;
            run.start = static_cast<int>(bold_it->position());
            run.length = static_cast<int>(bold_it->length());
            run.style = LivePreviewLine::StyleRun::Style::Bold;
            runs.push_back(run);
        }

        // Italic detection: single * not adjacent to another *
        // Note: std::regex ECMAScript doesn't support lookbehinds
        static const std::regex italic_re(R"((?:^|[^*])\*([^*]+)\*(?:[^*]|$))");
        std::sregex_iterator ital_it(text.begin(), text.end(), italic_re);
        for (; ital_it != end_it; ++ital_it)
        {
            auto match_pos = static_cast<int>(ital_it->position());
            auto match_len = static_cast<int>(ital_it->length());
            // If matched a leading non-* char, offset by 1
            if (match_pos > 0 || (text[0] != '*'))
            {
                if (ital_it->str()[0] != '*')
                {
                    match_pos += 1;
                    match_len -= 1;
                }
            }
            // If matched a trailing non-* char, reduce length by 1
            auto end_char = ital_it->str().back();
            if (end_char != '*')
            {
                match_len -= 1;
            }
            LivePreviewLine::StyleRun run;
            run.start = match_pos;
            run.length = match_len;
            run.style = LivePreviewLine::StyleRun::Style::Italic;
            runs.push_back(run);
        }

        // Inline code
        static const std::regex code_re(R"(`([^`]+)`)");
        std::sregex_iterator code_it(text.begin(), text.end(), code_re);
        for (; code_it != end_it; ++code_it)
        {
            LivePreviewLine::StyleRun run;
            run.start = static_cast<int>(code_it->position());
            run.length = static_cast<int>(code_it->length());
            run.style = LivePreviewLine::StyleRun::Style::Code;
            runs.push_back(run);
        }

        // Strikethrough
        static const std::regex strike_re(R"(~~(.+?)~~)");
        std::sregex_iterator strike_it(text.begin(), text.end(), strike_re);
        for (; strike_it != end_it; ++strike_it)
        {
            LivePreviewLine::StyleRun run;
            run.start = static_cast<int>(strike_it->position());
            run.length = static_cast<int>(strike_it->length());
            run.style = LivePreviewLine::StyleRun::Style::Strikethrough;
            runs.push_back(run);
        }

        // Highlight ==text==
        static const std::regex highlight_re(R"(==(.+?)==)");
        std::sregex_iterator hl_it(text.begin(), text.end(), highlight_re);
        for (; hl_it != end_it; ++hl_it)
        {
            LivePreviewLine::StyleRun run;
            run.start = static_cast<int>(hl_it->position());
            run.length = static_cast<int>(hl_it->length());
            run.style = LivePreviewLine::StyleRun::Style::Highlight;
            runs.push_back(run);
        }

        // WikiLink [[target]] or [[target|alias]]
        static const std::regex wiki_re(R"(\[\[([^\]]+)\]\])");
        std::sregex_iterator wiki_it(text.begin(), text.end(), wiki_re);
        for (; wiki_it != end_it; ++wiki_it)
        {
            LivePreviewLine::StyleRun run;
            run.start = static_cast<int>(wiki_it->position());
            run.length = static_cast<int>(wiki_it->length());
            run.style = LivePreviewLine::StyleRun::Style::WikiLink;
            runs.push_back(run);
        }

        // Inline math $...$
        static const std::regex math_re(R"(\$([^\$]+)\$)");
        std::sregex_iterator math_it(text.begin(), text.end(), math_re);
        for (; math_it != end_it; ++math_it)
        {
            LivePreviewLine::StyleRun run;
            run.start = static_cast<int>(math_it->position());
            run.length = static_cast<int>(math_it->length());
            run.style = LivePreviewLine::StyleRun::Style::MathInline;
            runs.push_back(run);
        }
    }
    catch (const std::regex_error&)
    {
        // Regex failure: return empty styles rather than crash
    }

    // Checkbox detection
    if (text.size() >= 5)
    {
        if (text.substr(0, 5) == "- [ ]" || text.substr(0, 5) == "* [ ]")
        {
            LivePreviewLine::StyleRun run;
            run.start = 0;
            run.length = 5;
            run.style = LivePreviewLine::StyleRun::Style::CheckboxUnchecked;
            runs.push_back(run);
        }
        else if (text.substr(0, 5) == "- [x]" || text.substr(0, 5) == "* [x]" ||
                 text.substr(0, 5) == "- [X]" || text.substr(0, 5) == "* [X]")
        {
            LivePreviewLine::StyleRun run;
            run.start = 0;
            run.length = 5;
            run.style = LivePreviewLine::StyleRun::Style::CheckboxChecked;
            runs.push_back(run);
        }
    }

    // Blockquote marker
    if (!text.empty() && text[0] == '>')
    {
        LivePreviewLine::StyleRun run;
        run.start = 0;
        // Include the '>' and optional trailing space
        run.length = (text.size() > 1 && text[1] == ' ') ? 2 : 1;
        run.style = LivePreviewLine::StyleRun::Style::BlockquoteMarker;
        runs.push_back(run);
    }

    // List marker detection
    if (text.size() >= 2)
    {
        if ((text[0] == '-' || text[0] == '*' || text[0] == '+') && text[1] == ' ')
        {
            LivePreviewLine::StyleRun run;
            run.start = 0;
            run.length = 2;
            run.style = LivePreviewLine::StyleRun::Style::ListMarker;
            runs.push_back(run);
        }
    }

    // Sort by start position
    std::sort(runs.begin(),
              runs.end(),
              [](const LivePreviewLine::StyleRun& lhs, const LivePreviewLine::StyleRun& rhs)
              { return lhs.start < rhs.start; });

    return runs;
}

// ============================================================================
// Private: Compute folded ranges
// ============================================================================

auto LivePreviewRenderer::compute_folded_ranges(const std::string& text) const
    -> std::vector<LivePreviewLine::FoldedRange>
{
    std::vector<LivePreviewLine::FoldedRange> folds;

    if (text.empty())
    {
        return folds;
    }

    // Fold heading markers: "## Title" → fold "## "
    const int heading_level = detect_heading_level(text);
    if (heading_level > 0)
    {
        LivePreviewLine::FoldedRange fold;
        fold.raw_start = 0;
        fold.raw_length = heading_level + 1; // # chars + space
        if (static_cast<size_t>(fold.raw_length) > text.size())
        {
            fold.raw_length = static_cast<int>(text.size());
        }
        folds.push_back(fold);

        // Also fold trailing hashes if present: "## Title ##" → fold " ##"
        auto trailing_start = text.find_last_not_of("# ");
        if (trailing_start != std::string::npos && trailing_start < text.size() - 1)
        {
            auto suffix = text.substr(trailing_start + 1);
            bool has_trailing_hashes = false;
            for (char character : suffix)
            {
                if (character == '#')
                {
                    has_trailing_hashes = true;
                    break;
                }
            }
            if (has_trailing_hashes)
            {
                LivePreviewLine::FoldedRange trailing_fold;
                trailing_fold.raw_start = static_cast<int>(trailing_start + 1);
                trailing_fold.raw_length = static_cast<int>(text.size() - trailing_start - 1);
                folds.push_back(trailing_fold);
            }
        }

        return folds;
    }

    try
    {
        // Fold bold markers: **text** → fold the ** on each side
        static const std::regex bold_re(R"(\*\*(.+?)\*\*)");
        std::sregex_iterator bold_it(text.begin(), text.end(), bold_re);
        std::sregex_iterator end_it;
        for (; bold_it != end_it; ++bold_it)
        {
            const auto pos = static_cast<int>(bold_it->position());
            const auto total_len = static_cast<int>(bold_it->length());
            // Fold opening **
            folds.push_back({pos, 2});
            // Fold closing **
            folds.push_back({pos + total_len - 2, 2});
        }

        // Fold italic markers: *text* → fold the * on each side
        static const std::regex italic_re(R"((?:^|[^*])\*([^*]+)\*(?:[^*]|$))");
        std::sregex_iterator ital_it(text.begin(), text.end(), italic_re);
        for (; ital_it != end_it; ++ital_it)
        {
            auto match_pos = static_cast<int>(ital_it->position());
            auto match_len = static_cast<int>(ital_it->length());
            // If matched a leading non-* char, offset by 1
            if (ital_it->str()[0] != '*')
            {
                match_pos += 1;
                match_len -= 1;
            }
            // If matched a trailing non-* char, reduce length by 1
            if (ital_it->str().back() != '*')
            {
                match_len -= 1;
            }
            folds.push_back({match_pos, 1});
            folds.push_back({match_pos + match_len - 1, 1});
        }

        // Fold inline code markers: `code` → fold the ` on each side
        static const std::regex code_re(R"(`([^`]+)`)");
        std::sregex_iterator code_it(text.begin(), text.end(), code_re);
        for (; code_it != end_it; ++code_it)
        {
            const auto pos = static_cast<int>(code_it->position());
            const auto total_len = static_cast<int>(code_it->length());
            folds.push_back({pos, 1});
            folds.push_back({pos + total_len - 1, 1});
        }

        // Fold strikethrough markers: ~~text~~ → fold ~~ on each side
        static const std::regex strike_re(R"(~~(.+?)~~)");
        std::sregex_iterator strike_it(text.begin(), text.end(), strike_re);
        for (; strike_it != end_it; ++strike_it)
        {
            const auto pos = static_cast<int>(strike_it->position());
            const auto total_len = static_cast<int>(strike_it->length());
            folds.push_back({pos, 2});
            folds.push_back({pos + total_len - 2, 2});
        }

        // Fold highlight markers: ==text== → fold == on each side
        static const std::regex hl_re(R"(==(.+?)==)");
        std::sregex_iterator hl_it(text.begin(), text.end(), hl_re);
        for (; hl_it != end_it; ++hl_it)
        {
            const auto pos = static_cast<int>(hl_it->position());
            const auto total_len = static_cast<int>(hl_it->length());
            folds.push_back({pos, 2});
            folds.push_back({pos + total_len - 2, 2});
        }

        // Fold wikilink markers: [[target]] → fold [[ and ]]
        static const std::regex wiki_re(R"(\[\[([^\]|]+)(\|[^\]]+)?\]\])");
        std::sregex_iterator wiki_it(text.begin(), text.end(), wiki_re);
        for (; wiki_it != end_it; ++wiki_it)
        {
            const auto pos = static_cast<int>(wiki_it->position());
            const auto total_len = static_cast<int>(wiki_it->length());

            // Check for alias: [[target|alias]]
            std::string match_str = wiki_it->str();
            auto pipe_pos = match_str.find('|');
            if (pipe_pos != std::string::npos)
            {
                // Fold [[target| and ]]
                folds.push_back({pos, static_cast<int>(pipe_pos + 1)});
                folds.push_back({pos + total_len - 2, 2});
            }
            else
            {
                // Fold [[ and ]]
                folds.push_back({pos, 2});
                folds.push_back({pos + total_len - 2, 2});
            }
        }
    }
    catch (const std::regex_error&)
    {
        // Regex failure: return empty folds rather than crash
    }

    // Sort folds by start position
    std::sort(folds.begin(),
              folds.end(),
              [](const LivePreviewLine::FoldedRange& lhs, const LivePreviewLine::FoldedRange& rhs)
              { return lhs.raw_start < rhs.raw_start; });

    return folds;
}

// ============================================================================
// Private: Build display text from raw text with folded ranges removed
// ============================================================================

auto LivePreviewRenderer::build_display_text(
    const std::string& raw, const std::vector<LivePreviewLine::FoldedRange>& folds) const
    -> std::string
{
    if (folds.empty())
    {
        return raw;
    }

    std::string result;
    result.reserve(raw.size());

    int pos = 0;
    for (const auto& fold : folds)
    {
        // Append text before this fold
        if (fold.raw_start > pos)
        {
            result.append(raw, static_cast<size_t>(pos), static_cast<size_t>(fold.raw_start - pos));
        }
        // Skip over the folded range
        pos = fold.raw_start + fold.raw_length;
    }

    // Append remaining text after last fold
    if (static_cast<size_t>(pos) < raw.size())
    {
        result.append(raw, static_cast<size_t>(pos));
    }

    return result;
}

// ============================================================================
// Private: Detect styles for cursor line (syntax highlighting, no folding)
// ============================================================================

auto LivePreviewRenderer::detect_cursor_line_styles(const std::string& text) const
    -> std::vector<LivePreviewLine::StyleRun>
{
    // On the cursor line, we show raw markdown but still apply syntax highlighting
    // so the user can see what they're editing with color cues
    return detect_inline_styles(text);
}

} // namespace markamp::ui
