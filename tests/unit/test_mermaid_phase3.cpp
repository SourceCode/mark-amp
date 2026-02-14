#include "core/MermaidRenderer.h"
#include "rendering/MermaidBlockRenderer.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Mock renderer for testing without mmdc CLI
// ---------------------------------------------------------------------------

namespace
{

class MockMermaidRenderer : public markamp::core::IMermaidRenderer
{
public:
    bool available{true};
    std::string mock_svg{"<svg><text>Test Diagram</text></svg>"};
    std::string mock_error;

    [[nodiscard]] auto render(std::string_view /*mermaid_source*/)
        -> std::expected<std::string, std::string> override
    {
        if (!mock_error.empty())
        {
            return std::unexpected(mock_error);
        }
        return mock_svg;
    }

    [[nodiscard]] auto is_available() const -> bool override
    {
        return available;
    }
};

} // anonymous namespace

// ---------------------------------------------------------------------------
// MermaidDiagnosticInfo / MermaidDiagnosticSeverity
// ---------------------------------------------------------------------------

TEST_CASE("MermaidDiagnosticInfo: defaults", "[mermaid_phase3]")
{
    markamp::core::MermaidDiagnosticInfo diag;
    REQUIRE(diag.line == 0);
    REQUIRE(diag.message.empty());
    REQUIRE(diag.severity == markamp::core::MermaidDiagnosticSeverity::Error);
}

TEST_CASE("MermaidDiagnosticInfo: can be constructed with values", "[mermaid_phase3]")
{
    markamp::core::MermaidDiagnosticInfo diag{
        5, "Unexpected token", markamp::core::MermaidDiagnosticSeverity::Warning};
    REQUIRE(diag.line == 5);
    REQUIRE(diag.message == "Unexpected token");
    REQUIRE(diag.severity == markamp::core::MermaidDiagnosticSeverity::Warning);
}

// ---------------------------------------------------------------------------
// MermaidRenderer: diagram_theme
// ---------------------------------------------------------------------------

TEST_CASE("MermaidRenderer: default diagram theme is dark", "[mermaid_phase3]")
{
    markamp::core::MermaidRenderer renderer;
    REQUIRE(renderer.diagram_theme() == "dark");
}

TEST_CASE("MermaidRenderer: set_diagram_theme overrides", "[mermaid_phase3]")
{
    markamp::core::MermaidRenderer renderer;
    renderer.set_diagram_theme("forest");
    REQUIRE(renderer.diagram_theme() == "forest");
}

TEST_CASE("MermaidRenderer: set_diagram_theme clears cache", "[mermaid_phase3]")
{
    markamp::core::MermaidRenderer renderer;
    // Cache should be cleared on theme change (we can't directly observe this
    // without mmdc, but we verify the theme change itself works)
    renderer.set_diagram_theme("neutral");
    REQUIRE(renderer.diagram_theme() == "neutral");
    renderer.set_diagram_theme("dark");
    REQUIRE(renderer.diagram_theme() == "dark");
}

TEST_CASE("MermaidRenderer: empty diagram theme returns mermaid_theme_", "[mermaid_phase3]")
{
    markamp::core::MermaidRenderer renderer;
    renderer.set_diagram_theme("");
    REQUIRE(renderer.diagram_theme() == "dark");
}

// ---------------------------------------------------------------------------
// MermaidRenderer: validate (without mmdc available)
// ---------------------------------------------------------------------------

TEST_CASE("MermaidRenderer: validate empty source", "[mermaid_phase3]")
{
    markamp::core::MermaidRenderer renderer;
    auto diagnostics = renderer.validate("");
    REQUIRE(diagnostics.size() == 1);
    REQUIRE(diagnostics[0].severity == markamp::core::MermaidDiagnosticSeverity::Error);
    REQUIRE(diagnostics[0].message.find("Empty") != std::string::npos);
}

// ---------------------------------------------------------------------------
// MermaidBlockRenderer: render_with_controls
// ---------------------------------------------------------------------------

TEST_CASE("MermaidBlockRenderer: render_with_controls produces enhanced container",
          "[mermaid_phase3]")
{
    MockMermaidRenderer mock;
    markamp::rendering::MermaidBlockRenderer block_renderer;

    auto html = block_renderer.render_with_controls("graph TD\n  A-->B", mock);

    // Should contain enhanced container structure
    REQUIRE(html.find("mermaid-enhanced") != std::string::npos);
    REQUIRE(html.find("mermaid-viewport") != std::string::npos);
    REQUIRE(html.find("mermaid-controls") != std::string::npos);

    // Should contain control buttons
    REQUIRE(html.find("Zoom In") != std::string::npos);
    REQUIRE(html.find("Zoom Out") != std::string::npos);
    REQUIRE(html.find("Fullscreen") != std::string::npos);
    REQUIRE(html.find("Export SVG") != std::string::npos);

    // Should contain base64-encoded SVG image
    REQUIRE(html.find("data:image/svg+xml;base64,") != std::string::npos);
}

TEST_CASE("MermaidBlockRenderer: render_with_controls handles error", "[mermaid_phase3]")
{
    MockMermaidRenderer mock;
    mock.mock_error = "Invalid syntax at line 3";
    markamp::rendering::MermaidBlockRenderer block_renderer;

    auto html = block_renderer.render_with_controls("invalid", mock);

    REQUIRE(html.find("mermaid-enhanced") != std::string::npos);
    REQUIRE(html.find("Mermaid Error") != std::string::npos);
    REQUIRE(html.find("Invalid syntax at line 3") != std::string::npos);
}

TEST_CASE("MermaidBlockRenderer: render_with_controls includes CSS styles", "[mermaid_phase3]")
{
    MockMermaidRenderer mock;
    markamp::rendering::MermaidBlockRenderer block_renderer;

    auto html = block_renderer.render_with_controls("graph TD\n  A-->B", mock);

    REQUIRE(html.find("<style>") != std::string::npos);
    REQUIRE(html.find(".mermaid-btn") != std::string::npos);
    REQUIRE(html.find("cursor: grab") != std::string::npos);
}

// ---------------------------------------------------------------------------
// MermaidBlockRenderer: render_diagnostics
// ---------------------------------------------------------------------------

TEST_CASE("MermaidBlockRenderer: render_diagnostics empty returns empty", "[mermaid_phase3]")
{
    std::vector<markamp::core::MermaidDiagnosticInfo> empty;
    auto html = markamp::rendering::MermaidBlockRenderer::render_diagnostics(empty);
    REQUIRE(html.empty());
}

TEST_CASE("MermaidBlockRenderer: render_diagnostics shows errors", "[mermaid_phase3]")
{
    std::vector<markamp::core::MermaidDiagnosticInfo> diagnostics = {
        {3, "Unexpected token", markamp::core::MermaidDiagnosticSeverity::Error},
        {0, "Missing end", markamp::core::MermaidDiagnosticSeverity::Warning},
    };

    auto html = markamp::rendering::MermaidBlockRenderer::render_diagnostics(diagnostics);

    REQUIRE(html.find("mermaid-diag") != std::string::npos);
    REQUIRE(html.find("mermaid-diag-error") != std::string::npos);
    REQUIRE(html.find("mermaid-diag-warning") != std::string::npos);
    REQUIRE(html.find("[ERROR]") != std::string::npos);
    REQUIRE(html.find("[WARN]") != std::string::npos);
    REQUIRE(html.find("Line 3") != std::string::npos);
    REQUIRE(html.find("Unexpected token") != std::string::npos);
    REQUIRE(html.find("Missing end") != std::string::npos);
}

TEST_CASE("MermaidBlockRenderer: render_diagnostics handles info severity", "[mermaid_phase3]")
{
    std::vector<markamp::core::MermaidDiagnosticInfo> diagnostics = {
        {1, "Use flowchart instead of graph", markamp::core::MermaidDiagnosticSeverity::Info},
    };

    auto html = markamp::rendering::MermaidBlockRenderer::render_diagnostics(diagnostics);

    REQUIRE(html.find("mermaid-diag-info") != std::string::npos);
    REQUIRE(html.find("[INFO]") != std::string::npos);
}

// ---------------------------------------------------------------------------
// MermaidBlockRenderer: original render still works
// ---------------------------------------------------------------------------

TEST_CASE("MermaidBlockRenderer: original render still produces container", "[mermaid_phase3]")
{
    MockMermaidRenderer mock;
    markamp::rendering::MermaidBlockRenderer block_renderer;

    auto html = block_renderer.render("graph TD\n  A-->B", mock);

    // Original container (not enhanced)
    REQUIRE(html.find("mermaid-container") != std::string::npos);
    REQUIRE(html.find("data:image/svg+xml;base64,") != std::string::npos);
}

// ---------------------------------------------------------------------------
// MermaidBlockRenderer: base64_encode
// ---------------------------------------------------------------------------

TEST_CASE("MermaidBlockRenderer: base64_encode works correctly", "[mermaid_phase3]")
{
    auto result = markamp::rendering::MermaidBlockRenderer::base64_encode("Hello, World!");
    REQUIRE(result == "SGVsbG8sIFdvcmxkIQ==");
}

TEST_CASE("MermaidBlockRenderer: base64_encode empty string", "[mermaid_phase3]")
{
    auto result = markamp::rendering::MermaidBlockRenderer::base64_encode("");
    REQUIRE(result.empty());
}

// ---------------------------------------------------------------------------
// MermaidRenderer: export_svg / export_png (without mmdc)
// ---------------------------------------------------------------------------

TEST_CASE("MermaidRenderer: export_svg rejects empty source", "[mermaid_phase3]")
{
    markamp::core::MermaidRenderer renderer;
    auto result = renderer.export_svg("");
    REQUIRE(!result.has_value());
    REQUIRE(result.error().find("Empty") != std::string::npos);
}

TEST_CASE("MermaidRenderer: export_png rejects empty source", "[mermaid_phase3]")
{
    markamp::core::MermaidRenderer renderer;
    auto result = renderer.export_png("");
    REQUIRE(!result.has_value());
    REQUIRE(result.error().find("Empty") != std::string::npos);
}

// ---------------------------------------------------------------------------
// MermaidRenderer: SVG sanitization
// ---------------------------------------------------------------------------

TEST_CASE("MermaidRenderer: sanitize_svg strips script tags", "[mermaid_phase3]")
{
    std::string svg = "<svg><script>alert('xss')</script><text>Safe</text></svg>";
    auto result = markamp::core::MermaidRenderer::sanitize_svg(svg);
    REQUIRE(result.find("<script>") == std::string::npos);
    REQUIRE(result.find("Safe") != std::string::npos);
}

TEST_CASE("MermaidRenderer: sanitize_svg strips foreignObject tags", "[mermaid_phase3]")
{
    std::string svg = "<svg><foreignObject>danger</foreignObject><circle/></svg>";
    auto result = markamp::core::MermaidRenderer::sanitize_svg(svg);
    REQUIRE(result.find("<foreignObject>") == std::string::npos);
    REQUIRE(result.find("<circle/>") != std::string::npos);
}

TEST_CASE("MermaidRenderer: sanitize_svg preserves clean SVG", "[mermaid_phase3]")
{
    std::string svg = "<svg><rect width=\"100\" height=\"50\"/><text>Hello</text></svg>";
    auto result = markamp::core::MermaidRenderer::sanitize_svg(svg);
    REQUIRE(result == svg);
}
