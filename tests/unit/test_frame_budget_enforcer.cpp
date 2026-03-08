/// test_frame_budget_enforcer.cpp — Unit tests for FrameBudgetEnforcer
#include "core/FrameBudgetEnforcer.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("FrameBudgetEnforcer: type compiles", "[frame_budget_enforcer]")
{
    static_assert(sizeof(FrameBudgetEnforcer) > 0);
}
