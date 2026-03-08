/// test_flashcard_types.cpp — Unit tests for FlashcardTypes
#include "core/fsrs/FSRSTypes.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("FSRSTypes: Card default values", "[flashcard_types]")
{
    fsrs::Card card;
    REQUIRE(card.stability == 0.0f);
    REQUIRE(card.difficulty == 0.0f);
}
