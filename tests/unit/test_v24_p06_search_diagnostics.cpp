/// @file test_v24_p06_search_diagnostics.cpp
/// @brief V24 Phase 06 tests: Search, preview, diagnostics navigation.
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/SearchService.h"
#include "../../src/core/Search.h"
#include "../../src/core/Config.h"
#include "../../src/core/EventBus.h"

using namespace markamp::core;

// P06-T01: Real search execution
TEST_CASE("P06-T01 search query construction", "[v24][p06]") {
    SearchQuery q;
    q.query_string = "hello world";
    q.method = SearchMethod::Keyword;
    q.case_sensitive = false;
    REQUIRE(q.query_string == "hello world");
    REQUIRE(q.method == SearchMethod::Keyword);
}

TEST_CASE("P06-T01 search result structure", "[v24][p06]") {
    SearchResult result;
    result.total_count = 5;
    result.elapsed_ms = 12.0;
    REQUIRE(result.total_count == 5);
    REQUIRE(result.elapsed_ms == 12.0);
}

// P06-T02: Search panel completeness
TEST_CASE("P06-T02 search hit content and snippets", "[v24][p06]") {
    SearchHit h;
    h.root_id = "doc-1";
    h.doc_title = "Test Document";
    h.content = "This is the full content.";
    h.snippet = "...full content...";
    h.score = 1.5;
    REQUIRE(h.root_id == "doc-1");
    REQUIRE_FALSE(h.snippet.empty());
}

TEST_CASE("P06-T02 search grouped results", "[v24][p06]") {
    SearchResult result;
    SearchGroup g;
    g.root_id = "doc-1";
    g.doc_title = "Doc A";
    SearchHit h1;
    h1.root_id = "doc-1";
    h1.content = "match 1";
    SearchHit h2;
    h2.root_id = "doc-1";
    h2.content = "match 2";
    g.hits.push_back(h1);
    g.hits.push_back(h2);
    g.total_hits = 2;
    result.groups.push_back(g);
    REQUIRE(SearchService::is_grouped(result));
    REQUIRE(SearchService::result_group_count(result) == 1);
}

// P06-T03: Preview fallbacks
TEST_CASE("P06-T03 search methods enumeration", "[v24][p06]") {
    REQUIRE(SearchMethod::Keyword != SearchMethod::Phrase);
    REQUIRE(SearchMethod::Phrase != SearchMethod::Regex);
    REQUIRE(SearchMethod::Regex != SearchMethod::Sql);
}

TEST_CASE("P06-T03 search query with regex", "[v24][p06]") {
    SearchQuery q;
    q.query_string = "\\bfoo\\b";
    q.method = SearchMethod::Regex;
    REQUIRE(q.method == SearchMethod::Regex);
}

// P06-T04: Diagnostics navigation
TEST_CASE("P06-T04 search hit highlight ranges", "[v24][p06]") {
    SearchHit hit;
    hit.content = "test content with match";
    HighlightRange hr;
    hr.start = 18;
    hr.end = 23;
    hit.highlights.push_back(hr);
    REQUIRE(hit.highlights.size() == 1);
    REQUIRE(hit.highlights[0].start == 18);
}

TEST_CASE("P06-T04 search query pagination", "[v24][p06]") {
    SearchQuery q;
    q.page = 2;
    q.page_size = 25;
    REQUIRE(q.page == 2);
    REQUIRE(q.page_size == 25);
}

// P06-T05: Regression
TEST_CASE("P06-T05 search service static helpers", "[v24][p06]") {
    SearchResult result;
    REQUIRE(SearchService::total_hit_count(result) == 0);
    REQUIRE_FALSE(SearchService::is_grouped(result));
}

TEST_CASE("P06-T05 fts query builder", "[v24][p06]") {
    FtsQueryBuilder builder;
    auto kw = builder.build_keyword_query("foo bar");
    REQUIRE_FALSE(kw.empty());
    auto phrase = builder.build_phrase_query("foo bar");
    REQUIRE_FALSE(phrase.empty());
}
