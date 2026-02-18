/// test_a_i_types.cpp — Unit tests
#include "core/AITypes.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("AIProvider: enum values", "[a_i_types]")
{
    REQUIRE(static_cast<int>(AIProvider::OpenAI) != static_cast<int>(AIProvider::Anthropic));
}

TEST_CASE("AIAction: enum values", "[a_i_types]")
{
    REQUIRE(static_cast<int>(AIAction::Chat) != static_cast<int>(AIAction::Summarize));
}

TEST_CASE("AIRequest: compiles", "[a_i_types]")
{
    static_assert(sizeof(AIRequest) > 0);
}

TEST_CASE("AIResponse: compiles", "[a_i_types]")
{
    static_assert(sizeof(AIResponse) > 0);
}
