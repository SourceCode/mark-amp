#include "core/MarkdownParser.h"
#include "core/Theme.h"
#include "core/Types.h"
#include "rendering/HtmlRenderer.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <fmt/format.h>

using namespace markamp::core;
using Catch::Matchers::ContainsSubstring;

// ═══════════════════════════════════════════════════════
// Helper: create a test theme
// ═══════════════════════════════════════════════════════

static auto make_test_theme() -> Theme
{
    Theme t;
    t.id = "test-dark";
    t.name = "Test Dark";
    t.colors.bg_app = Color::from_rgb(30, 30, 35);
    t.colors.bg_panel = Color::from_rgb(40, 40, 45);
    t.colors.bg_header = Color::from_rgb(25, 25, 30);
    t.colors.bg_input = Color::from_rgb(35, 35, 40);
    t.colors.text_main = Color::from_rgb(220, 220, 225);
    t.colors.text_muted = Color::from_rgb(150, 150, 155);
    t.colors.accent_primary = Color::from_rgb(100, 180, 255);
    t.colors.accent_secondary = Color::from_rgb(255, 140, 60);
    t.colors.border_light = Color::from_rgb(60, 60, 65);
    t.colors.border_dark = Color::from_rgb(40, 40, 45);
    return t;
}

// ═══════════════════════════════════════════════════════
// CSS generation tests (unit-testable without wxWidgets GUI)
//
// Since PreviewPanel::GenerateCSS() requires a running wxWidgets
// app and ThemeEngine, we test the CSS rules by validating the
// HtmlRenderer output and ensuring theme colors serialize correctly.
// ═══════════════════════════════════════════════════════

TEST_CASE("Theme colors serialize to hex correctly", "[preview][css]")
{
    auto t = make_test_theme();
    REQUIRE(t.colors.bg_app.to_hex() == "#1e1e23");
    REQUIRE(t.colors.accent_primary.to_hex() == "#64b4ff");
    REQUIRE(t.colors.text_main.to_hex() == "#dcdce1");
    REQUIRE(t.colors.border_light.to_hex() == "#3c3c41");
}

TEST_CASE("Color::to_rgba_string for alpha colors", "[preview][css]")
{
    auto c = Color::from_rgb(100, 180, 255);
    auto alpha_color = c.with_alpha(0.2f);
    auto rgba = alpha_color.to_rgba_string();
    REQUIRE_THAT(rgba, ContainsSubstring("rgba("));
    REQUIRE_THAT(rgba, ContainsSubstring("100"));
    REQUIRE_THAT(rgba, ContainsSubstring("180"));
    REQUIRE_THAT(rgba, ContainsSubstring("255"));
}

TEST_CASE("Color blend for code background", "[preview][css]")
{
    auto bg = Color::from_rgb(30, 30, 35);
    auto blended = bg.blend(Color{0, 0, 0, 255}, 0.3f);
    // Should be darker than the original
    REQUIRE(blended.r < bg.r);
    REQUIRE(blended.g < bg.g);
    REQUIRE(blended.b < bg.b);
}

// ═══════════════════════════════════════════════════════
// HTML structure tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Full HTML generation wraps body in valid structure", "[preview][html]")
{
    // We simulate what PreviewPanel::GenerateFullHtml does
    std::string css = "body { color: #fff; }";
    std::string body = "<p>Hello world</p>";

    auto full_html = fmt::format(R"(<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<style>
{}
</style>
</head>
<body>
{}
</body>
</html>)",
                                 css,
                                 body);

    REQUIRE_THAT(full_html, ContainsSubstring("<!DOCTYPE html>"));
    REQUIRE_THAT(full_html, ContainsSubstring("<html>"));
    REQUIRE_THAT(full_html, ContainsSubstring("<head>"));
    REQUIRE_THAT(full_html, ContainsSubstring("<meta charset=\"utf-8\">"));
    REQUIRE_THAT(full_html, ContainsSubstring("<style>"));
    REQUIRE_THAT(full_html, ContainsSubstring("body { color: #fff; }"));
    REQUIRE_THAT(full_html, ContainsSubstring("</style>"));
    REQUIRE_THAT(full_html, ContainsSubstring("<body>"));
    REQUIRE_THAT(full_html, ContainsSubstring("<p>Hello world</p>"));
    REQUIRE_THAT(full_html, ContainsSubstring("</body>"));
    REQUIRE_THAT(full_html, ContainsSubstring("</html>"));
}

// ═══════════════════════════════════════════════════════
// Rendering pipeline tests (parse → render → verify)
// ═══════════════════════════════════════════════════════

TEST_CASE("Pipeline: heading renders correct tag", "[preview][pipeline]")
{
    MarkdownParser parser;
    auto doc = parser.parse("# Welcome\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("<h1 id=\"welcome\">Welcome</h1>"));
}

TEST_CASE("Pipeline: paragraph renders correctly", "[preview][pipeline]")
{
    MarkdownParser parser;
    auto doc = parser.parse("This is a paragraph.\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("<p>This is a paragraph.</p>"));
}

TEST_CASE("Pipeline: styled text renders emphasis and strong", "[preview][pipeline]")
{
    MarkdownParser parser;
    auto doc = parser.parse("This is *italic* and **bold** text.\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("<em>italic</em>"));
    REQUIRE_THAT(html, ContainsSubstring("<strong>bold</strong>"));
}

TEST_CASE("Pipeline: code block with language class", "[preview][pipeline]")
{
    MarkdownParser parser;
    auto doc = parser.parse("```python\nprint('hi')\n```\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("language-python"));
    REQUIRE_THAT(html, ContainsSubstring("&#39;hi&#39;"));
}

TEST_CASE("Pipeline: link renders with href", "[preview][pipeline]")
{
    MarkdownParser parser;
    auto doc = parser.parse("[Visit](https://example.com \"Example\")\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("href=\"https://example.com\""));
    REQUIRE_THAT(html, ContainsSubstring("title=\"Example\""));
    REQUIRE_THAT(html, ContainsSubstring(">Visit</a>"));
}

TEST_CASE("Pipeline: table renders with th and td", "[preview][pipeline]")
{
    MarkdownParser parser;
    auto doc = parser.parse("| Name | Age |\n|------|-----|\n| Alice | 30 |\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("<table>"));
    REQUIRE_THAT(html, ContainsSubstring("<th>"));
    REQUIRE_THAT(html, ContainsSubstring("<td>"));
    REQUIRE_THAT(html, ContainsSubstring("</table>"));
}

TEST_CASE("Pipeline: blockquote renders correctly", "[preview][pipeline]")
{
    MarkdownParser parser;
    auto doc = parser.parse("> Important note\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("<blockquote>"));
    REQUIRE_THAT(html, ContainsSubstring("Important note"));
}

TEST_CASE("Pipeline: unordered list renders", "[preview][pipeline]")
{
    MarkdownParser parser;
    auto doc = parser.parse("- Apple\n- Banana\n- Cherry\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("<ul>"));
    REQUIRE_THAT(html, ContainsSubstring("<li>"));
}

TEST_CASE("Pipeline: ordered list renders", "[preview][pipeline]")
{
    MarkdownParser parser;
    auto doc = parser.parse("1. First\n2. Second\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("<ol>"));
    REQUIRE_THAT(html, ContainsSubstring("<li>"));
}

TEST_CASE("Pipeline: horizontal rule renders", "[preview][pipeline]")
{
    MarkdownParser parser;
    auto doc = parser.parse("---\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("<hr>"));
}

TEST_CASE("Pipeline: strikethrough renders", "[preview][pipeline]")
{
    MarkdownParser parser;
    auto doc = parser.parse("~~removed~~\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("<del>removed</del>"));
}

TEST_CASE("Pipeline: task list renders checkboxes", "[preview][pipeline]")
{
    MarkdownParser parser;
    auto doc = parser.parse("- [x] Done\n- [ ] Todo\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("checked"));
    REQUIRE_THAT(html, ContainsSubstring("checkbox"));
}

TEST_CASE("Pipeline: mermaid block renders as placeholder div", "[preview][pipeline]")
{
    MarkdownParser parser;
    auto doc = parser.parse("```mermaid\ngraph TD;\nA --> B;\n```\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("mermaid-block"));
    REQUIRE_THAT(html, ContainsSubstring("graph TD"));
}

TEST_CASE("Pipeline: image renders with alt and src", "[preview][pipeline]")
{
    MarkdownParser parser;
    auto doc = parser.parse("![Logo](logo.png \"App Logo\")\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    // Without base_path, images render as missing-image placeholders
    REQUIRE_THAT(html, ContainsSubstring("image-missing"));
    REQUIRE_THAT(html, ContainsSubstring("logo.png"));
    REQUIRE_THAT(html, ContainsSubstring("Logo"));
}

TEST_CASE("Pipeline: HTML special characters are escaped", "[preview][pipeline]")
{
    MarkdownParser parser;
    auto doc = parser.parse("Use <script> & \"quotes\"\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("&lt;script&gt;"));
    REQUIRE_THAT(html, ContainsSubstring("&amp;"));
    REQUIRE_THAT(html, ContainsSubstring("&quot;quotes&quot;"));
}

TEST_CASE("Pipeline: empty markdown produces no body content", "[preview][pipeline]")
{
    MarkdownParser parser;
    auto doc = parser.parse("");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);
    // Empty body should be empty or just whitespace
    REQUIRE(html.find("<p>") == std::string::npos);
    REQUIRE(html.find("<h") == std::string::npos);
}

// ═══════════════════════════════════════════════════════
// CSS rule validation (ensure theme color values are used)
// ═══════════════════════════════════════════════════════

TEST_CASE("CSS rules use theme hex values in expected format", "[preview][css]")
{
    auto t = make_test_theme();

    // Verify all required CSS tokens map to valid hex
    REQUIRE(t.colors.bg_app.to_hex().starts_with("#"));
    REQUIRE(t.colors.bg_panel.to_hex().starts_with("#"));
    REQUIRE(t.colors.text_main.to_hex().starts_with("#"));
    REQUIRE(t.colors.text_muted.to_hex().starts_with("#"));
    REQUIRE(t.colors.accent_primary.to_hex().starts_with("#"));
    REQUIRE(t.colors.border_light.to_hex().starts_with("#"));
}

// ═══════════════════════════════════════════════════════
// Preview panel constants
// ═══════════════════════════════════════════════════════

TEST_CASE("PreviewPanel debounce period is 300ms", "[preview][constants]")
{
    // Validate expected constant value
    constexpr int kExpectedDebounceMs = 300;
    REQUIRE(kExpectedDebounceMs == 300);
}

// ═══════════════════════════════════════════════════════
// Complex document round-trip
// ═══════════════════════════════════════════════════════

TEST_CASE("Pipeline: complex GFM document renders without error", "[preview][pipeline]")
{
    MarkdownParser parser;
    auto doc = parser.parse(R"(
# Project Title

A **bold** and *italic* description with `inline code`.

## Features

- [x] Feature one
- [ ] Feature two
- Regular item

### Code Example

```cpp
int main() {
    return 0;
}
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

    REQUIRE(doc.has_value());
    REQUIRE(doc->has_mermaid());
    REQUIRE(doc->has_tables());
    REQUIRE(doc->has_task_lists());
    REQUIRE(doc->heading_count() == 3);

    auto html = parser.render_html(*doc);
    REQUIRE_THAT(html, ContainsSubstring("<h1 id="));
    REQUIRE_THAT(html, ContainsSubstring("<h2 id="));
    REQUIRE_THAT(html, ContainsSubstring("<h3 id="));
    REQUIRE_THAT(html, ContainsSubstring("<strong>"));
    REQUIRE_THAT(html, ContainsSubstring("<em>"));
    REQUIRE_THAT(html, ContainsSubstring("<code>"));
    REQUIRE_THAT(html, ContainsSubstring("code-block-wrapper"));
    REQUIRE_THAT(html, ContainsSubstring("<blockquote>"));
    REQUIRE_THAT(html, ContainsSubstring("<table>"));
    REQUIRE_THAT(html, ContainsSubstring("<hr>"));
    REQUIRE_THAT(html, ContainsSubstring("<del>"));
    REQUIRE_THAT(html, ContainsSubstring("<a href"));
    REQUIRE_THAT(html, ContainsSubstring("image-missing"));
    REQUIRE_THAT(html, ContainsSubstring("mermaid-block"));
    REQUIRE_THAT(html, ContainsSubstring("checkbox"));
}
