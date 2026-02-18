/// test_card_factory.cpp — Unit tests
#include "core/CardFactory.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("CardFormat: enum", "[card_factory]")
{
    REQUIRE(static_cast<int>(CardFormat::ListBased) >= 0);
}

TEST_CASE("CardContent: compiles", "[card_factory]")
{
    static_assert(sizeof(CardContent) > 0);
}

TEST_CASE("CardFactory: compiles", "[card_factory]")
{
    static_assert(sizeof(CardFactory) > 0);
}
