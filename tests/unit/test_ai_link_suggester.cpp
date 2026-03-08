// test_ai_link_suggester.cpp — 10 tests for AI link suggestion types
#include "core/AITypes.h"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>

using namespace markamp::core;

TEST_CASE("LinkSuggestion default values", "[ai][link]")
{
    LinkSuggestion link;
    CHECK(link.target_document.empty());
    CHECK(link.anchor_text.empty());
    CHECK(link.relevance == 0.0);
    CHECK(link.reason.empty());
}

TEST_CASE("LinkSuggestion with document reference", "[ai][link]")
{
    LinkSuggestion link;
    link.target_document = "architecture.md";
    link.anchor_text = "system architecture";
    link.relevance = 0.87;
    link.reason = "Related topic discussed";
    CHECK(link.target_document == "architecture.md");
    CHECK(link.relevance > 0.8);
}

TEST_CASE("LinkSuggestion high relevance", "[ai][link]")
{
    LinkSuggestion link;
    link.relevance = 0.95;
    CHECK(link.relevance >= 0.9);
}

TEST_CASE("LinkSuggestion low relevance", "[ai][link]")
{
    LinkSuggestion link;
    link.relevance = 0.15;
    CHECK(link.relevance < 0.5);
}

TEST_CASE("LinkSuggestion collection sorted by relevance", "[ai][link]")
{
    std::vector<LinkSuggestion> links;
    LinkSuggestion l1;
    l1.target_document = "low.md";
    l1.relevance = 0.2;
    LinkSuggestion l2;
    l2.target_document = "high.md";
    l2.relevance = 0.9;
    links.push_back(l1);
    links.push_back(l2);
    std::sort(links.begin(),
              links.end(),
              [](const LinkSuggestion& lhs, const LinkSuggestion& rhs)
              { return lhs.relevance > rhs.relevance; });
    CHECK(links[0].target_document == "high.md");
}

TEST_CASE("LinkSuggestion with nested directory path", "[ai][link]")
{
    LinkSuggestion link;
    link.target_document = "docs/guides/getting-started.md";
    link.anchor_text = "getting started guide";
    CHECK(link.target_document.find('/') != std::string::npos);
}

TEST_CASE("LinkSuggestion anchor_text differs from document", "[ai][link]")
{
    LinkSuggestion link;
    link.target_document = "api-reference.md";
    link.anchor_text = "API documentation";
    CHECK(link.target_document != link.anchor_text);
}

TEST_CASE("LinkSuggestion reason provides context", "[ai][link]")
{
    LinkSuggestion link;
    link.reason = "Both documents discuss authentication patterns";
    CHECK(link.reason.find("authentication") != std::string::npos);
}

TEST_CASE("LinkSuggestion batch of related links", "[ai][link]")
{
    std::vector<LinkSuggestion> batch;
    for (int idx = 0; idx < 5; ++idx)
    {
        LinkSuggestion link;
        link.target_document = "doc-" + std::to_string(idx) + ".md";
        link.relevance = 0.95 - (0.1 * static_cast<double>(idx));
        batch.push_back(link);
    }
    CHECK(batch.size() == 5);
    CHECK(batch[0].relevance > batch[4].relevance);
}

TEST_CASE("LinkSuggestion relevance bounds", "[ai][link]")
{
    LinkSuggestion link;
    link.relevance = 0.0;
    CHECK(link.relevance >= 0.0);
    link.relevance = 1.0;
    CHECK(link.relevance <= 1.0);
}
