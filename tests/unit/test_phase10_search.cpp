/// @file test_phase10_search.cpp
/// @brief V9 Phase 10 – Comprehensive search system tests.
///
/// Tests cover:
///   - SearchHistoryManager (add/get/clear/find/serialize/deserialize/dedup/cap/merge)
///   - SavedSearchManager (add/remove/rename/pin/sort/serialize/cap)
///   - SearchResultNavigator (next/prev/wrap/visited/filter)
///   - SearchReplacePreview (generate/toggle/diff/summary/regex-validate)
///   - SelectionSearchHelper (find/highlight/escape)
///   - SearchIndexWatcher (queue/debounce/health/rebuild)
///   - SearchExporter (markdown/csv/json)
///   - SearchCommandProvider (commands/enabled/execute/keybinding)
///   - SearchAccessibility (announcements/metadata)
///   - Integration tests (full pipeline, round-trip, edge cases)
///   - Performance benchmarks

#include "core/SavedSearches.h"
#include "core/SearchAccessibility.h"
#include "core/SearchCommands.h"
#include "core/SearchExporter.h"
#include "core/SearchHistory.h"
#include "core/SearchIndexWatcher.h"
#include "core/SearchReplacePreview.h"
#include "core/SearchResultNavigator.h"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <string>
#include <thread>
#include <vector>

using namespace markamp::core;

// ============================================================================
// Helper: build a SearchResult with N hits
// ============================================================================
namespace
{

auto make_hit(const std::string& title, const std::string& content, double score = 1.0) -> SearchHit
{
    SearchHit hit;
    hit.doc_title = title;
    hit.content = content;
    hit.snippet = content.substr(0, std::min<std::size_t>(60, content.size()));
    hit.score = score;
    hit.block_type = SearchBlockType::Paragraph;
    hit.block_path = "/notes/" + title + ".md";
    hit.root_id = title;
    return hit;
}

auto make_result(int num_hits) -> SearchResult
{
    SearchResult result;
    result.query.query_string = "test_query";
    for (int idx = 0; idx < num_hits; ++idx)
    {
        result.hits.push_back(
            make_hit("Doc" + std::to_string(idx),
                     "Content with test_query keyword in document " + std::to_string(idx),
                     10.0 - static_cast<double>(idx) * 0.5));
    }
    result.total_count = num_hits;
    return result;
}

} // anonymous namespace

// ============================================================================
// Unit Tests: SearchHistoryManager (Tasks 1–2)
// ============================================================================

TEST_CASE("SearchHistoryManager: add and retrieve entries", "[search][history]")
{
    SearchHistoryManager mgr;
    REQUIRE(mgr.empty());

    mgr.add_entry("hello world", SearchMethod::Keyword, 42);
    REQUIRE(mgr.size() == 1);

    const auto& history = mgr.get_history();
    CHECK(history[0].query == "hello world");
    CHECK(history[0].method == SearchMethod::Keyword);
    CHECK(history[0].result_count == 42);
    CHECK(history[0].execution_count == 1);
}

TEST_CASE("SearchHistoryManager: deduplication updates existing entry", "[search][history]")
{
    SearchHistoryManager mgr;
    mgr.add_entry("duplicate", SearchMethod::Keyword, 10);
    mgr.add_entry("other", SearchMethod::Phrase, 5);
    mgr.add_entry("duplicate", SearchMethod::Keyword, 20);

    // Should still have only 2 entries
    REQUIRE(mgr.size() == 2);

    // The duplicate should be first (most recent) with updated count
    const auto& first = mgr.get_history()[0];
    CHECK(first.query == "duplicate");
    CHECK(first.result_count == 20);
    CHECK(first.execution_count == 2);
}

TEST_CASE("SearchHistoryManager: different methods are separate entries", "[search][history]")
{
    SearchHistoryManager mgr;
    mgr.add_entry("query", SearchMethod::Keyword, 10);
    mgr.add_entry("query", SearchMethod::Regex, 5);

    REQUIRE(mgr.size() == 2);
}

TEST_CASE("SearchHistoryManager: cap at 100 entries", "[search][history]")
{
    SearchHistoryManager mgr;
    for (int idx = 0; idx < 120; ++idx)
    {
        mgr.add_entry("query_" + std::to_string(idx), SearchMethod::Keyword, idx);
    }

    CHECK(mgr.size() == SearchHistoryManager::kMaxEntries);
}

TEST_CASE("SearchHistoryManager: clear removes all entries", "[search][history]")
{
    SearchHistoryManager mgr;
    mgr.add_entry("a", SearchMethod::Keyword, 1);
    mgr.add_entry("b", SearchMethod::Keyword, 2);
    mgr.clear();

    CHECK(mgr.empty());
    CHECK(mgr.size() == 0);
}

TEST_CASE("SearchHistoryManager: remove_at removes specific entry", "[search][history]")
{
    SearchHistoryManager mgr;
    mgr.add_entry("first", SearchMethod::Keyword, 1);
    mgr.add_entry("second", SearchMethod::Keyword, 2);
    mgr.add_entry("third", SearchMethod::Keyword, 3);

    mgr.remove_at(1); // Remove "second" (middle)
    REQUIRE(mgr.size() == 2);
    CHECK(mgr.get_history()[0].query == "third"); // Most recent first
    CHECK(mgr.get_history()[1].query == "first");
}

TEST_CASE("SearchHistoryManager: find_matching with prefix filter", "[search][history]")
{
    SearchHistoryManager mgr;
    mgr.add_entry("apple pie", SearchMethod::Keyword, 1);
    mgr.add_entry("banana split", SearchMethod::Keyword, 2);
    mgr.add_entry("apple sauce", SearchMethod::Keyword, 3);

    auto matches = mgr.find_matching("apple");
    CHECK(matches.size() == 2);

    // Case-insensitive
    auto case_matches = mgr.find_matching("BANANA");
    CHECK(case_matches.size() == 1);

    // Empty prefix returns all
    auto all = mgr.find_matching("");
    CHECK(all.size() == 3);
}

TEST_CASE("SearchHistoryManager: empty query is rejected", "[search][history]")
{
    SearchHistoryManager mgr;
    mgr.add_entry("", SearchMethod::Keyword, 0);
    CHECK(mgr.empty());
}

TEST_CASE("SearchHistoryManager: most_recent returns latest entry", "[search][history]")
{
    SearchHistoryManager mgr;
    CHECK_FALSE(mgr.most_recent().has_value());

    mgr.add_entry("first", SearchMethod::Keyword, 1);
    mgr.add_entry("latest", SearchMethod::Keyword, 2);

    auto recent = mgr.most_recent();
    REQUIRE(recent.has_value());
    CHECK(recent->query == "latest");
}

TEST_CASE("SearchHistoryManager: serialize and deserialize round-trip",
          "[search][history][persistence]")
{
    SearchHistoryManager mgr;
    mgr.add_entry("hello world", SearchMethod::Keyword, 42);
    mgr.add_entry("regex.*pattern", SearchMethod::Regex, 7);
    mgr.add_entry("tab\there", SearchMethod::Phrase, 3);

    auto serialized = mgr.serialize();
    REQUIRE_FALSE(serialized.empty());

    SearchHistoryManager restored;
    REQUIRE(restored.deserialize(serialized));

    CHECK(restored.size() == 3);
    // Most recent first
    CHECK(restored.get_history()[0].query == "tab\there");
    CHECK(restored.get_history()[1].query == "regex.*pattern");
    CHECK(restored.get_history()[2].query == "hello world");
}

TEST_CASE("SearchHistoryManager: deserialize rejects invalid data",
          "[search][history][persistence]")
{
    SearchHistoryManager mgr;
    CHECK_FALSE(mgr.deserialize(""));
    CHECK_FALSE(mgr.deserialize("INVALID_HEADER\n"));
    CHECK_FALSE(mgr.deserialize("SEARCH_HISTORY_V1\nbad_count_line\n"));
}

TEST_CASE("SearchHistoryManager: merge combines histories", "[search][history]")
{
    SearchHistoryManager mgr1;
    mgr1.add_entry("shared_query", SearchMethod::Keyword, 10);
    mgr1.add_entry("only_in_mgr1", SearchMethod::Keyword, 5);

    SearchHistoryManager mgr2;
    mgr2.add_entry("shared_query", SearchMethod::Keyword, 20);
    mgr2.add_entry("only_in_mgr2", SearchMethod::Keyword, 8);

    mgr1.merge(mgr2);

    // Should have 3 entries (shared deduped)
    CHECK(mgr1.size() == 3);

    // shared_query should have combined execution count
    auto matches = mgr1.find_matching("shared_query");
    REQUIRE(matches.size() == 1);
    CHECK(matches[0].execution_count == 2); // 1 + 1
}

// ============================================================================
// Unit Tests: SavedSearchManager (Tasks 3–5)
// ============================================================================

TEST_CASE("SavedSearchManager: add and retrieve saved searches", "[search][saved]")
{
    SavedSearchManager mgr;
    REQUIRE(mgr.empty());

    SearchQuery query;
    query.query_string = "project notes";
    query.method = SearchMethod::Keyword;

    CHECK(mgr.add("My Search", query));
    REQUIRE(mgr.size() == 1);

    auto found = mgr.find_by_name("My Search");
    REQUIRE(found.has_value());
    CHECK(found->name == "My Search");
    CHECK(found->query.query_string == "project notes");
}

TEST_CASE("SavedSearchManager: reject duplicate names", "[search][saved]")
{
    SavedSearchManager mgr;
    SearchQuery query;
    query.query_string = "test";

    CHECK(mgr.add("Search1", query));
    CHECK_FALSE(mgr.add("Search1", query)); // Duplicate name
    CHECK(mgr.size() == 1);
}

TEST_CASE("SavedSearchManager: reject empty name", "[search][saved]")
{
    SavedSearchManager mgr;
    SearchQuery query;
    CHECK_FALSE(mgr.add("", query));
}

TEST_CASE("SavedSearchManager: remove saved search", "[search][saved]")
{
    SavedSearchManager mgr;
    SearchQuery query;
    mgr.add("ToRemove", query);

    CHECK(mgr.remove("ToRemove"));
    CHECK(mgr.empty());
    CHECK_FALSE(mgr.remove("NonExistent"));
}

TEST_CASE("SavedSearchManager: rename saved search", "[search][saved]")
{
    SavedSearchManager mgr;
    SearchQuery query;
    mgr.add("OldName", query);

    CHECK(mgr.rename("OldName", "NewName"));
    CHECK(mgr.find_by_name("NewName").has_value());
    CHECK_FALSE(mgr.find_by_name("OldName").has_value());

    // Rename to empty should fail
    CHECK_FALSE(mgr.rename("NewName", ""));

    // Rename non-existent should fail
    CHECK_FALSE(mgr.rename("Ghost", "NewGhost"));
}

TEST_CASE("SavedSearchManager: toggle pin", "[search][saved]")
{
    SavedSearchManager mgr;
    SearchQuery query;
    mgr.add("Pinnable", query);

    auto before = mgr.find_by_name("Pinnable");
    CHECK_FALSE(before->is_pinned);

    CHECK(mgr.toggle_pin("Pinnable"));
    auto after = mgr.find_by_name("Pinnable");
    CHECK(after->is_pinned);

    CHECK(mgr.toggle_pin("Pinnable"));
    auto toggled_back = mgr.find_by_name("Pinnable");
    CHECK_FALSE(toggled_back->is_pinned);
}

TEST_CASE("SavedSearchManager: capacity eviction removes oldest non-pinned", "[search][saved]")
{
    SavedSearchManager mgr;
    SearchQuery query;

    // Fill to capacity
    for (std::size_t idx = 0; idx < SavedSearchManager::kMaxSavedSearches; ++idx)
    {
        mgr.add("Search" + std::to_string(idx), query);
    }
    CHECK(mgr.size() == SavedSearchManager::kMaxSavedSearches);

    // Add one more — should evict oldest non-pinned
    CHECK(mgr.add("Overflow", query));
    CHECK(mgr.size() == SavedSearchManager::kMaxSavedSearches);

    // Search0 (oldest) should have been evicted
    CHECK_FALSE(mgr.find_by_name("Search0").has_value());
    CHECK(mgr.find_by_name("Overflow").has_value());
}

TEST_CASE("SavedSearchManager: get_pinned returns only pinned", "[search][saved]")
{
    SavedSearchManager mgr;
    SearchQuery query;
    mgr.add("A", query);
    mgr.add("B", query);
    mgr.add("C", query);
    mgr.toggle_pin("B");

    auto pinned = mgr.get_pinned();
    REQUIRE(pinned.size() == 1);
    CHECK(pinned[0].name == "B");
}

TEST_CASE("SavedSearchManager: get_recent returns by last_used order", "[search][saved]")
{
    SavedSearchManager mgr;
    SearchQuery query;
    mgr.add("First", query);
    mgr.add("Second", query);
    mgr.add("Third", query);

    // Mark "First" as recently used
    mgr.mark_used("First");

    auto recent = mgr.get_recent(2);
    REQUIRE(recent.size() == 2);
    CHECK(recent[0].name == "First"); // Most recently used
}

TEST_CASE("SavedSearchManager: search_by_name case-insensitive", "[search][saved]")
{
    SavedSearchManager mgr;
    SearchQuery query;
    mgr.add("Project Alpha", query);
    mgr.add("Project Beta", query);
    mgr.add("Personal Notes", query);

    auto matches = mgr.search_by_name("project");
    CHECK(matches.size() == 2);

    auto all = mgr.search_by_name("");
    CHECK(all.size() == 3);
}

TEST_CASE("SavedSearchManager: sort_by orders correctly", "[search][saved]")
{
    SavedSearchManager mgr;
    SearchQuery query;
    mgr.add("Charlie", query);
    mgr.add("Alpha", query);
    mgr.add("Bravo", query);

    mgr.sort_by(SavedSearchSortOrder::kName);
    const auto& sorted = mgr.get_all();
    CHECK(sorted[0].name == "Alpha");
    CHECK(sorted[1].name == "Bravo");
    CHECK(sorted[2].name == "Charlie");

    // Pin Alpha and sort by pinned
    mgr.toggle_pin("Alpha");
    mgr.sort_by(SavedSearchSortOrder::kPinned);
    CHECK(mgr.get_all()[0].name == "Alpha");
}

TEST_CASE("SavedSearchManager: serialize and deserialize round-trip",
          "[search][saved][persistence]")
{
    SavedSearchManager mgr;
    SearchQuery query1;
    query1.query_string = "hello world";
    query1.method = SearchMethod::Keyword;
    query1.case_sensitive = true;

    SearchQuery query2;
    query2.query_string = "regex.*pat";
    query2.method = SearchMethod::Regex;

    mgr.add("Search One", query1);
    mgr.add("Search Two", query2);
    mgr.toggle_pin("Search One");
    mgr.set_description("Search One", "My description");
    mgr.set_color_label("Search One", "#FF0000");

    auto serialized = mgr.serialize();

    SavedSearchManager restored;
    REQUIRE(restored.deserialize(serialized));
    REQUIRE(restored.size() == 2);

    auto found = restored.find_by_name("Search One");
    REQUIRE(found.has_value());
    CHECK(found->query.query_string == "hello world");
    CHECK(found->query.case_sensitive == true);
    CHECK(found->is_pinned == true);
    CHECK(found->description == "My description");
    CHECK(found->color_label == "#FF0000");
}

// ============================================================================
// Unit Tests: SearchResultNavigator (Tasks 6–7)
// ============================================================================

TEST_CASE("SearchResultNavigator: empty results", "[search][navigator]")
{
    SearchResultNavigator nav;
    CHECK(nav.current() == nullptr);
    CHECK(nav.total() == 0);
    CHECK(nav.current_index() == -1);
    CHECK(nav.next() == nullptr);
    CHECK(nav.previous() == nullptr);
}

TEST_CASE("SearchResultNavigator: navigate next with wrap-around", "[search][navigator]")
{
    SearchResultNavigator nav;
    nav.set_results(make_result(3));

    CHECK(nav.current_index() == 0);
    CHECK(nav.total() == 3);

    nav.next();
    CHECK(nav.current_index() == 1);

    nav.next();
    CHECK(nav.current_index() == 2);

    // Wrap around
    nav.next();
    CHECK(nav.current_index() == 0);
}

TEST_CASE("SearchResultNavigator: navigate previous with wrap-around", "[search][navigator]")
{
    SearchResultNavigator nav;
    nav.set_results(make_result(3));

    // Start at 0, go previous → should wrap to last
    nav.previous();
    CHECK(nav.current_index() == 2);

    nav.previous();
    CHECK(nav.current_index() == 1);
}

TEST_CASE("SearchResultNavigator: go_to specific index", "[search][navigator]")
{
    SearchResultNavigator nav;
    nav.set_results(make_result(5));

    auto hit = nav.go_to(3);
    REQUIRE(hit != nullptr);
    CHECK(nav.current_index() == 3);
    CHECK(hit->doc_title == "Doc3");

    // Out of range
    CHECK(nav.go_to(-1) == nullptr);
    CHECK(nav.go_to(10) == nullptr);
}

TEST_CASE("SearchResultNavigator: visited tracking", "[search][navigator]")
{
    SearchResultNavigator nav;
    nav.set_results(make_result(5));

    // Index 0 is auto-visited
    CHECK(nav.is_visited(0));
    CHECK_FALSE(nav.is_visited(1));

    nav.next(); // Visit index 1
    CHECK(nav.is_visited(1));

    nav.mark_visited(3);
    CHECK(nav.is_visited(3));

    CHECK(nav.visited_count() == 3);
}

TEST_CASE("SearchResultNavigator: filter by score", "[search][navigator]")
{
    SearchResultNavigator nav;
    nav.set_results(make_result(5));

    auto filtered = nav.filter_by_score(9.0);
    // Scores: 10.0, 9.5, 9.0, 8.5, 8.0
    CHECK(filtered.total_count == 3);
}

TEST_CASE("SearchResultNavigator: filter by block type", "[search][navigator]")
{
    SearchResultNavigator nav;
    auto results = make_result(3);
    results.hits[1].block_type = SearchBlockType::Heading;
    nav.set_results(results);

    auto filtered = nav.filter_by_type(SearchBlockType::Paragraph);
    CHECK(filtered.total_count == 2);
}

// ============================================================================
// Unit Tests: SearchReplacePreview (Tasks 8–9)
// ============================================================================

TEST_CASE("SearchReplacePreview: generate preview entries", "[search][replace]")
{
    SearchReplacePreview preview;
    auto results = make_result(2);

    preview.generate_preview(results, "test_query", "REPLACED");
    CHECK(preview.size() > 0);

    const auto* entry = preview.get_preview(0);
    REQUIRE(entry != nullptr);
    CHECK(entry->is_selected);
    CHECK(entry->replaced_text.find("REPLACED") != std::string::npos);
}

TEST_CASE("SearchReplacePreview: empty search text produces no entries", "[search][replace]")
{
    SearchReplacePreview preview;
    auto results = make_result(3);

    preview.generate_preview(results, "", "anything");
    CHECK(preview.size() == 0);
}

TEST_CASE("SearchReplacePreview: toggle entry selection", "[search][replace]")
{
    SearchReplacePreview preview;
    auto results = make_result(1);

    preview.generate_preview(results, "test_query", "REPLACED");
    REQUIRE(preview.size() > 0);

    CHECK(preview.get_selected_count() == static_cast<int>(preview.size()));

    preview.toggle_entry(0);
    CHECK_FALSE(preview.get_preview(0)->is_selected);
}

TEST_CASE("SearchReplacePreview: select all and deselect all", "[search][replace]")
{
    SearchReplacePreview preview;
    preview.generate_preview(make_result(3), "test_query", "X");

    preview.deselect_all();
    CHECK(preview.get_selected_count() == 0);

    preview.select_all();
    CHECK(preview.get_selected_count() == static_cast<int>(preview.size()));
}

TEST_CASE("SearchReplacePreview: generate_diff produces unified diff", "[search][replace]")
{
    SearchReplacePreview preview;
    preview.generate_preview(make_result(1), "test_query", "REPLACED");
    REQUIRE(preview.size() > 0);

    auto diff = preview.generate_diff(0);
    CHECK(diff.find("---") != std::string::npos);
    CHECK(diff.find("+++") != std::string::npos);
    CHECK(diff.find("@@") != std::string::npos);
}

TEST_CASE("SearchReplacePreview: generate_summary statistics", "[search][replace]")
{
    SearchReplacePreview preview;
    preview.generate_preview(make_result(3), "test_query", "X");

    auto summary = preview.generate_summary();
    CHECK(summary.total_matches > 0);
    CHECK(summary.selected_count == summary.total_matches);
    CHECK(summary.files_affected > 0);
    CHECK(summary.estimated_time_ms > 0.0);
}

TEST_CASE("SearchReplacePreview: validate_regex_replace", "[search][replace]")
{
    auto [valid, error] = SearchReplacePreview::validate_regex_replace(R"(\d+)", "$0_suffix");
    CHECK(valid);
    CHECK(error.empty());

    auto [invalid, inv_error] =
        SearchReplacePreview::validate_regex_replace("[invalid(", "replacement");
    CHECK_FALSE(invalid);
    CHECK_FALSE(inv_error.empty());

    auto [empty_result, empty_error] =
        SearchReplacePreview::validate_regex_replace("", "replacement");
    CHECK_FALSE(empty_result);
}

// ============================================================================
// Unit Tests: SelectionSearchHelper (Task 10)
// ============================================================================

TEST_CASE("SelectionSearchHelper: find occurrences in document", "[search][selection]")
{
    const std::string document = "hello world\nhello again\ngoodbye hello";

    auto matches = SelectionSearchHelper::search_selection("hello", document);
    CHECK(matches.size() == 3);

    CHECK(matches[0].line == 0);
    CHECK(matches[0].column == 0);
    CHECK(matches[1].line == 1);
    CHECK(matches[2].line == 2);
}

TEST_CASE("SelectionSearchHelper: empty inputs return no matches", "[search][selection]")
{
    CHECK(SelectionSearchHelper::search_selection("", "content").empty());
    CHECK(SelectionSearchHelper::search_selection("query", "").empty());
}

TEST_CASE("SelectionSearchHelper: highlight_all_occurrences", "[search][selection]")
{
    auto ranges =
        SelectionSearchHelper::highlight_all_occurrences("test", "this is a test and another test");

    REQUIRE(ranges.size() == 2);
    CHECK(ranges[0].start == 10);
    CHECK(ranges[0].end == 14);
    CHECK(ranges[1].start == 27);
    CHECK(ranges[1].end == 31);
}

TEST_CASE("SelectionSearchHelper: escape_for_search handles special chars", "[search][selection]")
{
    CHECK(SelectionSearchHelper::escape_for_search("a.b") == "a\\.b");
    CHECK(SelectionSearchHelper::escape_for_search("a*b") == "a\\*b");
    CHECK(SelectionSearchHelper::escape_for_search("foo(bar)") == "foo\\(bar\\)");
    CHECK(SelectionSearchHelper::escape_for_search("plain") == "plain");
}

// ============================================================================
// Unit Tests: SearchIndexWatcher (Tasks 11–12)
// ============================================================================

TEST_CASE("SearchIndexWatcher: queue and process reindex", "[search][watcher]")
{
    SearchIndexWatcher watcher;
    watcher.set_debounce_ms(0); // No debounce for testing

    watcher.on_file_changed("/notes/doc1.md");
    watcher.on_file_created("/notes/doc2.md");

    CHECK(watcher.pending_count() == 2);
    CHECK(watcher.is_indexing());

    auto processed = watcher.process_queue();
    CHECK(processed == 2);
    CHECK(watcher.pending_count() == 0);
}

TEST_CASE("SearchIndexWatcher: deduplicates reindex requests", "[search][watcher]")
{
    SearchIndexWatcher watcher;
    watcher.set_debounce_ms(0);

    watcher.queue_reindex("doc1");
    watcher.queue_reindex("doc1"); // Duplicate
    watcher.queue_reindex("doc2");

    CHECK(watcher.pending_count() == 2);
}

TEST_CASE("SearchIndexWatcher: index health tracking", "[search][watcher]")
{
    SearchIndexWatcher watcher;

    IndexHealth health;
    health.total_docs = 100;
    health.indexed_docs = 100;
    health.stale_docs = 0;
    watcher.set_index_health(health);

    CHECK_FALSE(watcher.should_rebuild());

    // Simulate files changing
    watcher.on_file_changed("a.md");
    watcher.on_file_changed("b.md");

    auto current = watcher.get_index_health();
    CHECK(current.stale_docs == 2);
}

TEST_CASE("SearchIndexWatcher: should_rebuild detects threshold", "[search][watcher]")
{
    SearchIndexWatcher watcher;

    IndexHealth health;
    health.total_docs = 100;
    health.indexed_docs = 100;
    health.stale_docs = 15; // 15% > 10% threshold
    watcher.set_index_health(health);

    CHECK(watcher.should_rebuild());
}

TEST_CASE("SearchIndexWatcher: schedule and complete rebuild", "[search][watcher]")
{
    SearchIndexWatcher watcher;

    watcher.schedule_full_rebuild();
    CHECK(watcher.needs_full_rebuild());
    CHECK(watcher.should_rebuild());

    watcher.begin_rebuild();
    CHECK(watcher.get_index_health().is_rebuilding);

    watcher.complete_rebuild();
    CHECK_FALSE(watcher.get_index_health().is_rebuilding);
    CHECK_FALSE(watcher.needs_full_rebuild());
    CHECK(watcher.get_index_health().stale_docs == 0);
}

TEST_CASE("SearchIndexWatcher: IndexHealth health_ratio", "[search][watcher]")
{
    IndexHealth health;
    health.total_docs = 100;
    health.indexed_docs = 90;
    health.stale_docs = 10;

    const double ratio = health.health_ratio();
    CHECK(ratio > 0.79);
    CHECK(ratio < 0.81);

    const IndexHealth empty;
    const double empty_ratio = empty.health_ratio();
    CHECK(empty_ratio > 0.99);
    CHECK(empty_ratio < 1.01);
}

// ============================================================================
// Unit Tests: SearchExporter (Task 13)
// ============================================================================

TEST_CASE("SearchExporter: export to markdown", "[search][export]")
{
    SearchExporter exporter;
    auto results = make_result(2);

    auto markdown = exporter.export_to_markdown(results);
    CHECK(markdown.find("# Search Results") != std::string::npos);
    CHECK(markdown.find("test_query") != std::string::npos);
    CHECK(markdown.find("Doc0") != std::string::npos);
    CHECK(markdown.find("Doc1") != std::string::npos);
}

TEST_CASE("SearchExporter: export empty results to markdown", "[search][export]")
{
    SearchExporter exporter;
    SearchResult empty;

    auto markdown = exporter.export_to_markdown(empty);
    CHECK(markdown.find("No results found") != std::string::npos);
}

TEST_CASE("SearchExporter: export to CSV", "[search][export]")
{
    SearchExporter exporter;
    auto results = make_result(2);

    auto csv = exporter.export_to_csv(results);
    CHECK(csv.find("\"File\"") != std::string::npos); // Header
    CHECK(csv.find("Doc0") != std::string::npos);
}

TEST_CASE("SearchExporter: export to JSON", "[search][export]")
{
    SearchExporter exporter;
    auto results = make_result(2);

    auto json = exporter.export_to_json(results);
    CHECK(json.find("\"query\"") != std::string::npos);
    CHECK(json.find("\"hits\"") != std::string::npos);
    CHECK(json.find("Doc0") != std::string::npos);
}

TEST_CASE("SearchExporter: export_results dispatches correctly", "[search][export]")
{
    SearchExporter exporter;
    auto results = make_result(1);

    auto md = exporter.export_results(results, SearchExportFormat::kMarkdown);
    CHECK(md.find("# Search Results") != std::string::npos);

    auto csv = exporter.export_results(results, SearchExportFormat::kCsv);
    CHECK(csv.find("\"File\"") != std::string::npos);

    auto json = exporter.export_results(results, SearchExportFormat::kJson);
    CHECK(json.find("\"query\"") != std::string::npos);
}

// ============================================================================
// Unit Tests: SearchCommandProvider (Tasks 14–15)
// ============================================================================

TEST_CASE("SearchCommandProvider: get_commands returns 11 commands", "[search][commands]")
{
    SearchCommandProvider provider;
    auto commands = provider.get_commands();
    CHECK(commands.size() == 11);

    // Verify known commands exist
    bool found_find = false;
    bool found_replace = false;
    for (const auto& cmd : commands)
    {
        if (cmd.id == "search.find")
        {
            found_find = true;
        }
        if (cmd.id == "search.replace")
        {
            found_replace = true;
        }
    }
    CHECK(found_find);
    CHECK(found_replace);
}

TEST_CASE("SearchCommandProvider: is_enabled context-aware", "[search][commands]")
{
    SearchCommandProvider provider;

    SearchCommandContext no_results;
    no_results.has_results = false;

    // Find is always enabled
    CHECK(provider.is_enabled("search.find", no_results));

    // Navigation requires results
    CHECK_FALSE(provider.is_enabled("search.nextMatch", no_results));

    SearchCommandContext with_results;
    with_results.has_results = true;
    CHECK(provider.is_enabled("search.nextMatch", with_results));
}

TEST_CASE("SearchCommandProvider: execute_command", "[search][commands]")
{
    SearchCommandProvider provider;

    SearchCommandContext ctx;
    ctx.has_results = true;
    ctx.has_active_search = true;

    CHECK(provider.execute_command("search.find", ctx) == SearchCommandResult::kSuccess);
    CHECK(provider.execute_command("search.nextMatch", ctx) == SearchCommandResult::kSuccess);
    CHECK(provider.execute_command("search.nonexistent", ctx) == SearchCommandResult::kNotFound);

    SearchCommandContext no_results;
    CHECK(provider.execute_command("search.exportResults", no_results) ==
          SearchCommandResult::kDisabled);
}

TEST_CASE("SearchCommandProvider: get_keybinding", "[search][commands]")
{
    SearchCommandProvider provider;
    CHECK(provider.get_keybinding("search.find") == "Cmd+F");
    CHECK(provider.get_keybinding("search.replace") == "Cmd+H");
    CHECK(provider.get_keybinding("search.nonexistent").empty());
}

// ============================================================================
// Unit Tests: SearchAccessibility (Task 17)
// ============================================================================

TEST_CASE("SearchAccessibility: announce_result_count", "[search][accessibility]")
{
    CHECK(SearchAccessibility::announce_result_count(0) == "No results found");
    CHECK(SearchAccessibility::announce_result_count(1) == "1 result found");
    CHECK(SearchAccessibility::announce_result_count(42) == "42 results found");
}

TEST_CASE("SearchAccessibility: announce_navigation", "[search][accessibility]")
{
    CHECK(SearchAccessibility::announce_navigation(0, 10) == "Result 1 of 10");
    CHECK(SearchAccessibility::announce_navigation(4, 5) == "Result 5 of 5");
    CHECK(SearchAccessibility::announce_navigation(0, 0) == "No results to navigate");
}

TEST_CASE("SearchAccessibility: get_search_field_info", "[search][accessibility]")
{
    auto info = SearchAccessibility::get_search_field_info();
    CHECK(info.role == "searchbox");
    CHECK(info.label == "Search");
    CHECK_FALSE(info.description.empty());
}

TEST_CASE("SearchAccessibility: get_result_item_info", "[search][accessibility]")
{
    auto hit = make_hit("TestDoc", "Some content");
    auto info = SearchAccessibility::get_result_item_info(hit, 2, 10);
    CHECK(info.role == "option");
    CHECK(info.label.find("3 of 10") != std::string::npos);
    CHECK(info.label.find("TestDoc") != std::string::npos);
}

TEST_CASE("SearchAccessibility: announce_replace", "[search][accessibility]")
{
    CHECK(SearchAccessibility::announce_replace(0, 5) == "No replacements made");
    CHECK(SearchAccessibility::announce_replace(3, 5) == "3 of 5 occurrences replaced");
}

// ============================================================================
// Integration Tests (Task 19)
// ============================================================================

TEST_CASE("Integration: search → navigate → export pipeline", "[search][integration]")
{
    // Build a result set
    auto results = make_result(5);

    // Navigate through results
    SearchResultNavigator nav;
    nav.set_results(results);

    nav.next();
    nav.next();
    CHECK(nav.current_index() == 2);

    // Filter results
    auto filtered = nav.filter_by_score(9.0);
    CHECK(filtered.total_count <= results.total_count);

    // Export filtered results
    SearchExporter exporter;
    auto markdown = exporter.export_to_markdown(filtered);
    CHECK_FALSE(markdown.empty());
    CHECK(markdown.find("Search Results") != std::string::npos);
}

TEST_CASE("Integration: saved search round-trip", "[search][integration]")
{
    // Create and save a search
    SavedSearchManager mgr;
    SearchQuery query;
    query.query_string = "important notes";
    query.method = SearchMethod::Phrase;
    query.case_sensitive = true;

    mgr.add("Work Notes", query);
    mgr.toggle_pin("Work Notes");
    mgr.set_description("Work Notes", "All work-related notes");

    // Serialize
    auto serialized = mgr.serialize();

    // Deserialize into new manager
    SavedSearchManager restored;
    restored.deserialize(serialized);

    // Execute the restored search
    auto found = restored.find_by_name("Work Notes");
    REQUIRE(found.has_value());
    CHECK(found->query.query_string == "important notes");
    CHECK(found->query.method == SearchMethod::Phrase);
    CHECK(found->query.case_sensitive);
    CHECK(found->is_pinned);
    CHECK(found->description == "All work-related notes");
}

TEST_CASE("Integration: history records search and provides autocomplete", "[search][integration]")
{
    SearchHistoryManager history;

    // Simulate repeated searches
    history.add_entry("project planning", SearchMethod::Keyword, 15);
    history.add_entry("project review", SearchMethod::Keyword, 8);
    history.add_entry("meeting notes", SearchMethod::Keyword, 3);

    // Autocomplete
    auto suggestions = history.find_matching("proj");
    CHECK(suggestions.size() == 2);

    // Serialize and restore
    auto data = history.serialize();
    SearchHistoryManager restored;
    restored.deserialize(data);
    CHECK(restored.size() == 3);
}

TEST_CASE("Integration: index watcher triggers rebuild on threshold", "[search][integration]")
{
    SearchIndexWatcher watcher;
    watcher.set_debounce_ms(0);

    IndexHealth health;
    health.total_docs = 10;
    health.indexed_docs = 10;
    health.stale_docs = 0;
    watcher.set_index_health(health);

    // Simulate 2 file changes (20% stale > 10% threshold)
    watcher.on_file_changed("doc1.md");
    watcher.on_file_changed("doc2.md");

    CHECK(watcher.should_rebuild());

    // Rebuild
    watcher.schedule_full_rebuild();
    watcher.begin_rebuild();
    watcher.process_queue();
    watcher.complete_rebuild();

    CHECK_FALSE(watcher.should_rebuild());
}

TEST_CASE("Integration: command provider context with navigator", "[search][integration]")
{
    SearchCommandProvider provider;
    SearchResultNavigator nav;
    nav.set_results(make_result(5));

    SearchCommandContext ctx;
    ctx.has_active_search = true;
    ctx.has_results = (nav.total() > 0);
    ctx.search_panel_visible = true;

    CHECK(provider.is_enabled("search.nextMatch", ctx));
    CHECK(provider.execute_command("search.nextMatch", ctx) == SearchCommandResult::kSuccess);
}

// ============================================================================
// Edge Case Tests (Task 19)
// ============================================================================

TEST_CASE("Edge case: empty query handling", "[search][edge]")
{
    SearchHistoryManager history;
    history.add_entry("", SearchMethod::Keyword, 0);
    CHECK(history.empty());

    SearchReplacePreview preview;
    preview.generate_preview(make_result(1), "", "replacement");
    CHECK(preview.size() == 0);
}

TEST_CASE("Edge case: Unicode search terms", "[search][edge]")
{
    SearchHistoryManager history;
    history.add_entry("日本語テスト", SearchMethod::Keyword, 5);
    history.add_entry("émoji 🎉", SearchMethod::Keyword, 3);

    CHECK(history.size() == 2);

    auto matches = history.find_matching("日本");
    CHECK(matches.size() == 1);

    // Serialize round-trip preserves Unicode
    auto serialized = history.serialize();
    SearchHistoryManager restored;
    restored.deserialize(serialized);
    CHECK(restored.size() == 2);
    CHECK(restored.get_history()[0].query == "émoji 🎉");
}

TEST_CASE("Edge case: regex special characters in selection search", "[search][edge]")
{
    auto escaped = SelectionSearchHelper::escape_for_search("foo.*bar[0]");
    CHECK(escaped == "foo\\.\\*bar\\[0\\]");
}

TEST_CASE("Edge case: zero results navigation", "[search][edge]")
{
    SearchResultNavigator nav;
    SearchResult empty;
    nav.set_results(empty);

    CHECK(nav.total() == 0);
    CHECK(nav.current() == nullptr);
    CHECK(nav.next() == nullptr);
    CHECK(nav.previous() == nullptr);
    CHECK(nav.go_to(0) == nullptr);
}

TEST_CASE("Edge case: very long query in history", "[search][edge]")
{
    SearchHistoryManager history;
    std::string long_query(5000, 'x');
    history.add_entry(long_query, SearchMethod::Keyword, 0);
    CHECK(history.size() == 1);

    auto serialized = history.serialize();
    SearchHistoryManager restored;
    restored.deserialize(serialized);
    CHECK(restored.get_history()[0].query.size() == 5000);
}

// ============================================================================
// Performance Tests (Task 20)
// ============================================================================

TEST_CASE("Performance: history autocomplete with 100 entries", "[search][performance]")
{
    SearchHistoryManager history;
    for (int idx = 0; idx < 100; ++idx)
    {
        history.add_entry("query_prefix_" + std::to_string(idx), SearchMethod::Keyword, idx);
    }

    const auto start = std::chrono::steady_clock::now();
    auto matches = history.find_matching("query_prefix_5");
    const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - start);

    CHECK(matches.size() >= 1);
    CHECK(elapsed.count() < 10000); // < 10ms
}

TEST_CASE("Performance: navigator with 1000 results", "[search][performance]")
{
    SearchResultNavigator nav;
    nav.set_results(make_result(1000));

    const auto start = std::chrono::steady_clock::now();
    for (int idx = 0; idx < 1000; ++idx)
    {
        nav.next();
    }
    const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - start);

    CHECK(nav.visited_count() == 1000);
    CHECK(elapsed.count() < 50000); // < 50ms
}

TEST_CASE("Performance: replace preview with 500 matches", "[search][performance]")
{
    // Build content with many occurrences
    std::string big_content;
    for (int idx = 0; idx < 500; ++idx)
    {
        big_content += "line " + std::to_string(idx) + " findme content\n";
    }

    SearchResult results;
    SearchHit hit;
    hit.content = big_content;
    hit.doc_title = "BigDoc";
    hit.root_id = "big";
    results.hits.push_back(hit);
    results.total_count = 1;

    SearchReplacePreview preview;

    const auto start = std::chrono::steady_clock::now();
    preview.generate_preview(results, "findme", "REPLACED");
    const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - start);

    CHECK(preview.size() == 500);
    CHECK(elapsed.count() < 100000); // < 100ms
}

TEST_CASE("Performance: export 200 results to each format", "[search][performance]")
{
    SearchExporter exporter;
    auto results = make_result(200);

    const auto start = std::chrono::steady_clock::now();

    auto md = exporter.export_to_markdown(results);
    auto csv = exporter.export_to_csv(results);
    auto json = exporter.export_to_json(results);

    const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - start);

    CHECK_FALSE(md.empty());
    CHECK_FALSE(csv.empty());
    CHECK_FALSE(json.empty());
    CHECK(elapsed.count() < 100000); // < 100ms for all three
}
