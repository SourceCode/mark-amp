/// test_bounded_container.cpp — Unit tests
#include "core/BoundedContainer.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("BoundedString: compiles", "[bounded_container]")
{
    static_assert(sizeof(BoundedString) > 0);
}
