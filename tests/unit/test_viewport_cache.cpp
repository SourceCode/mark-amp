/// test_viewport_cache.cpp — Unit tests
#include "rendering/ViewportCache.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::rendering;

TEST_CASE("LRUCache: compiles", "[viewport_cache]")
{
    static_assert(sizeof(LRUCache<int, int>) > 0);
}

TEST_CASE("LazyCache: compiles", "[viewport_cache]")
{
    static_assert(sizeof(LazyCache<int>) > 0);
}

TEST_CASE("LineLayoutCache: compiles", "[viewport_cache]")
{
    static_assert(sizeof(LineLayoutCache) > 0);
}
