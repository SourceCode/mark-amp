#include "canvas/CanvasRenderer.h"
#include "canvas/TileCache.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

// ============================================================================
// TileCache Core Operations
// ============================================================================

TEST_CASE("TileCache basic insert and get", "[canvas][tilecache]")
{
    TileCache cache(64);

    SECTION("empty cache returns nullopt")
    {
        REQUIRE_FALSE(cache.get_tile({0, 0, 0}).has_value());
        REQUIRE(cache.size() == 0);
    }

    SECTION("put and get a tile")
    {
        const TileKey key{1, 2, 3};
        cache.put_tile(key);
        REQUIRE(cache.size() == 1);

        const auto entry = cache.get_tile(key);
        REQUIRE(entry.has_value());
        REQUIRE(entry->key == key);
        REQUIRE(entry->valid);
    }

    SECTION("multiple tiles at different positions")
    {
        cache.put_tile({0, 0, 0});
        cache.put_tile({1, 0, 0});
        cache.put_tile({0, 1, 0});
        REQUIRE(cache.size() == 3);

        REQUIRE(cache.get_tile({0, 0, 0}).has_value());
        REQUIRE(cache.get_tile({1, 0, 0}).has_value());
        REQUIRE(cache.get_tile({0, 1, 0}).has_value());
        REQUIRE_FALSE(cache.get_tile({5, 5, 0}).has_value());
    }
}

// ============================================================================
// TileCache LRU Eviction
// ============================================================================

TEST_CASE("TileCache LRU eviction", "[canvas][tilecache]")
{
    TileCache cache(3); // Very small cache.

    cache.put_tile({0, 0, 0});
    cache.put_tile({1, 0, 0});
    cache.put_tile({2, 0, 0});
    REQUIRE(cache.size() == 3);

    SECTION("adding beyond capacity evicts LRU")
    {
        // Access tile (0,0) to make it more recent.
        (void)cache.get_tile({0, 0, 0});

        // Insert a new tile, causing eviction.
        cache.put_tile({3, 0, 0});
        REQUIRE(cache.size() == 3);

        // Tile (1,0) should be evicted (it was accessed least recently).
        REQUIRE_FALSE(cache.get_tile({1, 0, 0}).has_value());

        // These should still exist.
        REQUIRE(cache.get_tile({0, 0, 0}).has_value());
        REQUIRE(cache.get_tile({2, 0, 0}).has_value());
        REQUIRE(cache.get_tile({3, 0, 0}).has_value());
    }
}

// ============================================================================
// TileCache Invalidation
// ============================================================================

TEST_CASE("TileCache invalidation", "[canvas][tilecache]")
{
    TileCache cache;

    cache.put_tile({0, 0, 0});
    cache.put_tile({1, 0, 0});
    cache.put_tile({0, 1, 0});

    SECTION("invalidate_all clears everything")
    {
        cache.invalidate_all();
        REQUIRE(cache.size() == 0);
    }

    SECTION("invalidate_region marks tiles invalid")
    {
        // The world-space region that corresponds to tile (0,0) at zoom level 0.
        const AABB region{0.0, 0.0, 100.0, 100.0};
        cache.invalidate_region(region, 0);

        // Tile (0,0) should be invalid (get_tile returns nullopt for invalid tiles).
        REQUIRE_FALSE(cache.get_tile({0, 0, 0}).has_value());
    }
}

// ============================================================================
// TileCache Viewport Queries
// ============================================================================

TEST_CASE("TileCache tiles_for_viewport", "[canvas][tilecache]")
{
    const TileCache cache;

    SECTION("small viewport at zoom level 0")
    {
        const AABB visible{0.0, 0.0, 512.0, 512.0};
        const auto tiles = cache.tiles_for_viewport(visible, 0);
        REQUIRE_FALSE(tiles.empty());
    }

    SECTION("all tiles have correct zoom level")
    {
        const AABB visible{0.0, 0.0, 1000.0, 1000.0};
        const auto tiles = cache.tiles_for_viewport(visible, 2);
        for (const auto& tile : tiles)
        {
            REQUIRE(tile.zoom_level == 2);
        }
    }
}

// ============================================================================
// TileCache Zoom Level Conversion
// ============================================================================

TEST_CASE("TileCache zoom_to_level", "[canvas][tilecache]")
{
    REQUIRE(TileCache::zoom_to_level(1.0) == 0);
    REQUIRE(TileCache::zoom_to_level(2.0) == 1);
    REQUIRE(TileCache::zoom_to_level(0.5) == -1);
    REQUIRE(TileCache::zoom_to_level(4.0) == 2);

    // Ensure clamping.
    REQUIRE(TileCache::zoom_to_level(0.01) >= -4);
    REQUIRE(TileCache::zoom_to_level(1000.0) <= 8);
}

// ============================================================================
// TileCache Grid Settings and Renderer Registration (pure logic, no wx)
// ============================================================================

TEST_CASE("CanvasRenderer GridSettings", "[canvas][renderer]")
{
    // Test GridSettings struct defaults.
    GridSettings settings;
    REQUIRE(settings.visible);
    REQUIRE(settings.spacing == 20.0);
    REQUIRE(settings.major_every == 5);
}

TEST_CASE("CanvasRenderer has_renderer", "[canvas][renderer]")
{
    CanvasRenderer renderer;
    REQUIRE_FALSE(renderer.has_renderer(CanvasObjectType::StickyNote));
    REQUIRE_FALSE(renderer.has_renderer(CanvasObjectType::Shape));
}

TEST_CASE("RenderStats defaults", "[canvas][renderer]")
{
    RenderStats stats;
    REQUIRE(stats.objects_rendered == 0);
    REQUIRE(stats.objects_culled == 0);
    REQUIRE(stats.frame_time_ms == 0.0);
}
