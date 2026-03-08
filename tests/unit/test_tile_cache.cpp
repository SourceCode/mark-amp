/// test_tile_cache.cpp
#include "canvas/TileCache.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;
TEST_CASE("TileCache: type compiles", "[tile_cache]")
{
    static_assert(sizeof(TileCache) > 0);
}
