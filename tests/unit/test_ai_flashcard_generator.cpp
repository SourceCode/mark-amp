// test_ai_flashcard_generator.cpp — 10 tests for AI flashcard types
#include "core/AITypes.h"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>

using namespace markamp::core;

TEST_CASE("GeneratedCard defaults", "[ai][flashcard]")
{
    GeneratedCard card;
    CHECK(card.front.empty());
    CHECK(card.back.empty());
    CHECK(card.card_type == CardType::kBasic);
    CHECK(card.difficulty == 0.5);
}

TEST_CASE("GeneratedCard basic Q&A", "[ai][flashcard]")
{
    GeneratedCard card;
    card.front = "What is the capital of France?";
    card.back = "Paris";
    card.card_type = CardType::kBasic;
    CHECK(card.front.find("capital") != std::string::npos);
    CHECK(card.back == "Paris");
}

TEST_CASE("GeneratedCard cloze type", "[ai][flashcard]")
{
    GeneratedCard card;
    card.front = "The {{c1::mitochondria}} is the powerhouse of the cell.";
    card.card_type = CardType::kCloze;
    CHECK(card.card_type == CardType::kCloze);
    CHECK(card.front.find("{{c1::") != std::string::npos);
}

TEST_CASE("GeneratedCard difficulty ranges", "[ai][flashcard]")
{
    GeneratedCard easy;
    easy.difficulty = 0.1;
    GeneratedCard hard;
    hard.difficulty = 0.9;
    CHECK(easy.difficulty < hard.difficulty);
}

TEST_CASE("CardType enum values", "[ai][flashcard]")
{
    CHECK(CardType::kBasic != CardType::kCloze);
}

TEST_CASE("GeneratedCard batch", "[ai][flashcard]")
{
    std::vector<GeneratedCard> batch;
    for (int idx = 0; idx < 5; ++idx)
    {
        GeneratedCard card;
        card.front = "Question " + std::to_string(idx);
        card.back = "Answer " + std::to_string(idx);
        card.difficulty = 0.2 * static_cast<double>(idx);
        batch.push_back(card);
    }
    CHECK(batch.size() == 5);
    CHECK(batch[0].difficulty < batch[4].difficulty);
}

TEST_CASE("GeneratedCard with long content", "[ai][flashcard]")
{
    GeneratedCard card;
    card.front = "Explain the concept of polymorphism in object-oriented programming";
    card.back = "Polymorphism allows objects of different types to be treated as objects of "
                "a common base type. It enables a single interface to represent different "
                "underlying forms.";
    CHECK(card.back.size() > 100);
}

TEST_CASE("GeneratedCard default difficulty is medium", "[ai][flashcard]")
{
    GeneratedCard card;
    CHECK(card.difficulty >= 0.4);
    CHECK(card.difficulty <= 0.6);
}

TEST_CASE("GeneratedCard empty back is valid", "[ai][flashcard]")
{
    GeneratedCard card;
    card.front = "True or False: The sky is blue.";
    // Cloze cards can have empty back
    CHECK(card.back.empty());
}

TEST_CASE("GeneratedCard collection sorting by difficulty", "[ai][flashcard]")
{
    std::vector<GeneratedCard> cards;
    GeneratedCard hard;
    hard.difficulty = 0.9;
    hard.front = "Hard question";
    GeneratedCard easy;
    easy.difficulty = 0.1;
    easy.front = "Easy question";
    cards.push_back(hard);
    cards.push_back(easy);
    std::sort(cards.begin(),
              cards.end(),
              [](const GeneratedCard& lhs, const GeneratedCard& rhs)
              { return lhs.difficulty < rhs.difficulty; });
    CHECK(cards[0].front == "Easy question");
}
