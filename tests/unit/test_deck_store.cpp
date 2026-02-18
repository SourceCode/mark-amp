/// test_deck_store.cpp — Unit tests
#include "core/DeckStore.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("JsonDeckStore: compiles", "[deck_store]")
{
    static_assert(sizeof(JsonDeckStore) > 0);
}
