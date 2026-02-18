/// test_ref_resolver.cpp — Unit tests
#include "core/RefResolver.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("RefResolver: compiles", "[ref_resolver]")
{
    static_assert(sizeof(RefResolver) > 0);
}
