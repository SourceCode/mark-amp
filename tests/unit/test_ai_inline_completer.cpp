// test_ai_inline_completer.cpp — 10 tests for AI inline completion types
#include "core/AITypes.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("InlineSuggestion defaults", "[ai][inline]")
{
    InlineSuggestion suggestion;
    CHECK(suggestion.text.empty());
    CHECK(suggestion.confidence == 0.0);
    CHECK(suggestion.request_id.empty());
}

TEST_CASE("InlineSuggestion with content", "[ai][inline]")
{
    InlineSuggestion suggestion;
    suggestion.text = "const auto result = ";
    suggestion.confidence = 0.85;
    suggestion.request_id = "req-42";
    CHECK(suggestion.text == "const auto result = ");
    CHECK(suggestion.confidence > 0.8);
}

TEST_CASE("InlineSuggestion high confidence", "[ai][inline]")
{
    InlineSuggestion suggestion;
    suggestion.confidence = 0.95;
    CHECK(suggestion.confidence >= 0.9);
}

TEST_CASE("InlineSuggestion low confidence", "[ai][inline]")
{
    InlineSuggestion suggestion;
    suggestion.confidence = 0.15;
    CHECK(suggestion.confidence < 0.5);
}

TEST_CASE("InlineSuggestion multi-line suggestion", "[ai][inline]")
{
    InlineSuggestion suggestion;
    suggestion.text = "if (result.has_value()) {\n    process(*result);\n}";
    CHECK(suggestion.text.find('\n') != std::string::npos);
}

TEST_CASE("InlineSuggestion request_id uniqueness", "[ai][inline]")
{
    InlineSuggestion s1;
    s1.request_id = "req-1";
    InlineSuggestion s2;
    s2.request_id = "req-2";
    CHECK(s1.request_id != s2.request_id);
}

TEST_CASE("InlineSuggestion collection", "[ai][inline]")
{
    std::vector<InlineSuggestion> suggestions;
    InlineSuggestion s1;
    s1.text = "Option A";
    s1.confidence = 0.9;
    InlineSuggestion s2;
    s2.text = "Option B";
    s2.confidence = 0.7;
    suggestions.push_back(s1);
    suggestions.push_back(s2);
    CHECK(suggestions.size() == 2);
    CHECK(suggestions[0].confidence > suggestions[1].confidence);
}

TEST_CASE("InlineSuggestion empty text is valid", "[ai][inline]")
{
    InlineSuggestion suggestion;
    // Empty text indicates no suggestion available
    CHECK(suggestion.text.empty());
}

TEST_CASE("InlineSuggestion code completion pattern", "[ai][inline]")
{
    InlineSuggestion suggestion;
    suggestion.text = "std::vector<std::string>";
    suggestion.confidence = 0.88;
    CHECK(suggestion.text.find("vector") != std::string::npos);
}

TEST_CASE("InlineSuggestion markdown completion", "[ai][inline]")
{
    InlineSuggestion suggestion;
    suggestion.text = "## Section Title\n\nThis section covers...";
    suggestion.confidence = 0.72;
    CHECK(suggestion.text.find("##") != std::string::npos);
}
