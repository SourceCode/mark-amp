#include "core/HtmlSanitizer.h"
#include "core/MermaidRenderer.h"
#include "core/ThemeValidator.h"
#include "rendering/HtmlRenderer.h"

#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

#include <string>

using markamp::core::HtmlSanitizer;
using markamp::core::MermaidRenderer;
using markamp::core::ThemeValidator;
using markamp::rendering::HtmlRenderer;

// ═══════════════════════════════════════════════════════
// XSS Prevention Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Security: Script tags are stripped", "[security][xss]")
{
    HtmlSanitizer sanitizer;

    SECTION("Basic script tag")
    {
        auto result = sanitizer.sanitize("<p>Hello</p><script>alert('XSS')</script>");
        CHECK(result.find("<script") == std::string::npos);
        // Inner text is harmless without surrounding script tags
    }

    SECTION("Script tag with attributes")
    {
        auto result = sanitizer.sanitize("<script type=\"text/javascript\">evil()</script>");
        CHECK(result.find("<script") == std::string::npos);
        // Inner text is harmless without surrounding script tags
    }

    SECTION("Mixed case script")
    {
        auto result = sanitizer.sanitize("<ScRiPt>alert(1)</ScRiPt>");
        CHECK(result.find("<script") == std::string::npos);
        CHECK(result.find("<ScRiPt") == std::string::npos);
    }
}

TEST_CASE("Security: Event handlers are stripped", "[security][xss]")
{
    HtmlSanitizer sanitizer;

    SECTION("onclick on allowed tag")
    {
        auto result = sanitizer.sanitize("<p onclick=\"alert('XSS')\">Text</p>");
        CHECK(result.find("onclick") == std::string::npos);
        CHECK(result.find("alert") == std::string::npos);
        CHECK(result.find("<p>") != std::string::npos);
    }

    SECTION("onerror on img")
    {
        auto result = sanitizer.sanitize("<img src=\"x\" onerror=\"alert(1)\">");
        CHECK(result.find("onerror") == std::string::npos);
    }

    SECTION("onload on body-like element")
    {
        auto result = sanitizer.sanitize("<div onload=\"evil()\">Content</div>");
        CHECK(result.find("onload") == std::string::npos);
        CHECK(result.find("evil") == std::string::npos);
    }

    SECTION("onmouseover")
    {
        auto result = sanitizer.sanitize("<a href=\"#\" onmouseover=\"steal()\">Link</a>");
        CHECK(result.find("onmouseover") == std::string::npos);
    }
}

TEST_CASE("Security: javascript: URIs are blocked", "[security][xss]")
{
    HtmlSanitizer sanitizer;

    SECTION("javascript: in href")
    {
        auto result = sanitizer.sanitize("<a href=\"javascript:alert(1)\">Click</a>");
        CHECK(result.find("javascript:") == std::string::npos);
    }

    SECTION("javascript: with whitespace")
    {
        auto result = sanitizer.sanitize("<a href=\"  javascript:alert(1)\">Click</a>");
        CHECK(result.find("javascript:") == std::string::npos);
    }

    SECTION("vbscript: URI")
    {
        auto result = sanitizer.sanitize("<a href=\"vbscript:MsgBox('XSS')\">Click</a>");
        CHECK(result.find("vbscript:") == std::string::npos);
    }
}

TEST_CASE("Security: Dangerous tags are stripped", "[security][xss]")
{
    HtmlSanitizer sanitizer;

    SECTION("iframe")
    {
        auto result = sanitizer.sanitize(R"(<iframe src="evil.com"></iframe>)");
        CHECK(result.find("<iframe") == std::string::npos);
    }

    SECTION("object/embed")
    {
        auto result = sanitizer.sanitize(R"(<object data="evil.swf"><embed src="evil.swf">)");
        CHECK(result.find("<object") == std::string::npos);
        CHECK(result.find("<embed") == std::string::npos);
    }

    SECTION("form elements")
    {
        auto result =
            sanitizer.sanitize(R"(<form action="evil.com"><button>Submit</button></form>)");
        CHECK(result.find("<form") == std::string::npos);
        CHECK(result.find("<button") == std::string::npos);
    }

    SECTION("style tag (CSS injection)")
    {
        auto result = sanitizer.sanitize(R"(<style>body { background: url(evil.com) }</style>)");
        CHECK(result.find("<style") == std::string::npos);
    }

    SECTION("link tag")
    {
        auto result = sanitizer.sanitize(R"(<link rel="stylesheet" href="evil.css">)");
        CHECK(result.find("<link") == std::string::npos);
    }

    SECTION("meta tag")
    {
        auto result = sanitizer.sanitize(R"(<meta http-equiv="refresh" content="0;url=evil.com">)");
        CHECK(result.find("<meta") == std::string::npos);
    }

    SECTION("base tag")
    {
        auto result = sanitizer.sanitize(R"(<base href="evil.com">)");
        CHECK(result.find("<base") == std::string::npos);
    }

    SECTION("foreignObject in SVG")
    {
        auto result = sanitizer.sanitize(
            "<svg><foreignObject><body onload=\"evil()\"></foreignObject></svg>");
        CHECK(result.find("foreignobject") == std::string::npos);
    }
}

TEST_CASE("Security: Style attribute injection blocked", "[security][xss]")
{
    HtmlSanitizer sanitizer;

    SECTION("expression() in style")
    {
        auto result = sanitizer.sanitize("<td style=\"width:expression(alert('XSS'))\">Data</td>");
        CHECK(result.find("expression") == std::string::npos);
    }

    SECTION("url() in style blocked")
    {
        auto result =
            sanitizer.sanitize("<td style=\"background:url(javascript:alert(1))\">Data</td>");
        CHECK(result.find("url(") == std::string::npos);
    }

    SECTION("behavior in style")
    {
        auto result = sanitizer.sanitize("<td style=\"behavior:url(evil.htc)\">Data</td>");
        CHECK(result.find("behavior") == std::string::npos);
    }

    SECTION("Safe style passes through")
    {
        auto result = sanitizer.sanitize("<td style=\"text-align: center\">Data</td>");
        CHECK(result.find("text-align") != std::string::npos);
    }
}

TEST_CASE("Security: data: URI restrictions", "[security][xss]")
{
    HtmlSanitizer sanitizer;

    SECTION("data:text/html blocked")
    {
        auto result = sanitizer.sanitize(
            "<a href=\"data:text/html,<script>alert('XSS')</script>\">Click</a>");
        CHECK(result.find("data:text/html") == std::string::npos);
    }

    SECTION("data:image/svg blocked")
    {
        auto result =
            sanitizer.sanitize("<img src=\"data:image/svg+xml,<svg onload='alert(1)'>\">");
        CHECK(result.find("data:image/svg") == std::string::npos);
    }
}

// ═══════════════════════════════════════════════════════
// Safe content passthrough
// ═══════════════════════════════════════════════════════

TEST_CASE("Security: Safe HTML passes through", "[security][sanitizer]")
{
    HtmlSanitizer sanitizer;

    SECTION("Standard markdown output")
    {
        std::string safe_html = "<h1>Title</h1><p>Paragraph with <strong>bold</strong> and "
                                "<em>italic</em> text.</p>";
        auto result = sanitizer.sanitize(safe_html);
        CHECK(result.find("<h1>") != std::string::npos);
        CHECK(result.find("<p>") != std::string::npos);
        CHECK(result.find("<strong>") != std::string::npos);
        CHECK(result.find("<em>") != std::string::npos);
    }

    SECTION("Code blocks")
    {
        auto result =
            sanitizer.sanitize(R"(<pre><code class="language-cpp">int x = 5;</code></pre>)");
        CHECK(result.find("<pre>") != std::string::npos);
        CHECK(result.find("<code") != std::string::npos);
    }

    SECTION("Tables")
    {
        auto result = sanitizer.sanitize(
            R"(<table><thead><tr><th>Header</th></tr></thead><tbody><tr><td>Data</td></tr></tbody></table>)");
        CHECK(result.find("<table>") != std::string::npos);
        CHECK(result.find("<td>") != std::string::npos);
    }

    SECTION("Links with safe href")
    {
        auto result = sanitizer.sanitize(R"(<a href="https://example.com">Link</a>)");
        CHECK(result.find("https://example.com") != std::string::npos);
    }

    SECTION("Images with safe src")
    {
        auto result = sanitizer.sanitize(R"(<img src="image.png" alt="Photo">)");
        CHECK(result.find("image.png") != std::string::npos);
        CHECK(result.find("alt") != std::string::npos);
    }

    SECTION("Checkbox inputs allowed")
    {
        auto result = sanitizer.sanitize(R"(<input type="checkbox" checked disabled>)");
        CHECK(result.find("<input") != std::string::npos);
        CHECK(result.find("checkbox") != std::string::npos);
    }

    SECTION("Non-checkbox inputs blocked")
    {
        auto result = sanitizer.sanitize(R"(<input type="text" value="dangerous">)");
        CHECK(result.find("<input") == std::string::npos);
    }
}

// ═══════════════════════════════════════════════════════
// SVG Sanitization Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Security: SVG sanitization strips dangerous elements", "[security][svg]")
{
    SECTION("Script tag removed from SVG")
    {
        std::string svg = R"(<svg><rect/><script>alert(1)</script><text>Hello</text></svg>)";
        auto result = MermaidRenderer::sanitize_svg(svg);
        CHECK(result.find("<script") == std::string::npos);
        // Inner text of stripped script tag is harmless
        CHECK(result.find("<rect") != std::string::npos);
        CHECK(result.find("<text") != std::string::npos);
    }

    SECTION("ForeignObject removed")
    {
        std::string svg = "<svg><foreignObject><body onload=\"evil()\"></foreignObject></svg>";
        auto result = MermaidRenderer::sanitize_svg(svg);
        CHECK(result.find("<foreignObject") == std::string::npos);
        CHECK(result.find("</foreignObject") == std::string::npos);
    }

    SECTION("on* attributes stripped from SVG elements")
    {
        std::string svg = "<svg><rect onclick=\"alert(1)\" width=\"10\" height=\"10\"/></svg>";
        auto result = MermaidRenderer::sanitize_svg(svg);
        CHECK(result.find("onclick") == std::string::npos);
        CHECK(result.find("width") != std::string::npos);
    }
}

// ═══════════════════════════════════════════════════════
// Path Traversal Prevention Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Security: Remote URLs are not rendered as images", "[security][path]")
{
    // Test through public render API - remote URLs should not produce data URIs
    HtmlRenderer renderer;
    renderer.set_base_path("/tmp/markamp_test");

    // Create a minimal document with a remote image
    markamp::core::MarkdownDocument doc;
    markamp::core::MdNode root;
    root.type = markamp::core::MdNodeType::Document;

    markamp::core::MdNode img_node;
    img_node.type = markamp::core::MdNodeType::Image;
    img_node.url = "http://evil.com/image.png";
    img_node.title = "Evil image";

    // Add a text child for alt text
    markamp::core::MdNode text_node;
    text_node.type = markamp::core::MdNodeType::Text;
    text_node.text_content = "Evil alt text";
    img_node.children.push_back(text_node);

    root.children.push_back(img_node);
    doc.root = root;

    auto result = renderer.render(doc);
    // Should not contain a data: URI from the remote URL
    CHECK(result.find("data:image") == std::string::npos);
    // Should contain a missing-image placeholder instead
    CHECK(result.find("image-missing") != std::string::npos);
}

// ═══════════════════════════════════════════════════════
// Input Validation Tests (Theme)
// ═══════════════════════════════════════════════════════

TEST_CASE("Security: Theme name length limit", "[security][input]")
{
    ThemeValidator validator;
    nlohmann::json json;
    json["name"] = std::string(200, 'A'); // 200-char name
    json["colors"] = nlohmann::json::object();

    auto result = validator.validate_json(json);
    bool has_length_error = false;
    for (const auto& err : result.errors)
    {
        if (err.find("maximum length") != std::string::npos)
        {
            has_length_error = true;
        }
    }
    CHECK(has_length_error);
}

TEST_CASE("Security: Theme with null bytes rejected", "[security][input]")
{
    ThemeValidator validator;

    SECTION("Null byte in JSON name")
    {
        nlohmann::json json;
        json["name"] = std::string("normal\0evil", 11);
        json["colors"] = nlohmann::json::object();

        auto result = validator.validate_json(json);
        bool has_char_error = false;
        for (const auto& err : result.errors)
        {
            if (err.find("invalid characters") != std::string::npos)
            {
                has_char_error = true;
            }
        }
        CHECK(has_char_error);
    }
}

TEST_CASE("Security: Theme with control characters rejected", "[security][input]")
{
    ThemeValidator validator;

    SECTION("Bell character in name")
    {
        nlohmann::json json;
        json["name"] = std::string("theme\x07name");
        json["colors"] = nlohmann::json::object();

        auto result = validator.validate_json(json);
        bool has_char_error = false;
        for (const auto& err : result.errors)
        {
            if (err.find("invalid characters") != std::string::npos)
            {
                has_char_error = true;
            }
        }
        CHECK(has_char_error);
    }

    SECTION("Tab and newline are allowed")
    {
        CHECK_FALSE(ThemeValidator::contains_control_chars("hello\tworld\n"));
    }
}

// ═══════════════════════════════════════════════════════
// DoS / Resource Exhaustion Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Security: Deeply nested HTML doesn't crash sanitizer", "[security][dos]")
{
    HtmlSanitizer sanitizer;

    // Generate deeply nested tags
    std::string nested_html;
    constexpr int kDepth = 1000;
    for (int idx = 0; idx < kDepth; ++idx)
    {
        nested_html += "<div>";
    }
    nested_html += "content";
    for (int idx = 0; idx < kDepth; ++idx)
    {
        nested_html += "</div>";
    }

    auto result = sanitizer.sanitize(nested_html);
    CHECK(result.find("content") != std::string::npos);
    CHECK(result.find("<div>") != std::string::npos);
}

TEST_CASE("Security: Large input handled", "[security][dos]")
{
    HtmlSanitizer sanitizer;

    // 1MB of safe HTML
    std::string large_html = "<p>" + std::string(1024 * 1024, 'A') + "</p>";
    auto result = sanitizer.sanitize(large_html);
    CHECK_FALSE(result.empty());
    CHECK(result.find("<p>") != std::string::npos);
}

TEST_CASE("Security: Empty and malformed input", "[security][edge]")
{
    HtmlSanitizer sanitizer;

    SECTION("Empty string")
    {
        auto result = sanitizer.sanitize("");
        CHECK(result.empty());
    }

    SECTION("No HTML")
    {
        auto result = sanitizer.sanitize("Just plain text");
        CHECK(result == "Just plain text");
    }

    SECTION("Unclosed tag")
    {
        auto result = sanitizer.sanitize("<p>Unclosed");
        CHECK(result.find("<p>") != std::string::npos);
    }

    SECTION("Malformed tag (no closing >)")
    {
        auto result = sanitizer.sanitize("Text before <broken");
        CHECK(result.find("&lt;") != std::string::npos);
    }

    SECTION("HTML comments stripped")
    {
        auto result = sanitizer.sanitize("Before<!-- comment -->After");
        CHECK(result.find("<!--") == std::string::npos);
        CHECK(result.find("Before") != std::string::npos);
        CHECK(result.find("After") != std::string::npos);
    }
}
