# Phase 17 -- Copy/Paste with Formatting

## Objective

Implement intelligent clipboard handling: when pasting rich text (HTML from web pages, formatted text from other apps), convert it to Markdown. When copying from the editor, offer both Markdown source and rendered HTML on the clipboard. Handle image paste (save to vault, insert link), URL paste (auto-detect and create link), and table paste (convert HTML/CSV tables to Markdown tables).

## Prerequisites

- Phase 02 (VaultService -- save pasted images)
- Existing EditorPanel (Scintilla)
- Existing MarkdownParser

## Feature References (PRD)

- PRD #45: Copy + Paste with Formatting
- PRD #46: Drag-and-Drop Image Embedding

## Data Structures to Implement

### File: `src/core/ClipboardService.h`

```cpp
#pragma once

#include <expected>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;
class VaultService;
class HtmlRenderer;

enum class PasteContentType : uint8_t
{
    PlainText,
    Html,
    Image,
    Url,
    FilePaths,
    CsvTable,
    Unknown
};

struct PasteContent
{
    PasteContentType type{PasteContentType::PlainText};
    std::string text;              // Plain text content
    std::string html;              // HTML content (if available)
    std::vector<uint8_t> image_data;  // Raw image data (PNG/JPEG)
    std::string image_format;      // "png", "jpeg"
    std::vector<std::string> file_paths;
    std::string url;
};

struct PasteResult
{
    std::string markdown;          // Converted markdown to insert
    std::string saved_file_path;   // If an image was saved to vault
    bool success{false};
    std::string error;
};

class ClipboardService
{
public:
    ClipboardService(EventBus& event_bus, VaultService& vault_service);

    /// Detect what type of content is on the clipboard.
    [[nodiscard]] auto detect_clipboard_content() const -> PasteContent;

    /// Convert clipboard content to markdown.
    [[nodiscard]] auto convert_to_markdown(const PasteContent& content) const
        -> PasteResult;

    /// Copy the given markdown as both plain text and rendered HTML.
    auto copy_as_rich(const std::string& markdown) -> void;

    /// Handle a paste operation in the editor context.
    [[nodiscard]] auto handle_paste() -> PasteResult;

    // Conversion helpers
    [[nodiscard]] auto html_to_markdown(const std::string& html) const -> std::string;
    [[nodiscard]] auto csv_to_markdown_table(const std::string& csv) const -> std::string;
    [[nodiscard]] auto url_to_markdown_link(const std::string& url) const -> std::string;
    [[nodiscard]] auto save_image_to_vault(const std::vector<uint8_t>& data,
                                            const std::string& format) const
        -> std::expected<std::string, std::string>;

private:
    EventBus& event_bus_;
    VaultService& vault_service_;

    // HTML -> Markdown conversion helpers
    [[nodiscard]] auto convert_html_element(const std::string& tag,
                                             const std::string& content,
                                             const std::string& attrs) const -> std::string;
    [[nodiscard]] auto strip_html_tags(const std::string& html) const -> std::string;
    [[nodiscard]] auto convert_html_table(const std::string& html) const -> std::string;
    [[nodiscard]] auto convert_html_list(const std::string& html, bool ordered) const -> std::string;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`detect_clipboard_content()`** -- Check platform clipboard for available formats. Priority: image bitmap > HTML > file paths > URL > plain text. Return PasteContent with the richest available format.

2. **`html_to_markdown(html)`** -- Convert common HTML elements to Markdown:
   - `<strong>/<b>` -> `**bold**`
   - `<em>/<i>` -> `*italic*`
   - `<h1>-<h6>` -> `# heading`
   - `<a href>` -> `[text](url)`
   - `<img>` -> `![alt](src)`
   - `<ul>/<ol>/<li>` -> `- item` / `1. item`
   - `<table>` -> Markdown table
   - `<code>` -> `` `code` ``
   - `<pre>` -> fenced code block
   - `<blockquote>` -> `> quote`
   - `<br>` -> newline
   - Strip all other tags, preserve text content

3. **`save_image_to_vault(data, format)`** -- Generate unique filename (timestamp-based). Save to vault's attachment folder. Return relative path for markdown embed.

4. **`csv_to_markdown_table(csv)`** -- Parse CSV (handle quoted fields with commas). Build markdown table with header separator row.

5. **`url_to_markdown_link(url)`** -- Detect URL. If URL points to an image (.png, .jpg, etc.), create image embed. Otherwise create `[url](url)` link.

6. **`copy_as_rich(markdown)`** -- Render markdown to HTML. Place both plain text (markdown source) and HTML on the clipboard so pasting into other apps gets formatted content.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PasteProcessedEvent)
PasteContentType content_type;
bool converted{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ImagePastedEvent)
std::string saved_path;
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_clipboard_service.cpp`

1. **HTML bold to markdown** -- `<strong>bold</strong>` -> `**bold**`.
2. **HTML italic** -- `<em>italic</em>` -> `*italic*`.
3. **HTML heading** -- `<h2>Title</h2>` -> `## Title`.
4. **HTML link** -- `<a href="url">text</a>` -> `[text](url)`.
5. **HTML list** -- `<ul><li>a</li><li>b</li></ul>` -> `- a\n- b`.
6. **HTML table** -- Simple 2x2 HTML table -> markdown table.
7. **HTML nested** -- `<strong><em>bold italic</em></strong>` -> `***bold italic***`.
8. **CSV to table** -- `a,b\n1,2` -> markdown table with header.
9. **URL detection** -- Paste "https://example.com" -> `[https://example.com](https://example.com)`.
10. **Image URL** -- Paste "https://img.png" -> `![](https://img.png)`.
11. **Strip unknown tags** -- `<div><span>text</span></div>` -> `text`.
12. **Code block** -- `<pre><code>x=1</code></pre>` -> fenced code block.

## Acceptance Criteria

- [ ] HTML rich text pastes as clean Markdown
- [ ] Common HTML elements (bold, italic, headers, links, lists, tables, code) convert correctly
- [ ] Image paste saves to vault attachment folder and inserts embed link
- [ ] URL paste creates Markdown link
- [ ] CSV paste creates Markdown table
- [ ] Copy provides both Markdown and HTML on clipboard
- [ ] All 12 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/ClipboardService.h` | ClipboardService, PasteContent, PasteResult |
| CREATE | `src/core/ClipboardService.cpp` | Full implementation |
| MODIFY | `src/ui/EditorPanel.cpp` | Wire clipboard service into paste handler |
| MODIFY | `src/core/Events.h` | Add 2 paste events |
| MODIFY | `src/CMakeLists.txt` | Add ClipboardService.cpp |
| CREATE | `tests/unit/test_clipboard_service.cpp` | 12 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_clipboard_service target |

## Architecture Notes

- HTML-to-Markdown conversion is recursive (nested elements)
- Platform clipboard access uses wxClipboard (already available via wxWidgets)
- Image paste uses wxBitmapDataObject / wxCustomDataObject
- Constructor injection: ClipboardService(EventBus&, VaultService&)

## Estimated Complexity

**L** -- HTML parsing/conversion, multiple paste types, image save pipeline, 12 tests.
