// test_ai_search_enhancer.cpp — 10 tests for AI search types and suggestions
#include "core/AITypes.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("TagSuggestion defaults", "[ai][search]")
{
    TagSuggestion tag;
    CHECK(tag.tag.empty());
    CHECK(tag.confidence == 0.0);
    CHECK_FALSE(tag.is_existing);
    CHECK(tag.reason.empty());
}

TEST_CASE("TagSuggestion with values", "[ai][search]")
{
    TagSuggestion tag;
    tag.tag = "machine-learning";
    tag.confidence = 0.95;
    tag.is_existing = true;
    tag.reason = "Mentioned frequently";
    CHECK(tag.tag == "machine-learning");
    CHECK(tag.confidence > 0.9);
    CHECK(tag.is_existing);
}

TEST_CASE("LinkSuggestion defaults", "[ai][search]")
{
    LinkSuggestion link;
    CHECK(link.target_document.empty());
    CHECK(link.anchor_text.empty());
    CHECK(link.relevance == 0.0);
    CHECK(link.reason.empty());
}

TEST_CASE("LinkSuggestion with values", "[ai][search]")
{
    LinkSuggestion link;
    link.target_document = "architecture.md";
    link.anchor_text = "system architecture";
    link.relevance = 0.87;
    link.reason = "Related topic";
    CHECK(link.target_document == "architecture.md");
    CHECK(link.relevance > 0.8);
}

TEST_CASE("SemanticSearchResult defaults", "[ai][search]")
{
    SemanticSearchResult result;
    CHECK(result.document_id.empty());
    CHECK(result.relevance == 0.0);
    CHECK(result.explanation.empty());
    CHECK(result.snippet.empty());
}

TEST_CASE("SemanticSearchResult with values", "[ai][search]")
{
    SemanticSearchResult result;
    result.document_id = "doc-42";
    result.relevance = 0.92;
    result.explanation = "High semantic similarity";
    result.snippet = "...related concept found in paragraph 3...";
    CHECK(result.document_id == "doc-42");
    CHECK(result.relevance > 0.9);
    CHECK_FALSE(result.snippet.empty());
}

TEST_CASE("AIAnswerResult defaults", "[ai][search]")
{
    AIAnswerResult answer;
    CHECK(answer.answer.empty());
    CHECK(answer.source_documents.empty());
    CHECK(answer.confidence == 0.0);
}

TEST_CASE("AIAnswerResult with sources", "[ai][search]")
{
    AIAnswerResult answer;
    answer.answer = "The system uses event-driven architecture.";
    answer.source_documents = {"architecture.md", "events.md"};
    answer.confidence = 0.85;
    CHECK(answer.source_documents.size() == 2);
    CHECK(answer.confidence > 0.8);
}

TEST_CASE("AIWritingTone enum values are distinct", "[ai][search]")
{
    CHECK(AIWritingTone::kFormal != AIWritingTone::kCasual);
    CHECK(AIWritingTone::kTechnical != AIWritingTone::kFriendly);
    CHECK(AIWritingTone::kAcademic != AIWritingTone::kConcise);
}

TEST_CASE("AIAction enum has all expected values", "[ai][search]")
{
    CHECK(AIAction::Chat != AIAction::Summarize);
    CHECK(AIAction::Translate != AIAction::ExpandContent);
    CHECK(AIAction::FixGrammar != AIAction::GenerateOutline);
    CHECK(AIAction::ExplainCode != AIAction::ExtractKeywords);
    CHECK(AIAction::Custom != AIAction::Chat);
}
