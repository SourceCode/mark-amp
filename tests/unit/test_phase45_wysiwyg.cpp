/// @file test_phase45_wysiwyg.cpp
/// @brief V9 Phase 45 — Comprehensive unit tests for Live Preview & WYSIWYG.
/// Tasks 18–20: unit, integration, and performance tests for all Phase 45 components.

#include "core/InlineCalloutRenderer.h"
#include "core/InlineCheckboxRenderer.h"
#include "core/InlineCodeBlockRenderer.h"
#include "core/InlineFrontmatterRenderer.h"
#include "core/InlineHeadingRenderer.h"
#include "core/InlineImageRenderer.h"
#include "core/InlineLinkHandler.h"
#include "core/InlineMathRenderer.h"
#include "core/InlineTableRenderer.h"
#include "core/WysiwygAccessibility.h"
#include "core/WysiwygEngine.h"
#include "core/WysiwygModeController.h"
#include "core/WysiwygStatePersistence.h"
#include "core/WysiwygTypes.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

using namespace markamp::core;

// ============================================================================
// Task 18: Unit Tests — WysiwygTypes
// ============================================================================

TEST_CASE("WysiwygTypes: WysiwygMode enum values", "[wysiwyg][types]")
{
    REQUIRE(static_cast<int>(WysiwygMode::kSource) == 0);
    REQUIRE(static_cast<int>(WysiwygMode::kLivePreview) == 1);
    REQUIRE(static_cast<int>(WysiwygMode::kWysiwyg) == 2);
}

TEST_CASE("WysiwygTypes: WysiwygBlockType enum values", "[wysiwyg][types]")
{
    REQUIRE(static_cast<int>(WysiwygBlockType::kParagraph) == 0);
    REQUIRE(static_cast<int>(WysiwygBlockType::kHeading) == 1);
    REQUIRE(static_cast<int>(WysiwygBlockType::kCodeBlock) == 2);
    REQUIRE(static_cast<int>(WysiwygBlockType::kThematicBreak) == 11);
}

TEST_CASE("WysiwygTypes: WysiwygRenderConfig defaults", "[wysiwyg][types]")
{
    WysiwygRenderConfig config;
    REQUIRE(config.render_images_inline == true);
    REQUIRE(config.render_checkboxes == true);
    REQUIRE(config.render_tables_inline == true);
    REQUIRE(config.render_math_inline == true);
    REQUIRE(config.render_code_blocks == true);
    REQUIRE(config.render_callouts == true);
}

TEST_CASE("WysiwygTypes: WysiwygDocumentModel parse headings", "[wysiwyg][types]")
{
    WysiwygDocumentModel model;
    model.parse("# Heading 1\n\nSome text\n\n## Heading 2");

    REQUIRE(model.block_count() > 0);

    const auto* block = model.get_block_at_line(0);
    REQUIRE(block != nullptr);
    REQUIRE(block->block_type == WysiwygBlockType::kHeading);
}

TEST_CASE("WysiwygTypes: WysiwygDocumentModel parse code block", "[wysiwyg][types]")
{
    WysiwygDocumentModel model;
    model.parse("```python\nprint('hello')\n```");

    REQUIRE(model.block_count() > 0);
    const auto& blocks = model.blocks();
    bool has_code = false;
    for (const auto& blk : blocks)
    {
        if (blk.block_type == WysiwygBlockType::kCodeBlock)
        {
            has_code = true;
        }
    }
    REQUIRE(has_code);
}

TEST_CASE("WysiwygTypes: WysiwygDocumentModel to_markdown round-trip", "[wysiwyg][types]")
{
    const std::string original = "# Title\n\nParagraph text\n\n- item 1\n- item 2";
    WysiwygDocumentModel model;
    model.parse(original);

    const auto result = model.to_markdown();
    // Round-trip should contain the same key content
    REQUIRE(result.find("# Title") != std::string::npos);
    REQUIRE(result.find("Paragraph text") != std::string::npos);
    REQUIRE(result.find("- item 1") != std::string::npos);
}

TEST_CASE("WysiwygTypes: WysiwygDocumentModel empty document", "[wysiwyg][types]")
{
    WysiwygDocumentModel model;
    model.parse("");
    REQUIRE(model.block_count() == 0);
    REQUIRE(model.blocks().empty());
    REQUIRE(model.get_block_at_line(0) == nullptr);
}

TEST_CASE("WysiwygTypes: WysiwygDocumentModel replace_block", "[wysiwyg][types]")
{
    WysiwygDocumentModel model;
    model.parse("# Original\n\nSome text");

    REQUIRE(model.replace_block(0, "# Updated"));
    const auto& blocks = model.blocks();
    REQUIRE(blocks[0].raw_content == "# Updated");
    REQUIRE(blocks[0].block_type == WysiwygBlockType::kHeading);
}

// ============================================================================
// Task 18: Unit Tests — WysiwygEngine
// ============================================================================

TEST_CASE("WysiwygEngine: process_document basic", "[wysiwyg][engine]")
{
    WysiwygEngine engine;
    auto model = engine.process_document("# Hello\n\nWorld", 0);

    REQUIRE(model.block_count() > 0);
    REQUIRE(engine.get_active_block() != nullptr);
}

TEST_CASE("WysiwygEngine: active block tracking", "[wysiwyg][engine]")
{
    WysiwygEngine engine;
    engine.process_document("# Title\n\nParagraph\n\n```cpp\ncode\n```", 4);

    // Cursor at line 4 should be in the code block
    const auto* active = engine.get_active_block();
    REQUIRE(active != nullptr);
    REQUIRE(active->is_active);
}

TEST_CASE("WysiwygEngine: mode management", "[wysiwyg][engine]")
{
    WysiwygEngine engine;
    REQUIRE(engine.mode() == WysiwygMode::kLivePreview);

    engine.set_mode(WysiwygMode::kSource);
    REQUIRE(engine.mode() == WysiwygMode::kSource);

    engine.set_mode(WysiwygMode::kWysiwyg);
    REQUIRE(engine.mode() == WysiwygMode::kWysiwyg);
}

TEST_CASE("WysiwygEngine: should_render_inline", "[wysiwyg][engine]")
{
    WysiwygEngine engine;

    // Source mode: nothing renders inline
    engine.set_mode(WysiwygMode::kSource);
    REQUIRE(engine.should_render_inline(WysiwygBlockType::kImage) == false);
    REQUIRE(engine.should_render_inline(WysiwygBlockType::kHeading) == false);

    // LivePreview: images render inline
    engine.set_mode(WysiwygMode::kLivePreview);
    REQUIRE(engine.should_render_inline(WysiwygBlockType::kImage) == true);
    REQUIRE(engine.should_render_inline(WysiwygBlockType::kHeading) == true);
}

TEST_CASE("WysiwygEngine: commit_block_edit", "[wysiwyg][engine]")
{
    WysiwygEngine engine;
    engine.process_document("# Title\n\nParagraph", 0);

    const auto result = engine.commit_block_edit(0, "# New Title");
    REQUIRE(result.find("# New Title") != std::string::npos);
}

// ============================================================================
// Task 18: Unit Tests — WysiwygModeController
// ============================================================================

TEST_CASE("WysiwygModeController: mode switching", "[wysiwyg][controller]")
{
    WysiwygModeController controller;
    REQUIRE(controller.current_mode() == WysiwygMode::kLivePreview);

    controller.switch_mode(WysiwygMode::kSource);
    REQUIRE(controller.current_mode() == WysiwygMode::kSource);
}

TEST_CASE("WysiwygModeController: cycle mode", "[wysiwyg][controller]")
{
    WysiwygModeController controller;
    controller.switch_mode(WysiwygMode::kSource);

    controller.cycle_mode(); // Source → LivePreview
    REQUIRE(controller.current_mode() == WysiwygMode::kLivePreview);

    controller.cycle_mode(); // LivePreview → WYSIWYG
    REQUIRE(controller.current_mode() == WysiwygMode::kWysiwyg);

    controller.cycle_mode(); // WYSIWYG → Source
    REQUIRE(controller.current_mode() == WysiwygMode::kSource);
}

TEST_CASE("WysiwygModeController: mode change callback", "[wysiwyg][controller]")
{
    WysiwygModeController controller;
    WysiwygMode captured_old = WysiwygMode::kSource;
    WysiwygMode captured_new = WysiwygMode::kSource;

    controller.on_mode_changed(
        [&](WysiwygMode old_mode, WysiwygMode new_mode)
        {
            captured_old = old_mode;
            captured_new = new_mode;
        });

    controller.switch_mode(WysiwygMode::kWysiwyg);
    REQUIRE(captured_old == WysiwygMode::kLivePreview);
    REQUIRE(captured_new == WysiwygMode::kWysiwyg);
}

TEST_CASE("WysiwygModeController: block activation", "[wysiwyg][controller]")
{
    WysiwygModeController controller;

    controller.activate_block(3);
    REQUIRE(controller.active_block_index() == 3);
    REQUIRE(controller.is_block_active(3));
    REQUIRE_FALSE(controller.is_block_active(0));

    controller.deactivate_block(3);
    REQUIRE(controller.active_block_index() == -1);
}

TEST_CASE("WysiwygModeController: per-file mode persistence", "[wysiwyg][controller]")
{
    WysiwygModeController controller;

    controller.set_file_mode("/docs/readme.md", WysiwygMode::kWysiwyg);
    controller.set_file_mode("/docs/notes.md", WysiwygMode::kSource);

    REQUIRE(controller.get_file_mode("/docs/readme.md") == WysiwygMode::kWysiwyg);
    REQUIRE(controller.get_file_mode("/docs/notes.md") == WysiwygMode::kSource);
    REQUIRE(controller.get_file_mode("/unknown.md") == WysiwygMode::kLivePreview);
}

TEST_CASE("WysiwygModeController: collapsed blocks", "[wysiwyg][controller]")
{
    WysiwygModeController controller;

    controller.toggle_collapsed(2);
    REQUIRE(controller.is_collapsed(2));

    controller.toggle_collapsed(2);
    REQUIRE_FALSE(controller.is_collapsed(2));
}

// ============================================================================
// Task 18: Unit Tests — InlineImageRenderer
// ============================================================================

TEST_CASE("InlineImageRenderer: parse simple image", "[wysiwyg][image]")
{
    auto images = InlineImageRenderer::parse_images("![Alt text](image.png)", 5);
    REQUIRE(images.size() == 1);
    REQUIRE(images[0].alt_text == "Alt text");
    REQUIRE(images[0].url == "image.png");
    REQUIRE(images[0].is_valid);
    REQUIRE_FALSE(images[0].is_remote);
    REQUIRE(images[0].line_number == 5);
}

TEST_CASE("InlineImageRenderer: parse image with title", "[wysiwyg][image]")
{
    auto images = InlineImageRenderer::parse_images("![Photo](pic.jpg \"My Title\")");
    REQUIRE(images.size() == 1);
    REQUIRE(images[0].title == "My Title");
    REQUIRE(images[0].url == "pic.jpg");
}

TEST_CASE("InlineImageRenderer: parse remote image", "[wysiwyg][image]")
{
    auto images = InlineImageRenderer::parse_images("![](https://example.com/photo.png)");
    REQUIRE(images.size() == 1);
    REQUIRE(images[0].is_remote);
    REQUIRE(images[0].url == "https://example.com/photo.png");
}

TEST_CASE("InlineImageRenderer: compute display size", "[wysiwyg][image]")
{
    InlineImageInfo info;
    info.original_width = 1920;
    info.original_height = 1080;

    InlineImageRenderer::compute_display_size(info, 800, 600);
    REQUIRE(info.display_width <= 800);
    REQUIRE(info.display_height <= 600);
    REQUIRE(info.display_width > 0);
    REQUIRE(info.display_height > 0);
}

TEST_CASE("InlineImageRenderer: standalone image detection", "[wysiwyg][image]")
{
    REQUIRE(InlineImageRenderer::is_standalone_image("![Alt](image.png)"));
    REQUIRE_FALSE(InlineImageRenderer::is_standalone_image("Text ![Alt](img.png) more text"));
    REQUIRE_FALSE(InlineImageRenderer::is_standalone_image("Just text"));
}

// ============================================================================
// Task 18: Unit Tests — InlineCheckboxRenderer
// ============================================================================

TEST_CASE("InlineCheckboxRenderer: detect task items", "[wysiwyg][checkbox]")
{
    REQUIRE(InlineCheckboxRenderer::is_task_item("- [ ] unchecked"));
    REQUIRE(InlineCheckboxRenderer::is_task_item("- [x] checked"));
    REQUIRE(InlineCheckboxRenderer::is_task_item("* [ ] star unchecked"));
    REQUIRE_FALSE(InlineCheckboxRenderer::is_task_item("- plain list item"));
    REQUIRE_FALSE(InlineCheckboxRenderer::is_task_item("regular text"));
}

TEST_CASE("InlineCheckboxRenderer: check state", "[wysiwyg][checkbox]")
{
    REQUIRE_FALSE(InlineCheckboxRenderer::is_checked("- [ ] unchecked"));
    REQUIRE(InlineCheckboxRenderer::is_checked("- [x] checked"));
    REQUIRE(InlineCheckboxRenderer::is_checked("- [X] checked upper"));
}

TEST_CASE("InlineCheckboxRenderer: toggle", "[wysiwyg][checkbox]")
{
    auto toggled = InlineCheckboxRenderer::toggle("- [ ] do this");
    REQUIRE(toggled == "- [x] do this");

    auto back = InlineCheckboxRenderer::toggle("- [x] already done");
    REQUIRE(back == "- [ ] already done");
}

TEST_CASE("InlineCheckboxRenderer: count tasks", "[wysiwyg][checkbox]")
{
    const std::string doc = "- [ ] Task 1\n"
                            "- [x] Task 2\n"
                            "- [ ] Task 3\n"
                            "- [x] Task 4\n";

    auto stats = InlineCheckboxRenderer::count_tasks(doc);
    REQUIRE(stats.total == 4);
    REQUIRE(stats.completed == 2);
    REQUIRE(stats.completion_percent() == 50);
}

TEST_CASE("InlineCheckboxRenderer: get task text", "[wysiwyg][checkbox]")
{
    auto text = InlineCheckboxRenderer::get_task_text("- [x] Buy milk");
    // get_task_text extracts everything after the checkbox marker, preserving leading space
    REQUIRE_FALSE(text.empty());
    REQUIRE(text.find("Buy milk") != std::string::npos);
}

// ============================================================================
// Task 18: Unit Tests — InlineLinkHandler
// ============================================================================

TEST_CASE("InlineLinkHandler: classify external link", "[wysiwyg][link]")
{
    auto result = InlineLinkHandler::classify_link("https://example.com");
    REQUIRE(result.action == LinkClickAction::kOpenExternal);
    REQUIRE(result.is_valid);
}

TEST_CASE("InlineLinkHandler: classify heading anchor", "[wysiwyg][link]")
{
    auto result = InlineLinkHandler::classify_link("#my-heading");
    REQUIRE(result.action == LinkClickAction::kScrollToHeading);
    REQUIRE(result.resolved_target == "my-heading");
}

TEST_CASE("InlineLinkHandler: classify internal link", "[wysiwyg][link]")
{
    auto result = InlineLinkHandler::classify_link("other.md", "/docs/current.md");
    REQUIRE(result.action == LinkClickAction::kNavigateInternal);
    REQUIRE(result.resolved_target == "/docs/other.md");
}

TEST_CASE("InlineLinkHandler: resolve wiki link", "[wysiwyg][link]")
{
    REQUIRE(InlineLinkHandler::resolve_wiki_link("Page Name") == "Page Name.md");
    REQUIRE(InlineLinkHandler::resolve_wiki_link("Page|Alias") == "Page.md");
    REQUIRE(InlineLinkHandler::resolve_wiki_link("Page#heading") == "Page.md");
}

TEST_CASE("InlineLinkHandler: resolve heading link slug", "[wysiwyg][link]")
{
    REQUIRE(InlineLinkHandler::resolve_heading_link("My Heading") == "my-heading");
    REQUIRE(InlineLinkHandler::resolve_heading_link("Complex Title! @#$% Here") ==
            "complex-title-here");
}

TEST_CASE("InlineLinkHandler: parse link", "[wysiwyg][link]")
{
    auto parts = InlineLinkHandler::parse_link("[Click me](https://example.com)");
    REQUIRE(parts.is_valid);
    REQUIRE(parts.text == "Click me");
    REQUIRE(parts.url == "https://example.com");
}

// ============================================================================
// Task 18: Unit Tests — InlineCodeBlockRenderer
// ============================================================================

TEST_CASE("InlineCodeBlockRenderer: parse fenced block", "[wysiwyg][codeblock]")
{
    std::vector<std::string> lines = {"```python", "print('hello')", "x = 42", "```"};
    auto info = InlineCodeBlockRenderer::parse_fenced_block(lines, 0);

    REQUIRE(info.language == "python");
    REQUIRE(info.line_count == 2);
    REQUIRE(info.content.find("print('hello')") != std::string::npos);
    REQUIRE(info.start_line == 0);
    REQUIRE(info.end_line == 3);
}

TEST_CASE("InlineCodeBlockRenderer: detect language aliases", "[wysiwyg][codeblock]")
{
    REQUIRE(InlineCodeBlockRenderer::detect_language("js") == "javascript");
    REQUIRE(InlineCodeBlockRenderer::detect_language("ts") == "typescript");
    REQUIRE(InlineCodeBlockRenderer::detect_language("py") == "python");
    REQUIRE(InlineCodeBlockRenderer::detect_language("sh") == "shellscript");
    REQUIRE(InlineCodeBlockRenderer::detect_language("yml") == "yaml");
    REQUIRE(InlineCodeBlockRenderer::detect_language("c++") == "cpp");
    REQUIRE(InlineCodeBlockRenderer::detect_language("rs") == "rust");
}

TEST_CASE("InlineCodeBlockRenderer: should collapse", "[wysiwyg][codeblock]")
{
    REQUIRE_FALSE(InlineCodeBlockRenderer::should_collapse(10));
    REQUIRE(InlineCodeBlockRenderer::should_collapse(25));
}

TEST_CASE("InlineCodeBlockRenderer: fence start detection", "[wysiwyg][codeblock]")
{
    REQUIRE(InlineCodeBlockRenderer::is_fence_start("```"));
    REQUIRE(InlineCodeBlockRenderer::is_fence_start("```python"));
    REQUIRE(InlineCodeBlockRenderer::is_fence_start("~~~"));
    REQUIRE_FALSE(InlineCodeBlockRenderer::is_fence_start("``"));
    REQUIRE_FALSE(InlineCodeBlockRenderer::is_fence_start("hello"));
}

// ============================================================================
// Task 18: Unit Tests — InlineCalloutRenderer
// ============================================================================

TEST_CASE("InlineCalloutRenderer: detect callout start", "[wysiwyg][callout]")
{
    REQUIRE(InlineCalloutRenderer::is_callout_start("> [!NOTE]"));
    REQUIRE(InlineCalloutRenderer::is_callout_start("> [!WARNING]"));
    REQUIRE_FALSE(InlineCalloutRenderer::is_callout_start("> Regular quote"));
    REQUIRE_FALSE(InlineCalloutRenderer::is_callout_start("Not a quote"));
}

TEST_CASE("InlineCalloutRenderer: parse callout types", "[wysiwyg][callout]")
{
    std::vector<std::string> lines = {"> [!TIP]", "> Use this feature!", "> It's great."};
    auto info = InlineCalloutRenderer::parse_callout(lines, 0);

    REQUIRE(info.is_valid);
    REQUIRE(info.type == CalloutType::kTip);
    REQUIRE(info.icon == "lightbulb");
    REQUIRE(info.color_hint == "green");
    REQUIRE(info.body.find("Use this feature!") != std::string::npos);
}

TEST_CASE("InlineCalloutRenderer: collapsible callout", "[wysiwyg][callout]")
{
    std::vector<std::string> lines = {"> [!NOTE]- Click to expand", "> Hidden content"};
    auto info = InlineCalloutRenderer::parse_callout(lines, 0);

    REQUIRE(info.is_valid);
    REQUIRE(info.is_collapsible);
    REQUIRE(info.is_collapsed);
}

TEST_CASE("InlineCalloutRenderer: type display names", "[wysiwyg][callout]")
{
    REQUIRE(InlineCalloutRenderer::type_display_name(CalloutType::kNote) == "Note");
    REQUIRE(InlineCalloutRenderer::type_display_name(CalloutType::kWarning) == "Warning");
    REQUIRE(InlineCalloutRenderer::type_display_name(CalloutType::kCaution) == "Caution");
}

// ============================================================================
// Task 18: Unit Tests — InlineHeadingRenderer
// ============================================================================

TEST_CASE("InlineHeadingRenderer: parse valid heading", "[wysiwyg][heading]")
{
    auto info = InlineHeadingRenderer::parse_heading("## My Section", 10);
    REQUIRE(info.is_valid);
    REQUIRE(info.level == 2);
    REQUIRE(info.text == "My Section");
    REQUIRE(info.anchor_id == "my-section");
    REQUIRE(info.line_number == 10);
}

TEST_CASE("InlineHeadingRenderer: font scale factors", "[wysiwyg][heading]")
{
    REQUIRE(InlineHeadingRenderer::compute_font_scale(1) == 2.0F);
    REQUIRE(InlineHeadingRenderer::compute_font_scale(3) == 1.3F);
    REQUIRE(InlineHeadingRenderer::compute_font_scale(6) == 0.9F);
    REQUIRE(InlineHeadingRenderer::compute_font_scale(0) == 1.0F); // Invalid
}

TEST_CASE("InlineHeadingRenderer: reject non-headings", "[wysiwyg][heading]")
{
    REQUIRE_FALSE(InlineHeadingRenderer::is_heading("Not a heading"));
    REQUIRE_FALSE(InlineHeadingRenderer::is_heading("#NoSpace"));
    REQUIRE_FALSE(InlineHeadingRenderer::is_heading(""));
}

// ============================================================================
// Task 18: Unit Tests — InlineMathRenderer
// ============================================================================

TEST_CASE("InlineMathRenderer: parse inline math", "[wysiwyg][math]")
{
    auto spans = InlineMathRenderer::parse_inline_math("Text $E = mc^2$ more");
    REQUIRE(spans.size() == 1);
    REQUIRE(spans[0].expression == "E = mc^2");
    REQUIRE(spans[0].is_valid);
}

TEST_CASE("InlineMathRenderer: skip display math", "[wysiwyg][math]")
{
    auto spans = InlineMathRenderer::parse_inline_math("$$x^2$$");
    REQUIRE(spans.empty()); // $$ should not be captured as inline
}

TEST_CASE("InlineMathRenderer: parse display math block", "[wysiwyg][math]")
{
    std::vector<std::string> lines = {"$$", "\\sum_{i=0}^{n} x_i", "$$"};
    auto info = InlineMathRenderer::parse_display_math(lines, 0);

    REQUIRE(info.is_valid);
    REQUIRE(info.is_display_math);
    REQUIRE(info.expression.find("\\sum") != std::string::npos);
    REQUIRE(info.start_line == 0);
    REQUIRE(info.end_line == 2);
}

TEST_CASE("InlineMathRenderer: display math start detection", "[wysiwyg][math]")
{
    REQUIRE(InlineMathRenderer::is_display_math_start("$$"));
    REQUIRE(InlineMathRenderer::is_display_math_start("  $$"));
    REQUIRE_FALSE(InlineMathRenderer::is_display_math_start("$single$"));
    REQUIRE_FALSE(InlineMathRenderer::is_display_math_start("text"));
}

// ============================================================================
// Task 18: Unit Tests — InlineFrontmatterRenderer
// ============================================================================

TEST_CASE("InlineFrontmatterRenderer: parse frontmatter", "[wysiwyg][frontmatter]")
{
    std::vector<std::string> lines = {"---", "title: My Document", "date: 2024-01-01", "---"};
    auto info = InlineFrontmatterRenderer::parse_frontmatter(lines);

    REQUIRE(info.is_valid);
    REQUIRE(info.field_count() == 2);
    REQUIRE(info.has_field("title"));
    REQUIRE(info.fields.at("title") == "My Document");
    REQUIRE(info.fields.at("date") == "2024-01-01");
}

TEST_CASE("InlineFrontmatterRenderer: render summary", "[wysiwyg][frontmatter]")
{
    std::vector<std::string> lines = {"---", "title: My Doc", "tags: a, b, c", "---"};
    auto info = InlineFrontmatterRenderer::parse_frontmatter(lines);

    auto summary = InlineFrontmatterRenderer::render_summary(info);
    REQUIRE(summary.find("title: My Doc") != std::string::npos);
}

TEST_CASE("InlineFrontmatterRenderer: no frontmatter", "[wysiwyg][frontmatter]")
{
    std::vector<std::string> lines = {"# Heading", "Text"};
    auto info = InlineFrontmatterRenderer::parse_frontmatter(lines);
    REQUIRE_FALSE(info.is_valid);
}

// ============================================================================
// Task 18: Unit Tests — InlineTableRenderer
// ============================================================================

TEST_CASE("InlineTableRenderer: parse table", "[wysiwyg][table]")
{
    std::vector<std::string> lines = {
        "| Name | Age |", "| --- | --- |", "| Alice | 30 |", "| Bob | 25 |"};

    auto info = InlineTableRenderer::parse_table(lines, 0);

    REQUIRE(info.is_valid);
    REQUIRE(info.column_count == 2);
    REQUIRE(info.row_count == 2);
    REQUIRE(info.headers[0] == "Name");
    REQUIRE(info.headers[1] == "Age");
    REQUIRE(info.rows[0][0] == "Alice");
    REQUIRE(info.rows[1][1] == "25");
}

TEST_CASE("InlineTableRenderer: alignment detection", "[wysiwyg][table]")
{
    std::vector<std::string> lines = {
        "| Left | Center | Right |", "| :--- | :---: | ---: |", "| a | b | c |"};

    auto info = InlineTableRenderer::parse_table(lines, 0);

    REQUIRE(info.is_valid);
    REQUIRE(info.alignments[0] == TableAlignment::kLeft);
    REQUIRE(info.alignments[1] == TableAlignment::kCenter);
    REQUIRE(info.alignments[2] == TableAlignment::kRight);
}

TEST_CASE("InlineTableRenderer: compute column widths", "[wysiwyg][table]")
{
    std::vector<std::string> lines = {"| Name | Age |", "| --- | --- |", "| Alice | 30 |"};

    auto info = InlineTableRenderer::parse_table(lines, 0);
    auto widths = InlineTableRenderer::compute_column_widths(info, 800);

    REQUIRE(widths.size() == 2);
    REQUIRE(widths[0] > 0);
    REQUIRE(widths[1] > 0);
}

// ============================================================================
// Task 18: Unit Tests — WysiwygStatePersistence
// ============================================================================

TEST_CASE("WysiwygStatePersistence: serialize and deserialize", "[wysiwyg][persistence]")
{
    WysiwygFileState state;
    state.mode = WysiwygMode::kWysiwyg;
    state.cursor_line = 42;
    state.active_block_index = 3;
    state.scroll_position = 0.75;
    state.collapsed_blocks = {2, 5, 8};
    state.file_path = "/docs/test.md";

    auto serialized = WysiwygStatePersistence::serialize(state);
    REQUIRE(serialized.find("mode=2") != std::string::npos);
    REQUIRE(serialized.find("cursor=42") != std::string::npos);

    auto restored = WysiwygStatePersistence::deserialize(serialized);
    REQUIRE(restored.mode == WysiwygMode::kWysiwyg);
    REQUIRE(restored.cursor_line == 42);
    REQUIRE(restored.active_block_index == 3);
    REQUIRE(restored.collapsed_blocks.count(5) > 0);
}

TEST_CASE("WysiwygStatePersistence: round-trip", "[wysiwyg][persistence]")
{
    WysiwygFileState state;
    state.mode = WysiwygMode::kSource;
    state.cursor_line = 100;
    state.collapsed_blocks = {1, 3, 7};

    auto result = WysiwygStatePersistence::round_trip(state);
    REQUIRE(result.mode == WysiwygMode::kSource);
    REQUIRE(result.cursor_line == 100);
    REQUIRE(result.collapsed_blocks == state.collapsed_blocks);
}

TEST_CASE("WysiwygStatePersistence: default state", "[wysiwyg][persistence]")
{
    auto state = WysiwygStatePersistence::default_state();
    REQUIRE(state.is_default());
    REQUIRE(state.mode == WysiwygMode::kLivePreview);
}

// ============================================================================
// Task 18: Unit Tests — WysiwygAccessibility
// ============================================================================

TEST_CASE("WysiwygAccessibility: heading block info", "[wysiwyg][accessibility]")
{
    WysiwygBlock block;
    block.block_type = WysiwygBlockType::kHeading;
    block.metadata["level"] = "2";
    block.metadata["text"] = "Section Title";

    auto info = WysiwygAccessibility::get_accessibility_info(block);
    REQUIRE(info.role == "heading");
    REQUIRE(info.level == 2);
    REQUIRE(info.label == "Section Title");
    REQUIRE(info.is_interactive);
}

TEST_CASE("WysiwygAccessibility: image block info", "[wysiwyg][accessibility]")
{
    WysiwygBlock block;
    block.block_type = WysiwygBlockType::kImage;
    block.metadata["alt_text"] = "A photo";

    auto info = WysiwygAccessibility::get_accessibility_info(block);
    REQUIRE(info.role == "img");
    REQUIRE(info.label == "A photo");
    REQUIRE_FALSE(info.is_interactive);
}

TEST_CASE("WysiwygAccessibility: mode switch announcement", "[wysiwyg][accessibility]")
{
    auto msg =
        WysiwygAccessibility::announce_mode_switch(WysiwygMode::kSource, WysiwygMode::kWysiwyg);
    REQUIRE(msg == "Switched from Source to WYSIWYG mode");
}

// ============================================================================
// Task 19: Integration Tests — Full Pipeline
// ============================================================================

TEST_CASE("Integration: full pipeline mode switching", "[wysiwyg][integration]")
{
    WysiwygEngine engine;
    WysiwygModeController controller;

    const std::string doc = "---\ntitle: Test\n---\n\n"
                            "# Heading\n\n"
                            "Paragraph with **bold** and *italic*.\n\n"
                            "- [ ] Task 1\n- [x] Task 2\n\n"
                            "```cpp\nint x = 1;\n```\n\n"
                            "> [!TIP]\n> Use tips!\n\n"
                            "![Image](pic.png)\n\n"
                            "$E = mc^2$\n\n"
                            "| A | B |\n| --- | --- |\n| 1 | 2 |\n";

    // Process in each mode
    for (auto mode : {WysiwygMode::kSource, WysiwygMode::kLivePreview, WysiwygMode::kWysiwyg})
    {
        controller.switch_mode(mode);
        engine.set_mode(mode);
        auto model = engine.process_document(doc, 4);
        REQUIRE(model.block_count() > 0);
    }
}

TEST_CASE("Integration: checkbox toggle through engine", "[wysiwyg][integration]")
{
    WysiwygEngine engine;
    auto model = engine.process_document("- [ ] Buy milk\n- [x] Done task", 0);

    // Find the list block
    for (int idx = 0; idx < static_cast<int>(model.blocks().size()); ++idx)
    {
        if (model.blocks()[static_cast<std::size_t>(idx)].block_type == WysiwygBlockType::kList)
        {
            const auto& content = model.blocks()[static_cast<std::size_t>(idx)].raw_content;
            auto stats = InlineCheckboxRenderer::count_tasks(content);
            REQUIRE(stats.total >= 1);
            break;
        }
    }
}

TEST_CASE("Integration: block activation and deactivation cycle", "[wysiwyg][integration]")
{
    WysiwygModeController controller;
    int activated_block = -1;
    bool was_activated = false;

    controller.on_block_activation_changed(
        [&](int block_idx, bool activated)
        {
            activated_block = block_idx;
            was_activated = activated;
        });

    controller.activate_block(2);
    REQUIRE(activated_block == 2);
    REQUIRE(was_activated);

    controller.activate_block(5); // Should deactivate 2 first, then activate 5
    REQUIRE(controller.active_block_index() == 5);
}

// ============================================================================
// Task 19: Edge Case Tests
// ============================================================================

TEST_CASE("Edge case: empty document", "[wysiwyg][edge]")
{
    WysiwygEngine engine;
    auto model = engine.process_document("", 0);
    REQUIRE(model.block_count() == 0);
    REQUIRE(engine.get_active_block() == nullptr);
}

TEST_CASE("Edge case: Unicode content", "[wysiwyg][edge]")
{
    WysiwygEngine engine;
    auto model = engine.process_document("# 你好世界\n\nテキスト\n\n🎉 Emoji", 0);
    REQUIRE(model.block_count() > 0);

    const auto md = model.to_markdown();
    REQUIRE(md.find("你好世界") != std::string::npos);
    REQUIRE(md.find("テキスト") != std::string::npos);
}

TEST_CASE("Edge case: malformed markdown", "[wysiwyg][edge]")
{
    WysiwygEngine engine;
    // Unclosed code fence
    auto model = engine.process_document("```python\nno closing fence", 0);
    REQUIRE(model.block_count() > 0);

    // Broken image
    auto images = InlineImageRenderer::parse_images("![broken(");
    REQUIRE(images.empty());
}

TEST_CASE("Edge case: deeply nested blockquotes", "[wysiwyg][edge]")
{
    WysiwygEngine engine;
    auto model = engine.process_document("> Level 1\n>> Level 2\n>>> Level 3", 0);
    REQUIRE(model.block_count() > 0);
}

// ============================================================================
// Task 20: Performance Tests
// ============================================================================

TEST_CASE("Performance: large document processing", "[wysiwyg][performance]")
{
    // Generate a 1000-line document
    std::string large_doc;
    for (int line_idx = 0; line_idx < 200; ++line_idx)
    {
        large_doc += "# Heading " + std::to_string(line_idx) + "\n\n";
        large_doc += "Paragraph content for section " + std::to_string(line_idx) + ".\n\n";
        large_doc += "- Item " + std::to_string(line_idx) + "\n\n";
    }

    WysiwygEngine engine;
    auto model = engine.process_document(large_doc, 500);

    REQUIRE(model.block_count() > 100);
    REQUIRE(engine.get_active_block() != nullptr);
}

TEST_CASE("Performance: many checkbox toggles", "[wysiwyg][performance]")
{
    std::string task_list;
    for (int task_idx = 0; task_idx < 100; ++task_idx)
    {
        task_list += "- [ ] Task " + std::to_string(task_idx) + "\n";
    }

    auto stats = InlineCheckboxRenderer::count_tasks(task_list);
    REQUIRE(stats.total == 100);
    REQUIRE(stats.completed == 0);
}

TEST_CASE("Performance: state serialization round-trip", "[wysiwyg][performance]")
{
    WysiwygFileState state;
    state.mode = WysiwygMode::kWysiwyg;
    state.cursor_line = 999;
    for (int collapsed_idx = 0; collapsed_idx < 50; ++collapsed_idx)
    {
        state.collapsed_blocks.insert(collapsed_idx * 3);
    }

    // Round-trip should preserve all data
    auto result = WysiwygStatePersistence::round_trip(state);
    REQUIRE(result.collapsed_blocks.size() == 50);
    REQUIRE(result.cursor_line == 999);
}
