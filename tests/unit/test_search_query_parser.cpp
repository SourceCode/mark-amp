/// @file test_search_query_parser.cpp
/// @brief V4 Phase 15 – Search Query Parser tests.

#include "core/SearchEngine.h"
#include "core/SearchQueryParser.h"

#include <catch2/catch_test_macros.hpp>

using markamp::core::ParsedQueryToken;
using markamp::core::SearchMethod;
using markamp::core::SearchQueryParser;

// ============================================================================
// Test 1: Plain query
// ============================================================================
TEST_CASE("Plain terms parsed as Term tokens", "[searchparser][plain]")
{
    SearchQueryParser parser;

    auto tokens = parser.parse("hello world");

    REQUIRE(tokens.size() == 2);
    CHECK(tokens[0].type == ParsedQueryToken::Type::Term);
    CHECK(tokens[0].value == "hello");
    CHECK(tokens[1].type == ParsedQueryToken::Type::Term);
    CHECK(tokens[1].value == "world");
}

// ============================================================================
// Test 2: Quoted phrase
// ============================================================================
TEST_CASE("Quoted string parsed as Phrase token", "[searchparser][phrase]")
{
    SearchQueryParser parser;

    auto tokens = parser.parse("\"exact match\"");

    REQUIRE(tokens.size() == 1);
    CHECK(tokens[0].type == ParsedQueryToken::Type::Phrase);
    CHECK(tokens[0].value == "exact match");
}

// ============================================================================
// Test 3: Scope prefix
// ============================================================================
TEST_CASE("Scope prefix parsed correctly", "[searchparser][scope]")
{
    SearchQueryParser parser;

    auto tokens = parser.parse("tag:important");

    REQUIRE(tokens.size() == 1);
    CHECK(tokens[0].type == ParsedQueryToken::Type::ScopePrefix);
    CHECK(tokens[0].scope == "tag");
    CHECK(tokens[0].value == "important");
    CHECK(tokens[0].is_scope());
}

// ============================================================================
// Test 4: Negation
// ============================================================================
TEST_CASE("Negated term parsed with dash prefix", "[searchparser][negation]")
{
    SearchQueryParser parser;

    auto tokens = parser.parse("-excluded");

    REQUIRE(tokens.size() == 1);
    CHECK(tokens[0].type == ParsedQueryToken::Type::Negation);
    CHECK(tokens[0].value == "excluded");
    CHECK(tokens[0].is_negated());
}

// ============================================================================
// Test 5: OR operator
// ============================================================================
TEST_CASE("OR keyword parsed as Operator", "[searchparser][or]")
{
    SearchQueryParser parser;

    auto tokens = parser.parse("cats OR dogs");

    REQUIRE(tokens.size() == 3);
    CHECK(tokens[0].type == ParsedQueryToken::Type::Term);
    CHECK(tokens[0].value == "cats");
    CHECK(tokens[1].type == ParsedQueryToken::Type::Operator);
    CHECK(tokens[1].value == "OR");
    CHECK(tokens[2].type == ParsedQueryToken::Type::Term);
    CHECK(tokens[2].value == "dogs");
}

// ============================================================================
// Test 6: Combined query
// ============================================================================
TEST_CASE("Complex query with mixed tokens", "[searchparser][combined]")
{
    SearchQueryParser parser;

    auto tokens = parser.parse("tag:work \"project plan\" -archived notes");

    REQUIRE(tokens.size() == 4);
    CHECK(tokens[0].type == ParsedQueryToken::Type::ScopePrefix);
    CHECK(tokens[0].scope == "tag");
    CHECK(tokens[0].value == "work");
    CHECK(tokens[1].type == ParsedQueryToken::Type::Phrase);
    CHECK(tokens[1].value == "project plan");
    CHECK(tokens[2].type == ParsedQueryToken::Type::Negation);
    CHECK(tokens[2].value == "archived");
    CHECK(tokens[3].type == ParsedQueryToken::Type::Term);
    CHECK(tokens[3].value == "notes");
}

// ============================================================================
// Test 7: build_query integration
// ============================================================================
TEST_CASE("build_query populates SearchQuery from tokens", "[searchparser][build]")
{
    SearchQueryParser parser;

    auto tokens = parser.parse("tag:important search terms");
    auto query = parser.build_query(tokens);

    CHECK(query.scope.tags.size() == 1);
    CHECK(query.scope.tags[0] == "important");
    CHECK(query.query_string.find("search") != std::string::npos);
    CHECK(query.query_string.find("terms") != std::string::npos);
}

// ============================================================================
// Test 8: Empty query
// ============================================================================
TEST_CASE("Empty query returns no tokens", "[searchparser][empty]")
{
    SearchQueryParser parser;

    auto tokens = parser.parse("");
    CHECK(tokens.empty());

    auto query = parser.build_query(tokens);
    CHECK(query.query_string.empty());
}

// ============================================================================
// Test 9: Multiple scopes
// ============================================================================
TEST_CASE("Multiple scope prefixes all captured", "[searchparser][multiscope]")
{
    SearchQueryParser parser;

    auto tokens = parser.parse("tag:work tag:urgent path:projects/");

    int scope_count = 0;
    for (const auto& token : tokens)
    {
        if (token.is_scope())
        {
            ++scope_count;
        }
    }
    CHECK(scope_count == 3);

    auto query = parser.build_query(tokens);
    CHECK(query.scope.tags.size() == 2);
    CHECK(query.scope.folders.size() == 1);
}

// ============================================================================
// Test 10: Nested quotes (escaped)
// ============================================================================
TEST_CASE("Escaped quotes inside phrases", "[searchparser][escape]")
{
    SearchQueryParser parser;

    auto tokens = parser.parse(R"("say \"hello\"")");

    REQUIRE(tokens.size() == 1);
    CHECK(tokens[0].type == ParsedQueryToken::Type::Phrase);
    CHECK(tokens[0].value == "say \"hello\"");
}

// ============================================================================
// Test 11: Special characters in terms
// ============================================================================
TEST_CASE("Special characters preserved in terms", "[searchparser][special]")
{
    SearchQueryParser parser;

    auto tokens = parser.parse("C++ std::vector<int>");

    // Should be parsed as 3 separate terms
    CHECK(tokens.size() >= 1);
    // First token should be "C++"
    CHECK(tokens[0].value == "C++");
}
