/// test_math_engine.cpp
#include "core/MathEngine.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("MathEngine: type compiles", "[math_engine]")
{
    static_assert(sizeof(MathEngine) > 0);
}
