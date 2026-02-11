/// @file test_preview_integration.cpp
/// Integration tests for the Phase 18 preview rendering pipeline.
/// Tests the full render path: markdown → parse → HtmlRenderer → HTML output,
/// including footnote preprocessing, image resolution, CSS generation, and HTML export.

#include "core/Color.h"
#include "core/Events.h"
#include "core/MarkdownParser.h"
#include "core/Theme.h"
#include "rendering/HtmlRenderer.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <filesystem>
#include <fstream>

using Catch::Matchers::ContainsSubstring;

namespace
{

/// Create a minimal theme for test CSS generation.
auto make_test_theme() -> markamp::core::Theme
{
    markamp::core::Theme theme;
    theme.id = "test_theme";
    theme.name = "Test Theme";
    theme.colors.bg_app = markamp::core::Color{30, 30, 30, 255};
    theme.colors.bg_panel = markamp::core::Color{40, 40, 40, 255};
    theme.colors.bg_header = markamp::core::Color{50, 50, 50, 255};
    theme.colors.text_main = markamp::core::Color{220, 220, 220, 255};
    theme.colors.text_muted = markamp::core::Color{150, 150, 150, 255};
    theme.colors.accent_primary = markamp::core::Color{0, 170, 255, 255};
    theme.colors.accent_secondary = markamp::core::Color{170, 255, 0, 255};
    theme.colors.border_light = markamp::core::Color{70, 70, 70, 255};
    theme.colors.border_dark = markamp::core::Color{20, 20, 20, 255};
    return theme;
}

/// Helper to render markdown through the full pipeline.
auto render_markdown(const std::string& markdown, const std::filesystem::path& base_path = {})
    -> std::string
{
    // 1. Footnote preprocessing
    markamp::rendering::FootnotePreprocessor footnote_proc;
    auto footnote_result = footnote_proc.process(markdown);

    // 2. Parse
    markamp::core::MarkdownParser parser;
    auto doc_result = parser.parse(footnote_result.processed_markdown);
    REQUIRE(doc_result.has_value());

    // 3. Render
    markamp::rendering::HtmlRenderer renderer;
    if (!base_path.empty())
    {
        renderer.set_base_path(base_path);
    }

    if (footnote_result.has_footnotes)
    {
        return renderer.render_with_footnotes(*doc_result, footnote_result.footnote_section_html);
    }
    return renderer.render(*doc_result);
}

} // namespace

// ─────────────────────────────────────────────────────
// 1. Empty content renders without crashing
// ─────────────────────────────────────────────────────

TEST_CASE("Preview integration: empty content", "[preview][integration]")
{
    auto html = render_markdown("");
    // Should not crash — the content may be empty or minimal
    SUCCEED("Empty content rendered without crash");
}

// ─────────────────────────────────────────────────────
// 2. Single heading renders correctly
// ─────────────────────────────────────────────────────

TEST_CASE("Preview integration: heading", "[preview][integration]")
{
    auto html = render_markdown("# Hello World");
    CHECK_THAT(html, ContainsSubstring("<h1>"));
    CHECK_THAT(html, ContainsSubstring("Hello World"));
    CHECK_THAT(html, ContainsSubstring("</h1>"));
}

// ─────────────────────────────────────────────────────
// 3. Complex markdown with multiple element types
// ─────────────────────────────────────────────────────

TEST_CASE("Preview integration: complex markdown", "[preview][integration]")
{
    std::string markdown = R"(
# Title

Paragraph text with **bold** and *italic*.

- Item 1
- Item 2

> Blockquote text

---

```cpp
int main() {}
```
)";

    auto html = render_markdown(markdown);
    CHECK_THAT(html, ContainsSubstring("<h1>"));
    CHECK_THAT(html, ContainsSubstring("<strong>bold</strong>"));
    CHECK_THAT(html, ContainsSubstring("<em>italic</em>"));
    CHECK_THAT(html, ContainsSubstring("<li>"));
    CHECK_THAT(html, ContainsSubstring("<blockquote>"));
    CHECK_THAT(html, ContainsSubstring("<hr"));
    CHECK_THAT(html, ContainsSubstring("code"));
}

// ─────────────────────────────────────────────────────
// 4. Table renders with proper elements
// ─────────────────────────────────────────────────────

TEST_CASE("Preview integration: table rendering", "[preview][integration]")
{
    std::string markdown = R"(
| Name | Value |
|------|-------|
| A    | 1     |
| B    | 2     |
)";

    auto html = render_markdown(markdown);
    CHECK_THAT(html, ContainsSubstring("<table"));
    CHECK_THAT(html, ContainsSubstring("<th"));
    CHECK_THAT(html, ContainsSubstring("<td"));
}

// ─────────────────────────────────────────────────────
// 5. Task list renders with checkboxes
// ─────────────────────────────────────────────────────

TEST_CASE("Preview integration: task list", "[preview][integration]")
{
    std::string markdown = R"(
- [x] Completed task
- [ ] Incomplete task
)";

    auto html = render_markdown(markdown);
    CHECK_THAT(html, ContainsSubstring("checkbox"));
}

// ─────────────────────────────────────────────────────
// 6. Footnote preprocessing produces section
// ─────────────────────────────────────────────────────

TEST_CASE("Preview integration: footnote preprocessing", "[preview][integration]")
{
    std::string markdown = R"(
This has a footnote[^1].

[^1]: Footnote content here.
)";

    markamp::rendering::FootnotePreprocessor proc;
    auto result = proc.process(markdown);

    CHECK(result.has_footnotes);
    CHECK_THAT(result.footnote_section_html, ContainsSubstring("footnote"));
    CHECK_THAT(result.footnote_section_html, ContainsSubstring("Footnote content here"));
}

// ─────────────────────────────────────────────────────
// 7. Mermaid block renders placeholder without renderer
// ─────────────────────────────────────────────────────

TEST_CASE("Preview integration: mermaid block without renderer", "[preview][integration]")
{
    std::string markdown = R"(
```mermaid
graph TD
    A-->B
```
)";

    auto html = render_markdown(markdown);
    // Without a mermaid renderer, should render a placeholder or the code block
    CHECK_FALSE(html.empty());
}

// ─────────────────────────────────────────────────────
// 8. Code block with known language
// ─────────────────────────────────────────────────────

TEST_CASE("Preview integration: code block with language", "[preview][integration]")
{
    std::string markdown = R"(
```cpp
void foo() { return; }
```
)";

    auto html = render_markdown(markdown);
    CHECK_THAT(html, ContainsSubstring("foo"));
}

// ─────────────────────────────────────────────────────
// 9. Local image path resolution (relative → placeholder when no file)
// ─────────────────────────────────────────────────────

TEST_CASE("Preview integration: local image missing", "[preview][integration]")
{
    auto html = render_markdown("![Alt text](nonexistent.png)");
    CHECK_THAT(html, ContainsSubstring("image-missing"));
    CHECK_THAT(html, ContainsSubstring("nonexistent.png"));
}

// ─────────────────────────────────────────────────────
// 10. Remote image URL blocked
// ─────────────────────────────────────────────────────

TEST_CASE("Preview integration: remote image blocked", "[preview][integration]")
{
    auto html = render_markdown("![Remote](https://example.com/image.png)");
    CHECK_THAT(html, ContainsSubstring("image-missing"));
}

// ─────────────────────────────────────────────────────
// 11. Image with valid local file renders data URI
// ─────────────────────────────────────────────────────

TEST_CASE("Preview integration: valid local image", "[preview][integration]")
{
    // Create a temp PNG file (1x1 pixel PNG)
    auto temp_dir = std::filesystem::temp_directory_path() / "markamp_test_images";
    std::filesystem::create_directories(temp_dir);
    auto image_path = temp_dir / "test.png";

    // Minimal valid PNG file (1x1 transparent pixel)
    const unsigned char png_data[] = {
        0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, // PNG signature
        0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, // IHDR chunk
        0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x08, 0x02, 0x00, 0x00, 0x00, 0x90,
        0x77, 0x53, 0xDE, 0x00, 0x00, 0x00, 0x0C, 0x49, 0x44, 0x41, 0x54, 0x08, 0xD7, 0x63,
        0xF8, 0xCF, 0xC0, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0xE2, 0x21, 0xBC, 0x33, 0x00,
        0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82 // IEND chunk
    };

    {
        std::ofstream f(image_path, std::ios::binary);
        f.write(reinterpret_cast<const char*>(png_data), sizeof(png_data));
    }

    auto html = render_markdown("![Test image](test.png)", temp_dir);
    CHECK_THAT(html, ContainsSubstring("data:image/png;base64,"));

    // Cleanup
    std::filesystem::remove_all(temp_dir);
}

// ─────────────────────────────────────────────────────
// 12. HtmlRenderer image path resolution: blocked extensions
// ─────────────────────────────────────────────────────

TEST_CASE("Preview integration: blocked image extension", "[preview][integration]")
{
    auto temp_dir = std::filesystem::temp_directory_path() / "markamp_test_ext";
    std::filesystem::create_directories(temp_dir);
    auto bad_file = temp_dir / "script.exe";
    {
        std::ofstream f(bad_file);
        f << "not an image";
    }

    auto html = render_markdown("![Bad](script.exe)", temp_dir);
    CHECK_THAT(html, ContainsSubstring("image-missing"));

    std::filesystem::remove_all(temp_dir);
}

// ─────────────────────────────────────────────────────
// 13. Footnote with render_with_footnotes
// ─────────────────────────────────────────────────────

TEST_CASE("Preview integration: footnote render pipeline", "[preview][integration]")
{
    std::string markdown = R"(
Text with reference[^note].

[^note]: This is the note.
)";

    auto html = render_markdown(markdown);
    // The rendered output should contain the footnote section
    CHECK_THAT(html, ContainsSubstring("footnote"));
}

// ─────────────────────────────────────────────────────
// 14. EditorScrollChangedEvent struct
// ─────────────────────────────────────────────────────

TEST_CASE("Preview integration: EditorScrollChangedEvent", "[preview][integration]")
{
    markamp::core::events::EditorScrollChangedEvent evt;
    CHECK(evt.scroll_fraction == 0.0);
    CHECK(evt.type_name() == "EditorScrollChangedEvent");

    evt.scroll_fraction = 0.5;
    CHECK(evt.scroll_fraction == 0.5);
}

// ─────────────────────────────────────────────────────
// 15. Strikethrough rendering
// ─────────────────────────────────────────────────────

TEST_CASE("Preview integration: strikethrough", "[preview][integration]")
{
    auto html = render_markdown("~~deleted~~");
    CHECK_THAT(html, ContainsSubstring("<del>"));
    CHECK_THAT(html, ContainsSubstring("deleted"));
}

// ─────────────────────────────────────────────────────
// 16. Link rendering
// ─────────────────────────────────────────────────────

TEST_CASE("Preview integration: links", "[preview][integration]")
{
    auto html = render_markdown("[MarkAmp](https://markamp.dev)");
    CHECK_THAT(html, ContainsSubstring("<a"));
    CHECK_THAT(html, ContainsSubstring("href"));
    CHECK_THAT(html, ContainsSubstring("MarkAmp"));
}

// ─────────────────────────────────────────────────────
// 17. Multiple heading levels
// ─────────────────────────────────────────────────────

TEST_CASE("Preview integration: heading levels", "[preview][integration]")
{
    std::string markdown = R"(
# H1
## H2
### H3
#### H4
)";

    auto html = render_markdown(markdown);
    CHECK_THAT(html, ContainsSubstring("<h1>"));
    CHECK_THAT(html, ContainsSubstring("<h2>"));
    CHECK_THAT(html, ContainsSubstring("<h3>"));
    CHECK_THAT(html, ContainsSubstring("<h4>"));
}
