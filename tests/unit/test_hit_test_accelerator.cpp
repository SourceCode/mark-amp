/// test_hit_test_accelerator.cpp — Unit tests
#include "rendering/HitTestAccelerator.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::rendering;

TEST_CASE("LineAdvanceTable: compiles", "[hit_test_accelerator]")
{
    static_assert(sizeof(LineAdvanceTable) > 0);
}

TEST_CASE("HitTestAccelerator: compiles", "[hit_test_accelerator]")
{
    static_assert(sizeof(HitTestAccelerator) > 0);
}
