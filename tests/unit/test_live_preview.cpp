/// @file test_live_preview.cpp
/// @brief V4 Phase 13 – Live Preview Renderer tests.

#include "ui/LivePreviewRenderer.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

using markamp::ui::LivePreviewLine;
using markamp::ui::LivePreviewRenderer;

// ============================================================================
// Test 1: Heading folding
// ============================================================================
TEST_CASE("Heading markers are folded", "[livepreview][heading]")
{
    LivePreviewRenderer renderer;

    auto result = renderer.process_line("# Title", 0, false);

    CHECK_FALSE(result.is_cursor_line);
    CHECK(result.display_text == "Title");
    REQUIRE_FALSE(result.styles.empty());
    CHECK(result.styles[0].style == LivePreviewLine::StyleRun::Style::Heading1);
}

// ============================================================================
// Test 2: Bold folding
// ============================================================================
TEST_CASE("Bold markers are folded", "[livepreview][bold]")
{
    LivePreviewRenderer renderer;

    auto result = renderer.process_line("This is **bold text** here", 0, false);

    CHECK_FALSE(result.is_cursor_line);
    CHECK(result.display_text == "This is bold text here");

    bool has_bold = false;
    for (const auto& run : result.styles)
    {
        if (run.style == LivePreviewLine::StyleRun::Style::Bold)
        {
            has_bold = true;
        }
    }
    CHECK(has_bold);
}

// ============================================================================
// Test 3: Italic folding
// ============================================================================
TEST_CASE("Italic markers are folded", "[livepreview][italic]")
{
    LivePreviewRenderer renderer;

    auto result = renderer.process_line("This is *italic* text", 0, false);

    CHECK(result.display_text == "This is italic text");
}

// ============================================================================
// Test 4: Inline code folding
// ============================================================================
TEST_CASE("Code backtick markers are folded", "[livepreview][code]")
{
    LivePreviewRenderer renderer;

    auto result = renderer.process_line("Use `code` here", 0, false);

    CHECK(result.display_text == "Use code here");

    bool has_code = false;
    for (const auto& run : result.styles)
    {
        if (run.style == LivePreviewLine::StyleRun::Style::Code)
        {
            has_code = true;
        }
    }
    CHECK(has_code);
}

// ============================================================================
// Test 5: WikiLink folding (simple)
// ============================================================================
TEST_CASE("WikiLink brackets are folded", "[livepreview][wikilink]")
{
    LivePreviewRenderer renderer;

    auto result = renderer.process_line("Link to [[My Note]] here", 0, false);

    CHECK(result.display_text == "Link to My Note here");
}

// ============================================================================
// Test 6: WikiLink with alias
// ============================================================================
TEST_CASE("WikiLink alias shows alias only", "[livepreview][wikilink][alias]")
{
    LivePreviewRenderer renderer;

    auto result = renderer.process_line("See [[My Note|display text]] for details", 0, false);

    CHECK(result.display_text == "See display text for details");
}

// ============================================================================
// Test 7: Cursor line shows raw text
// ============================================================================
TEST_CASE("Cursor line shows raw markdown", "[livepreview][cursor]")
{
    LivePreviewRenderer renderer;

    const std::string raw_line = "**bold** and *italic*";
    auto result = renderer.process_line(raw_line, 5, true);

    CHECK(result.is_cursor_line);
    CHECK(result.display_text == raw_line);
    CHECK(result.folded_ranges.empty());
}

// ============================================================================
// Test 8: Checkbox rendering
// ============================================================================
TEST_CASE("Checkbox lines have checkbox styles", "[livepreview][checkbox]")
{
    LivePreviewRenderer renderer;

    auto unchecked = renderer.process_line("- [ ] task item", 0, false);
    auto checked = renderer.process_line("- [x] done item", 1, false);

    bool has_unchecked = false;
    bool has_checked = false;

    for (const auto& run : unchecked.styles)
    {
        if (run.style == LivePreviewLine::StyleRun::Style::CheckboxUnchecked)
        {
            has_unchecked = true;
        }
    }

    for (const auto& run : checked.styles)
    {
        if (run.style == LivePreviewLine::StyleRun::Style::CheckboxChecked)
        {
            has_checked = true;
        }
    }

    CHECK(has_unchecked);
    CHECK(has_checked);
}

// ============================================================================
// Test 9: Code block passthrough
// ============================================================================
TEST_CASE("Code block lines are not processed", "[livepreview][codeblock]")
{
    LivePreviewRenderer renderer;

    const std::string markdown = "# Title\n"
                                 "```\n"
                                 "# Not a heading\n"
                                 "**not bold**\n"
                                 "```\n"
                                 "## Real Heading\n";

    auto lines = renderer.process(markdown, -1); // No cursor

    REQUIRE(lines.size() == 6);

    // Line 0: heading
    CHECK(lines[0].display_text == "Title");

    // Lines 2-3: inside code block, raw text preserved
    CHECK(lines[2].display_text == "# Not a heading");
    CHECK(lines[3].display_text == "**not bold**");

    // Line 5: heading processed
    CHECK(lines[5].display_text == "Real Heading");
}

// ============================================================================
// Test 10: Multi-marker (nested formatting)
// ============================================================================
TEST_CASE("Multiple inline markers on one line", "[livepreview][multi]")
{
    LivePreviewRenderer renderer;

    // Line with multiple different markers
    auto result = renderer.process_line("Text with `code` and ~~strike~~", 0, false);

    CHECK(result.display_text == "Text with code and strike");
}

// ============================================================================
// Test 11: Folded range detection
// ============================================================================
TEST_CASE("is_folded returns true for hidden marker positions", "[livepreview][folded]")
{
    LivePreviewRenderer renderer;

    auto result = renderer.process_line("**bold**", 0, false);

    // Positions 0,1 (opening **) and 6,7 (closing **) should be folded in raw text
    CHECK(LivePreviewRenderer::is_folded(result, 0));
    CHECK(LivePreviewRenderer::is_folded(result, 1));
    CHECK_FALSE(LivePreviewRenderer::is_folded(result, 2)); // 'b'
    CHECK(LivePreviewRenderer::is_folded(result, 6));
    CHECK(LivePreviewRenderer::is_folded(result, 7));
}

// ============================================================================
// Test 12: Heading level detection
// ============================================================================
TEST_CASE("Heading levels correctly detected", "[livepreview][headinglevel]")
{
    CHECK(LivePreviewRenderer::detect_heading_level("# H1") == 1);
    CHECK(LivePreviewRenderer::detect_heading_level("## H2") == 2);
    CHECK(LivePreviewRenderer::detect_heading_level("### H3") == 3);
    CHECK(LivePreviewRenderer::detect_heading_level("#### H4") == 4);
    CHECK(LivePreviewRenderer::detect_heading_level("##### H5") == 5);
    CHECK(LivePreviewRenderer::detect_heading_level("###### H6") == 6);
    CHECK(LivePreviewRenderer::detect_heading_level("####### Too many") == 0);
    CHECK(LivePreviewRenderer::detect_heading_level("Not a heading") == 0);
    CHECK(LivePreviewRenderer::detect_heading_level("") == 0);
    CHECK(LivePreviewRenderer::detect_heading_level("#NoSpace") == 0);
}
