/// test_diff_engine.cpp — Unit tests

#include "core/DiffEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("DiffEngine: compiles", "[diff_engine]")
{
    static_assert(sizeof(DiffEngine) > 0);
}
