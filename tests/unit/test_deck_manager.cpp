/// test_deck_manager.cpp — Unit tests for DeckManager
#include "core/DeckManager.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("DeckManager: type compiles", "[deck_manager]")
{
    static_assert(sizeof(DeckManager) > 0);
}
