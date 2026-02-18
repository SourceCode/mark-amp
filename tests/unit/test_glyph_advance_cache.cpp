/// test_glyph_advance_cache.cpp — Unit tests
#include "rendering/GlyphAdvanceCache.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::rendering;

TEST_CASE("GlyphAdvanceCache: compiles", "[glyph_advance_cache]")
{
    static_assert(sizeof(GlyphAdvanceCache) > 0);
}

TEST_CASE("CommonWidthCache: compiles", "[glyph_advance_cache]")
{
    static_assert(sizeof(CommonWidthCache) > 0);
}
