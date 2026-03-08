/// test_force_directed_layout.cpp — Unit tests
#include "core/ForceDirectedLayout.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("ForceDirectedLayout: type compiles", "[force_directed_layout]")
{
    static_assert(sizeof(ForceDirectedLayout) > 0);
}
