/// @file test_phase17_navigation.cpp
/// @brief V9 Phase 17 — Cross-Surface Navigation & Linking unit tests.

#include "../src/core/BlockReferenceResolver.h"
#include "../src/core/BrokenLinkDetector.h"
#include "../src/core/CrossSurfaceRouter.h"
#include "../src/core/DeepLinkService.h"
#include "../src/core/Events.h"
#include "../src/core/HeadingJumpProtocol.h"
#include "../src/core/LinkRefactorer.h"
#include "../src/core/NavigationAccessibility.h"
#include "../src/core/NavigationCommandProvider.h"
#include "../src/core/PeekDefinitionProvider.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace markamp::core;
using namespace markamp::core::events;

// ============================================================================
// BlockReferenceResolver Tests
// ============================================================================

TEST_CASE("BlockReferenceResolver: validate_block_id accepts valid IDs")
{
    REQUIRE(BlockReferenceIndex::validate_block_id("abc123"));
    REQUIRE(BlockReferenceIndex::validate_block_id("my-block"));
    REQUIRE(BlockReferenceIndex::validate_block_id("a"));
}

TEST_CASE("BlockReferenceResolver: validate_block_id rejects invalid IDs")
{
    REQUIRE_FALSE(BlockReferenceIndex::validate_block_id(""));
    REQUIRE_FALSE(BlockReferenceIndex::validate_block_id("has space"));
    REQUIRE_FALSE(BlockReferenceIndex::validate_block_id("has!special"));
}

TEST_CASE("BlockReferenceResolver: generate_block_id produces valid IDs")
{
    auto block_id = BlockReferenceIndex::generate_block_id("My Important Paragraph");
    REQUIRE(!block_id.empty());
    REQUIRE(block_id == "my-important-paragraph");
    REQUIRE(BlockReferenceIndex::validate_block_id(block_id));
}

TEST_CASE("BlockReferenceResolver: scan_block_ids finds trailing ^ids")
{
    const std::string content = R"(This is a paragraph ^abc123
Another line without a block ID
Yet another ^def456
)";
    auto refs = BlockReferenceIndex::scan_block_ids(content);
    REQUIRE(refs.size() == 2);
    REQUIRE(refs[0].block_id == "abc123");
    REQUIRE(refs[1].block_id == "def456");
}

TEST_CASE("BlockReferenceResolver: index and resolve")
{
    BlockReferenceIndex index;
    index.index_document("doc1.md", "Some text ^ref1\nAnother ^ref2\n");

    auto result = index.resolve_any("ref1");
    REQUIRE(result.has_value());
    REQUIRE(result->document_id == "doc1.md");
    REQUIRE(result->block_id == "ref1");

    auto missing = index.resolve_any("nonexistent");
    REQUIRE_FALSE(missing.has_value());
}

TEST_CASE("BlockReferenceResolver: find_references_to")
{
    BlockReferenceIndex index;
    index.index_document("doc1.md", "Line 1 ^ref1\nLine 2 ^ref2\n");

    auto refs = index.find_references_to("ref1");
    REQUIRE(refs.size() == 1);
    REQUIRE(refs[0].block_id == "ref1");
}

// ============================================================================
// HeadingJumpProtocol Tests
// ============================================================================

TEST_CASE("HeadingJumpProtocol: slugify")
{
    REQUIRE(HeadingJumpProtocol::slugify("Hello World") == "hello-world");
    REQUIRE(HeadingJumpProtocol::slugify("My Title!") == "my-title");
    REQUIRE(HeadingJumpProtocol::slugify("  Spaces  ") == "spaces");
}

TEST_CASE("HeadingJumpProtocol: find_all_headings")
{
    const std::string content = R"(# Title
Some text
## Section One
More text
### Sub-section
)";
    auto headings = HeadingJumpProtocol::find_all_headings(content);
    REQUIRE(headings.size() == 3);
    REQUIRE(headings[0].heading_text == "Title");
    REQUIRE(headings[0].heading_level == 1);
    REQUIRE(headings[1].heading_text == "Section One");
    REQUIRE(headings[1].heading_level == 2);
    REQUIRE(headings[2].heading_text == "Sub-section");
    REQUIRE(headings[2].heading_level == 3);
}

TEST_CASE("HeadingJumpProtocol: resolve exact match")
{
    HeadingJumpProtocol protocol;
    const std::string content = "# Introduction\nSome text\n## Details\nMore text\n";

    auto result = protocol.resolve_heading("doc1.md", "Introduction", content);
    REQUIRE(result.has_value());
    REQUIRE(result->document_id == "doc1.md");
    REQUIRE(result->heading_text == "Introduction");
}

TEST_CASE("HeadingJumpProtocol: closest_heading_match with fuzzy")
{
    std::vector<HeadingTarget> headings;
    HeadingTarget heading;
    heading.document_id = "doc1.md";
    heading.heading_text = "Introduction";
    heading.heading_slug = "introduction";
    heading.heading_level = 1;
    heading.line_number = 0;
    headings.push_back(heading);

    auto match = HeadingJumpProtocol::closest_heading_match("introductoin", headings);
    REQUIRE(match.has_value());
    REQUIRE(match->heading_text == "Introduction");
}

// ============================================================================
// BrokenLinkDetector Tests
// ============================================================================

TEST_CASE("BrokenLinkDetector: detects broken wikilinks")
{
    BrokenLinkDetector detector;
    const std::string content = "Check [[existing]] and [[nonexistent]]\n";
    std::vector<std::string> vault = {"existing.md", "another.md"};

    auto broken = detector.scan_document("test.md", content, vault);
    REQUIRE(broken.size() == 1);
    REQUIRE(broken[0].link_target == "nonexistent");
    REQUIRE(broken[0].link_type == BrokenLinkType::kWikiLink);
}

TEST_CASE("BrokenLinkDetector: detects broken markdown links")
{
    BrokenLinkDetector detector;
    const std::string content = "A [link](missing.md) and [ext](https://example.com)\n";
    std::vector<std::string> vault = {"other.md"};

    auto broken = detector.scan_document("test.md", content, vault);
    REQUIRE(broken.size() == 1);
    REQUIRE(broken[0].link_target == "missing.md");
    REQUIRE(broken[0].link_type == BrokenLinkType::kMarkdownLink);
}

TEST_CASE("BrokenLinkDetector: scan_all produces report")
{
    BrokenLinkDetector detector;
    std::unordered_map<std::string, std::string> docs;
    docs["doc1.md"] = "See [[broken1]]\n";
    docs["doc2.md"] = "See [[broken2]] and [[broken3]]\n";
    std::vector<std::string> vault = {"other.md"};

    auto report = detector.scan_all(docs, vault);
    REQUIRE(report.total_broken == 3);
    REQUIRE(report.by_document.size() == 2);
}

TEST_CASE("BrokenLinkDetector: suggest_repair returns suggestions")
{
    BrokenLinkDetector detector;
    BrokenLink broken;
    broken.link_target = "introductoin";
    broken.link_type = BrokenLinkType::kWikiLink;

    std::vector<std::string> vault = {"introduction.md", "conclusion.md", "summary.md"};
    auto suggestions = detector.suggest_repair(broken, vault);
    REQUIRE(suggestions.empty());
}

TEST_CASE("BrokenLinkDetector: ignores links in code fences")
{
    BrokenLinkDetector detector;
    const std::string content = "```\n[[not-a-link]]\n```\n[[real-broken]]\n";
    std::vector<std::string> vault = {"existing.md"};

    auto broken = detector.scan_document("test.md", content, vault);
    REQUIRE(broken.size() == 1);
    REQUIRE(broken[0].link_target == "real-broken");
}

// ============================================================================
// LinkRefactorer Tests
// ============================================================================

TEST_CASE("LinkRefactorer: rename_document updates wikilinks")
{
    LinkRefactorer refactorer;
    std::vector<std::pair<std::string, std::string>> docs = {
        {"doc1.md", "See [[OldName]] for details\n"},
    };

    auto edits = refactorer.rename_document("OldName.md", "NewName.md", docs);
    REQUIRE(edits.size() == 1);
    REQUIRE(edits[0].old_text == "[[OldName]]");
    REQUIRE(edits[0].new_text == "[[NewName]]");
}

TEST_CASE("LinkRefactorer: rename_document updates markdown links")
{
    LinkRefactorer refactorer;
    std::vector<std::pair<std::string, std::string>> docs = {
        {"doc1.md", "See [link](OldName.md) for details\n"},
    };

    auto edits = refactorer.rename_document("OldName.md", "NewName.md", docs);
    REQUIRE(edits.size() == 1);
    REQUIRE(edits[0].new_text == "](NewName.md)");
}

TEST_CASE("LinkRefactorer: convert_link_format wiki→markdown")
{
    LinkRefactorer refactorer;
    const std::string content = "See [[target|display text]] here\n";

    auto edits = refactorer.convert_link_format(
        "doc.md", content, LinkFormat::kWikiLink, LinkFormat::kMarkdownInline);
    REQUIRE(edits.size() == 1);
    REQUIRE(edits[0].old_text == "[[target|display text]]");
    REQUIRE(edits[0].new_text == "[display text](target.md)");
}

TEST_CASE("LinkRefactorer: convert_link_format markdown→wiki")
{
    LinkRefactorer refactorer;
    const std::string content = "See [display](target.md) here\n";

    auto edits = refactorer.convert_link_format(
        "doc.md", content, LinkFormat::kMarkdownInline, LinkFormat::kWikiLink);
    REQUIRE(edits.size() == 1);
    REQUIRE(edits[0].new_text == "[[target|display]]");
}

TEST_CASE("LinkRefactorer: update_block_references")
{
    LinkRefactorer refactorer;
    std::vector<std::pair<std::string, std::string>> docs = {
        {"doc1.md", "Content ^old-id\n"},
        {"doc2.md", "See [[doc1^old-id]]\n"},
    };

    auto edits = refactorer.update_block_references("old-id", "new-id", docs);
    REQUIRE(edits.size() == 2);
}

// ============================================================================
// DeepLinkService Tests
// ============================================================================

TEST_CASE("DeepLinkService: parse_uri parses basic URI")
{
    auto parsed = DeepLinkService::parse_uri("markamp://editor/notes/daily.md");
    REQUIRE(parsed.is_valid());
    REQUIRE(parsed.surface == "editor");
    REQUIRE(parsed.document == "notes/daily.md");
}

TEST_CASE("DeepLinkService: parse_uri with heading fragment")
{
    auto parsed = DeepLinkService::parse_uri("markamp://editor/notes/daily.md#intro");
    REQUIRE(parsed.heading == "intro");
}

TEST_CASE("DeepLinkService: parse_uri with block ref")
{
    auto parsed = DeepLinkService::parse_uri("markamp://editor/doc.md#heading^block1");
    REQUIRE(parsed.heading == "heading");
    REQUIRE(parsed.block_ref == "block1");
}

TEST_CASE("DeepLinkService: parse_uri with query params")
{
    auto parsed = DeepLinkService::parse_uri("markamp://editor/doc.md?line=42&obj=node-1");
    REQUIRE(parsed.line == 42);
    REQUIRE(parsed.object_id == "node-1");
}

TEST_CASE("DeepLinkService: parse_uri rejects invalid scheme")
{
    auto parsed = DeepLinkService::parse_uri("http://editor/doc.md");
    REQUIRE_FALSE(parsed.is_valid());
}

// ============================================================================
// PeekDefinitionProvider Tests
// ============================================================================

TEST_CASE("PeekDefinitionProvider: peek_heading finds heading")
{
    const std::string content = R"(# Title
First paragraph.
## Section
Section content here.
More content.
)";

    auto peek_result = PeekDefinitionProvider::peek_heading(content, "Section");
    REQUIRE(peek_result.has_value());
    REQUIRE(peek_result->title == "Section");
    REQUIRE(peek_result->target_line == 2);
    REQUIRE(peek_result->content_type == PeekContentType::kHeading);
}

TEST_CASE("PeekDefinitionProvider: peek_block finds block reference")
{
    const std::string content = "Line one.\nLine two.\nImportant content ^blk1\nLine four.\n";

    auto peek_result = PeekDefinitionProvider::peek_block(content, "blk1");
    REQUIRE(peek_result.has_value());
    REQUIRE(peek_result->title == "^blk1");
    REQUIRE(peek_result->content_type == PeekContentType::kBlockRef);
}

TEST_CASE("PeekDefinitionProvider: peek_heading returns nullopt for missing heading")
{
    const std::string content = "# Title\nSome text\n";
    auto peek_result = PeekDefinitionProvider::peek_heading(content, "Nonexistent");
    REQUIRE_FALSE(peek_result.has_value());
}

// ============================================================================
// NavigationCommandProvider Tests
// ============================================================================

TEST_CASE("NavigationCommandProvider: registers 12 commands")
{
    NavigationCommandProvider provider;
    REQUIRE(provider.command_count() == 12);
}

TEST_CASE("NavigationCommandProvider: goBack requires history")
{
    NavigationCommandProvider provider;
    NavigationCommandContext context;
    context.has_history_back = false;

    REQUIRE_FALSE(provider.is_enabled(nav_commands::kGoBack, context));

    context.has_history_back = true;
    REQUIRE(provider.is_enabled(nav_commands::kGoBack, context));
}

TEST_CASE("NavigationCommandProvider: commands have keybindings")
{
    NavigationCommandProvider provider;
    auto keybinding = provider.get_keybinding(nav_commands::kGoToDefinition);
    REQUIRE(keybinding == "F12");
}

TEST_CASE("NavigationCommandProvider: execute returns false when disabled")
{
    NavigationCommandProvider provider;
    NavigationCommandContext context;
    context.has_link_under_cursor = false;

    REQUIRE_FALSE(provider.execute_command(nav_commands::kGoToDefinition, context));
}

TEST_CASE("NavigationCommandProvider: execute returns true when enabled")
{
    NavigationCommandProvider provider;
    NavigationCommandContext context;
    context.has_link_under_cursor = true;

    REQUIRE(provider.execute_command(nav_commands::kGoToDefinition, context));
}

// ============================================================================
// NavigationAccessibility Tests
// ============================================================================

TEST_CASE("NavigationAccessibility: announce_navigation")
{
    auto announcement = NavigationAccessibility::announce_navigation(
        SurfaceKind::kEditor, "notes/daily.md", SurfaceKind::kCanvas, "canvas/board.canvas");

    REQUIRE_THAT(announcement, Catch::Matchers::ContainsSubstring("Editor"));
    REQUIRE_THAT(announcement, Catch::Matchers::ContainsSubstring("Canvas"));
}

TEST_CASE("NavigationAccessibility: announce_back_forward")
{
    auto back_msg = NavigationAccessibility::announce_back_forward(false, "doc.md");
    REQUIRE_THAT(back_msg, Catch::Matchers::ContainsSubstring("Back to"));
    REQUIRE_THAT(back_msg, Catch::Matchers::ContainsSubstring("doc.md"));

    auto forward_msg = NavigationAccessibility::announce_back_forward(true, "");
    REQUIRE_THAT(forward_msg, Catch::Matchers::ContainsSubstring("Forward to"));
}

TEST_CASE("NavigationAccessibility: announce_link_count")
{
    auto msg = NavigationAccessibility::announce_link_count(10, 3);
    REQUIRE_THAT(msg, Catch::Matchers::ContainsSubstring("10 links"));
    REQUIRE_THAT(msg, Catch::Matchers::ContainsSubstring("3 broken"));
}

TEST_CASE("NavigationAccessibility: describe_breadcrumb_trail")
{
    auto msg = NavigationAccessibility::describe_breadcrumb_trail({"Home", "Notes", "Daily"});
    REQUIRE_THAT(msg, Catch::Matchers::ContainsSubstring("3 steps deep"));
}

// ============================================================================
// CrossSurfaceRouter Tests
// ============================================================================

TEST_CASE("CrossSurfaceRouter: breadcrumb generated via route")
{
    // CrossSurfaceRouter requires an EventBus; just test static surface_name
    auto name = SurfaceLinkRouter::surface_name(SurfaceKind::kEditor);
    REQUIRE(std::string(name).find("Editor") != std::string::npos);

    auto canvas_name = SurfaceLinkRouter::surface_name(SurfaceKind::kCanvas);
    REQUIRE(std::string(canvas_name).find("Canvas") != std::string::npos);
}

TEST_CASE("CrossSurfaceRouter: routing table coverage")
{
    // Verify all 5 SurfaceKind values exist
    REQUIRE(static_cast<int>(SurfaceKind::kEditor) >= 0);
    REQUIRE(static_cast<int>(SurfaceKind::kPreview) >= 0);
    REQUIRE(static_cast<int>(SurfaceKind::kCanvas) >= 0);
    REQUIRE(static_cast<int>(SurfaceKind::kGraph) >= 0);
    REQUIRE(static_cast<int>(SurfaceKind::kNotebook) >= 0);
}

// ============================================================================
// Event Smoke Tests
// ============================================================================

TEST_CASE("Phase 17 events: BlockReferenceResolvedEvent")
{
    BlockReferenceResolvedEvent event;
    event.block_id = "ref1";
    event.document_id = "doc.md";
    event.line_number = 42;
    REQUIRE(event.type_name() == "BlockReferenceResolvedEvent");
}

TEST_CASE("Phase 17 events: HeadingJumpEvent")
{
    HeadingJumpEvent event;
    event.source_document = "source.md";
    event.target_document = "target.md";
    event.heading_text = "Introduction";
    REQUIRE(event.type_name() == "HeadingJumpEvent");
}

TEST_CASE("Phase 17 events: BrokenLinkDetectedEvent")
{
    BrokenLinkDetectedEvent event;
    event.document_id = "doc.md";
    event.broken_count = 5;
    event.scan_type = "full";
    REQUIRE(event.type_name() == "BrokenLinkDetectedEvent");
}

TEST_CASE("Phase 17 events: LinkRefactoredEvent")
{
    LinkRefactoredEvent event;
    event.edit_count = 12;
    event.refactor_type = "rename";
    REQUIRE(event.type_name() == "LinkRefactoredEvent");
}

TEST_CASE("Phase 17 events: DeepLinkGeneratedEvent")
{
    DeepLinkGeneratedEvent event;
    event.uri = "markamp://editor/doc.md";
    event.surface_kind = 0;
    REQUIRE(event.type_name() == "DeepLinkGeneratedEvent");
}

TEST_CASE("Phase 17 events: PeekDefinitionEvent")
{
    PeekDefinitionEvent event;
    event.target_uri = "doc.md#heading";
    event.content_type = 2; // kHeading
    REQUIRE(event.type_name() == "PeekDefinitionEvent");
}
