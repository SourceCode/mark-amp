/// test_chaos_plugin.cpp — Unit tests
#include "core/ChaosPlugin.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ChaosBehavior: enum values", "[chaos_plugin]")
{
    REQUIRE(static_cast<int>(ChaosBehavior::kNone) != static_cast<int>(ChaosBehavior::kThrowOnActivate));
}

TEST_CASE("ChaosBehavior: compiles", "[chaos_plugin]")
{
    static_assert(sizeof(ChaosBehavior) > 0);
}
