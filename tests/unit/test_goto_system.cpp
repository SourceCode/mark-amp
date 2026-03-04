/// @file test_goto_system.cpp
/// @brief V13 Phase 32 — Unit tests for Go-To System components.

#include "core/BacklinkFinder.h"
#include "core/ChangeTracker.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/FuzzyScorer.h"
#include "core/ISymbolProvider.h"
#include "core/LinkResolver.h"
#include "core/NavigationService.h"
#include "core/WorkspaceSymbolIndex.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>

using namespace markamp::core;

// ============================================================================
// NavigationService Tests
// ============================================================================

TEST_CASE("NavigationService - initial state", "[phase32][nav]")
{
    EventBus bus;
    NavigationService nav(bus);

    REQUIRE(nav.stack_size() == 0);
    REQUIRE(nav.current_index() == -1);
    REQUIRE(nav.current_location() == nullptr);
    REQUIRE_FALSE(nav.can_go_back());
    REQUIRE_FALSE(nav.can_go_forward());
}

TEST_CASE("NavigationService - go_to pushes entry", "[phase32][nav]")
{
    EventBus bus;
    NavigationService nav(bus);

    nav.go_to("file.md", 10, 0);
    REQUIRE(nav.stack_size() == 1);
    REQUIRE(nav.current_index() == 0);

    const auto* loc = nav.current_location();
    REQUIRE(loc != nullptr);
    REQUIRE(loc->document_id == "file.md");
    REQUIRE(loc->line == 10);
}

TEST_CASE("NavigationService - back and forward", "[phase32][nav]")
{
    EventBus bus;
    NavigationService nav(bus);

    nav.go_to("a.md", 1);
    nav.go_to("b.md", 5);
    nav.go_to("c.md", 10);

    REQUIRE(nav.stack_size() == 3);
    REQUIRE(nav.can_go_back());
    REQUIRE_FALSE(nav.can_go_forward());

    // Go back to b.md
    REQUIRE(nav.go_back());
    REQUIRE(nav.current_location()->document_id == "b.md");
    REQUIRE(nav.can_go_forward());

    // Go back to a.md
    REQUIRE(nav.go_back());
    REQUIRE(nav.current_location()->document_id == "a.md");
    REQUIRE_FALSE(nav.can_go_back());

    // Go forward to b.md
    REQUIRE(nav.go_forward());
    REQUIRE(nav.current_location()->document_id == "b.md");
}

TEST_CASE("NavigationService - go_to truncates forward history", "[phase32][nav]")
{
    EventBus bus;
    NavigationService nav(bus);

    nav.go_to("a.md", 1);
    nav.go_to("b.md", 5);
    nav.go_to("c.md", 10);

    nav.go_back();         // at b.md
    nav.go_to("d.md", 20); // c.md is now dropped

    REQUIRE(nav.stack_size() == 3); // a, b, d
    REQUIRE_FALSE(nav.can_go_forward());
    REQUIRE(nav.current_location()->document_id == "d.md");
}

TEST_CASE("NavigationService - duplicate coalescing", "[phase32][nav]")
{
    EventBus bus;
    NavigationService nav(bus);

    nav.go_to("file.md", 10, 0);
    nav.go_to("file.md", 10, 0); // duplicate — should be ignored

    REQUIRE(nav.stack_size() == 1);
}

TEST_CASE("NavigationService - emits NavigationChangedEvent", "[phase32][nav]")
{
    EventBus bus;
    NavigationService nav(bus);

    std::string last_doc;
    int last_line = -1;
    auto sub = bus.subscribe<events::NavigationChangedEvent>(
        [&](const events::NavigationChangedEvent& evt)
        {
            last_doc = evt.document_id;
            last_line = evt.line;
        });

    nav.go_to("test.md", 42);
    REQUIRE(last_doc == "test.md");
    REQUIRE(last_line == 42);

    nav.go_to("other.md", 7);
    REQUIRE(last_doc == "other.md");
    REQUIRE(last_line == 7);

    nav.go_back();
    REQUIRE(last_doc == "test.md");
    REQUIRE(last_line == 42);
}

TEST_CASE("NavigationService - max stack size", "[phase32][nav]")
{
    EventBus bus;
    NavigationService nav(bus);

    for (int idx = 0; idx < NavigationService::kMaxStackSize + 10; ++idx)
    {
        nav.go_to("file_" + std::to_string(idx) + ".md", idx);
    }

    REQUIRE(nav.stack_size() == static_cast<size_t>(NavigationService::kMaxStackSize));
}

TEST_CASE("NavigationService - clear", "[phase32][nav]")
{
    EventBus bus;
    NavigationService nav(bus);

    nav.go_to("a.md", 1);
    nav.go_to("b.md", 2);
    nav.clear();

    REQUIRE(nav.stack_size() == 0);
    REQUIRE(nav.current_location() == nullptr);
}

TEST_CASE("NavigationService - history access", "[phase32][nav]")
{
    EventBus bus;
    NavigationService nav(bus);

    nav.go_to("a.md", 1);
    nav.go_to("b.md", 2);

    const auto& history = nav.history();
    REQUIRE(history.size() == 2);
    REQUIRE(history[0].document_id == "a.md");
    REQUIRE(history[1].document_id == "b.md");
}

// ============================================================================
// LinkResolver Tests
// ============================================================================

TEST_CASE("LinkResolver - parse wiki-link", "[phase32][link]")
{
    auto [text, type] = LinkResolver::parse_link_token("[[MyDocument]]");
    REQUIRE(text == "MyDocument");
    REQUIRE(type == LinkType::kWikiLink);
}

TEST_CASE("LinkResolver - parse block ref", "[phase32][link]")
{
    auto [text, type] = LinkResolver::parse_link_token("((abc123))");
    REQUIRE(text == "abc123");
    REQUIRE(type == LinkType::kBlockRef);
}

TEST_CASE("LinkResolver - parse markdown link", "[phase32][link]")
{
    auto [text, type] = LinkResolver::parse_link_token("[link text](./other.md)");
    REQUIRE(text == "./other.md");
    REQUIRE(type == LinkType::kMarkdownLink);
}

TEST_CASE("LinkResolver - parse unknown token", "[phase32][link]")
{
    auto [text, type] = LinkResolver::parse_link_token("plain text");
    REQUIRE(text == "plain text");
    REQUIRE(type == LinkType::kUnknown);
}

TEST_CASE("LinkResolver - resolve wiki-link with workspace files", "[phase32][link]")
{
    LinkResolver resolver("/tmp/test_workspace");
    resolver.set_workspace_files(
        {"/tmp/test_workspace/MyDocument.md", "/tmp/test_workspace/Other.md"});

    auto result = resolver.resolve_wiki_link("MyDocument");
    REQUIRE(result.success);
    REQUIRE(result.document_path == "/tmp/test_workspace/MyDocument.md");
}

TEST_CASE("LinkResolver - resolve wiki-link with anchor", "[phase32][link]")
{
    LinkResolver resolver("/tmp/test_workspace");
    resolver.set_workspace_files({"/tmp/test_workspace/Doc.md"});

    auto result = resolver.resolve_wiki_link("Doc#Section 2");
    REQUIRE(result.success);
    REQUIRE(result.anchor == "Section 2");
}

TEST_CASE("LinkResolver - empty link fails", "[phase32][link]")
{
    LinkResolver resolver("/tmp");
    auto result = resolver.resolve_wiki_link("");
    REQUIRE_FALSE(result.success);
}

TEST_CASE("LinkResolver - workspace file count", "[phase32][link]")
{
    LinkResolver resolver("/tmp");
    REQUIRE(resolver.workspace_file_count() == 0);
    resolver.set_workspace_files({"a.md", "b.md"});
    REQUIRE(resolver.workspace_file_count() == 2);
}

// ============================================================================
// BacklinkFinder Tests
// ============================================================================

TEST_CASE("BacklinkFinder - find wiki backlinks", "[phase32][backlink]")
{
    // Create temp files
    namespace fs = std::filesystem;
    auto temp_dir = fs::temp_directory_path() / "test_backlinks";
    fs::create_directories(temp_dir);

    // Create a file that links to "Target"
    std::ofstream source_file(temp_dir / "source.md");
    source_file << "# Source\n\nSee [[Target]] for details.\n\nAlso [[Target#heading]].\n";
    source_file.close();

    std::ofstream other_file(temp_dir / "other.md");
    other_file << "# Other\n\nNo links here.\n";
    other_file.close();

    BacklinkFinder finder(temp_dir.string());
    auto backlinks = finder.find_backlinks(
        "Target", {(temp_dir / "source.md").string(), (temp_dir / "other.md").string()});

    REQUIRE(backlinks.size() == 2); // [[Target]] and [[Target#heading]]
    REQUIRE(backlinks[0].source_file == (temp_dir / "source.md").string());
    REQUIRE(backlinks[0].link_type == "wiki");

    // Cleanup
    fs::remove_all(temp_dir);
}

TEST_CASE("BacklinkFinder - find heading references", "[phase32][backlink]")
{
    namespace fs = std::filesystem;
    auto temp_dir = fs::temp_directory_path() / "test_heading_refs";
    fs::create_directories(temp_dir);

    std::ofstream source_file(temp_dir / "source.md");
    source_file << "Check [[Doc#Section A]] for details.\n";
    source_file.close();

    BacklinkFinder finder(temp_dir.string());
    auto refs =
        finder.find_heading_references("Doc", "Section A", {(temp_dir / "source.md").string()});

    REQUIRE(refs.size() == 1);
    REQUIRE(refs[0].link_type == "wiki");

    fs::remove_all(temp_dir);
}

// ============================================================================
// ChangeTracker Tests
// ============================================================================

TEST_CASE("ChangeTracker - initial state", "[phase32][change]")
{
    ChangeTracker tracker;
    REQUIRE_FALSE(tracker.has_changes());
    REQUIRE(tracker.change_count() == 0);
    REQUIRE(tracker.next_change(0) == -1);
    REQUIRE(tracker.prev_change(100) == -1);
}

TEST_CASE("ChangeTracker - record and query edits", "[phase32][change]")
{
    ChangeTracker tracker;
    tracker.record_edit(5);
    tracker.record_edit(10);
    tracker.record_edit(15);

    REQUIRE(tracker.has_changes());
    REQUIRE(tracker.change_count() == 3);
    REQUIRE(tracker.is_changed(5));
    REQUIRE(tracker.is_changed(10));
    REQUIRE_FALSE(tracker.is_changed(7));
}

TEST_CASE("ChangeTracker - next_change navigation", "[phase32][change]")
{
    ChangeTracker tracker;
    tracker.record_edit(5);
    tracker.record_edit(10);
    tracker.record_edit(15);

    REQUIRE(tracker.next_change(0) == 5);
    REQUIRE(tracker.next_change(5) == 10);
    REQUIRE(tracker.next_change(12) == 15);
    REQUIRE(tracker.next_change(15) == 5); // wraps
}

TEST_CASE("ChangeTracker - prev_change navigation", "[phase32][change]")
{
    ChangeTracker tracker;
    tracker.record_edit(5);
    tracker.record_edit(10);
    tracker.record_edit(15);

    REQUIRE(tracker.prev_change(15) == 10);
    REQUIRE(tracker.prev_change(10) == 5);
    REQUIRE(tracker.prev_change(5) == 15); // wraps
}

TEST_CASE("ChangeTracker - record range", "[phase32][change]")
{
    ChangeTracker tracker;
    tracker.record_edit_range(3, 7);

    REQUIRE(tracker.change_count() == 5);
    REQUIRE(tracker.is_changed(3));
    REQUIRE(tracker.is_changed(5));
    REQUIRE(tracker.is_changed(7));
    REQUIRE_FALSE(tracker.is_changed(2));
    REQUIRE_FALSE(tracker.is_changed(8));
}

TEST_CASE("ChangeTracker - mark_saved clears", "[phase32][change]")
{
    ChangeTracker tracker;
    tracker.record_edit(5);
    tracker.record_edit(10);
    tracker.mark_saved();

    REQUIRE_FALSE(tracker.has_changes());
    REQUIRE(tracker.change_count() == 0);
}

TEST_CASE("ChangeTracker - changed_lines returns sorted", "[phase32][change]")
{
    ChangeTracker tracker;
    tracker.record_edit(15);
    tracker.record_edit(5);
    tracker.record_edit(10);

    auto lines = tracker.changed_lines();
    REQUIRE(lines.size() == 3);
    REQUIRE(lines[0] == 5);
    REQUIRE(lines[1] == 10);
    REQUIRE(lines[2] == 15);
}

// ============================================================================
// WorkspaceSymbolIndex Tests
// ============================================================================

TEST_CASE("WorkspaceSymbolIndex - initial state", "[phase32][workspace]")
{
    WorkspaceSymbolIndex index;
    REQUIRE(index.symbol_count() == 0);
    REQUIRE(index.document_count() == 0);
}

TEST_CASE("WorkspaceSymbolIndex - update and search", "[phase32][workspace]")
{
    WorkspaceSymbolIndex index;

    std::vector<SymbolInfo> symbols;
    SymbolInfo heading;
    heading.name = "Introduction";
    heading.kind = SymbolKind::kHeading;
    heading.line = 1;
    heading.level = 1;
    symbols.push_back(heading);

    SymbolInfo heading2;
    heading2.name = "Getting Started";
    heading2.kind = SymbolKind::kHeading;
    heading2.line = 10;
    heading2.level = 2;
    symbols.push_back(heading2);

    index.update_document("readme.md", symbols);

    REQUIRE(index.document_count() == 1);
    REQUIRE(index.symbol_count() == 2);

    auto results = index.search("intro", 10);
    REQUIRE_FALSE(results.empty());
    REQUIRE(results[0].info.name == "Introduction");
    REQUIRE(results[0].document_id == "readme.md");
}

TEST_CASE("WorkspaceSymbolIndex - rebuild", "[phase32][workspace]")
{
    WorkspaceSymbolIndex index;

    SymbolInfo sym;
    sym.name = "Title";
    sym.kind = SymbolKind::kHeading;
    sym.line = 1;

    index.rebuild({{"a.md", {sym}}, {"b.md", {sym}}});

    REQUIRE(index.document_count() == 2);
    REQUIRE(index.symbol_count() == 2);
}

TEST_CASE("WorkspaceSymbolIndex - remove document", "[phase32][workspace]")
{
    WorkspaceSymbolIndex index;

    SymbolInfo sym;
    sym.name = "Header";
    sym.kind = SymbolKind::kHeading;
    sym.line = 1;

    index.update_document("a.md", {sym});
    index.update_document("b.md", {sym});
    REQUIRE(index.document_count() == 2);

    index.remove_document("a.md");
    REQUIRE(index.document_count() == 1);
}

TEST_CASE("WorkspaceSymbolIndex - symbols_for_document", "[phase32][workspace]")
{
    WorkspaceSymbolIndex index;

    SymbolInfo sym;
    sym.name = "MyFunction";
    sym.kind = SymbolKind::kFunction;
    sym.line = 5;

    index.update_document("code.cpp", {sym});

    auto symbols = index.symbols_for_document("code.cpp");
    REQUIRE(symbols.size() == 1);
    REQUIRE(symbols[0].name == "MyFunction");

    auto none = index.symbols_for_document("nonexistent.md");
    REQUIRE(none.empty());
}

TEST_CASE("WorkspaceSymbolIndex - clear", "[phase32][workspace]")
{
    WorkspaceSymbolIndex index;

    SymbolInfo sym;
    sym.name = "Test";
    sym.kind = SymbolKind::kHeading;
    sym.line = 1;
    index.update_document("test.md", {sym});

    index.clear();
    REQUIRE(index.document_count() == 0);
    REQUIRE(index.symbol_count() == 0);
}

// ============================================================================
// Phase 32 Events Tests
// ============================================================================

TEST_CASE("NavigationChangedEvent construction", "[phase32][events]")
{
    events::NavigationChangedEvent evt;
    evt.document_id = "file.md";
    evt.line = 42;
    evt.column = 10;
    REQUIRE(evt.document_id == "file.md");
    REQUIRE(evt.line == 42);
    REQUIRE(evt.column == 10);
}

TEST_CASE("GoToLineEvent construction", "[phase32][events]")
{
    events::GoToLineEvent evt;
    evt.line = 100;
    REQUIRE(evt.line == 100);
}

TEST_CASE("FileOpenRequestEvent exists", "[phase32][events]")
{
    events::FileOpenRequestEvent evt;
    evt.file_path = "/some/path.md";
    REQUIRE(evt.file_path == "/some/path.md");
}
