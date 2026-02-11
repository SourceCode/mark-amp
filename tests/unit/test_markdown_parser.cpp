#include "core/MarkdownParser.h"
#include "core/Md4cWrapper.h"
#include "core/Types.h"
#include "rendering/HtmlRenderer.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace markamp::core;
using Catch::Matchers::ContainsSubstring;

// Helper: parse or fail
static auto parse_ok(std::string_view md) -> MarkdownDocument
{
    Md4cParser parser;
    auto result = parser.parse(md);
    REQUIRE(result.has_value());
    return std::move(*result);
}

// Helper: parse + render
static auto render(std::string_view md) -> std::string
{
    auto doc = parse_ok(md);
    markamp::rendering::HtmlRenderer renderer;
    return renderer.render(doc);
}

// ═══════════════════════════════════════════════════════
// MdNode helpers
// ═══════════════════════════════════════════════════════

TEST_CASE("MdNode::is_block for block types", "[markdown][ast]")
{
    MdNode heading;
    heading.type = MdNodeType::Heading;
    REQUIRE(heading.is_block());

    MdNode para;
    para.type = MdNodeType::Paragraph;
    REQUIRE(para.is_block());
}

TEST_CASE("MdNode::is_inline for inline types", "[markdown][ast]")
{
    MdNode text;
    text.type = MdNodeType::Text;
    REQUIRE(text.is_inline());

    MdNode em;
    em.type = MdNodeType::Emphasis;
    REQUIRE(em.is_inline());
}

TEST_CASE("MdNode::plain_text extracts text recursively", "[markdown][ast]")
{
    MdNode parent;
    parent.type = MdNodeType::Paragraph;

    MdNode child1;
    child1.type = MdNodeType::Text;
    child1.text_content = "Hello ";

    MdNode child2;
    child2.type = MdNodeType::Text;
    child2.text_content = "World";

    parent.children.push_back(child1);
    parent.children.push_back(child2);

    REQUIRE(parent.plain_text() == "Hello World");
}

TEST_CASE("MdNode::find_all finds nodes of given type", "[markdown][ast]")
{
    MdNode root;
    root.type = MdNodeType::Document;

    MdNode h1;
    h1.type = MdNodeType::Heading;
    h1.heading_level = 1;

    MdNode h2;
    h2.type = MdNodeType::Heading;
    h2.heading_level = 2;

    MdNode para;
    para.type = MdNodeType::Paragraph;

    root.children.push_back(h1);
    root.children.push_back(para);
    root.children.push_back(h2);

    auto headings = root.find_all(MdNodeType::Heading);
    REQUIRE(headings.size() == 2);
}

// ═══════════════════════════════════════════════════════
// MarkdownDocument helpers
// ═══════════════════════════════════════════════════════

TEST_CASE("MarkdownDocument::heading_count", "[markdown][ast]")
{
    auto doc = parse_ok("# H1\n## H2\n### H3\n");
    REQUIRE(doc.heading_count() == 3);
}

TEST_CASE("MarkdownDocument::word_count", "[markdown][ast]")
{
    auto doc = parse_ok("Hello world, this is a test.\n");
    REQUIRE(doc.word_count() == 6);
}

TEST_CASE("MarkdownDocument::has_mermaid", "[markdown][ast]")
{
    auto doc_no = parse_ok("# No mermaid here\n");
    REQUIRE_FALSE(doc_no.has_mermaid());

    auto doc_yes = parse_ok("```mermaid\ngraph TD;\n```\n");
    REQUIRE(doc_yes.has_mermaid());
}

TEST_CASE("MarkdownDocument::has_tables", "[markdown][ast]")
{
    auto doc = parse_ok("| A | B |\n|---|---|\n| 1 | 2 |\n");
    REQUIRE(doc.has_tables());
}

TEST_CASE("MarkdownDocument::has_task_lists", "[markdown][ast]")
{
    auto doc = parse_ok("- [x] Done\n- [ ] Todo\n");
    REQUIRE(doc.has_task_lists());
}

// ═══════════════════════════════════════════════════════
// Parsing: paragraph
// ═══════════════════════════════════════════════════════

TEST_CASE("Parse plain paragraph", "[markdown][parse]")
{
    auto doc = parse_ok("Hello, world!\n");
    REQUIRE(doc.root.type == MdNodeType::Document);
    REQUIRE(doc.root.children.size() >= 1);

    auto& para = doc.root.children[0];
    REQUIRE(para.type == MdNodeType::Paragraph);
    REQUIRE(para.plain_text() == "Hello, world!");
}

// ═══════════════════════════════════════════════════════
// Parsing: headings
// ═══════════════════════════════════════════════════════

TEST_CASE("Parse headings H1-H6", "[markdown][parse]")
{
    auto doc = parse_ok("# H1\n## H2\n### H3\n#### H4\n##### H5\n###### H6\n");
    auto headings = doc.root.find_all(MdNodeType::Heading);
    REQUIRE(headings.size() == 6);
    REQUIRE(headings[0]->heading_level == 1);
    REQUIRE(headings[1]->heading_level == 2);
    REQUIRE(headings[2]->heading_level == 3);
    REQUIRE(headings[3]->heading_level == 4);
    REQUIRE(headings[4]->heading_level == 5);
    REQUIRE(headings[5]->heading_level == 6);
}

// ═══════════════════════════════════════════════════════
// Parsing: emphasis and strong
// ═══════════════════════════════════════════════════════

TEST_CASE("Parse emphasis (italic)", "[markdown][parse]")
{
    auto doc = parse_ok("*italic*\n");
    auto& para = doc.root.children[0];
    auto emphasis = para.find_all(MdNodeType::Emphasis);
    REQUIRE(emphasis.size() == 1);
}

TEST_CASE("Parse strong (bold)", "[markdown][parse]")
{
    auto doc = parse_ok("**bold**\n");
    auto& para = doc.root.children[0];
    auto strong = para.find_all(MdNodeType::Strong);
    REQUIRE(strong.size() == 1);
}

// ═══════════════════════════════════════════════════════
// Parsing: inline code
// ═══════════════════════════════════════════════════════

TEST_CASE("Parse inline code", "[markdown][parse]")
{
    auto doc = parse_ok("Use `code` here\n");
    auto codes = doc.root.find_all(MdNodeType::Code);
    REQUIRE(codes.size() == 1);
}

// ═══════════════════════════════════════════════════════
// Parsing: fenced code block
// ═══════════════════════════════════════════════════════

TEST_CASE("Parse fenced code block with language", "[markdown][parse]")
{
    auto doc = parse_ok("```javascript\nconsole.log('hi');\n```\n");
    auto blocks = doc.root.find_all(MdNodeType::FencedCodeBlock);
    REQUIRE(blocks.size() == 1);
    REQUIRE(blocks[0]->language == "javascript");
    REQUIRE(blocks[0]->text_content == "console.log('hi');\n");
    REQUIRE(doc.code_languages.size() == 1);
    REQUIRE(doc.code_languages[0] == "javascript");
}

TEST_CASE("Parse fenced code block without language", "[markdown][parse]")
{
    auto doc = parse_ok("```\nplain code\n```\n");
    auto blocks = doc.root.find_all(MdNodeType::FencedCodeBlock);
    REQUIRE(blocks.size() == 1);
    REQUIRE(blocks[0]->language.empty());
}

// ═══════════════════════════════════════════════════════
// Parsing: mermaid detection
// ═══════════════════════════════════════════════════════

TEST_CASE("Parse mermaid code block", "[markdown][parse][mermaid]")
{
    auto doc = parse_ok("```mermaid\ngraph TD;\nA --> B;\n```\n");
    REQUIRE(doc.has_mermaid());
    REQUIRE(doc.mermaid_blocks.size() == 1);
    REQUIRE_THAT(doc.mermaid_blocks[0], ContainsSubstring("graph TD"));

    auto meramaids = doc.root.find_all(MdNodeType::MermaidBlock);
    REQUIRE(meramaids.size() == 1);
}

TEST_CASE("Multiple mermaid blocks detected", "[markdown][parse][mermaid]")
{
    auto doc = parse_ok("```mermaid\ngraph A;\n```\n\n"
                        "```mermaid\ngraph B;\n```\n");
    REQUIRE(doc.mermaid_blocks.size() == 2);
}

// ═══════════════════════════════════════════════════════
// Parsing: lists
// ═══════════════════════════════════════════════════════

TEST_CASE("Parse unordered list", "[markdown][parse]")
{
    auto doc = parse_ok("- item 1\n- item 2\n- item 3\n");
    auto lists = doc.root.find_all(MdNodeType::UnorderedList);
    REQUIRE(lists.size() == 1);

    auto items = lists[0]->find_all(MdNodeType::ListItem);
    REQUIRE(items.size() == 3);
}

TEST_CASE("Parse ordered list", "[markdown][parse]")
{
    auto doc = parse_ok("1. first\n2. second\n3. third\n");
    auto lists = doc.root.find_all(MdNodeType::OrderedList);
    REQUIRE(lists.size() == 1);

    auto items = lists[0]->find_all(MdNodeType::ListItem);
    REQUIRE(items.size() == 3);
}

TEST_CASE("Parse ordered list with start number", "[markdown][parse]")
{
    auto doc = parse_ok("5. five\n6. six\n");
    auto lists = doc.root.find_all(MdNodeType::OrderedList);
    REQUIRE(lists.size() == 1);
    REQUIRE(lists[0]->start_number == 5);
}

TEST_CASE("Parse task list", "[markdown][parse]")
{
    auto doc = parse_ok("- [x] Done\n- [ ] Todo\n");
    auto markers = doc.root.find_all(MdNodeType::TaskListMarker);
    REQUIRE(markers.size() == 2);
    REQUIRE(markers[0]->is_checked);
    REQUIRE_FALSE(markers[1]->is_checked);
}

// ═══════════════════════════════════════════════════════
// Parsing: links and images
// ═══════════════════════════════════════════════════════

TEST_CASE("Parse inline link", "[markdown][parse]")
{
    auto doc = parse_ok("[click here](https://example.com)\n");
    auto links = doc.root.find_all(MdNodeType::Link);
    REQUIRE(links.size() == 1);
    REQUIRE(links[0]->url == "https://example.com");
}

TEST_CASE("Parse link with title", "[markdown][parse]")
{
    auto doc = parse_ok("[link](https://example.com \"My Title\")\n");
    auto links = doc.root.find_all(MdNodeType::Link);
    REQUIRE(links.size() == 1);
    REQUIRE(links[0]->title == "My Title");
}

TEST_CASE("Parse image", "[markdown][parse]")
{
    auto doc = parse_ok("![alt text](image.png)\n");
    auto images = doc.root.find_all(MdNodeType::Image);
    REQUIRE(images.size() == 1);
    REQUIRE(images[0]->url == "image.png");
}

// ═══════════════════════════════════════════════════════
// Parsing: blockquote
// ═══════════════════════════════════════════════════════

TEST_CASE("Parse blockquote", "[markdown][parse]")
{
    auto doc = parse_ok("> This is a quote\n");
    auto quotes = doc.root.find_all(MdNodeType::BlockQuote);
    REQUIRE(quotes.size() == 1);
}

// ═══════════════════════════════════════════════════════
// Parsing: table
// ═══════════════════════════════════════════════════════

TEST_CASE("Parse table with alignment", "[markdown][parse]")
{
    auto doc = parse_ok("| Left | Center | Right |\n|:-----|:------:|------:|\n| a | b | c |\n");
    auto cells = doc.root.find_all(MdNodeType::TableCell);
    REQUIRE(cells.size() >= 6);

    // Header cells
    REQUIRE(cells[0]->is_header);
    REQUIRE(cells[0]->alignment == MdAlignment::Left);
    REQUIRE(cells[1]->alignment == MdAlignment::Center);
    REQUIRE(cells[2]->alignment == MdAlignment::Right);
}

// ═══════════════════════════════════════════════════════
// Parsing: horizontal rule
// ═══════════════════════════════════════════════════════

TEST_CASE("Parse horizontal rule", "[markdown][parse]")
{
    auto doc = parse_ok("---\n");
    auto hrs = doc.root.find_all(MdNodeType::HorizontalRule);
    REQUIRE(hrs.size() == 1);
}

// ═══════════════════════════════════════════════════════
// Parsing: strikethrough
// ═══════════════════════════════════════════════════════

TEST_CASE("Parse strikethrough", "[markdown][parse]")
{
    auto doc = parse_ok("~~deleted~~\n");
    auto dels = doc.root.find_all(MdNodeType::Strikethrough);
    REQUIRE(dels.size() == 1);
}

// ═══════════════════════════════════════════════════════
// HTML rendering
// ═══════════════════════════════════════════════════════

TEST_CASE("Render heading HTML", "[markdown][html]")
{
    auto html = render("# Hello\n");
    REQUIRE_THAT(html, ContainsSubstring("<h1 id=\"hello\">Hello</h1>"));
}

TEST_CASE("Render paragraph HTML", "[markdown][html]")
{
    auto html = render("A paragraph.\n");
    REQUIRE_THAT(html, ContainsSubstring("<p>A paragraph.</p>"));
}

TEST_CASE("Render emphasis HTML", "[markdown][html]")
{
    auto html = render("*italic*\n");
    REQUIRE_THAT(html, ContainsSubstring("<em>italic</em>"));
}

TEST_CASE("Render strong HTML", "[markdown][html]")
{
    auto html = render("**bold**\n");
    REQUIRE_THAT(html, ContainsSubstring("<strong>bold</strong>"));
}

TEST_CASE("Render inline code HTML", "[markdown][html]")
{
    auto html = render("Use `foo` here\n");
    REQUIRE_THAT(html, ContainsSubstring("<code>foo</code>"));
}

TEST_CASE("Render fenced code block HTML", "[markdown][html]")
{
    auto html = render("```js\nalert('hi');\n```\n");
    REQUIRE_THAT(html, ContainsSubstring("<pre class=\"code-block\"><code class=\"language-js\">"));
    REQUIRE_THAT(html, ContainsSubstring("alert"));
    REQUIRE_THAT(html, ContainsSubstring("&#39;hi&#39;"));
}

TEST_CASE("Render link HTML", "[markdown][html]")
{
    auto html = render("[link](https://example.com)\n");
    REQUIRE_THAT(html, ContainsSubstring("<a href=\"https://example.com\">link</a>"));
}

TEST_CASE("Render image HTML", "[markdown][html]")
{
    auto html = render("![alt](img.png)\n");
    // Without a valid base_path + file, images render as placeholders
    REQUIRE_THAT(html, ContainsSubstring("image-missing"));
    REQUIRE_THAT(html, ContainsSubstring("img.png"));
}

TEST_CASE("Render unordered list HTML", "[markdown][html]")
{
    auto html = render("- a\n- b\n");
    REQUIRE_THAT(html, ContainsSubstring("<ul>"));
    REQUIRE_THAT(html, ContainsSubstring("<li>"));
}

TEST_CASE("Render ordered list HTML", "[markdown][html]")
{
    auto html = render("1. a\n2. b\n");
    REQUIRE_THAT(html, ContainsSubstring("<ol>"));
    REQUIRE_THAT(html, ContainsSubstring("<li>"));
}

TEST_CASE("Render blockquote HTML", "[markdown][html]")
{
    auto html = render("> quote\n");
    REQUIRE_THAT(html, ContainsSubstring("<blockquote>"));
}

TEST_CASE("Render table HTML", "[markdown][html]")
{
    auto html = render("| A | B |\n|---|---|\n| 1 | 2 |\n");
    REQUIRE_THAT(html, ContainsSubstring("<table>"));
    REQUIRE_THAT(html, ContainsSubstring("<th>"));
    REQUIRE_THAT(html, ContainsSubstring("<td>"));
}

TEST_CASE("Render horizontal rule HTML", "[markdown][html]")
{
    auto html = render("---\n");
    REQUIRE_THAT(html, ContainsSubstring("<hr>"));
}

TEST_CASE("Render strikethrough HTML", "[markdown][html]")
{
    auto html = render("~~deleted~~\n");
    REQUIRE_THAT(html, ContainsSubstring("<del>deleted</del>"));
}

TEST_CASE("Render task list HTML", "[markdown][html]")
{
    auto html = render("- [x] Done\n- [ ] Todo\n");
    REQUIRE_THAT(html, ContainsSubstring("checked"));
    REQUIRE_THAT(html, ContainsSubstring("checkbox"));
}

TEST_CASE("Render mermaid block HTML", "[markdown][html]")
{
    auto html = render("```mermaid\ngraph TD;\n```\n");
    REQUIRE_THAT(html, ContainsSubstring("mermaid-block"));
}

// ═══════════════════════════════════════════════════════
// HTML escaping
// ═══════════════════════════════════════════════════════

TEST_CASE("HTML special characters are escaped", "[markdown][html]")
{
    auto html = render("Use <div> & \"quotes\"\n");
    REQUIRE_THAT(html, ContainsSubstring("&lt;div&gt;"));
    REQUIRE_THAT(html, ContainsSubstring("&amp;"));
    REQUIRE_THAT(html, ContainsSubstring("&quot;quotes&quot;"));
}

// ═══════════════════════════════════════════════════════
// Edge cases
// ═══════════════════════════════════════════════════════

TEST_CASE("Parse empty document", "[markdown][edge]")
{
    auto doc = parse_ok("");
    REQUIRE(doc.root.children.empty());
    REQUIRE(doc.word_count() == 0);
}

TEST_CASE("Parse whitespace-only document", "[markdown][edge]")
{
    auto doc = parse_ok("   \n   \n   \n");
    REQUIRE(doc.word_count() == 0);
}

TEST_CASE("Parse deeply nested blockquotes", "[markdown][edge]")
{
    auto doc = parse_ok("> > > > deep\n");
    auto quotes = doc.root.find_all(MdNodeType::BlockQuote);
    REQUIRE(quotes.size() >= 4);
}

// ═══════════════════════════════════════════════════════
// IMarkdownParser interface
// ═══════════════════════════════════════════════════════

TEST_CASE("MarkdownParser implements IMarkdownParser", "[markdown][interface]")
{
    MarkdownParser parser;
    auto result = parser.parse("# Test\n\nParagraph text.\n");
    REQUIRE(result.has_value());

    auto html = parser.render_html(*result);
    REQUIRE_THAT(html, ContainsSubstring("<h1 id=\"test\">Test</h1>"));
    REQUIRE_THAT(html, ContainsSubstring("<p>Paragraph text.</p>"));
}

// ═══════════════════════════════════════════════════════
// Builder flags
// ═══════════════════════════════════════════════════════

TEST_CASE("Md4cParser builder flags", "[markdown][parse]")
{
    Md4cParser parser;
    parser.enable_tables()
        .enable_task_lists()
        .enable_strikethrough()
        .enable_autolinks()
        .enable_no_html();

    auto result = parser.parse("| A |\n|---|\n| 1 |\n");
    REQUIRE(result.has_value());
    REQUIRE(result->has_tables());
}
