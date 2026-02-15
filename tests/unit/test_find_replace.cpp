/// @file test_find_replace.cpp
/// @brief V4 Phase 16 – Find/Replace Service tests.
///
/// Tests the pure-logic methods (search_file, expand_replacement,
/// validate_pattern, replace_single, path_matches_filter) directly.
/// The vault-dependent methods (find_all, replace_all) are integration-tested
/// through the vault, and are not covered here.

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/FindReplaceService.h"
#include "core/VaultService.h"

#include <catch2/catch_test_macros.hpp>

#include <regex>

using namespace markamp::core;

// ============================================================================
// Helpers
// ============================================================================

namespace
{

/// Construct a real FindReplaceService backed by a (no-op) VaultService.
/// The tests only call search_file / expand_replacement / validate_pattern
/// which don't touch the VaultService.
struct TestFixture
{
    EventBus bus;
    Config config;
    VaultService vault{bus, config};
    FindReplaceService service{bus, vault};
};

} // namespace

// ============================================================================
// Test 1: Literal find
// ============================================================================
TEST_CASE("Literal find matches all occurrences", "[findreplace][literal]")
{
    TestFixture f;
    FindReplaceOptions opts;
    opts.pattern = "hello";

    auto matches = f.service.search_file("hello world hello", "doc1", "/test.md", opts);
    REQUIRE(matches.size() == 2);
    CHECK(matches[0].column == 0);
    CHECK(matches[0].matched_text == "hello");
    CHECK(matches[1].column == 12);
    CHECK(matches[0].context_before.empty());
    CHECK(matches[0].context_after == " world hello");
}

// ============================================================================
// Test 2: Case insensitive
// ============================================================================
TEST_CASE("Case insensitive literal search", "[findreplace][case]")
{
    TestFixture f;
    FindReplaceOptions opts;
    opts.pattern = "Hello";
    opts.case_sensitive = false;

    auto matches = f.service.search_file("hello HELLO Hello", "doc1", "/test.md", opts);
    REQUIRE(matches.size() == 3);
}

// ============================================================================
// Test 3: Whole word
// ============================================================================
TEST_CASE("Whole word does not match partial", "[findreplace][wholeword]")
{
    TestFixture f;
    FindReplaceOptions opts;
    opts.pattern = "the";
    opts.whole_word = true;

    auto matches = f.service.search_file("the there other the", "doc1", "/test.md", opts);
    REQUIRE(matches.size() == 2);
    CHECK(matches[0].column == 0);
    CHECK(matches[1].column == 16);
}

// ============================================================================
// Test 4: Regex find
// ============================================================================
TEST_CASE("Regex find with digit pattern", "[findreplace][regex]")
{
    TestFixture f;
    FindReplaceOptions opts;
    opts.pattern = R"(\d{3}-\d{4})";
    opts.use_regex = true;

    auto matches = f.service.search_file("call 555-1234 or 55-1234", "doc1", "/test.md", opts);
    REQUIRE(matches.size() == 1);
    CHECK(matches[0].matched_text == "555-1234");
    CHECK(matches[0].column == 5);
}

// ============================================================================
// Test 5: Regex capture groups
// ============================================================================
TEST_CASE("Capture group expansion in replacement", "[findreplace][capture]")
{
    TestFixture f;
    std::regex re(R"((\w+)@(\w+))");
    std::string text = "user@host";
    std::smatch captures;
    std::regex_search(text, captures, re);

    auto result = f.service.expand_replacement(text, captures, "$2/$1");
    CHECK(result == "host/user");
}

// ============================================================================
// Test 6: Replace single
// ============================================================================
TEST_CASE("Replace single rewrites line correctly", "[findreplace][replace]")
{
    TestFixture f;
    FindReplaceOptions opts;
    opts.pattern = "foo";
    opts.replacement = "bar";

    auto matches = f.service.search_file("foo and foo and baz", "doc1", "/test.md", opts);
    REQUIRE(matches.size() == 2);

    auto result = f.service.replace_single(matches[0], opts);
    REQUIRE(result.has_value());
    CHECK(*result == "bar and foo and baz");
}

// ============================================================================
// Test 7: Replace with different length
// ============================================================================
TEST_CASE("Replace single handles different replacement length", "[findreplace][positions]")
{
    TestFixture f;
    FindReplaceOptions opts;
    opts.pattern = "ab";
    opts.replacement = "XYZ";

    auto matches = f.service.search_file("ab cd ab ef", "doc1", "/test.md", opts);
    REQUIRE(matches.size() == 2);

    auto result = f.service.replace_single(matches[1], opts);
    REQUIRE(result.has_value());
    CHECK(*result == "ab cd XYZ ef");
}

// ============================================================================
// Test 8: Multi-file search
// ============================================================================
TEST_CASE("Multi-file search aggregates matches", "[findreplace][vault]")
{
    TestFixture f;
    FindReplaceOptions opts;
    opts.pattern = "TODO";

    auto m1 = f.service.search_file("TODO item 1", "doc1", "/a.md", opts);
    auto m2 = f.service.search_file("no match", "doc2", "/b.md", opts);
    auto m3 = f.service.search_file("another TODO here", "doc3", "/c.md", opts);

    CHECK(m1.size() == 1);
    CHECK(m2.empty());
    CHECK(m3.size() == 1);
    CHECK(m1[0].file_path == "/a.md");
    CHECK(m3[0].file_path == "/c.md");
}

// ============================================================================
// Test 9: Preview line
// ============================================================================
TEST_CASE("Replace single produces correct preview line", "[findreplace][preview]")
{
    TestFixture f;
    FindReplaceOptions opts;
    opts.pattern = "old";
    opts.replacement = "new";

    auto matches = f.service.search_file("the old way", "doc1", "/test.md", opts);
    REQUIRE(matches.size() == 1);

    auto result = f.service.replace_single(matches[0], opts);
    REQUIRE(result.has_value());
    CHECK(*result == "the new way");
}

// ============================================================================
// Test 10: Invalid regex
// ============================================================================
TEST_CASE("Invalid regex returns error message", "[findreplace][invalid]")
{
    TestFixture f;
    auto error = f.service.validate_pattern("[invalid");
    REQUIRE(error.has_value());
    CHECK(!error->empty());

    // Valid pattern
    auto no_error = f.service.validate_pattern("\\d+");
    CHECK_FALSE(no_error.has_value());
}

// ============================================================================
// Test 11: Multiline content
// ============================================================================
TEST_CASE("Search across multiple lines", "[findreplace][multiline]")
{
    TestFixture f;
    FindReplaceOptions opts;
    opts.pattern = "match";

    std::string content = "line 1 match\nline 2\nline 3 match match";
    auto matches = f.service.search_file(content, "doc1", "/test.md", opts);
    REQUIRE(matches.size() == 3);
    CHECK(matches[0].line_number == 0);
    CHECK(matches[1].line_number == 2);
    CHECK(matches[2].line_number == 2);
}

// ============================================================================
// Test 12: Path filtering
// ============================================================================
TEST_CASE("Path filtering includes and excludes correctly", "[findreplace][filter]")
{
    CHECK(FindReplaceService::path_matches_filter("/vault/projects/note.md", {"projects"}, {}));
    CHECK_FALSE(
        FindReplaceService::path_matches_filter("/vault/archive/note.md", {"projects"}, {}));
    CHECK_FALSE(FindReplaceService::path_matches_filter("/vault/.trash/note.md", {}, {".trash"}));
    CHECK(FindReplaceService::path_matches_filter("/vault/notes/note.md", {}, {".trash"}));
    CHECK(FindReplaceService::path_matches_filter("/vault/anything.md", {}, {}));
}
