/// test_chaos_engine.cpp — Unit tests

#include "core/ChaosEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ChaosEngine: compiles", "[chaos_engine]")
{
    static_assert(sizeof(ChaosEngine) > 0);
}
