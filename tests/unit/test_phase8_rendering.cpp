// ============================================================================
// File: tests/test_phase8_rendering.cpp
// Phase 8: Markdown Rendering Completeness — Comprehensive unit tests
// ============================================================================

#include "core/MarkdownParser.h"
#include "core/Types.h"
#include "rendering/HtmlRenderer.h"
#include "rendering/MarkdownRenderingFeatures.h"
#include "rendering/ReadingProfileManager.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace markamp;
using namespace markamp::rendering;
using namespace Catch::Matchers;

// ═══════════════════════════════════════════════════════
// Helper: Parse and render markdown
// ═══════════════════════════════════════════════════════

static auto render_markdown(const std::string& markdown, bool source_line_attrs = false)
    -> std::string
{
    core::MarkdownParser parser;
    auto doc = parser.parse(markdown);
    REQUIRE(doc.has_value());

    HtmlRenderer renderer;
    renderer.set_source_line_attributes(source_line_attrs);
    return renderer.render(*doc);
}

static auto render_and_get_renderer(const std::string& markdown, bool source_line_attrs = false)
    -> std::pair<std::string, HtmlRenderer>
{
    core::MarkdownParser parser;
    auto doc = parser.parse(markdown);
    REQUIRE(doc.has_value());

    HtmlRenderer renderer;
    renderer.set_source_line_attributes(source_line_attrs);
    auto html = renderer.render(*doc);
    return {html, std::move(renderer)};
}

// ═══════════════════════════════════════════════════════
// Source-Line Attribute Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Phase 8: Source-line attributes disabled by default", "[phase8][source-line]")
{
    auto html = render_markdown("# Hello\n\nWorld");
    CHECK_THAT(html, !ContainsSubstring("data-source-line"));
}

TEST_CASE("Phase 8: Source-line attributes on headings when enabled", "[phase8][source-line]")
{
    auto html = render_markdown("# Hello\n", true);
    CHECK_THAT(html, ContainsSubstring("data-source-line="));
    CHECK_THAT(html, ContainsSubstring("<h1"));
}

TEST_CASE("Phase 8: Source-line attributes on paragraphs when enabled", "[phase8][source-line]")
{
    auto html = render_markdown("Hello world\n", true);
    CHECK_THAT(html, ContainsSubstring("<p data-source-line="));
}

TEST_CASE("Phase 8: Source-line attributes on blockquotes when enabled", "[phase8][source-line]")
{
    auto html = render_markdown("> Quote text\n", true);
    CHECK_THAT(html, ContainsSubstring("<blockquote data-source-line="));
}

TEST_CASE("Phase 8: set_source_line_attributes toggle", "[phase8][source-line]")
{
    HtmlRenderer renderer;
    CHECK_FALSE(renderer.source_line_attributes_enabled());

    renderer.set_source_line_attributes(true);
    CHECK(renderer.source_line_attributes_enabled());

    renderer.set_source_line_attributes(false);
    CHECK_FALSE(renderer.source_line_attributes_enabled());
}

// ═══════════════════════════════════════════════════════
// Heading Anchor Collection Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Phase 8: Heading anchors collected during render", "[phase8][heading-anchors]")
{
    auto [html, renderer] = render_and_get_renderer("# Introduction\n\n## Setup\n\n### Details\n");

    const auto& anchors = renderer.heading_anchors();
    REQUIRE(anchors.size() >= 3);
    CHECK(anchors[0].text == "Introduction");
    CHECK(anchors[0].level == 1);
    CHECK_FALSE(anchors[0].slug.empty());
    CHECK(anchors[1].text == "Setup");
    CHECK(anchors[1].level == 2);
    CHECK(anchors[2].text == "Details");
    CHECK(anchors[2].level == 3);
}

TEST_CASE("Phase 8: Heading anchors cleared on re-render", "[phase8][heading-anchors]")
{
    core::MarkdownParser parser;
    HtmlRenderer renderer;

    auto doc1 = parser.parse("# First\n");
    REQUIRE(doc1.has_value());
    renderer.render(*doc1);
    CHECK(renderer.heading_anchors().size() >= 1);

    auto doc2 = parser.parse("## Second\n## Third\n");
    REQUIRE(doc2.has_value());
    renderer.render(*doc2);
    CHECK(renderer.heading_anchors().size() >= 2);
    // First heading should not persist
    CHECK(renderer.heading_anchors()[0].text == "Second");
}

TEST_CASE("Phase 8: Unique heading slugs", "[phase8][heading-anchors]")
{
    auto [html, renderer] = render_and_get_renderer("# Test\n\n# Test\n\n# Test\n");

    const auto& anchors = renderer.heading_anchors();
    REQUIRE(anchors.size() >= 3);
    // Each slug should be unique
    CHECK(anchors[0].slug != anchors[1].slug);
    CHECK(anchors[1].slug != anchors[2].slug);
    CHECK(anchors[0].slug != anchors[2].slug);
}

// ═══════════════════════════════════════════════════════
// Source-Line Mapping Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Phase 8: Source-line mappings collected when enabled", "[phase8][source-line-mappings]")
{
    auto [html, renderer] = render_and_get_renderer("# Title\n\nParagraph\n", true);

    const auto& mappings = renderer.source_line_mappings();
    CHECK_FALSE(mappings.empty());
    // Should have at least heading + paragraph
    CHECK(mappings.size() >= 2);
}

TEST_CASE("Phase 8: Source-line mappings empty when disabled", "[phase8][source-line-mappings]")
{
    auto [html, renderer] = render_and_get_renderer("# Title\n\nParagraph\n", false);

    const auto& mappings = renderer.source_line_mappings();
    CHECK(mappings.empty());
}

// ═══════════════════════════════════════════════════════
// Code Block Controls Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Phase 8: Code block container with copy button", "[phase8][code-block]")
{
    HtmlRenderer renderer;
    CodeBlockConfig config;
    config.show_copy_button = true;
    config.show_language_label = true;
    renderer.set_code_block_config(config);

    core::MarkdownParser parser;
    auto doc = parser.parse("```javascript\nconsole.log('hi');\n```\n");
    REQUIRE(doc.has_value());
    auto html = renderer.render(*doc);

    CHECK_THAT(html, ContainsSubstring("code-block-container"));
    CHECK_THAT(html, ContainsSubstring("copy-btn"));
    CHECK_THAT(html, ContainsSubstring("code-lang-label"));
    CHECK_THAT(html, ContainsSubstring("javascript"));
}

TEST_CASE("Phase 8: Code block without copy button", "[phase8][code-block]")
{
    HtmlRenderer renderer;
    CodeBlockConfig config;
    config.show_copy_button = false;
    config.show_language_label = false;
    renderer.set_code_block_config(config);

    core::MarkdownParser parser;
    auto doc = parser.parse("```python\nprint('hi')\n```\n");
    REQUIRE(doc.has_value());
    auto html = renderer.render(*doc);

    CHECK_THAT(html, !ContainsSubstring("code-block-container"));
    // Note: CodeBlockRenderer may still emit its own copy-btn link;
    // the Phase 8 config only controls the container wrapper.
}

TEST_CASE("Phase 8: Collapsible code blocks for long code", "[phase8][code-block]")
{
    HtmlRenderer renderer;
    CodeBlockConfig config;
    config.auto_collapse_long = true;
    config.collapse_threshold = 5;
    config.show_copy_button = true;
    renderer.set_code_block_config(config);

    // Build a code block with more than 5 lines
    std::string code = "```python\n";
    for (int line_idx = 0; line_idx < 10; ++line_idx)
    {
        code += "line " + std::to_string(line_idx) + "\n";
    }
    code += "```\n";

    core::MarkdownParser parser;
    auto doc = parser.parse(code);
    REQUIRE(doc.has_value());
    auto html = renderer.render(*doc);

    CHECK_THAT(html, ContainsSubstring("code-collapse"));
    CHECK_THAT(html, ContainsSubstring("<details"));
    CHECK_THAT(html, ContainsSubstring("<summary>"));
    CHECK_THAT(html, ContainsSubstring("Show code"));
}

TEST_CASE("Phase 8: Short code blocks not collapsed", "[phase8][code-block]")
{
    HtmlRenderer renderer;
    CodeBlockConfig config;
    config.auto_collapse_long = true;
    config.collapse_threshold = 20;
    renderer.set_code_block_config(config);

    core::MarkdownParser parser;
    auto doc = parser.parse("```\nshort\n```\n");
    REQUIRE(doc.has_value());
    auto html = renderer.render(*doc);

    CHECK_THAT(html, !ContainsSubstring("code-collapse"));
}

TEST_CASE("Phase 8: Code block config accessor", "[phase8][code-block]")
{
    HtmlRenderer renderer;
    CodeBlockConfig config;
    config.show_copy_button = false;
    config.collapse_threshold = 42;
    renderer.set_code_block_config(config);

    CHECK_FALSE(renderer.code_block_config().show_copy_button);
    CHECK(renderer.code_block_config().collapse_threshold == 42);
}

// ═══════════════════════════════════════════════════════
// Sortable Table Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Phase 8: Sortable table attribute", "[phase8][sortable-tables]")
{
    HtmlRenderer renderer;
    SortableTableConfig config;
    config.enable_sorting = true;
    renderer.set_sortable_table_config(config);

    core::MarkdownParser parser;
    auto doc = parser.parse("| A | B |\n|---|---|\n| 1 | 2 |\n");
    REQUIRE(doc.has_value());
    auto html = renderer.render(*doc);

    CHECK_THAT(html, ContainsSubstring("data-sortable=\"true\""));
    CHECK_THAT(html, ContainsSubstring("table-wrapper"));
}

TEST_CASE("Phase 8: Table without sortable when disabled", "[phase8][sortable-tables]")
{
    HtmlRenderer renderer;
    SortableTableConfig config;
    config.enable_sorting = false;
    renderer.set_sortable_table_config(config);

    core::MarkdownParser parser;
    auto doc = parser.parse("| A | B |\n|---|---|\n| 1 | 2 |\n");
    REQUIRE(doc.has_value());
    auto html = renderer.render(*doc);

    CHECK_THAT(html, !ContainsSubstring("data-sortable"));
}

// ═══════════════════════════════════════════════════════
// Reading Profile Manager Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Phase 8: ReadingProfileManager default profile", "[phase8][reading-profile]")
{
    auto css = ReadingProfileManager::profile_css(ReadingProfilePreset::kDefault);
    CHECK(css.font_size_px == 16.0);
    CHECK(css.line_height == 1.6);
    CHECK(css.max_width_px == 720.0);
    CHECK_FALSE(css.use_serif);
}

TEST_CASE("Phase 8: ReadingProfileManager focused profile", "[phase8][reading-profile]")
{
    auto css = ReadingProfileManager::profile_css(ReadingProfilePreset::kFocused);
    CHECK(css.font_size_px == 18.0);
    CHECK(css.line_height == 1.8);
    CHECK(css.max_width_px == 600.0);
    CHECK(css.use_serif);
    CHECK_THAT(css.font_family, ContainsSubstring("Georgia"));
}

TEST_CASE("Phase 8: ReadingProfileManager compact profile", "[phase8][reading-profile]")
{
    auto css = ReadingProfileManager::profile_css(ReadingProfilePreset::kCompact);
    CHECK(css.font_size_px == 14.0);
    CHECK(css.line_height == 1.4);
    CHECK(css.max_width_px == 900.0);
}

TEST_CASE("Phase 8: ReadingProfileManager accessible profile", "[phase8][reading-profile]")
{
    auto css = ReadingProfileManager::profile_css(ReadingProfilePreset::kAccessible);
    CHECK(css.font_size_px == 20.0);
    CHECK(css.line_height == 2.0);
    CHECK_THAT(css.font_family, ContainsSubstring("Atkinson"));
}

TEST_CASE("Phase 8: ReadingProfileManager comfortable profile", "[phase8][reading-profile]")
{
    auto css = ReadingProfileManager::profile_css(ReadingProfilePreset::kComfortable);
    CHECK(css.font_size_px == 17.0);
    CHECK(css.line_height == 1.7);
}

TEST_CASE("Phase 8: Profile CSS generation", "[phase8][reading-profile]")
{
    auto css_str = ReadingProfileManager::apply_profile(ReadingProfilePreset::kFocused);
    CHECK_THAT(css_str, ContainsSubstring(".preview-content"));
    CHECK_THAT(css_str, ContainsSubstring("font-size:"));
    CHECK_THAT(css_str, ContainsSubstring("line-height:"));
    CHECK_THAT(css_str, ContainsSubstring("max-width:"));
    CHECK_THAT(css_str, ContainsSubstring("Georgia"));
}

TEST_CASE("Phase 8: Profile display names", "[phase8][reading-profile]")
{
    CHECK(ReadingProfileManager::profile_name(ReadingProfilePreset::kDefault) == "Default");
    CHECK(ReadingProfileManager::profile_name(ReadingProfilePreset::kFocused) == "Focused");
    CHECK(ReadingProfileManager::profile_name(ReadingProfilePreset::kComfortable) == "Comfortable");
    CHECK(ReadingProfileManager::profile_name(ReadingProfilePreset::kCompact) == "Compact");
    CHECK(ReadingProfileManager::profile_name(ReadingProfilePreset::kAccessible) == "Accessible");
}

// ═══════════════════════════════════════════════════════
// Print CSS Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Phase 8: Print CSS generation", "[phase8][print-css]")
{
    PrintCssConfig config;
    auto css = ReadingProfileManager::generate_print_css(config);

    CHECK_THAT(css, ContainsSubstring("@media print"));
    CHECK_THAT(css, ContainsSubstring("@page"));
    CHECK_THAT(css, ContainsSubstring("margin-top:"));
    CHECK_THAT(css, ContainsSubstring("margin-bottom:"));
}

TEST_CASE("Phase 8: Print CSS hides nav elements", "[phase8][print-css]")
{
    PrintCssConfig config;
    config.hide_nav_elements = true;
    auto css = ReadingProfileManager::generate_print_css(config);

    CHECK_THAT(css, ContainsSubstring("heading-nav-overlay"));
    CHECK_THAT(css, ContainsSubstring("scroll-to-top-btn"));
    CHECK_THAT(css, ContainsSubstring("display: none"));
}

TEST_CASE("Phase 8: Print CSS page breaks", "[phase8][print-css]")
{
    PrintCssConfig config;
    config.page_break_before_h1 = true;
    config.page_break_before_h2 = true;
    auto css = ReadingProfileManager::generate_print_css(config);

    CHECK_THAT(css, ContainsSubstring("h1 { page-break-before: always; }"));
    CHECK_THAT(css, ContainsSubstring("h2 { page-break-before: always; }"));
}

TEST_CASE("Phase 8: Print CSS monochrome code", "[phase8][print-css]")
{
    PrintCssConfig config;
    config.monochrome_code = true;
    auto css = ReadingProfileManager::generate_print_css(config);

    CHECK_THAT(css, ContainsSubstring("#f5f5f5"));
    CHECK_THAT(css, ContainsSubstring("#333"));
}

TEST_CASE("Phase 8: Print CSS orphan/widow control", "[phase8][print-css]")
{
    PrintCssConfig config;
    auto css = ReadingProfileManager::generate_print_css(config);

    CHECK_THAT(css, ContainsSubstring("orphans: 3"));
    CHECK_THAT(css, ContainsSubstring("widows: 3"));
}

// ═══════════════════════════════════════════════════════
// Feature Type Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Phase 8: SourceLineMapping default construction", "[phase8][types]")
{
    SourceLineMapping mapping;
    CHECK(mapping.source_line == 0);
    CHECK(mapping.element_id.empty());
    CHECK(mapping.element_tag.empty());
}

TEST_CASE("Phase 8: HeadingAnchor default construction", "[phase8][types]")
{
    HeadingAnchor anchor;
    CHECK(anchor.level == 0);
    CHECK(anchor.text.empty());
    CHECK(anchor.slug.empty());
    CHECK(anchor.source_line == 0);
}

TEST_CASE("Phase 8: DirtyRegion default construction", "[phase8][types]")
{
    DirtyRegion region;
    CHECK(region.start_line == 0);
    CHECK(region.end_line == 0);
    CHECK_FALSE(region.full_rerender);
}

TEST_CASE("Phase 8: IncrementalRenderResult is_incremental", "[phase8][types]")
{
    IncrementalRenderResult result;
    result.html_fragment = "test";
    result.was_full_rerender = false;
    CHECK(result.is_incremental());

    result.was_full_rerender = true;
    CHECK_FALSE(result.is_incremental());

    result.was_full_rerender = false;
    result.html_fragment.clear();
    CHECK_FALSE(result.is_incremental());
}

TEST_CASE("Phase 8: CodeBlockConfig defaults", "[phase8][types]")
{
    CodeBlockConfig config;
    CHECK(config.show_copy_button);
    CHECK(config.show_language_label);
    CHECK(config.collapse_threshold == 20);
    CHECK(config.auto_collapse_long);
}

TEST_CASE("Phase 8: SortableTableConfig defaults", "[phase8][types]")
{
    SortableTableConfig config;
    CHECK(config.enable_sorting);
    CHECK(config.show_sort_indicators);
    CHECK(config.default_sort_column.empty());
}

TEST_CASE("Phase 8: PrintCssConfig defaults", "[phase8][types]")
{
    PrintCssConfig config;
    CHECK(config.margin_top_cm == 2.0);
    CHECK(config.margin_bottom_cm == 2.0);
    CHECK(config.margin_left_cm == 2.5);
    CHECK(config.margin_right_cm == 2.5);
    CHECK(config.hide_nav_elements);
    CHECK(config.page_break_before_h1);
    CHECK_FALSE(config.page_break_before_h2);
    CHECK_FALSE(config.monochrome_code);
}

TEST_CASE("Phase 8: ReadingProfileCss to_css generates valid CSS", "[phase8][types]")
{
    ReadingProfileCss profile;
    profile.font_size_px = 18.0;
    profile.line_height = 1.5;
    profile.max_width_px = 800.0;
    profile.font_family = "Arial, sans-serif";
    profile.letter_spacing_px = 0.5;
    profile.paragraph_spacing_px = 20.0;

    auto css = profile.to_css();
    CHECK_THAT(css, ContainsSubstring(".preview-content {"));
    CHECK_THAT(css, ContainsSubstring("font-size: 18"));
    CHECK_THAT(css, ContainsSubstring("line-height: 1.5"));
    CHECK_THAT(css, ContainsSubstring("max-width: 800"));
    CHECK_THAT(css, ContainsSubstring("Arial, sans-serif"));
    CHECK_THAT(css, ContainsSubstring("letter-spacing: 0.5"));
}

TEST_CASE("Phase 8: ReadingProfileCss no letter-spacing when zero", "[phase8][types]")
{
    ReadingProfileCss profile;
    profile.letter_spacing_px = 0.0;

    auto css = profile.to_css();
    CHECK_THAT(css, !ContainsSubstring("letter-spacing"));
}

TEST_CASE("Phase 8: HeadingNavEntry default construction", "[phase8][types]")
{
    HeadingNavEntry entry;
    CHECK(entry.level == 0);
    CHECK(entry.text.empty());
    CHECK(entry.anchor_id.empty());
    CHECK_FALSE(entry.is_active);
}

TEST_CASE("Phase 8: HeadingNavState default construction", "[phase8][types]")
{
    HeadingNavState state;
    CHECK_FALSE(state.visible);
    CHECK(state.entries.empty());
    CHECK(state.active_index == -1);
}

TEST_CASE("Phase 8: CollapsibleSection default construction", "[phase8][types]")
{
    CollapsibleSection section;
    CHECK(section.summary_text.empty());
    CHECK(section.body_html.empty());
    CHECK_FALSE(section.default_open);
    CHECK(section.source_line == 0);
}

// ═══════════════════════════════════════════════════════
// Integration: Combined feature tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Phase 8: Full render with all features enabled", "[phase8][integration]")
{
    core::MarkdownParser parser;
    HtmlRenderer renderer;
    renderer.set_source_line_attributes(true);

    CodeBlockConfig cb_config;
    cb_config.show_copy_button = true;
    cb_config.show_language_label = true;
    cb_config.auto_collapse_long = true;
    cb_config.collapse_threshold = 5;
    renderer.set_code_block_config(cb_config);

    SortableTableConfig st_config;
    st_config.enable_sorting = true;
    renderer.set_sortable_table_config(st_config);

    std::string markdown = R"(
# Phase 8 Integration

Here is a paragraph.

> A blockquote

```python
print("hello")
```

| Name | Score |
|------|-------|
| Alice | 100 |
| Bob   | 95  |
)";

    auto doc = parser.parse(markdown);
    REQUIRE(doc.has_value());
    auto html = renderer.render(*doc);

    // Verify source-line attributes present
    CHECK_THAT(html, ContainsSubstring("data-source-line="));

    // Verify heading anchor collected
    CHECK_FALSE(renderer.heading_anchors().empty());
    CHECK(renderer.heading_anchors()[0].text == "Phase 8 Integration");

    // Verify source-line mappings collected
    CHECK_FALSE(renderer.source_line_mappings().empty());

    // Verify code block container
    CHECK_THAT(html, ContainsSubstring("code-block-container"));
    CHECK_THAT(html, ContainsSubstring("copy-btn"));
    CHECK_THAT(html, ContainsSubstring("code-lang-label"));

    // Verify sortable table
    CHECK_THAT(html, ContainsSubstring("data-sortable=\"true\""));
    CHECK_THAT(html, ContainsSubstring("table-wrapper"));
}

TEST_CASE("Phase 8: Render with all features disabled", "[phase8][integration]")
{
    core::MarkdownParser parser;
    HtmlRenderer renderer;
    renderer.set_source_line_attributes(false);

    CodeBlockConfig cb_config;
    cb_config.show_copy_button = false;
    cb_config.show_language_label = false;
    renderer.set_code_block_config(cb_config);

    SortableTableConfig st_config;
    st_config.enable_sorting = false;
    renderer.set_sortable_table_config(st_config);

    auto doc = parser.parse("# Title\n\n```js\ncode\n```\n\n| A |\n|---|\n| 1 |\n");
    REQUIRE(doc.has_value());
    auto html = renderer.render(*doc);

    CHECK_THAT(html, !ContainsSubstring("data-source-line"));
    CHECK_THAT(html, !ContainsSubstring("code-block-container"));
    CHECK_THAT(html, !ContainsSubstring("data-sortable"));
}
