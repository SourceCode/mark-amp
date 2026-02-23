#include "ui/IconCache.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

TEST_CASE("IconCache: basic operations", "[icon_cache]")
{
    IconCache cache;

    CHECK(cache.size() == 0);

    wxColour c(255, 128, 64, 255);
    uint32_t rgba = IconCache::color_to_rgba(c);

    IconCacheKey key1{"test_icon", 24, 24, rgba, 1.0};
    IconCacheKey key2{"test_icon", 16, 16, rgba, 1.0};

    wxBitmap bmp1(24, 24, 32);
    wxBitmap bmp2(16, 16, 32);

    cache.put(key1, bmp1);
    CHECK(cache.size() == 1);

    auto retrieved1 = cache.get(key1);
    REQUIRE(retrieved1.has_value());
    CHECK(retrieved1->GetWidth() == 24);

    auto retrieved2 = cache.get(key2);
    REQUIRE(!retrieved2.has_value());

    cache.put(key2, bmp2);
    CHECK(cache.size() == 2);

    cache.clear();
    CHECK(cache.size() == 0);
}
