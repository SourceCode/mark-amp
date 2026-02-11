#include "core/MarkdownParser.h"
#include "core/Md4cWrapper.h"
#include "core/Types.h"
#include "rendering/HtmlRenderer.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace markamp::core;
using namespace markamp::rendering;
using Catch::Matchers::ContainsSubstring;

// ═══════════════════════════════════════════════════════
// Helper: parse markdown → render HTML via HtmlRenderer
// ═══════════════════════════════════════════════════════

static auto render(std::string_view md) -> std::string
{
    Md4cParser parser;
    auto result = parser.parse(md);
    REQUIRE(result.has_value());
    HtmlRenderer renderer;
    return renderer.render(*result);
}

static auto render_with_footnotes(std::string_view md) -> std::string
{
    MarkdownParser parser;
    auto result = parser.parse(md);
    REQUIRE(result.has_value());
    return parser.render_html(*result);
}

// ═══════════════════════════════════════════════════════
// Headings — the bug fix area
// ═══════════════════════════════════════════════════════

TEST_CASE("Heading H1 renders correctly", "[html_renderer][heading]")
{
    auto html = render("# Hello\n");

    SECTION("contains h1 open tag with id")
    {
        REQUIRE_THAT(html, ContainsSubstring("<h1 id=\"hello\">"));
    }

    SECTION("contains heading text")
    {
        REQUIRE_THAT(html, ContainsSubstring("Hello"));
    }

    SECTION("contains h1 close tag")
    {
        REQUIRE_THAT(html, ContainsSubstring("</h1>"));
    }

    SECTION("does NOT contain heading-anchor link")
    {
        REQUIRE_THAT(html, !ContainsSubstring("heading-anchor"));
    }

    SECTION("does NOT append a trailing '#'")
    {
        REQUIRE_THAT(html, !ContainsSubstring(">#</a>"));
        REQUIRE_THAT(html, !ContainsSubstring(">#<"));
    }
}

TEST_CASE("Heading H2 renders correctly", "[html_renderer][heading]")
{
    auto html = render("## World\n");
    REQUIRE_THAT(html, ContainsSubstring("<h2 id=\"world\">"));
    REQUIRE_THAT(html, ContainsSubstring("World</h2>"));
    REQUIRE_THAT(html, !ContainsSubstring("heading-anchor"));
}

TEST_CASE("Heading H3 renders correctly", "[html_renderer][heading]")
{
    auto html = render("### Third Level\n");
    REQUIRE_THAT(html, ContainsSubstring("<h3 id=\"third-level\">"));
    REQUIRE_THAT(html, ContainsSubstring("Third Level</h3>"));
}

TEST_CASE("Heading H4 renders correctly", "[html_renderer][heading]")
{
    auto html = render("#### Fourth\n");
    REQUIRE_THAT(html, ContainsSubstring("<h4 id=\"fourth\">"));
    REQUIRE_THAT(html, ContainsSubstring("Fourth</h4>"));
}

TEST_CASE("Heading H5 renders correctly", "[html_renderer][heading]")
{
    auto html = render("##### Fifth\n");
    REQUIRE_THAT(html, ContainsSubstring("<h5 id=\"fifth\">"));
    REQUIRE_THAT(html, ContainsSubstring("Fifth</h5>"));
}

TEST_CASE("Heading H6 renders correctly", "[html_renderer][heading]")
{
    auto html = render("###### Sixth\n");
    REQUIRE_THAT(html, ContainsSubstring("<h6 id=\"sixth\">"));
    REQUIRE_THAT(html, ContainsSubstring("Sixth</h6>"));
}

TEST_CASE("Heading slug generation", "[html_renderer][heading][slug]")
{
    SECTION("spaces become dashes")
    {
        auto html = render("# Hello World\n");
        REQUIRE_THAT(html, ContainsSubstring("id=\"hello-world\""));
    }

    SECTION("special characters are stripped")
    {
        auto html = render("# What's New!\n");
        REQUIRE_THAT(html, ContainsSubstring("id=\"whats-new\""));
    }

    SECTION("uppercase is lowercased")
    {
        auto html = render("# UPPER CASE\n");
        REQUIRE_THAT(html, ContainsSubstring("id=\"upper-case\""));
    }

    SECTION("numbers are preserved")
    {
        auto html = render("# Version 2.0\n");
        REQUIRE_THAT(html, ContainsSubstring("id=\"version-20\""));
    }

    SECTION("consecutive spaces become single dash")
    {
        auto html = render("# Multiple   Spaces\n");
        REQUIRE_THAT(html, ContainsSubstring("id=\"multiple-spaces\""));
    }

    SECTION("trailing dashes are trimmed")
    {
        auto html = render("# Trailing ---\n");
        // Internal dashes are legitimate, but trailing special chars are stripped
        REQUIRE_THAT(html, ContainsSubstring("id=\"trailing\""));
    }
}

TEST_CASE("Heading with inline formatting", "[html_renderer][heading]")
{
    SECTION("bold inside heading")
    {
        auto html = render("# **Bold** heading\n");
        REQUIRE_THAT(html, ContainsSubstring("<h1"));
        REQUIRE_THAT(html, ContainsSubstring("<strong>Bold</strong>"));
        REQUIRE_THAT(html, ContainsSubstring("</h1>"));
        REQUIRE_THAT(html, !ContainsSubstring("heading-anchor"));
    }

    SECTION("italic inside heading")
    {
        auto html = render("## *Italic* heading\n");
        REQUIRE_THAT(html, ContainsSubstring("<h2"));
        REQUIRE_THAT(html, ContainsSubstring("<em>Italic</em>"));
    }

    SECTION("code inside heading")
    {
        auto html = render("### `Code` heading\n");
        REQUIRE_THAT(html, ContainsSubstring("<h3"));
        REQUIRE_THAT(html, ContainsSubstring("<code>Code</code>"));
    }
}

// ═══════════════════════════════════════════════════════
// Paragraphs
// ═══════════════════════════════════════════════════════

TEST_CASE("Paragraph renders with p tags", "[html_renderer][paragraph]")
{
    auto html = render("Hello world.\n");
    REQUIRE_THAT(html, ContainsSubstring("<p>Hello world.</p>"));
}

TEST_CASE("Multiple paragraphs render separately", "[html_renderer][paragraph]")
{
    auto html = render("First paragraph.\n\nSecond paragraph.\n");
    REQUIRE_THAT(html, ContainsSubstring("<p>First paragraph.</p>"));
    REQUIRE_THAT(html, ContainsSubstring("<p>Second paragraph.</p>"));
}

// ═══════════════════════════════════════════════════════
// Emphasis and strong
// ═══════════════════════════════════════════════════════

TEST_CASE("Emphasis renders with em tag", "[html_renderer][inline]")
{
    auto html = render("*italic text*\n");
    REQUIRE_THAT(html, ContainsSubstring("<em>italic text</em>"));
}

TEST_CASE("Strong renders with strong tag", "[html_renderer][inline]")
{
    auto html = render("**bold text**\n");
    REQUIRE_THAT(html, ContainsSubstring("<strong>bold text</strong>"));
}

TEST_CASE("Strong emphasis renders with both tags", "[html_renderer][inline]")
{
    auto html = render("***bold italic***\n");
    // Could be <strong><em> or <em><strong> — check both elements present
    REQUIRE_THAT(html, ContainsSubstring("<strong>"));
    REQUIRE_THAT(html, ContainsSubstring("<em>"));
    REQUIRE_THAT(html, ContainsSubstring("bold italic"));
}

// ═══════════════════════════════════════════════════════
// Inline code
// ═══════════════════════════════════════════════════════

TEST_CASE("Inline code renders with code tag", "[html_renderer][inline]")
{
    auto html = render("Use `printf()` here\n");
    REQUIRE_THAT(html, ContainsSubstring("<code>printf()</code>"));
}

// ═══════════════════════════════════════════════════════
// Links
// ═══════════════════════════════════════════════════════

TEST_CASE("Link renders with href", "[html_renderer][link]")
{
    auto html = render("[Click](https://example.com)\n");
    REQUIRE_THAT(html, ContainsSubstring("<a href=\"https://example.com\">Click</a>"));
}

TEST_CASE("Link with title renders title attribute", "[html_renderer][link]")
{
    auto html = render("[Click](https://example.com \"My Title\")\n");
    REQUIRE_THAT(html, ContainsSubstring("href=\"https://example.com\""));
    REQUIRE_THAT(html, ContainsSubstring("title=\"My Title\""));
    REQUIRE_THAT(html, ContainsSubstring(">Click</a>"));
}

// ═══════════════════════════════════════════════════════
// Images
// ═══════════════════════════════════════════════════════

TEST_CASE("Image without valid file renders missing placeholder", "[html_renderer][image]")
{
    auto html = render("![Logo](logo.png)\n");
    REQUIRE_THAT(html, ContainsSubstring("image-missing"));
    REQUIRE_THAT(html, ContainsSubstring("logo.png"));
    REQUIRE_THAT(html, ContainsSubstring("Logo"));
}

// ═══════════════════════════════════════════════════════
// Blockquotes
// ═══════════════════════════════════════════════════════

TEST_CASE("Blockquote renders correctly", "[html_renderer][blockquote]")
{
    auto html = render("> A wise quote\n");
    REQUIRE_THAT(html, ContainsSubstring("<blockquote>"));
    REQUIRE_THAT(html, ContainsSubstring("A wise quote"));
    REQUIRE_THAT(html, ContainsSubstring("</blockquote>"));
}

TEST_CASE("Nested blockquote renders", "[html_renderer][blockquote]")
{
    auto html = render("> > Nested\n");
    // Should have at least two blockquote tags
    auto first = html.find("<blockquote>");
    REQUIRE(first != std::string::npos);
    auto second = html.find("<blockquote>", first + 1);
    REQUIRE(second != std::string::npos);
}

// ═══════════════════════════════════════════════════════
// Lists
// ═══════════════════════════════════════════════════════

TEST_CASE("Unordered list renders", "[html_renderer][list]")
{
    auto html = render("- Alpha\n- Beta\n- Gamma\n");
    REQUIRE_THAT(html, ContainsSubstring("<ul>"));
    REQUIRE_THAT(html, ContainsSubstring("<li>"));
    REQUIRE_THAT(html, ContainsSubstring("Alpha"));
    REQUIRE_THAT(html, ContainsSubstring("Beta"));
    REQUIRE_THAT(html, ContainsSubstring("Gamma"));
    REQUIRE_THAT(html, ContainsSubstring("</ul>"));
}

TEST_CASE("Ordered list renders", "[html_renderer][list]")
{
    auto html = render("1. First\n2. Second\n3. Third\n");
    REQUIRE_THAT(html, ContainsSubstring("<ol>"));
    REQUIRE_THAT(html, ContainsSubstring("<li>"));
    REQUIRE_THAT(html, ContainsSubstring("First"));
    REQUIRE_THAT(html, ContainsSubstring("Third"));
    REQUIRE_THAT(html, ContainsSubstring("</ol>"));
}

TEST_CASE("Ordered list with custom start number", "[html_renderer][list]")
{
    auto html = render("5. Five\n6. Six\n");
    REQUIRE_THAT(html, ContainsSubstring("start=\"5\""));
}

// ═══════════════════════════════════════════════════════
// Code blocks
// ═══════════════════════════════════════════════════════

TEST_CASE("Fenced code block with language renders", "[html_renderer][codeblock]")
{
    auto html = render("```python\nprint('hello')\n```\n");
    REQUIRE_THAT(html, ContainsSubstring("language-python"));
    REQUIRE_THAT(html, ContainsSubstring("code-block"));
    REQUIRE_THAT(html, ContainsSubstring("print"));
}

TEST_CASE("Fenced code block without language renders", "[html_renderer][codeblock]")
{
    auto html = render("```\nplain text\n```\n");
    REQUIRE_THAT(html, ContainsSubstring("code-block"));
    REQUIRE_THAT(html, ContainsSubstring("plain text"));
}

TEST_CASE("Code block escapes HTML", "[html_renderer][codeblock]")
{
    auto html = render("```\n<script>alert('xss')</script>\n```\n");
    REQUIRE_THAT(html, ContainsSubstring("&lt;script&gt;"));
    REQUIRE_THAT(html, !ContainsSubstring("<script>"));
}

// ═══════════════════════════════════════════════════════
// Horizontal rule
// ═══════════════════════════════════════════════════════

TEST_CASE("Horizontal rule renders hr tag", "[html_renderer][hr]")
{
    auto html = render("---\n");
    REQUIRE_THAT(html, ContainsSubstring("<hr>"));
}

TEST_CASE("Horizontal rule with asterisks", "[html_renderer][hr]")
{
    auto html = render("***\n");
    REQUIRE_THAT(html, ContainsSubstring("<hr>"));
}

// ═══════════════════════════════════════════════════════
// Tables
// ═══════════════════════════════════════════════════════

TEST_CASE("Table renders basic structure", "[html_renderer][table]")
{
    auto html = render("| A | B |\n|---|---|\n| 1 | 2 |\n");
    REQUIRE_THAT(html, ContainsSubstring("<table>"));
    REQUIRE_THAT(html, ContainsSubstring("<thead>"));
    REQUIRE_THAT(html, ContainsSubstring("<tbody>"));
    REQUIRE_THAT(html, ContainsSubstring("<th>"));
    REQUIRE_THAT(html, ContainsSubstring("<td>"));
    REQUIRE_THAT(html, ContainsSubstring("</table>"));
}

TEST_CASE("Table with alignment renders style", "[html_renderer][table]")
{
    auto html = render("| Left | Center | Right |\n|:-----|:------:|------:|\n| a | b | c |\n");
    REQUIRE_THAT(html, ContainsSubstring("text-align: left"));
    REQUIRE_THAT(html, ContainsSubstring("text-align: center"));
    REQUIRE_THAT(html, ContainsSubstring("text-align: right"));
}

TEST_CASE("Table wraps in table-wrapper div", "[html_renderer][table]")
{
    auto html = render("| H |\n|---|\n| D |\n");
    REQUIRE_THAT(html, ContainsSubstring("table-wrapper"));
}

// ═══════════════════════════════════════════════════════
// Strikethrough (GFM)
// ═══════════════════════════════════════════════════════

TEST_CASE("Strikethrough renders del tag", "[html_renderer][gfm]")
{
    auto html = render("~~deleted text~~\n");
    REQUIRE_THAT(html, ContainsSubstring("<del>deleted text</del>"));
}

// ═══════════════════════════════════════════════════════
// Task lists (GFM)
// ═══════════════════════════════════════════════════════

TEST_CASE("Task list renders checkboxes", "[html_renderer][gfm]")
{
    auto html = render("- [x] Done\n- [ ] Todo\n");
    REQUIRE_THAT(html, ContainsSubstring("type=\"checkbox\""));
    REQUIRE_THAT(html, ContainsSubstring("checked"));
    REQUIRE_THAT(html, ContainsSubstring("disabled"));
}

TEST_CASE("Unchecked task list item", "[html_renderer][gfm]")
{
    auto html = render("- [ ] Not done\n");
    REQUIRE_THAT(html, ContainsSubstring("type=\"checkbox\""));
    REQUIRE_THAT(html, ContainsSubstring("disabled"));
    REQUIRE_THAT(html, !ContainsSubstring("checked"));
}

// ═══════════════════════════════════════════════════════
// Mermaid blocks
// ═══════════════════════════════════════════════════════

TEST_CASE("Mermaid block renders placeholder", "[html_renderer][mermaid]")
{
    auto html = render("```mermaid\ngraph TD;\nA --> B;\n```\n");
    REQUIRE_THAT(html, ContainsSubstring("mermaid-block"));
}

// ═══════════════════════════════════════════════════════
// Line breaks
// ═══════════════════════════════════════════════════════

TEST_CASE("Hard line break renders br", "[html_renderer][linebreak]")
{
    // Two trailing spaces + newline = hard break
    auto html = render("Line one  \nLine two\n");
    REQUIRE_THAT(html, ContainsSubstring("<br>"));
}

// ═══════════════════════════════════════════════════════
// HTML escaping
// ═══════════════════════════════════════════════════════

TEST_CASE("Special characters are HTML-escaped", "[html_renderer][escape]")
{
    auto html = render("Use <div> & \"quotes\"\n");
    REQUIRE_THAT(html, ContainsSubstring("&lt;div&gt;"));
    REQUIRE_THAT(html, ContainsSubstring("&amp;"));
    REQUIRE_THAT(html, ContainsSubstring("&quot;quotes&quot;"));
}

TEST_CASE("Angle brackets in text are escaped", "[html_renderer][escape]")
{
    auto html = render("a < b > c\n");
    REQUIRE_THAT(html, ContainsSubstring("&lt;"));
    REQUIRE_THAT(html, ContainsSubstring("&gt;"));
}

// ═══════════════════════════════════════════════════════
// Footnotes
// ═══════════════════════════════════════════════════════

TEST_CASE("Footnote preprocessor extracts definitions", "[html_renderer][footnotes]")
{
    FootnotePreprocessor processor;
    auto result = processor.process("Hello[^1] world\n\n[^1]: A note\n");

    REQUIRE(result.has_footnotes);
    REQUIRE_THAT(result.footnote_section_html, ContainsSubstring("A note"));
    REQUIRE_THAT(result.footnote_section_html, ContainsSubstring("fn-1"));
}

TEST_CASE("Footnote references become superscript links", "[html_renderer][footnotes]")
{
    FootnotePreprocessor processor;
    auto result = processor.process("Text[^ref] here\n\n[^ref]: Definition\n");

    REQUIRE(result.has_footnotes);
    REQUIRE_THAT(result.processed_markdown, ContainsSubstring("footnote-ref"));
    REQUIRE_THAT(result.processed_markdown, ContainsSubstring("fnref-ref"));
}

TEST_CASE("No footnotes returns original markdown", "[html_renderer][footnotes]")
{
    FootnotePreprocessor processor;
    auto result = processor.process("No footnotes here.\n");

    REQUIRE_FALSE(result.has_footnotes);
    REQUIRE(result.footnote_section_html.empty());
    REQUIRE_THAT(result.processed_markdown, ContainsSubstring("No footnotes here."));
}

TEST_CASE("Multiple footnotes are numbered sequentially", "[html_renderer][footnotes]")
{
    FootnotePreprocessor processor;
    auto result = processor.process("A[^a] and B[^b]\n\n[^a]: First\n[^b]: Second\n");

    REQUIRE(result.has_footnotes);
    REQUIRE_THAT(result.footnote_section_html, ContainsSubstring("fn-a"));
    REQUIRE_THAT(result.footnote_section_html, ContainsSubstring("fn-b"));
    REQUIRE_THAT(result.footnote_section_html, ContainsSubstring("First"));
    REQUIRE_THAT(result.footnote_section_html, ContainsSubstring("Second"));
}

// ═══════════════════════════════════════════════════════
// Full render-with-footnotes pipeline
// ═══════════════════════════════════════════════════════

TEST_CASE("render_with_footnotes includes footnote section", "[html_renderer][footnotes]")
{
    auto html = render_with_footnotes("Hello[^1] world\n\n[^1]: A note\n");
    REQUIRE_THAT(html, ContainsSubstring("footnotes"));
    REQUIRE_THAT(html, ContainsSubstring("A note"));
}

// ═══════════════════════════════════════════════════════
// Edge cases
// ═══════════════════════════════════════════════════════

TEST_CASE("Empty document renders empty string", "[html_renderer][edge]")
{
    auto html = render("");
    REQUIRE(html.empty());
}

TEST_CASE("Whitespace-only document produces no block elements", "[html_renderer][edge]")
{
    auto html = render("   \n   \n   \n");
    // Should not produce any paragraph or heading tags
    REQUIRE_THAT(html, !ContainsSubstring("<h1"));
    REQUIRE_THAT(html, !ContainsSubstring("<h2"));
}

TEST_CASE("Deeply nested blockquotes render", "[html_renderer][edge]")
{
    auto html = render("> > > > Deep\n");
    // Count <blockquote> occurrences
    int count = 0;
    std::string::size_type pos = 0;
    while ((pos = html.find("<blockquote>", pos)) != std::string::npos)
    {
        ++count;
        pos += 12;
    }
    REQUIRE(count >= 4);
}

TEST_CASE("Complex document renders without error", "[html_renderer][edge]")
{
    auto html = render(R"(
# Project Title

A **bold** and *italic* description with `inline code`.

## Features

- [x] Feature one
- [ ] Feature two

```python
def hello():
    return 0
```

> This is a blockquote
> with multiple lines

| Header 1 | Header 2 |
|----------|----------|
| Cell 1   | Cell 2   |

---

~~deprecated~~ text

[Link](https://example.com)

![Image](logo.png)

```mermaid
graph TD;
A --> B;
```
)");

    REQUIRE_FALSE(html.empty());
    REQUIRE_THAT(html, ContainsSubstring("<h1"));
    REQUIRE_THAT(html, ContainsSubstring("<h2"));
    REQUIRE_THAT(html, ContainsSubstring("<strong>bold</strong>"));
    REQUIRE_THAT(html, ContainsSubstring("<em>italic</em>"));
    REQUIRE_THAT(html, ContainsSubstring("<code>inline code</code>"));
    REQUIRE_THAT(html, ContainsSubstring("<ul>"));
    REQUIRE_THAT(html, ContainsSubstring("checkbox"));
    REQUIRE_THAT(html, ContainsSubstring("code-block"));
    REQUIRE_THAT(html, ContainsSubstring("<blockquote>"));
    REQUIRE_THAT(html, ContainsSubstring("<table>"));
    REQUIRE_THAT(html, ContainsSubstring("<hr>"));
    REQUIRE_THAT(html, ContainsSubstring("<del>"));
    REQUIRE_THAT(html, ContainsSubstring("example.com"));
    REQUIRE_THAT(html, ContainsSubstring("image-missing"));
    REQUIRE_THAT(html, ContainsSubstring("mermaid-block"));
    // Most critically: no heading-anchor '#' leaking
    REQUIRE_THAT(html, !ContainsSubstring("heading-anchor"));
}
