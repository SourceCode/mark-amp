/// test_grapheme_boundary_cache.cpp — Unit tests
#include "core/GraphemeBoundaryCache.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("GraphemeBoundaryCache: type compiles", "[grapheme_boundary_cache]")
{
    static_assert(sizeof(GraphemeBoundaryCache) > 0);
}
