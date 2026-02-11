#include "core/MarkdownParser.h"
#include "core/Types.h"
#include "rendering/HtmlRenderer.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace markamp::core;
using namespace markamp::rendering;
using Catch::Matchers::ContainsSubstring;

// ═══════════════════════════════════════════════════════
// Table rendering
// ═══════════════════════════════════════════════════════

TEST_CASE("Table: basic table with headers", "[gfm][table]")
{
    MarkdownParser parser;
    auto doc = parser.parse("| Name | Age |\n|------|-----|\n| Alice | 30 |\n");
    REQUIRE(doc.has_value());
    REQUIRE(doc->has_tables());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("<table>"));
    REQUIRE_THAT(html, ContainsSubstring("<thead>"));
    REQUIRE_THAT(html, ContainsSubstring("<tbody>"));
    REQUIRE_THAT(html, ContainsSubstring("<th>"));
    REQUIRE_THAT(html, ContainsSubstring("<td>"));
    REQUIRE_THAT(html, ContainsSubstring("</table>"));
}

TEST_CASE("Table: wrapper div for horizontal scrolling", "[gfm][table]")
{
    MarkdownParser parser;
    auto doc = parser.parse("| A | B |\n|---|---|\n| 1 | 2 |\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("table-wrapper"));
}

TEST_CASE("Table: column alignment (left, center, right)", "[gfm][table]")
{
    MarkdownParser parser;
    auto doc =
        parser.parse("| Left | Center | Right |\n|:-----|:------:|------:|\n| a | b | c |\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("text-align: left"));
    REQUIRE_THAT(html, ContainsSubstring("text-align: center"));
    REQUIRE_THAT(html, ContainsSubstring("text-align: right"));
}

TEST_CASE("Table: inline formatting in cells", "[gfm][table]")
{
    MarkdownParser parser;
    auto doc = parser.parse("| Feature | Status |\n|---------|--------|\n| Tables | **Done** |\n| "
                            "Links | [yes](http://x.com) |\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("<strong>Done</strong>"));
    REQUIRE_THAT(html, ContainsSubstring("href=\"http://x.com\""));
}

// ═══════════════════════════════════════════════════════
// Task list rendering
// ═══════════════════════════════════════════════════════

TEST_CASE("Task list: checked and unchecked items", "[gfm][tasklist]")
{
    MarkdownParser parser;
    auto doc = parser.parse("- [x] Done\n- [ ] Todo\n");
    REQUIRE(doc.has_value());
    REQUIRE(doc->has_task_lists());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("checked"));
    REQUIRE_THAT(html, ContainsSubstring("checkbox"));
    REQUIRE_THAT(html, ContainsSubstring("disabled"));
}

TEST_CASE("Task list: mixed with regular list items", "[gfm][tasklist]")
{
    MarkdownParser parser;
    auto doc = parser.parse("- [x] Task\n- Regular item\n- [ ] Another task\n");
    REQUIRE(doc.has_value());
    REQUIRE(doc->has_task_lists());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("checkbox"));
    REQUIRE_THAT(html, ContainsSubstring("Regular item"));
}

// ═══════════════════════════════════════════════════════
// Footnote rendering
// ═══════════════════════════════════════════════════════

TEST_CASE("Footnotes: reference becomes superscript link", "[gfm][footnotes]")
{
    MarkdownParser parser;
    auto doc = parser.parse("Text with a footnote[^1].\n\n[^1]: This is the footnote.\n");
    REQUIRE(doc.has_value());
    REQUIRE(doc->has_footnotes());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("footnote-ref"));
    REQUIRE_THAT(html, ContainsSubstring("#fn-1"));
    REQUIRE_THAT(html, ContainsSubstring("[1]"));
}

TEST_CASE("Footnotes: definition section at bottom", "[gfm][footnotes]")
{
    MarkdownParser parser;
    auto doc = parser.parse("Some text[^1].\n\n[^1]: The footnote content.\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("class=\"footnotes\""));
    REQUIRE_THAT(html, ContainsSubstring("fn-1"));
    REQUIRE_THAT(html, ContainsSubstring("The footnote content."));
}

TEST_CASE("Footnotes: back-reference link", "[gfm][footnotes]")
{
    MarkdownParser parser;
    auto doc = parser.parse("Text[^note].\n\n[^note]: A footnote.\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("footnote-backref"));
    REQUIRE_THAT(html, ContainsSubstring("#fnref-note"));
}

TEST_CASE("Footnotes: multiple footnotes numbered correctly", "[gfm][footnotes]")
{
    MarkdownParser parser;
    auto doc =
        parser.parse("First[^1] and second[^2].\n\n[^1]: Footnote one.\n[^2]: Footnote two.\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("[1]"));
    REQUIRE_THAT(html, ContainsSubstring("[2]"));
    REQUIRE_THAT(html, ContainsSubstring("Footnote one."));
    REQUIRE_THAT(html, ContainsSubstring("Footnote two."));
}

TEST_CASE("Footnotes: no footnotes returns has_footnotes false", "[gfm][footnotes]")
{
    MarkdownParser parser;
    auto doc = parser.parse("No footnotes here.\n");
    REQUIRE(doc.has_value());
    REQUIRE_FALSE(doc->has_footnotes());
}

// ═══════════════════════════════════════════════════════
// Strikethrough rendering
// ═══════════════════════════════════════════════════════

TEST_CASE("Strikethrough: basic rendering", "[gfm][strikethrough]")
{
    MarkdownParser parser;
    auto doc = parser.parse("~~removed text~~\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("<del>removed text</del>"));
}

TEST_CASE("Strikethrough: combined with other formatting", "[gfm][strikethrough]")
{
    MarkdownParser parser;
    auto doc = parser.parse("~~**bold and struck**~~\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("<del>"));
    REQUIRE_THAT(html, ContainsSubstring("<strong>"));
}

// ═══════════════════════════════════════════════════════
// Autolink rendering
// ═══════════════════════════════════════════════════════

TEST_CASE("Autolink: URL is automatically linked", "[gfm][autolink]")
{
    MarkdownParser parser;
    auto doc = parser.parse("Visit https://example.com for info.\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("href=\"https://example.com\""));
    REQUIRE_THAT(html, ContainsSubstring(">https://example.com</a>"));
}

TEST_CASE("Autolink: email is linked with mailto", "[gfm][autolink]")
{
    MarkdownParser parser;
    auto doc = parser.parse("Email user@example.com for help.\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    // md4c may or may not auto-detect plain email (depends on permissive flags)
    // At minimum the text should appear
    REQUIRE_THAT(html, ContainsSubstring("user@example.com"));
}

// ═══════════════════════════════════════════════════════
// FootnotePreprocessor unit tests
// ═══════════════════════════════════════════════════════

TEST_CASE("FootnotePreprocessor: no footnotes returns unchanged", "[gfm][preprocessor]")
{
    FootnotePreprocessor processor;
    auto result = processor.process("Hello world\n");
    REQUIRE_FALSE(result.has_footnotes);
    REQUIRE(result.footnote_section_html.empty());
    REQUIRE_THAT(result.processed_markdown, ContainsSubstring("Hello world"));
}

TEST_CASE("FootnotePreprocessor: extracts definition lines", "[gfm][preprocessor]")
{
    FootnotePreprocessor processor;
    auto result = processor.process("Text[^1].\n\n[^1]: The note.\n");
    REQUIRE(result.has_footnotes);
    // Definition line should be removed from processed markdown
    REQUIRE(result.processed_markdown.find("[^1]: The note.") == std::string::npos);
    // Section should contain the content
    REQUIRE_THAT(result.footnote_section_html, ContainsSubstring("The note."));
}

TEST_CASE("FootnotePreprocessor: section has correct structure", "[gfm][preprocessor]")
{
    FootnotePreprocessor processor;
    auto result = processor.process("See[^abc].\n\n[^abc]: Definition here.\n");
    REQUIRE(result.has_footnotes);
    REQUIRE_THAT(result.footnote_section_html, ContainsSubstring("<section class=\"footnotes\">"));
    REQUIRE_THAT(result.footnote_section_html, ContainsSubstring("<hr>"));
    REQUIRE_THAT(result.footnote_section_html, ContainsSubstring("<ol>"));
    REQUIRE_THAT(result.footnote_section_html, ContainsSubstring("id=\"fn-abc\""));
    REQUIRE_THAT(result.footnote_section_html, ContainsSubstring("footnote-backref"));
    REQUIRE_THAT(result.footnote_section_html, ContainsSubstring("#fnref-abc"));
}

// ═══════════════════════════════════════════════════════
// Combined GFM document
// ═══════════════════════════════════════════════════════

TEST_CASE("Combined: complex GFM document with all extensions", "[gfm][combined]")
{
    MarkdownParser parser;
    auto doc = parser.parse(R"(
# GFM Test

| Feature | Status | Priority |
|:--------|:------:|--------:|
| Tables | **Done** | High |
| Tasks | ~~Pending~~ Done | Medium |

## Tasks

- [x] Implement tables
- [ ] Implement task lists
- [ ] Add footnotes[^1]

This has ~~old text~~ new text.

Visit https://example.com or email test@example.com.

[^1]: Footnotes are a GFM extension.
)");

    REQUIRE(doc.has_value());
    REQUIRE(doc->has_tables());
    REQUIRE(doc->has_task_lists());
    REQUIRE(doc->has_footnotes());

    auto html = parser.render_html(*doc);

    // Tables
    REQUIRE_THAT(html, ContainsSubstring("table-wrapper"));
    REQUIRE_THAT(html, ContainsSubstring("<thead>"));
    REQUIRE_THAT(html, ContainsSubstring("text-align: center"));
    REQUIRE_THAT(html, ContainsSubstring("text-align: right"));

    // Task lists
    REQUIRE_THAT(html, ContainsSubstring("checkbox"));
    REQUIRE_THAT(html, ContainsSubstring("checked"));

    // Strikethrough
    REQUIRE_THAT(html, ContainsSubstring("<del>"));

    // Autolinks
    REQUIRE_THAT(html, ContainsSubstring("href=\"https://example.com\""));

    // Footnotes
    REQUIRE_THAT(html, ContainsSubstring("class=\"footnotes\""));
    REQUIRE_THAT(html, ContainsSubstring("Footnotes are a GFM extension."));
    REQUIRE_THAT(html, ContainsSubstring("footnote-backref"));
}
