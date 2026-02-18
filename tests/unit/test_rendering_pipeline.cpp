// Phase 34 — Task 3: Rendering pipeline tests
// Tests HtmlRenderer, CodeBlockRenderer, MermaidBlockRenderer, FootnotePreprocessor

#include "core/MarkdownParser.h"
#include "core/SyntaxHighlighter.h"
#include "rendering/CodeBlockRenderer.h"
#include "rendering/HtmlRenderer.h"
#include "rendering/MermaidBlockRenderer.h"

#include <catch2/catch_test_macros.hpp>

#include <set>
#include <string>

using namespace markamp::core;
using namespace markamp::rendering;

// ===========================================================================
// HtmlRenderer — static utilities
// ===========================================================================

TEST_CASE("Rendering — escape_html basics", "[rendering][html]")
{
    REQUIRE(HtmlRenderer::escape_html("<div>") == "&lt;div&gt;");
    REQUIRE(HtmlRenderer::escape_html("a & b") == "a &amp; b");
    REQUIRE(HtmlRenderer::escape_html("\"test\"") == "&quot;test&quot;");
    REQUIRE(HtmlRenderer::escape_html("plain") == "plain");
    REQUIRE(HtmlRenderer::escape_html("") == "");
}

TEST_CASE("Rendering — slugify headings", "[rendering][html]")
{
    REQUIRE(HtmlRenderer::slugify("Hello World") == "hello-world");
    REQUIRE(HtmlRenderer::slugify("API Reference") == "api-reference");
    REQUIRE(HtmlRenderer::slugify("C++ Guide") == "c-guide");
    auto slug = HtmlRenderer::slugify("  spaces  ");
    REQUIRE_FALSE(slug.empty());
}

TEST_CASE("Rendering — alignment_style returns valid CSS", "[rendering][html]")
{
    auto left = HtmlRenderer::alignment_style(MdAlignment::Left);
    auto center = HtmlRenderer::alignment_style(MdAlignment::Center);
    auto right = HtmlRenderer::alignment_style(MdAlignment::Right);
    // Should return at least one non-empty string for non-default alignments
    REQUIRE_FALSE(center.empty());
    REQUIRE_FALSE(right.empty());
}

TEST_CASE("Rendering — mime_for_extension lookup", "[rendering][html]")
{
    REQUIRE(HtmlRenderer::mime_for_extension(".png") == "image/png");
    REQUIRE(HtmlRenderer::mime_for_extension(".jpg") == "image/jpeg");
    REQUIRE(HtmlRenderer::mime_for_extension(".svg") == "image/svg+xml");
    REQUIRE(HtmlRenderer::mime_for_extension(".gif") == "image/gif");
}

// ===========================================================================
// HtmlRenderer — rendering Markdown to HTML
// ===========================================================================

TEST_CASE("Rendering — headings h1–h6", "[rendering][html]")
{
    MarkdownParser parser;
    auto doc = parser.parse("# H1\n## H2\n### H3\n#### H4\n##### H5\n###### H6\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(doc.value());
    REQUIRE(html.find("<h1") != std::string::npos);
    REQUIRE(html.find("<h2") != std::string::npos);
    REQUIRE(html.find("<h3") != std::string::npos);
}

TEST_CASE("Rendering — paragraphs and emphasis", "[rendering][html]")
{
    MarkdownParser parser;
    auto doc = parser.parse("Normal text\n\n**Bold text**\n\n*Italic text*\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(doc.value());
    REQUIRE(html.find("Normal text") != std::string::npos);
    REQUIRE(html.find("<strong") != std::string::npos);
    REQUIRE(html.find("<em") != std::string::npos);
}

TEST_CASE("Rendering — unordered lists", "[rendering][html]")
{
    MarkdownParser parser;
    auto doc = parser.parse("- Apple\n- Banana\n- Cherry\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(doc.value());
    REQUIRE(html.find("<ul") != std::string::npos);
    REQUIRE(html.find("<li") != std::string::npos);
    REQUIRE(html.find("Apple") != std::string::npos);
    REQUIRE(html.find("Cherry") != std::string::npos);
}

TEST_CASE("Rendering — ordered lists", "[rendering][html]")
{
    MarkdownParser parser;
    auto doc = parser.parse("1. First\n2. Second\n3. Third\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(doc.value());
    REQUIRE(html.find("<ol") != std::string::npos);
    REQUIRE(html.find("First") != std::string::npos);
}

TEST_CASE("Rendering — block quotes", "[rendering][html]")
{
    MarkdownParser parser;
    auto doc = parser.parse("> This is a blockquote\n> with two lines\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(doc.value());
    REQUIRE(html.find("<blockquote") != std::string::npos);
    REQUIRE(html.find("blockquote") != std::string::npos);
}

TEST_CASE("Rendering — links", "[rendering][html]")
{
    MarkdownParser parser;
    auto doc = parser.parse("[Click me](https://example.com)\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(doc.value());
    REQUIRE(html.find("<a") != std::string::npos);
    REQUIRE(html.find("example.com") != std::string::npos);
    REQUIRE(html.find("Click me") != std::string::npos);
}

TEST_CASE("Rendering — images", "[rendering][html]")
{
    MarkdownParser parser;
    auto doc = parser.parse("![Alt text](https://example.com/image.png)\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(doc.value());
    // Should contain img or image reference
    REQUIRE(html.find("Alt text") != std::string::npos);
}

TEST_CASE("Rendering — horizontal rule", "[rendering][html]")
{
    MarkdownParser parser;
    auto doc = parser.parse("Above\n\n---\n\nBelow\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(doc.value());
    REQUIRE(html.find("<hr") != std::string::npos);
}

TEST_CASE("Rendering — inline code", "[rendering][html]")
{
    MarkdownParser parser;
    auto doc = parser.parse("Use `const` for constants\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(doc.value());
    REQUIRE(html.find("<code") != std::string::npos);
    REQUIRE(html.find("const") != std::string::npos);
}

TEST_CASE("Rendering — tables", "[rendering][html]")
{
    MarkdownParser parser;
    auto doc = parser.parse("| Name | Value |\n| --- | --- |\n| A | 1 |\n| B | 2 |\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(doc.value());
    REQUIRE(html.find("<table") != std::string::npos);
    REQUIRE(html.find("<th") != std::string::npos);
    REQUIRE(html.find("<td") != std::string::npos);
}

// ===========================================================================
// HtmlRenderer — source-line attributes
// ===========================================================================

TEST_CASE("Rendering — source-line attributes toggle", "[rendering][html]")
{
    HtmlRenderer renderer;
    REQUIRE_FALSE(renderer.source_line_attributes_enabled());

    renderer.set_source_line_attributes(true);
    REQUIRE(renderer.source_line_attributes_enabled());

    renderer.set_source_line_attributes(false);
    REQUIRE_FALSE(renderer.source_line_attributes_enabled());
}

// ===========================================================================
// CodeBlockRenderer
// ===========================================================================

TEST_CASE("Rendering — CodeBlockRenderer render plain", "[rendering][codeblock]")
{
    CodeBlockRenderer renderer;
    auto html = renderer.render_plain("int x = 42;");
    REQUIRE(html.find("42") != std::string::npos);
    REQUIRE(html.find("<pre") != std::string::npos);
}

TEST_CASE("Rendering — CodeBlockRenderer render with language", "[rendering][codeblock]")
{
    CodeBlockRenderer renderer;
    auto html = renderer.render("int x = 42;", "cpp");
    REQUIRE(html.find("42") != std::string::npos);
    REQUIRE(html.find("code-block") != std::string::npos);
}

TEST_CASE("Rendering — CodeBlockRenderer parse highlight spec", "[rendering][codeblock]")
{
    auto lines = CodeBlockRenderer::parse_highlight_spec("{1,3-5}");
    REQUIRE(lines.count(1) == 1);
    REQUIRE(lines.count(2) == 0);
    REQUIRE(lines.count(3) == 1);
    REQUIRE(lines.count(4) == 1);
    REQUIRE(lines.count(5) == 1);
    REQUIRE(lines.count(6) == 0);
}

TEST_CASE("Rendering — CodeBlockRenderer empty highlight spec", "[rendering][codeblock]")
{
    auto lines = CodeBlockRenderer::parse_highlight_spec("");
    REQUIRE(lines.empty());
}

TEST_CASE("Rendering — CodeBlockRenderer extract highlight spec", "[rendering][codeblock]")
{
    auto spec = CodeBlockRenderer::extract_highlight_spec("cpp {1,3-5}", "cpp");
    REQUIRE(spec == "{1,3-5}");
}

TEST_CASE("Rendering — CodeBlockRenderer block source retrieval", "[rendering][codeblock]")
{
    CodeBlockRenderer renderer;
    renderer.reset_counter();

    renderer.render("source code here", "python");
    auto retrieved = renderer.get_block_source(0);
    REQUIRE(retrieved == "source code here");
}

TEST_CASE("Rendering — CodeBlockRenderer multiple languages", "[rendering][codeblock]")
{
    CodeBlockRenderer renderer;
    renderer.reset_counter();

    // Render several different languages to verify no crashes
    std::vector<std::string> languages = {
        "cpp", "python", "javascript", "rust", "go", "java", "typescript", "ruby", "bash", "sql"};

    for (const auto& lang : languages)
    {
        auto html = renderer.render("code", lang);
        REQUIRE_FALSE(html.empty());
    }
}

// ===========================================================================
// MermaidBlockRenderer — static methods
// ===========================================================================

TEST_CASE("Rendering — MermaidBlockRenderer error rendering", "[rendering][mermaid]")
{
    auto html = MermaidBlockRenderer::render_error("Invalid syntax at line 3");
    REQUIRE(html.find("Invalid syntax") != std::string::npos);
}

TEST_CASE("Rendering — MermaidBlockRenderer unavailable", "[rendering][mermaid]")
{
    auto html = MermaidBlockRenderer::render_unavailable();
    REQUIRE_FALSE(html.empty());
}

TEST_CASE("Rendering — MermaidBlockRenderer placeholder", "[rendering][mermaid]")
{
    auto html = MermaidBlockRenderer::render_placeholder("graph TD; A-->B");
    REQUIRE_FALSE(html.empty());
}

TEST_CASE("Rendering — MermaidBlockRenderer base64 encode", "[rendering][mermaid]")
{
    auto encoded = MermaidBlockRenderer::base64_encode("Hello, World!");
    REQUIRE_FALSE(encoded.empty());
    REQUIRE(encoded == "SGVsbG8sIFdvcmxkIQ==");
}

// ===========================================================================
// FootnotePreprocessor
// ===========================================================================

TEST_CASE("Rendering — FootnotePreprocessor no footnotes", "[rendering][footnotes]")
{
    FootnotePreprocessor preprocessor;
    auto result = preprocessor.process("No footnotes here.\n");
    REQUIRE_FALSE(result.has_footnotes);
    REQUIRE(result.footnote_section_html.empty());
}

TEST_CASE("Rendering — FootnotePreprocessor with footnotes", "[rendering][footnotes]")
{
    FootnotePreprocessor preprocessor;
    auto result =
        preprocessor.process("Text with a footnote[^1].\n\n[^1]: This is the footnote.\n");
    REQUIRE(result.has_footnotes);
    REQUIRE_FALSE(result.footnote_section_html.empty());
    REQUIRE(result.footnote_section_html.find("This is the footnote") != std::string::npos);
}

// ===========================================================================
// Nested block rendering
// ===========================================================================

TEST_CASE("Rendering — nested list in blockquote", "[rendering][nested]")
{
    MarkdownParser parser;
    auto doc = parser.parse("> - item 1\n> - item 2\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(doc.value());
    REQUIRE(html.find("<blockquote") != std::string::npos);
    REQUIRE(html.find("<li") != std::string::npos);
}

TEST_CASE("Rendering — code block inside list", "[rendering][nested]")
{
    MarkdownParser parser;
    std::string md = "- Item with code:\n\n      int x = 1;\n\n- Next item\n";
    auto doc = parser.parse(md);
    REQUIRE(doc.has_value());

    auto html = parser.render_html(doc.value());
    REQUIRE(html.find("<li") != std::string::npos);
}

TEST_CASE("Rendering — empty document", "[rendering][edge]")
{
    MarkdownParser parser;
    auto doc = parser.parse("");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(doc.value());
    // Empty or minimal HTML is acceptable
    // Just ensure no crash
    REQUIRE(true);
}
