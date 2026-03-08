// test_ai_document_generator.cpp — 10 tests for AIDocumentGenerator (static methods + types)
#include "core/AIDocumentGenerator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("AIDocumentGenerator style_name Article", "[ai][docgen]")
{
    auto name = AIDocumentGenerator::style_name(DocumentStyle::kArticle);
    CHECK_FALSE(name.empty());
}

TEST_CASE("AIDocumentGenerator style_name Notes", "[ai][docgen]")
{
    auto name = AIDocumentGenerator::style_name(DocumentStyle::kNotes);
    CHECK_FALSE(name.empty());
}

TEST_CASE("AIDocumentGenerator style_name Outline", "[ai][docgen]")
{
    auto name = AIDocumentGenerator::style_name(DocumentStyle::kOutline);
    CHECK_FALSE(name.empty());
}

TEST_CASE("AIDocumentGenerator style_name Report", "[ai][docgen]")
{
    auto name = AIDocumentGenerator::style_name(DocumentStyle::kReport);
    CHECK_FALSE(name.empty());
}

TEST_CASE("AIDocumentGenerator style_name returns unique names", "[ai][docgen]")
{
    auto article = AIDocumentGenerator::style_name(DocumentStyle::kArticle);
    auto notes = AIDocumentGenerator::style_name(DocumentStyle::kNotes);
    auto outline = AIDocumentGenerator::style_name(DocumentStyle::kOutline);
    auto report = AIDocumentGenerator::style_name(DocumentStyle::kReport);
    CHECK(article != notes);
    CHECK(outline != report);
    CHECK(article != outline);
}

TEST_CASE("GenerationState enum values", "[ai][docgen]")
{
    CHECK(GenerationState::kIdle != GenerationState::kGenerating);
    CHECK(GenerationState::kDone != GenerationState::kError);
    CHECK(GenerationState::kCancelled != GenerationState::kIdle);
}

TEST_CASE("GenerationStatus tracks progress", "[ai][docgen]")
{
    GenerationStatus status;
    status.state = GenerationState::kGenerating;
    status.progress = 0.5;
    status.content_so_far = "# Draft\n\nPartial content...";
    CHECK(status.state == GenerationState::kGenerating);
    CHECK(status.progress == 0.5);
    CHECK_FALSE(status.content_so_far.empty());
}

TEST_CASE("GenerationStatus error state", "[ai][docgen]")
{
    GenerationStatus status;
    status.state = GenerationState::kError;
    status.error_message = "API rate limit exceeded";
    CHECK(status.state == GenerationState::kError);
    CHECK(status.error_message == "API rate limit exceeded");
}

TEST_CASE("DocumentStyle enum distinct values", "[ai][docgen]")
{
    CHECK(static_cast<uint8_t>(DocumentStyle::kArticle) !=
          static_cast<uint8_t>(DocumentStyle::kNotes));
    CHECK(static_cast<uint8_t>(DocumentStyle::kOutline) !=
          static_cast<uint8_t>(DocumentStyle::kReport));
}

TEST_CASE("GeneratedCard defaults", "[ai][docgen]")
{
    GeneratedCard card;
    CHECK(card.front.empty());
    CHECK(card.back.empty());
    CHECK(card.card_type == CardType::kBasic);
    CHECK(card.difficulty == 0.5);
}
