/// test_deck_store.cpp
#include "core/DeckStore.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("IDeckStore: type compiles", "[deck_store]")
{
    static_assert(sizeof(IDeckStore) > 0);
}
