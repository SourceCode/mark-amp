/// test_frame_budget_token.cpp — Unit tests
#include "core/FrameBudgetToken.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("FrameBudgetToken: compiles", "[frame_budget_token]")
{
    static_assert(sizeof(FrameBudgetToken) > 0);
}
