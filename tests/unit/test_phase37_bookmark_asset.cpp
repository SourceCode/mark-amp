/// @file test_phase37_bookmark_asset.cpp
/// @brief V9 Phase 37 — Comprehensive tests for Bookmark & Asset Management.

#include "core/AssetCommandProvider.h"
#include "core/AssetLinkIntegrity.h"
#include "core/AssetOptimizer.h"
#include "core/AssetTypes.h"
#include "core/Bookmark.h"
#include "core/BookmarkCommandProvider.h"
#include "core/BookmarkExporter.h"
#include "core/BookmarkNavigator.h"
#include "core/CommandRegistry.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace markamp::core;
using namespace markamp::core::events;

// ── Helper: build sample BookmarkEntry  ──────────────────────────────────────

static auto make_entry(const std::string& block,
                       const std::string& label,
                       const std::string& doc = "doc1",
                       const std::string& title = "My Doc",
                       const std::string& block_type = "heading",
                       const std::string& snippet = "Some content") -> BookmarkEntry
{
    BookmarkEntry entry;
    entry.block_id.value = block;
    entry.label = label;
    entry.root_id = doc;
    entry.doc_title = title;
    entry.block_type = block_type;
    entry.content_snippet = snippet;
    return entry;
}

// ============================================================================
// BookmarkNavigator Tests
// ============================================================================

TEST_CASE("BookmarkNavigator — empty state", "[phase37][navigator]")
{
    BookmarkNavigator nav;
    REQUIRE(nav.count() == 0);
    REQUIRE(nav.current() == nullptr);
    REQUIRE(nav.next() == nullptr);
    REQUIRE(nav.previous() == nullptr);
}

TEST_CASE("BookmarkNavigator — basic navigation", "[phase37][navigator]")
{
    BookmarkNavigator nav;
    nav.load({make_entry("b1", "todo"), make_entry("b2", "todo"), make_entry("b3", "important")});

    REQUIRE(nav.count() == 3);
    REQUIRE(nav.current_index() == 0);

    auto* cur = nav.current();
    REQUIRE(cur != nullptr);
    REQUIRE(cur->block_id.value == "b1");

    auto* nxt = nav.next();
    REQUIRE(nxt != nullptr);
    REQUIRE(nxt->block_id.value == "b2");

    nxt = nav.next();
    REQUIRE(nxt->block_id.value == "b3");

    // Wrap around
    nxt = nav.next();
    REQUIRE(nxt->block_id.value == "b1");
}

TEST_CASE("BookmarkNavigator — previous with wrap", "[phase37][navigator]")
{
    BookmarkNavigator nav;
    nav.load({make_entry("b1", "a"), make_entry("b2", "a"), make_entry("b3", "a")});

    auto* prev = nav.previous();
    REQUIRE(prev != nullptr);
    REQUIRE(prev->block_id.value == "b3"); // Wraps to end
}

TEST_CASE("BookmarkNavigator — go_to", "[phase37][navigator]")
{
    BookmarkNavigator nav;
    nav.load({make_entry("b1", "a"), make_entry("b2", "a"), make_entry("b3", "a")});

    auto* result = nav.go_to(2);
    REQUIRE(result != nullptr);
    REQUIRE(result->block_id.value == "b3");

    REQUIRE(nav.go_to(10) == nullptr); // Out of range
}

TEST_CASE("BookmarkNavigator — filter by label", "[phase37][navigator]")
{
    BookmarkNavigator nav;
    nav.load({make_entry("b1", "todo"), make_entry("b2", "important"), make_entry("b3", "todo")});

    nav.filter_by_label("todo");
    REQUIRE(nav.count() == 2);
    REQUIRE(nav.filter_label() == "todo");

    auto* cur = nav.current();
    REQUIRE(cur != nullptr);
    REQUIRE(cur->label == "todo");

    // Clear filter
    nav.filter_by_label("");
    REQUIRE(nav.count() == 3);
}

TEST_CASE("BookmarkNavigator — sort order", "[phase37][navigator]")
{
    BookmarkNavigator nav;
    nav.load({make_entry("b1", "zebra"), make_entry("b2", "alpha"), make_entry("b3", "middle")});

    nav.set_sort_order(BookmarkSortOrder::kLabel);
    REQUIRE(nav.sort_order() == BookmarkSortOrder::kLabel);

    auto* first = nav.current();
    REQUIRE(first != nullptr);
    REQUIRE(first->label == "alpha");
}

TEST_CASE("BookmarkNavigator — visited tracking", "[phase37][navigator]")
{
    BookmarkNavigator nav;
    nav.load({make_entry("b1", "a"), make_entry("b2", "a"), make_entry("b3", "a")});

    REQUIRE(nav.visited_count() == 0);

    nav.mark_visited();
    REQUIRE(nav.visited_count() == 1);
    REQUIRE(nav.is_visited(0));
    REQUIRE_FALSE(nav.is_visited(1));

    nav.next();
    nav.mark_visited();
    REQUIRE(nav.visited_count() == 2);

    auto visited = nav.visited_indices();
    REQUIRE(visited.size() == 2);

    nav.clear_visited();
    REQUIRE(nav.visited_count() == 0);
}

TEST_CASE("BookmarkNavigator — summary", "[phase37][navigator]")
{
    BookmarkNavigator nav;
    nav.load({make_entry("b1", "todo", "doc1"),
              make_entry("b2", "todo", "doc1"),
              make_entry("b3", "important", "doc2")});

    auto summary = nav.summary();
    REQUIRE(summary.total_bookmarks == 3);
    REQUIRE(summary.total_labels == 2);
    REQUIRE(summary.total_documents == 2);
    REQUIRE(summary.per_label_counts.size() == 2);
}

TEST_CASE("BookmarkNavigator — has_next has_previous", "[phase37][navigator]")
{
    BookmarkNavigator nav;
    REQUIRE_FALSE(nav.has_next());
    REQUIRE_FALSE(nav.has_previous());

    nav.load({make_entry("b1", "a")});
    REQUIRE(nav.has_next());
    REQUIRE(nav.has_previous());
}

// ============================================================================
// BookmarkExporter Tests
// ============================================================================

TEST_CASE("BookmarkExporter — format names and extensions", "[phase37][exporter]")
{
    REQUIRE(BookmarkExporter::format_name(BookmarkExportFormat::kMarkdown) == "Markdown");
    REQUIRE(BookmarkExporter::format_name(BookmarkExportFormat::kJson) == "JSON");
    REQUIRE(BookmarkExporter::format_name(BookmarkExportFormat::kCsv) == "CSV");
    REQUIRE(BookmarkExporter::format_name(BookmarkExportFormat::kOpml) == "OPML");

    REQUIRE(BookmarkExporter::format_extension(BookmarkExportFormat::kMarkdown) == "md");
    REQUIRE(BookmarkExporter::format_extension(BookmarkExportFormat::kJson) == "json");
    REQUIRE(BookmarkExporter::format_extension(BookmarkExportFormat::kCsv) == "csv");
    REQUIRE(BookmarkExporter::format_extension(BookmarkExportFormat::kOpml) == "opml");
}

TEST_CASE("BookmarkExporter — export markdown", "[phase37][exporter]")
{
    BookmarkExporter exporter;
    BookmarkGroup group;
    group.label = "todo";
    group.entries = {make_entry("b1", "todo", "doc1", "My Doc")};

    auto md = exporter.export_bookmarks({group}, BookmarkExportFormat::kMarkdown);
    REQUIRE_THAT(md, Catch::Matchers::ContainsSubstring("# Bookmarks"));
    REQUIRE_THAT(md, Catch::Matchers::ContainsSubstring("## todo"));
    REQUIRE_THAT(md, Catch::Matchers::ContainsSubstring("My Doc"));
}

TEST_CASE("BookmarkExporter — export JSON round-trip", "[phase37][exporter]")
{
    BookmarkExporter exporter;
    BookmarkGroup group;
    group.label = "research";
    group.entries = {make_entry("b1", "research", "doc1", "Paper Notes", "heading", "Abstract"),
                     make_entry("b2", "research", "doc2", "References", "paragraph", "Source A")};

    auto json = exporter.export_bookmarks({group}, BookmarkExportFormat::kJson);
    REQUIRE_THAT(json, Catch::Matchers::ContainsSubstring("\"bookmarks\""));

    // Import back
    auto result = exporter.import_bookmarks(json, BookmarkExportFormat::kJson);
    REQUIRE(result.error.empty());
    REQUIRE(result.imported_count == 2);
    REQUIRE(result.entries.size() == 2);
    REQUIRE(result.entries[0].label == "research");
}

TEST_CASE("BookmarkExporter — export CSV round-trip", "[phase37][exporter]")
{
    BookmarkExporter exporter;
    BookmarkGroup group;
    group.label = "todo";
    group.entries = {make_entry("b1", "todo", "doc1", "Title", "heading", "Content")};

    auto csv = exporter.export_bookmarks({group}, BookmarkExportFormat::kCsv);
    REQUIRE_THAT(csv, Catch::Matchers::ContainsSubstring("label,block_id"));

    auto result = exporter.import_bookmarks(csv, BookmarkExportFormat::kCsv);
    REQUIRE(result.error.empty());
    REQUIRE(result.imported_count == 1);
    REQUIRE(result.entries[0].label == "todo");
}

TEST_CASE("BookmarkExporter — export OPML", "[phase37][exporter]")
{
    BookmarkExporter exporter;
    BookmarkGroup group;
    group.label = "project";
    group.entries = {make_entry("b1", "project", "doc1", "Ideas")};

    auto opml = exporter.export_bookmarks({group}, BookmarkExportFormat::kOpml);
    REQUIRE_THAT(opml, Catch::Matchers::ContainsSubstring("<opml"));
    REQUIRE_THAT(opml, Catch::Matchers::ContainsSubstring("project"));
    REQUIRE_THAT(opml, Catch::Matchers::ContainsSubstring("Ideas"));
}

TEST_CASE("BookmarkExporter — export single label", "[phase37][exporter]")
{
    BookmarkExporter exporter;
    BookmarkGroup group;
    group.label = "single";
    group.entries = {make_entry("b1", "single")};

    auto md = exporter.export_label(group, BookmarkExportFormat::kMarkdown);
    REQUIRE_THAT(md, Catch::Matchers::ContainsSubstring("## single"));
}

TEST_CASE("BookmarkExporter — import unsupported format", "[phase37][exporter]")
{
    BookmarkExporter exporter;
    auto result = exporter.import_bookmarks("data", BookmarkExportFormat::kMarkdown);
    REQUIRE_FALSE(result.error.empty());
}

// ============================================================================
// AssetOptimizer Tests
// ============================================================================

TEST_CASE("AssetOptimizer — is_optimizable", "[phase37][optimizer]")
{
    AssetInfo png;
    png.category = AssetCategory::Image;
    png.mime_type = "image/png";
    REQUIRE(AssetOptimizer::is_optimizable(png));

    AssetInfo doc;
    doc.category = AssetCategory::Document;
    doc.mime_type = "application/pdf";
    REQUIRE_FALSE(AssetOptimizer::is_optimizable(doc));

    AssetInfo svg;
    svg.category = AssetCategory::Image;
    svg.mime_type = "image/svg+xml";
    REQUIRE_FALSE(AssetOptimizer::is_optimizable(svg));
}

TEST_CASE("AssetOptimizer — optimize image", "[phase37][optimizer]")
{
    AssetOptimizer optimizer;
    optimizer.set_max_dimension(2000);
    REQUIRE(optimizer.max_dimension() == 2000);

    AssetInfo large_image;
    large_image.asset_id = "img1";
    large_image.category = AssetCategory::Image;
    large_image.mime_type = "image/jpeg";
    large_image.file_size = 5000000; // 5MB
    large_image.width = 6000;
    large_image.height = 4000;

    auto result = optimizer.optimize_image(large_image);
    REQUIRE(result.asset_id == "img1");
    REQUIRE(result.original_size == 5000000);
    REQUIRE(result.optimized_size < result.original_size);
    REQUIRE(result.savings_percent > 0.0);
}

TEST_CASE("AssetOptimizer — batch optimize", "[phase37][optimizer]")
{
    AssetOptimizer optimizer;

    AssetInfo img1;
    img1.asset_id = "a1";
    img1.category = AssetCategory::Image;
    img1.mime_type = "image/png";
    img1.file_size = 1000000;
    img1.width = 5000;
    img1.height = 5000;

    AssetInfo img2;
    img2.asset_id = "a2";
    img2.category = AssetCategory::Image;
    img2.mime_type = "image/jpeg";
    img2.file_size = 2000000;
    img2.width = 8000;
    img2.height = 6000;

    auto results = optimizer.batch_optimize({img1, img2});
    REQUIRE(results.size() == 2);
    REQUIRE(results[0].asset_id == "a1");
    REQUIRE(results[1].asset_id == "a2");
}

TEST_CASE("AssetOptimizer — estimate savings (dry run)", "[phase37][optimizer]")
{
    AssetOptimizer optimizer;

    AssetInfo image;
    image.asset_id = "img_est";
    image.category = AssetCategory::Image;
    image.mime_type = "image/png";
    image.file_size = 3000000;
    image.width = 5000;
    image.height = 3000;

    auto result = optimizer.estimate_savings(image);
    REQUIRE(result.asset_id == "img_est");
    REQUIRE_FALSE(result.optimized); // Dry run
    REQUIRE(result.message == "Estimate only");
}

TEST_CASE("AssetOptimizer — generate thumbnail", "[phase37][optimizer]")
{
    AssetOptimizer optimizer;

    AssetInfo image;
    image.asset_id = "avatar";
    image.category = AssetCategory::Image;
    image.mime_type = "image/png";

    auto thumb = optimizer.generate_thumbnail(image, 256);
    REQUIRE_THAT(thumb, Catch::Matchers::ContainsSubstring("avatar"));
    REQUIRE_THAT(thumb, Catch::Matchers::ContainsSubstring("256"));

    AssetInfo doc;
    doc.category = AssetCategory::Document;
    REQUIRE(optimizer.generate_thumbnail(doc, 256).empty());
}

TEST_CASE("AssetOptimizer — storage report", "[phase37][optimizer]")
{
    AssetOptimizer optimizer;

    AssetInfo img;
    img.asset_id = "a1";
    img.category = AssetCategory::Image;
    img.mime_type = "image/png";
    img.file_size = 1000000;
    img.width = 5000;
    img.height = 5000;
    img.reference_count = 1;

    AssetInfo orphan;
    orphan.asset_id = "a2";
    orphan.category = AssetCategory::Document;
    orphan.mime_type = "application/pdf";
    orphan.file_size = 500000;
    orphan.reference_count = 0;

    auto report = optimizer.generate_report({img, orphan});
    REQUIRE(report.total_assets == 2);
    REQUIRE(report.used_bytes == 1500000);
    REQUIRE(report.orphan_assets == 1);
    REQUIRE(report.orphan_bytes == 500000);
    REQUIRE(report.optimizable_assets == 1); // Only the image
}

// ============================================================================
// AssetLinkIntegrity Tests
// ============================================================================

TEST_CASE("AssetLinkIntegrity — extract asset links", "[phase37][links]")
{
    std::string content = R"(
# My Document
Some text here.
![Screenshot](screenshot.png)
More text.
![Diagram](./images/diagram.svg)
![External](https://example.com/img.png)
)";

    auto links = AssetLinkIntegrity::extract_asset_links(content);
    REQUIRE(links.size() == 2); // External URL excluded
    REQUIRE(links[0].second == "screenshot.png");
    REQUIRE(links[1].second == "./images/diagram.svg");
}

TEST_CASE("AssetLinkIntegrity — filename similarity", "[phase37][links]")
{
    REQUIRE(AssetLinkIntegrity::filename_similarity("photo.png", "photo.png") == 1.0);
    REQUIRE(AssetLinkIntegrity::filename_similarity("path/photo.png", "photo.png") == 0.9);
    REQUIRE(AssetLinkIntegrity::filename_similarity("Photo.PNG", "photo.png") == 0.85);
    REQUIRE(AssetLinkIntegrity::filename_similarity("", "photo.png") == 0.0);
    REQUIRE(AssetLinkIntegrity::filename_similarity("abc", "xyz") < 0.5);
}

TEST_CASE("AssetLinkIntegrity — scan document for broken links", "[phase37][links]")
{
    AssetLinkIntegrity checker;

    std::string content = R"(# Test
![Found](image.png)
![Missing](deleted.png)
)";

    AssetInfo available;
    available.original_name = "image.png";

    auto broken = checker.scan_document("doc1", content, {available});
    REQUIRE(broken.size() == 1);
    REQUIRE(broken[0].expected_path == "deleted.png");
    REQUIRE(broken[0].document_id == "doc1");
}

TEST_CASE("AssetLinkIntegrity — scan all documents", "[phase37][links]")
{
    AssetLinkIntegrity checker;

    std::vector<std::pair<std::string, std::string>> docs = {
        {"doc1", "![img](found.png)\n![img](missing.png)"},
        {"doc2", "![img](also_missing.png)"},
    };

    AssetInfo available;
    available.original_name = "found.png";

    auto report = checker.scan_all(docs, {available});
    REQUIRE(report.total_links == 3);
    REQUIRE(report.broken == 2);
}

TEST_CASE("AssetLinkIntegrity — suggest repair", "[phase37][links]")
{
    AssetLinkIntegrity checker;

    BrokenAssetLink broken;
    broken.expected_path = "screensht.png"; // Typo

    AssetInfo available;
    available.original_name = "screenshot.png";

    auto suggestion = checker.suggest_repair(broken, {available});
    REQUIRE(suggestion == "screenshot.png");
}

TEST_CASE("AssetLinkIntegrity — auto repair", "[phase37][links]")
{
    AssetLinkIntegrity checker;

    BrokenAssetLink broken;
    broken.expected_path = "photo.png";
    broken.suggestion = "photo.png";
    broken.confidence = 0.95;

    std::vector<BrokenAssetLink> broken_links = {broken};

    AssetInfo available;
    available.original_name = "photo.png";

    int repaired = checker.auto_repair(broken_links, {available}, 0.8);
    REQUIRE(repaired == 1);
}

// ============================================================================
// BookmarkCommandProvider Tests
// ============================================================================

TEST_CASE("BookmarkCommandProvider — provides 8 commands", "[phase37][commands]")
{
    REQUIRE(BookmarkCommandProvider::command_count() == 8);

    auto ids = BookmarkCommandProvider::command_ids();
    REQUIRE(ids.size() == 8);
    REQUIRE(ids[0] == "bookmark.toggle");
    REQUIRE(ids[1] == "bookmark.next");
    REQUIRE(ids[2] == "bookmark.previous");
}

TEST_CASE("BookmarkCommandProvider — register in registry", "[phase37][commands]")
{
    CommandRegistry registry;
    BookmarkCommandProvider provider;
    provider.register_commands(registry);

    REQUIRE(registry.has_command("bookmark.toggle"));
    REQUIRE(registry.has_command("bookmark.next"));
    REQUIRE(registry.has_command("bookmark.goTo"));

    auto* cmd = registry.get_command("bookmark.toggle");
    REQUIRE(cmd != nullptr);
    REQUIRE(cmd->category == "Bookmark");
    REQUIRE(cmd->shortcut == "Cmd+Shift+B");
}

TEST_CASE("BookmarkCommandProvider — get specific command", "[phase37][commands]")
{
    BookmarkCommandProvider provider;
    auto cmd = provider.get_command("bookmark.export");
    REQUIRE(cmd.id == "bookmark.export");
    REQUIRE(cmd.title == "Export Bookmarks");
    REQUIRE(cmd.icon == "export");
}

// ============================================================================
// AssetCommandProvider Tests
// ============================================================================

TEST_CASE("AssetCommandProvider — provides 8 commands", "[phase37][commands]")
{
    REQUIRE(AssetCommandProvider::command_count() == 8);

    auto ids = AssetCommandProvider::command_ids();
    REQUIRE(ids.size() == 8);
    REQUIRE(ids[0] == "asset.import");
    REQUIRE(ids[7] == "asset.checkLinks");
}

TEST_CASE("AssetCommandProvider — register in registry", "[phase37][commands]")
{
    CommandRegistry registry;
    AssetCommandProvider provider;
    provider.register_commands(registry);

    REQUIRE(registry.has_command("asset.import"));
    REQUIRE(registry.has_command("asset.optimize"));
    REQUIRE(registry.has_command("asset.checkLinks"));

    auto* cmd = registry.get_command("asset.import");
    REQUIRE(cmd != nullptr);
    REQUIRE(cmd->category == "Asset");
    REQUIRE(cmd->shortcut == "Cmd+Shift+I");
}

// ============================================================================
// Events Tests
// ============================================================================

TEST_CASE("Phase 37 — BookmarkExportedEvent", "[phase37][events]")
{
    BookmarkExportedEvent event;
    event.format = "json";
    event.bookmark_count = 15;
    REQUIRE(event.format == "json");
    REQUIRE(event.bookmark_count == 15);
}

TEST_CASE("Phase 37 — BookmarkNavigatedEvent", "[phase37][events]")
{
    BookmarkNavigatedEvent event;
    event.block_id = "block-42";
    event.label = "research";
    event.direction = "next";
    REQUIRE(event.direction == "next");
}

TEST_CASE("Phase 37 — AssetOptimizedEvent", "[phase37][events]")
{
    AssetOptimizedEvent event;
    event.asset_id = "img1";
    event.original_size = 5000000;
    event.optimized_size = 3000000;
    event.savings_percent = 40.0;
    REQUIRE(event.savings_percent == 40.0);
}

TEST_CASE("Phase 37 — AssetLinkBrokenEvent", "[phase37][events]")
{
    AssetLinkBrokenEvent event;
    event.document_id = "doc1";
    event.expected_path = "missing.png";
    event.suggestion = "found.png";
    REQUIRE(event.suggestion == "found.png");
}

TEST_CASE("Phase 37 — AssetLinkRepairedEvent", "[phase37][events]")
{
    AssetLinkRepairedEvent event;
    event.document_id = "doc1";
    event.old_path = "old.png";
    event.new_path = "new.png";
    REQUIRE(event.new_path == "new.png");
}

TEST_CASE("Phase 37 — AssetStorageReportEvent", "[phase37][events]")
{
    AssetStorageReportEvent event;
    event.used_bytes = 10000000;
    event.total_assets = 50;
    event.orphans = 3;
    event.savings_estimate = 12.5;
    REQUIRE(event.total_assets == 50);
    REQUIRE(event.savings_estimate == 12.5);
}

// ============================================================================
// Integration Test
// ============================================================================

TEST_CASE("Phase 37 — combined bookmark + asset command providers", "[phase37][integration]")
{
    CommandRegistry registry;
    BookmarkCommandProvider bookmark_provider;
    AssetCommandProvider asset_provider;

    bookmark_provider.register_commands(registry);
    asset_provider.register_commands(registry);

    // Total: 8 + 8 = 16 commands
    REQUIRE(registry.command_count() == 16);

    // Bookmark commands
    auto bookmark_cmds = registry.commands_for_category("Bookmark");
    REQUIRE(bookmark_cmds.size() == 8);

    // Asset commands
    auto asset_cmds = registry.commands_for_category("Asset");
    REQUIRE(asset_cmds.size() == 8);

    // Search should find both
    auto results = registry.search("Bookmark");
    REQUIRE_FALSE(results.empty());
}
