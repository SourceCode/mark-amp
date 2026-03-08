/// test_frame_budget_token.cpp — Unit tests for FrameBudgetToken
#include "core/FrameBudgetToken.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("FrameBudgetToken: type compiles", "[frame_budget]")
{
    static_assert(sizeof(FrameBudgetToken) > 0);
}
