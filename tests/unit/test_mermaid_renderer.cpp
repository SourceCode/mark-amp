#include "core/IMermaidRenderer.h"
#include "core/MarkdownParser.h"
#include "core/MermaidRenderer.h"
#include "rendering/HtmlRenderer.h"
#include "rendering/MermaidBlockRenderer.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using Catch::Matchers::ContainsSubstring;

namespace
{

/// Mock MermaidRenderer that returns pre-configured results without needing mmdc.
class MockMermaidRenderer : public markamp::core::IMermaidRenderer
{
public:
    /// Configure the mock to return a successful SVG result.
    void set_success(const std::string& svg)
    {
        svg_ = svg;
        should_fail_ = false;
        available_ = true;
    }

    /// Configure the mock to return an error.
    void set_error(const std::string& error)
    {
        error_ = error;
        should_fail_ = true;
        available_ = true;
    }

    /// Configure mock availability.
    void set_available(bool available)
    {
        available_ = available;
    }

    [[nodiscard]] auto render(std::string_view /*mermaid_source*/)
        -> std::expected<std::string, std::string> override
    {
        if (should_fail_)
        {
            return std::unexpected(error_);
        }
        return svg_;
    }

    [[nodiscard]] auto is_available() const -> bool override
    {
        return available_;
    }

private:
    std::string svg_{"<svg>mock</svg>"};
    std::string error_{"Mock error"};
    bool should_fail_{false};
    bool available_{true};
};

} // anonymous namespace

// ============================================================
// MermaidRenderer unit tests
// ============================================================

TEST_CASE("MermaidRenderer: is_available returns a boolean", "[mermaid][renderer]")
{
    markamp::core::MermaidRenderer renderer;
    // is_available() should not crash, returns true only if mmdc is on PATH
    auto available = renderer.is_available();
    REQUIRE((available == true || available == false));
}

TEST_CASE("MermaidRenderer: empty source returns error", "[mermaid][renderer]")
{
    markamp::core::MermaidRenderer renderer;
    auto result = renderer.render("");
    REQUIRE_FALSE(result.has_value());
    REQUIRE_THAT(result.error(), ContainsSubstring("Empty"));
}

TEST_CASE("MermaidRenderer: get_mermaid_config returns valid JSON", "[mermaid][renderer]")
{
    markamp::core::MermaidRenderer renderer;
    auto config = renderer.get_mermaid_config();
    REQUIRE_THAT(config, ContainsSubstring("\"theme\""));
    REQUIRE_THAT(config, ContainsSubstring("\"themeVariables\""));
    REQUIRE_THAT(config, ContainsSubstring("\"primaryColor\""));
    REQUIRE_THAT(config, ContainsSubstring("\"fontFamily\""));
    REQUIRE_THAT(config, ContainsSubstring("\"securityLevel\""));
    REQUIRE_THAT(config, ContainsSubstring("strict"));
}

TEST_CASE("MermaidRenderer: default theme is dark", "[mermaid][renderer]")
{
    markamp::core::MermaidRenderer renderer;
    auto config = renderer.get_mermaid_config();
    REQUIRE_THAT(config, ContainsSubstring("\"dark\""));
}

TEST_CASE("MermaidRenderer: set_theme with dark theme", "[mermaid][renderer]")
{
    markamp::core::MermaidRenderer renderer;

    markamp::core::Theme dark_theme;
    dark_theme.id = "test-dark";
    dark_theme.name = "Test Dark";
    dark_theme.colors.bg_app = markamp::core::Color(10, 10, 20);
    dark_theme.colors.bg_panel = markamp::core::Color(20, 20, 30);
    dark_theme.colors.bg_header = markamp::core::Color(30, 30, 40);
    dark_theme.colors.bg_input = markamp::core::Color(25, 25, 35);
    dark_theme.colors.text_main = markamp::core::Color(224, 224, 224);
    dark_theme.colors.text_muted = markamp::core::Color(153, 153, 153);
    dark_theme.colors.accent_primary = markamp::core::Color(108, 99, 255);
    dark_theme.colors.accent_secondary = markamp::core::Color(255, 107, 157);
    dark_theme.colors.border_light = markamp::core::Color(51, 51, 51);
    dark_theme.colors.border_dark = markamp::core::Color(20, 20, 20);

    renderer.set_theme(dark_theme);
    auto config = renderer.get_mermaid_config();
    REQUIRE_THAT(config, ContainsSubstring("\"dark\""));
}

TEST_CASE("MermaidRenderer: set_theme with light theme", "[mermaid][renderer]")
{
    markamp::core::MermaidRenderer renderer;

    markamp::core::Theme light_theme;
    light_theme.id = "test-light";
    light_theme.name = "Test Light";
    light_theme.colors.bg_app = markamp::core::Color(250, 250, 250);
    light_theme.colors.bg_panel = markamp::core::Color(245, 245, 245);
    light_theme.colors.bg_header = markamp::core::Color(240, 240, 240);
    light_theme.colors.bg_input = markamp::core::Color(255, 255, 255);
    light_theme.colors.text_main = markamp::core::Color(30, 30, 30);
    light_theme.colors.text_muted = markamp::core::Color(100, 100, 100);
    light_theme.colors.accent_primary = markamp::core::Color(108, 99, 255);
    light_theme.colors.accent_secondary = markamp::core::Color(255, 107, 157);
    light_theme.colors.border_light = markamp::core::Color(200, 200, 200);
    light_theme.colors.border_dark = markamp::core::Color(180, 180, 180);

    renderer.set_theme(light_theme);
    auto config = renderer.get_mermaid_config();
    REQUIRE_THAT(config, ContainsSubstring("\"default\""));
}

TEST_CASE("MermaidRenderer: set_font_family changes config", "[mermaid][renderer]")
{
    markamp::core::MermaidRenderer renderer;
    renderer.set_font_family("Fira Code");
    auto config = renderer.get_mermaid_config();
    REQUIRE_THAT(config, ContainsSubstring("Fira Code"));
}

TEST_CASE("MermaidRenderer: render when unavailable returns error", "[mermaid][renderer]")
{
    markamp::core::MermaidRenderer renderer;
    if (!renderer.is_available())
    {
        auto result = renderer.render("graph TD\n    A --> B");
        REQUIRE_FALSE(result.has_value());
        REQUIRE_THAT(result.error(), ContainsSubstring("not available"));
        REQUIRE_THAT(result.error(), ContainsSubstring("npm install"));
    }
    else
    {
        // If mmdc is available, test that a valid diagram renders
        auto result = renderer.render("graph TD\n    A --> B");
        REQUIRE(result.has_value());
        REQUIRE_THAT(*result, ContainsSubstring("<svg"));
    }
}

// ============================================================
// MermaidBlockRenderer unit tests (using MockMermaidRenderer)
// ============================================================

TEST_CASE("MermaidBlockRenderer: render produces container HTML", "[mermaid][block]")
{
    MockMermaidRenderer mock;
    mock.set_success("<svg><rect/></svg>");

    markamp::rendering::MermaidBlockRenderer block_renderer;
    auto html = block_renderer.render("graph TD\n    A --> B", mock);

    REQUIRE_THAT(html, ContainsSubstring("mermaid-container"));
    REQUIRE_THAT(html, ContainsSubstring("data:image/svg+xml;base64,"));
    REQUIRE_THAT(html, ContainsSubstring("<img"));
}

TEST_CASE("MermaidBlockRenderer: render error produces error overlay", "[mermaid][block]")
{
    MockMermaidRenderer mock;
    mock.set_error("Parse error at line 2: unexpected token");

    markamp::rendering::MermaidBlockRenderer block_renderer;
    auto html = block_renderer.render("invalid mermaid", mock);

    REQUIRE_THAT(html, ContainsSubstring("mermaid-error"));
    REQUIRE_THAT(html, ContainsSubstring("Mermaid Error"));
    REQUIRE_THAT(html, ContainsSubstring("Parse error at line 2"));
}

TEST_CASE("MermaidBlockRenderer: render_error static method", "[mermaid][block]")
{
    auto html = markamp::rendering::MermaidBlockRenderer::render_error("Bad syntax");

    REQUIRE_THAT(html, ContainsSubstring("mermaid-error"));
    REQUIRE_THAT(html, ContainsSubstring("Mermaid Error"));
    REQUIRE_THAT(html, ContainsSubstring("Bad syntax"));
}

TEST_CASE("MermaidBlockRenderer: render_unavailable static method", "[mermaid][block]")
{
    auto html = markamp::rendering::MermaidBlockRenderer::render_unavailable();

    REQUIRE_THAT(html, ContainsSubstring("mermaid-unavailable"));
    REQUIRE_THAT(html, ContainsSubstring("not available"));
    REQUIRE_THAT(html, ContainsSubstring("npm install"));
}

TEST_CASE("MermaidBlockRenderer: render_placeholder shows source", "[mermaid][block]")
{
    auto html =
        markamp::rendering::MermaidBlockRenderer::render_placeholder("graph TD\n    A --> B");

    REQUIRE_THAT(html, ContainsSubstring("mermaid-block"));
    REQUIRE_THAT(html, ContainsSubstring("graph TD"));
    REQUIRE_THAT(html, ContainsSubstring("A --&gt; B"));
}

TEST_CASE("MermaidBlockRenderer: error HTML-escapes message", "[mermaid][block]")
{
    auto html = markamp::rendering::MermaidBlockRenderer::render_error(
        "Error <script>alert('xss')</script>");

    REQUIRE_THAT(html, ContainsSubstring("&lt;script&gt;"));
    REQUIRE_FALSE(html.find("<script>") != std::string::npos);
}

// ============================================================
// Base64 encoding tests
// ============================================================

TEST_CASE("MermaidBlockRenderer: base64_encode empty string", "[mermaid][base64]")
{
    auto result = markamp::rendering::MermaidBlockRenderer::base64_encode("");
    REQUIRE(result.empty());
}

TEST_CASE("MermaidBlockRenderer: base64_encode known value", "[mermaid][base64]")
{
    auto result = markamp::rendering::MermaidBlockRenderer::base64_encode("Hello");
    REQUIRE(result == "SGVsbG8=");
}

TEST_CASE("MermaidBlockRenderer: base64_encode padding cases", "[mermaid][base64]")
{
    // 1 byte → 4 chars with 2 padding
    auto r1 = markamp::rendering::MermaidBlockRenderer::base64_encode("M");
    REQUIRE(r1 == "TQ==");

    // 2 bytes → 4 chars with 1 padding
    auto r2 = markamp::rendering::MermaidBlockRenderer::base64_encode("Ma");
    REQUIRE(r2 == "TWE=");

    // 3 bytes → 4 chars with 0 padding
    auto r3 = markamp::rendering::MermaidBlockRenderer::base64_encode("Man");
    REQUIRE(r3 == "TWFu");
}

TEST_CASE("MermaidBlockRenderer: base64_encode SVG content", "[mermaid][base64]")
{
    auto result = markamp::rendering::MermaidBlockRenderer::base64_encode("<svg></svg>");
    REQUIRE_FALSE(result.empty());
    // Should not contain raw SVG
    REQUIRE(result.find("<svg>") == std::string::npos);
}

// ============================================================
// HtmlRenderer integration tests
// ============================================================

TEST_CASE("HtmlRenderer: MermaidBlock with renderer", "[mermaid][integration]")
{
    MockMermaidRenderer mock;
    mock.set_success("<svg><rect/></svg>");

    markamp::rendering::HtmlRenderer renderer;
    renderer.set_mermaid_renderer(&mock);

    // Parse markdown with a mermaid block
    markamp::core::MarkdownParser parser;
    auto doc = parser.parse("```mermaid\ngraph TD\n    A --> B\n```\n");
    REQUIRE(doc.has_value());

    auto html = renderer.render(*doc);
    REQUIRE_THAT(html, ContainsSubstring("mermaid-container"));
    REQUIRE_THAT(html, ContainsSubstring("data:image/svg+xml;base64,"));
}

TEST_CASE("HtmlRenderer: MermaidBlock with unavailable renderer", "[mermaid][integration]")
{
    MockMermaidRenderer mock;
    mock.set_available(false);

    markamp::rendering::HtmlRenderer renderer;
    renderer.set_mermaid_renderer(&mock);

    markamp::core::MarkdownParser parser;
    auto doc = parser.parse("```mermaid\ngraph TD\n    A --> B\n```\n");
    REQUIRE(doc.has_value());

    auto html = renderer.render(*doc);
    REQUIRE_THAT(html, ContainsSubstring("mermaid-unavailable"));
}

TEST_CASE("HtmlRenderer: MermaidBlock without renderer (fallback)", "[mermaid][integration]")
{
    markamp::rendering::HtmlRenderer renderer;
    // No renderer set, should fall back to placeholder

    markamp::core::MarkdownParser parser;
    auto doc = parser.parse("```mermaid\ngraph TD\n    A --> B\n```\n");
    REQUIRE(doc.has_value());

    auto html = renderer.render(*doc);
    REQUIRE_THAT(html, ContainsSubstring("mermaid-block"));
    REQUIRE_THAT(html, ContainsSubstring("graph TD"));
}

TEST_CASE("HtmlRenderer: MermaidBlock render error", "[mermaid][integration]")
{
    MockMermaidRenderer mock;
    mock.set_error("Invalid diagram syntax");

    markamp::rendering::HtmlRenderer renderer;
    renderer.set_mermaid_renderer(&mock);

    markamp::core::MarkdownParser parser;
    auto doc = parser.parse("```mermaid\ninvalid stuff\n```\n");
    REQUIRE(doc.has_value());

    auto html = renderer.render(*doc);
    REQUIRE_THAT(html, ContainsSubstring("mermaid-error"));
    REQUIRE_THAT(html, ContainsSubstring("Invalid diagram syntax"));
}

// ============================================================
// Conditional live render test (only if mmdc is installed)
// ============================================================

TEST_CASE("MermaidRenderer: live flowchart render (if available)", "[mermaid][live][!mayfail]")
{
    markamp::core::MermaidRenderer renderer;
    if (!renderer.is_available())
    {
        SKIP("mmdc not installed, skipping live render test");
    }

    auto result = renderer.render("graph TD\n    A[Start] --> B[End]");
    REQUIRE(result.has_value());
    REQUIRE_THAT(*result, ContainsSubstring("<svg"));
}

TEST_CASE("MermaidRenderer: live sequence diagram (if available)", "[mermaid][live][!mayfail]")
{
    markamp::core::MermaidRenderer renderer;
    if (!renderer.is_available())
    {
        SKIP("mmdc not installed, skipping live render test");
    }

    auto result = renderer.render("sequenceDiagram\n    Alice->>Bob: Hello\n    Bob->>Alice: Hi");
    REQUIRE(result.has_value());
    REQUIRE_THAT(*result, ContainsSubstring("<svg"));
}

TEST_CASE("MermaidRenderer: live invalid syntax (if available)", "[mermaid][live][!mayfail]")
{
    markamp::core::MermaidRenderer renderer;
    if (!renderer.is_available())
    {
        SKIP("mmdc not installed, skipping live render test");
    }

    auto result = renderer.render("this is not valid mermaid at all!!!");
    REQUIRE_FALSE(result.has_value());
}
