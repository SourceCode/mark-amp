/// @file test_search_engine.cpp
/// @brief V4 Phase 05 – Comprehensive search engine tests.

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/SearchEngine.h"
#include "core/VaultService.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

namespace
{

struct SearchTestFixture
{
    SearchTestFixture()
        : vault_dir_(fs::temp_directory_path() /
                     ("markamp_search_test_" +
                      std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())))
        , vault_service_(event_bus_, config_)
        , search_engine_(event_bus_, vault_service_)
    {
        fs::create_directories(vault_dir_);
        auto sub = event_bus_.subscribe<markamp::core::events::VaultOpenedEvent>(
            [](const markamp::core::events::VaultOpenedEvent&) {});
        (void)sub;
    }

    ~SearchTestFixture()
    {
        fs::remove_all(vault_dir_);
    }

    auto create_file(const std::string& name, const std::string& content) -> void
    {
        const fs::path file_path = vault_dir_ / name;
        fs::create_directories(file_path.parent_path());
        std::ofstream out(file_path);
        out << content;
    }

    auto open_and_index() -> void
    {
        auto result = vault_service_.open_vault(vault_dir_);
        REQUIRE(result.has_value());
        search_engine_.rebuild_index();
    }

    markamp::core::EventBus event_bus_;
    markamp::core::Config config_;
    fs::path vault_dir_;
    markamp::core::VaultService vault_service_;
    markamp::core::SearchEngine search_engine_;
};

} // anonymous namespace

// ============================================================================
// Test 1: Keyword AND search
// ============================================================================
TEST_CASE_METHOD(SearchTestFixture,
                 "Keyword AND search returns only docs with all terms",
                 "[search][keyword]")
{
    create_file("doc1.md", "---\ntitle: Doc1\n---\nThe quick brown fox jumps.");
    create_file("doc2.md", "---\ntitle: Doc2\n---\nThe quick rabbit hops.");
    create_file("doc3.md", "---\ntitle: Doc3\n---\nA brown bear sleeps.");
    open_and_index();

    markamp::core::SearchQuery query;
    query.query_string = "quick brown";
    query.method = markamp::core::SearchMethod::Keyword;

    auto result = search_engine_.search(query);

    // Only doc1 has BOTH "quick" AND "brown"
    REQUIRE(result.total_count == 1);
    CHECK(result.hits[0].document_title == "Doc1");
}

// ============================================================================
// Test 2: Phrase search
// ============================================================================
TEST_CASE_METHOD(SearchTestFixture,
                 "Phrase search matches exact token sequences",
                 "[search][phrase]")
{
    create_file("doc1.md", "---\ntitle: Doc1\n---\nThe quick brown fox jumps.");
    create_file("doc2.md", "---\ntitle: Doc2\n---\nThe brown quick fox leaps.");
    open_and_index();

    markamp::core::SearchQuery query;
    query.query_string = "quick brown fox";
    query.method = markamp::core::SearchMethod::Phrase;

    auto result = search_engine_.search(query);

    // Only doc1 has the exact phrase "quick brown fox"
    REQUIRE(result.total_count == 1);
    CHECK(result.hits[0].document_title == "Doc1");
}

// ============================================================================
// Test 3: Regex search
// ============================================================================
TEST_CASE_METHOD(SearchTestFixture, "Regex search finds pattern matches", "[search][regex]")
{
    create_file("doc1.md", "---\ntitle: Doc1\n---\nError code 404 occurred.");
    create_file("doc2.md", "---\ntitle: Doc2\n---\nStatus code 200 returned.");
    create_file("doc3.md", "---\ntitle: Doc3\n---\nNo numbers here, just text.");
    open_and_index();

    markamp::core::SearchQuery query;
    query.query_string = "\\d{3}";
    query.method = markamp::core::SearchMethod::Regex;

    auto result = search_engine_.search(query);

    // doc1 (404) and doc2 (200) both have 3-digit numbers
    CHECK(result.total_count == 2);
}

// ============================================================================
// Test 4: Fuzzy search
// ============================================================================
TEST_CASE_METHOD(SearchTestFixture, "Fuzzy search finds approximate matches", "[search][fuzzy]")
{
    create_file("doc1.md", "---\ntitle: Doc1\n---\nThe algorithm processes data efficiently.");
    open_and_index();

    markamp::core::SearchQuery query;
    query.query_string = "algoritm"; // Typo: missing 'h'
    query.method = markamp::core::SearchMethod::Fuzzy;

    auto result = search_engine_.search(query);

    // Should find "algorithm" with edit distance 1
    REQUIRE(result.total_count >= 1);
    CHECK(result.hits[0].document_title == "Doc1");
}

// ============================================================================
// Test 5: BM25 relevance scoring
// ============================================================================
TEST_CASE_METHOD(SearchTestFixture, "BM25 ranks more relevant documents higher", "[search][bm25]")
{
    // Doc A mentions "test" 5 times, Doc B mentions it once
    create_file("doc_a.md", "---\ntitle: DocA\n---\ntest test test test test filler words.");
    create_file("doc_b.md",
                "---\ntitle: DocB\n---\ntest surrounded by lots of other words content stuff.");
    open_and_index();

    markamp::core::SearchQuery query;
    query.query_string = "test";
    query.method = markamp::core::SearchMethod::Keyword;

    auto result = search_engine_.search(query);

    REQUIRE(result.total_count == 2);
    // Doc A should score higher due to more occurrences
    CHECK(result.hits[0].score >= result.hits[1].score);
}

// ============================================================================
// Test 6: Case insensitive search
// ============================================================================
TEST_CASE_METHOD(SearchTestFixture,
                 "Case insensitive search matches regardless of case",
                 "[search][case]")
{
    create_file("doc1.md", "---\ntitle: Doc1\n---\nHello World Greeting.");
    open_and_index();

    markamp::core::SearchQuery query;
    query.query_string = "hello";
    query.method = markamp::core::SearchMethod::Keyword;
    query.case_sensitive = false;

    auto result = search_engine_.search(query);

    REQUIRE(result.total_count == 1);
    CHECK(result.hits[0].document_title == "Doc1");
}

// ============================================================================
// Test 7: Scope – folder filter
// ============================================================================
TEST_CASE_METHOD(SearchTestFixture,
                 "Scope restricts results to specified folders",
                 "[search][scope]")
{
    create_file("notes/note1.md", "---\ntitle: Note1\n---\nImportant information here.");
    create_file("notes/note2.md", "---\ntitle: Note2\n---\nMore important information.");
    create_file("archive/old.md", "---\ntitle: Old\n---\nImportant archived information.");
    open_and_index();

    markamp::core::SearchQuery query;
    query.query_string = "important";
    query.method = markamp::core::SearchMethod::Keyword;
    query.scope.folders = {"notes"};

    auto result = search_engine_.search(query);

    // Only 2 results from notes/ folder
    CHECK(result.total_count == 2);
    for (const auto& hit : result.hits)
    {
        CHECK(hit.file_path.starts_with("notes"));
    }
}

// ============================================================================
// Test 8: Scope – tag filter
// ============================================================================
TEST_CASE_METHOD(SearchTestFixture, "Scope filters by required tags", "[search][scope][tags]")
{
    create_file("doc1.md", "---\ntitle: Doc1\ntags:\n  - project\n---\nProject planning details.");
    create_file("doc2.md", "---\ntitle: Doc2\ntags:\n  - personal\n---\nPersonal planning notes.");
    open_and_index();

    markamp::core::SearchQuery query;
    query.query_string = "planning";
    query.method = markamp::core::SearchMethod::Keyword;
    query.scope.tags = {"project"};

    auto result = search_engine_.search(query);

    // Only doc1 has the "project" tag
    REQUIRE(result.total_count == 1);
    CHECK(result.hits[0].document_title == "Doc1");
}

// ============================================================================
// Test 9: Pagination
// ============================================================================
TEST_CASE_METHOD(SearchTestFixture, "Pagination returns correct slices", "[search][pagination]")
{
    // Create 25 documents with a common searchable term
    for (int idx = 0; idx < 25; ++idx)
    {
        std::string name = "doc" + std::to_string(idx) + ".md";
        std::string content = "---\ntitle: Doc" + std::to_string(idx) +
                              "\n---\nSearchable content number " + std::to_string(idx) + ".";
        create_file(name, content);
    }
    open_and_index();

    markamp::core::SearchQuery query;
    query.query_string = "searchable content";
    query.method = markamp::core::SearchMethod::Keyword;
    query.page = 2;
    query.page_size = 10;

    auto result = search_engine_.search(query);

    CHECK(result.total_count == 25);
    CHECK(result.page == 2);
    CHECK(result.page_size == 10);
    CHECK(static_cast<int>(result.hits.size()) == 10); // 10 results on page 2
}

// ============================================================================
// Test 10: Snippet extraction
// ============================================================================
TEST_CASE_METHOD(SearchTestFixture,
                 "Snippet extraction includes query term context",
                 "[search][snippet]")
{
    create_file("doc1.md",
                "---\ntitle: Doc1\n---\n"
                "This is some preamble text that goes on for a while. "
                "Then the important keyword appears here with some context around it. "
                "And then there is more trailing text after the match.");
    open_and_index();

    markamp::core::SearchQuery query;
    query.query_string = "keyword";
    query.method = markamp::core::SearchMethod::Keyword;

    auto result = search_engine_.search(query);

    REQUIRE(result.total_count == 1);
    CHECK_FALSE(result.hits[0].snippet.empty());
    // Snippet should contain the term
    CHECK(result.hits[0].snippet.find("keyword") != std::string::npos);
}

// ============================================================================
// Test 11: Incremental index – add document
// ============================================================================
TEST_CASE_METHOD(SearchTestFixture, "Incremental index adds new documents", "[search][incremental]")
{
    create_file("doc1.md", "---\ntitle: Doc1\n---\nExisting content.");
    open_and_index();

    // Verify initial state
    auto stats = search_engine_.index_stats();
    REQUIRE(stats.first == 1); // 1 document

    // Create new file and add to the vault
    create_file("doc2.md", "---\ntitle: Doc2\n---\nBrand new unique content.");

    // Re-open vault to pick up new file, then index
    vault_service_.close_vault();
    auto reopen = vault_service_.open_vault(vault_dir_);
    REQUIRE(reopen.has_value());

    // Find the new document
    auto docs = vault_service_.list_documents();
    for (const auto& entry : docs)
    {
        if (entry.title == "Doc2")
        {
            search_engine_.index_document(entry.document_id);
            break;
        }
    }

    // Search for the new content
    markamp::core::SearchQuery query;
    query.query_string = "unique";
    query.method = markamp::core::SearchMethod::Keyword;

    auto result = search_engine_.search(query);
    REQUIRE(result.total_count == 1);
    CHECK(result.hits[0].document_title == "Doc2");
}

// ============================================================================
// Test 12: Remove from index
// ============================================================================
TEST_CASE_METHOD(SearchTestFixture,
                 "Removing document removes it from search results",
                 "[search][remove]")
{
    create_file("doc1.md", "---\ntitle: Doc1\n---\nUnique findable term alpha.");
    create_file("doc2.md", "---\ntitle: Doc2\n---\nAnother findable term beta.");
    open_and_index();

    // Verify both found
    markamp::core::SearchQuery query;
    query.query_string = "findable";
    query.method = markamp::core::SearchMethod::Keyword;

    auto result1 = search_engine_.search(query);
    REQUIRE(result1.total_count == 2);

    // Remove doc1
    auto docs = vault_service_.list_documents();
    for (const auto& entry : docs)
    {
        if (entry.title == "Doc1")
        {
            search_engine_.remove_document(entry.document_id);
            break;
        }
    }

    auto result2 = search_engine_.search(query);
    REQUIRE(result2.total_count == 1);
    CHECK(result2.hits[0].document_title == "Doc2");
}

// ============================================================================
// Test 13: Suggestions / autocomplete
// ============================================================================
TEST_CASE_METHOD(SearchTestFixture, "Suggestions return terms matching prefix", "[search][suggest]")
{
    create_file("doc1.md", "---\ntitle: Doc1\n---\nalgorithm algebraic alternate alpha.");
    open_and_index();

    auto suggestions = search_engine_.suggest("al", 10);

    // Should find terms starting with "al"
    REQUIRE(suggestions.size() >= 2);
    for (const auto& suggestion : suggestions)
    {
        CHECK(suggestion.starts_with("al"));
    }
}
