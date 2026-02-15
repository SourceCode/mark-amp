# Phase 13 -- Live Preview / WYSIWYG Mode

## Objective

Implement a Live Preview editing mode where Markdown syntax is rendered inline as the user types, similar to Obsidian's Live Preview or Typora. In this mode, `**bold**` text appears bold without showing the asterisks (unless the cursor is on that line), headings display at their rendered size, links show as clickable text, and code blocks are syntax-highlighted. The cursor line always shows raw Markdown for editing; all other lines show rendered output.

## Prerequisites

- Phase 01 (DocumentModel)
- Existing EditorPanel (Scintilla-based), HtmlRenderer, ThemeEngine
- Existing ViewMode enum (Editor, Preview, Split)

## Feature References (PRD)

- PRD #17: Live Preview / WYSIWYG

## Data Structures to Implement

### File: `src/ui/LivePreviewRenderer.h`

```cpp
#pragma once

#include <string>
#include <vector>

namespace markamp::core { class ThemeEngine; }

namespace markamp::ui
{

/// Describes how a line should be rendered in Live Preview mode.
struct LivePreviewLine
{
    int line_number{0};
    bool is_cursor_line{false};     // Show raw markdown on this line
    std::string raw_text;           // Original markdown
    std::string display_text;       // Rendered display text (stripped markers)

    // Inline style runs for rendering
    struct StyleRun
    {
        int start{0};
        int length{0};
        enum class Style : uint8_t
        {
            Normal, Bold, Italic, BoldItalic, Code, Strikethrough,
            Link, LinkUrl, Heading1, Heading2, Heading3, Heading4,
            Heading5, Heading6, BlockquoteMarker, ListMarker,
            CheckboxUnchecked, CheckboxChecked, Tag, WikiLink,
            MathInline, Highlight, InternalLink
        } style{Style::Normal};
    };
    std::vector<StyleRun> styles;

    // Folding: hide markdown syntax characters
    struct FoldedRange
    {
        int raw_start{0};     // Position in raw_text
        int raw_length{0};    // Characters to hide
    };
    std::vector<FoldedRange> folded_ranges;
};

/// Converts markdown lines into live preview rendering instructions.
class LivePreviewRenderer
{
public:
    explicit LivePreviewRenderer(markamp::core::ThemeEngine& theme_engine);

    /// Process a document and generate live preview lines.
    [[nodiscard]] auto process(const std::string& markdown, int cursor_line) const
        -> std::vector<LivePreviewLine>;

    /// Process a single line (for incremental updates).
    [[nodiscard]] auto process_line(const std::string& line,
                                     int line_number,
                                     bool is_cursor_line) const
        -> LivePreviewLine;

    /// Check if a character position is inside a folded range.
    [[nodiscard]] static auto is_folded(const LivePreviewLine& line, int position) -> bool;

private:
    markamp::core::ThemeEngine& theme_engine_;

    auto detect_inline_styles(const std::string& text) const -> std::vector<LivePreviewLine::StyleRun>;
    auto compute_folded_ranges(const std::string& text) const -> std::vector<LivePreviewLine::FoldedRange>;
    auto strip_heading_markers(const std::string& text) const -> std::string;
    auto strip_bold_italic_markers(const std::string& text) const -> std::string;
    auto detect_heading_level(const std::string& text) const -> int; // 0 = not heading
};

} // namespace markamp::ui
```

## Key Functions to Implement

1. **`process(markdown, cursor_line)`** -- Split markdown into lines. For each line: if it's the cursor line, show raw markdown with syntax highlighting. For all other lines, compute display_text (with folded markers) and style runs. Handle multi-line constructs (code blocks, blockquotes).

2. **`process_line(line, line_number, is_cursor_line)`** -- If is_cursor_line: set display_text = raw_text, apply syntax highlighting styles, no folded ranges. Otherwise: detect heading level and strip `#` markers. Detect bold/italic/code and fold their marker characters. Detect `[[wikilinks]]` and fold the `[[` and `]]`. Set display_text with markers removed. Build style runs for rendering.

3. **`detect_inline_styles(text)`** -- Regex-based detection of markdown inline syntax: `**bold**`, `*italic*`, `` `code` ``, `~~strike~~`, `==highlight==`, `[[link]]`, `$math$`. Return style runs with positions in display_text.

4. **`compute_folded_ranges(text)`** -- Identify characters that should be hidden in live preview: `**` around bold, `*` around italic, `#` heading markers (and trailing space), `[[` `]]` around wikilinks (show only display text or target), `- [ ]` checkbox syntax (render as actual checkbox character).

5. **Integration with EditorPanel** -- The EditorPanel in LivePreview mode uses Scintilla's styling API to:
   - Apply font sizes for headings (H1 larger than H2, etc.)
   - Apply bold/italic fonts
   - Hide folded ranges using Scintilla annotations or custom drawing
   - Show raw markdown on the cursor line
   - Re-process when cursor moves to a new line

6. **ViewMode integration** -- Add `LivePreview` to the existing ViewMode enum. EditorPanel checks view mode to decide rendering path.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(LivePreviewCursorLineChangedEvent)
int old_line{0};
int new_line{0};
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `editor.live_preview` | bool | `false` | Enable live preview mode by default |
| `editor.live_preview_fold_links` | bool | `true` | Hide [[ ]] around wikilinks |
| `editor.live_preview_heading_size` | bool | `true` | Scale heading font sizes |
| `editor.live_preview_checkboxes` | bool | `true` | Render checkboxes as icons |

## Test Cases

File: `tests/unit/test_live_preview.cpp`

1. **Heading folding** -- `# Title` becomes "Title" with H1 style. `##` markers hidden.
2. **Bold folding** -- `**bold text**` becomes "bold text" with Bold style. `**` hidden.
3. **Italic folding** -- `*italic*` becomes "italic" with Italic style.
4. **Code inline** -- `` `code` `` becomes "code" with Code style. Backticks hidden.
5. **WikiLink folding** -- `[[My Note]]` becomes "My Note" with WikiLink style. `[[]]` hidden.
6. **WikiLink with alias** -- `[[My Note|display]]` becomes "display" with WikiLink style.
7. **Cursor line shows raw** -- Line with `**bold**` at cursor shows raw text unchanged.
8. **Checkbox rendering** -- `- [ ] task` displays with checkbox character. `- [x]` shows checked.
9. **Code block passthrough** -- Lines inside fenced code block are not processed.
10. **Multi-marker** -- `**bold _and italic_**` correctly handles nested formatting.
11. **Folded range detection** -- is_folded() returns true for hidden marker positions.
12. **Heading level detection** -- Verify H1-H6 correctly identified, non-headings return 0.

## Acceptance Criteria

- [ ] Non-cursor lines render with Markdown syntax hidden (markers folded)
- [ ] Cursor line always shows raw Markdown for editing
- [ ] Bold, italic, code, strikethrough, highlight markers are folded
- [ ] Heading `#` markers are folded and heading text sized by level
- [ ] WikiLink `[[]]` syntax is folded, showing target or alias
- [ ] Code blocks are not processed (shown as-is with syntax highlighting)
- [ ] Checkbox syntax renders as visual checkbox indicators
- [ ] View mode toggle switches between raw editor and live preview
- [ ] All 12 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/ui/LivePreviewRenderer.h` | LivePreviewLine, LivePreviewRenderer |
| CREATE | `src/ui/LivePreviewRenderer.cpp` | Full implementation |
| MODIFY | `src/ui/EditorPanel.cpp` | Integrate live preview rendering |
| MODIFY | `src/core/Events.h` | Add LivePreviewCursorLineChangedEvent |
| MODIFY | `src/CMakeLists.txt` | Add LivePreviewRenderer.cpp |
| CREATE | `tests/unit/test_live_preview.cpp` | 12 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_live_preview target |

## Architecture Notes

- LivePreviewRenderer is a pure transform: markdown string -> rendering instructions
- The actual rendering is done by Scintilla's styling API in EditorPanel
- Line-level granularity: only the cursor line changes rendering on cursor move
- Performance: process_line is called per visible line, must be fast (<1ms per line)
- No dependency on VaultService or BacklinkIndex -- purely visual

## Estimated Complexity

**XL** -- Complex inline syntax detection, marker folding, Scintilla integration, cursor-aware rendering, 12 tests.
