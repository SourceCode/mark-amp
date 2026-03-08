/// @file test_v16_polish.cpp
/// @brief V16 Phases 21-23 — Tests for UI layout tokens, icon cache LRU/memory budget,
///        and resilience scenarios.

#include "ui/UILayoutTokens.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
// Icon cache tests require wxWidgets (wxBitmap), conditionally compile
#include "ui/IconCache.h"

using namespace markamp::ui;

// ============================================================================
// Phase 21: UILayoutTokens spacing scale
// ============================================================================

TEST_CASE("SpacingScale follows 4px base unit", "[layout][spacing]")
{
    REQUIRE(SpacingScale::kUnit == 4);
    REQUIRE(SpacingScale::kXxs == 2);
    REQUIRE(SpacingScale::kXs == 4);
    REQUIRE(SpacingScale::kSm == 8);
    REQUIRE(SpacingScale::kMd == 12);
    REQUIRE(SpacingScale::kLg == 16);
    REQUIRE(SpacingScale::kXl == 24);
    REQUIRE(SpacingScale::kXxl == 32);
    REQUIRE(SpacingScale::kXxxl == 48);
}

TEST_CASE("IconSizeTokens progressive scale", "[layout][icons]")
{
    REQUIRE(IconSizeTokens::kMicro < IconSizeTokens::kSmall);
    REQUIRE(IconSizeTokens::kSmall < IconSizeTokens::kMedium);
    REQUIRE(IconSizeTokens::kMedium < IconSizeTokens::kLarge);
    REQUIRE(IconSizeTokens::kLarge < IconSizeTokens::kXlarge);
    REQUIRE(IconSizeTokens::kXlarge < IconSizeTokens::kHero);
    REQUIRE(IconSizeTokens::kSmall == 16); // most common icon size
}

TEST_CASE("IconTextPairTokens gap ordering", "[layout][spacing]")
{
    REQUIRE(IconTextPairTokens::kGapTight < IconTextPairTokens::kGapStandard);
    REQUIRE(IconTextPairTokens::kGapStandard < IconTextPairTokens::kGapLoose);
}

TEST_CASE("DensePanelTokens row geometry", "[layout][panels]")
{
    REQUIRE(DensePanelTokens::kRowHeight > DensePanelTokens::kRowHeightCompact);
    REQUIRE(DensePanelTokens::kIndentWidth == 16); // matches standard icon width
    REQUIRE(DensePanelTokens::kMaxNestDepth >= 8); // reasonable nesting
}

TEST_CASE("ToolbarTokens touch target minimum", "[layout][toolbar]")
{
    // Minimum touch target 24px for accessibility
    REQUIRE(ToolbarTokens::kButtonSize >= 24);
    REQUIRE(ToolbarTokens::kButtonSizeCompact >= 24);
    REQUIRE(ToolbarTokens::kToolbarHeight >= ToolbarTokens::kButtonSize);
}

TEST_CASE("ContextMenuTokens dimensions", "[layout][menu]")
{
    REQUIRE(ContextMenuTokens::kItemHeight >= 24); // accessibility minimum
    REQUIRE(ContextMenuTokens::kMenuMinWidth < ContextMenuTokens::kMenuMaxWidth);
    REQUIRE(ContextMenuTokens::kIconColumnWidth >= IconSizeTokens::kMedium);
}

TEST_CASE("InteractionTimingTokens reduced motion", "[layout][a11y]")
{
    REQUIRE(InteractionTimingTokens::kReducedMotionMs == 0); // instant
    REQUIRE(InteractionTimingTokens::kTransitionFastMs <
            InteractionTimingTokens::kTransitionStandardMs);
    REQUIRE(InteractionTimingTokens::kTransitionStandardMs <
            InteractionTimingTokens::kTransitionSlowMs);
}

TEST_CASE("StatusBarTokens geometry", "[layout][status]")
{
    REQUIRE(StatusBarTokens::kHeight >= 20);
    REQUIRE(StatusBarTokens::kIconSize < StatusBarTokens::kHeight);
    REQUIRE(StatusBarTokens::kMaxVisibleItems >= 5);
}

TEST_CASE("EmptyStateTokens hero layout", "[layout][states]")
{
    REQUIRE(EmptyStateTokens::kIconSize == 48); // hero icon
    REQUIRE(EmptyStateTokens::kMaxWidth >= 200);
    REQUIRE(EmptyStateTokens::kSpinnerSize >= 16);
}

TEST_CASE("TabBarTokens constraints", "[layout][tabs]")
{
    REQUIRE(TabBarTokens::kTabMinWidth < TabBarTokens::kTabMaxWidth);
    REQUIRE(TabBarTokens::kTabHeight >= TabBarTokens::kCloseButtonSize);
    REQUIRE(TabBarTokens::kDirtyIndicatorSize > 0);
}

// ============================================================================
// Phase 22: IconCache LRU eviction & memory budget
// ============================================================================

TEST_CASE("IconCache basic put and get", "[cache][v16]")
{
    IconCache cache;
    IconCacheKey key{"test_icon", 16, 16, 0xFF0000FF, 1.0};

    REQUIRE(cache.size() == 0);
    REQUIRE_FALSE(cache.get(key).has_value());

    // Put a valid bitmap
    wxBitmap bmp(16, 16);
    cache.put(key, bmp);

    REQUIRE(cache.size() == 1);
    auto result = cache.get(key);
    REQUIRE(result.has_value());
}

TEST_CASE("IconCache rejects invalid bitmaps", "[cache][v16]")
{
    IconCache cache;
    IconCacheKey key{"bad", 0, 0, 0, 1.0};
    wxBitmap invalid_bmp; // default-constructed = not ok

    cache.put(key, invalid_bmp);
    REQUIRE(cache.size() == 0);
}

TEST_CASE("IconCache LRU eviction by max entries", "[cache][eviction][v16]")
{
    IconCache cache;
    cache.set_max_entries(3);

    // Insert 4 entries
    for (int idx = 0; idx < 4; ++idx)
    {
        IconCacheKey key{"icon_" + std::to_string(idx), 16, 16, 0, 1.0};
        wxBitmap bmp(16, 16);
        cache.put(key, bmp);
    }

    // Should have evicted the first entry
    REQUIRE(cache.size() == 3);

    // Entry 0 should be evicted (LRU)
    IconCacheKey key0{"icon_0", 16, 16, 0, 1.0};
    REQUIRE_FALSE(cache.get(key0).has_value());

    // Entries 1-3 should still be present
    for (int idx = 1; idx < 4; ++idx)
    {
        IconCacheKey key{"icon_" + std::to_string(idx), 16, 16, 0, 1.0};
        REQUIRE(cache.get(key).has_value());
    }
}

TEST_CASE("IconCache LRU promotes accessed entries", "[cache][eviction][v16]")
{
    IconCache cache;
    cache.set_max_entries(3);

    // Insert 3 entries
    for (int idx = 0; idx < 3; ++idx)
    {
        IconCacheKey key{"icon_" + std::to_string(idx), 16, 16, 0, 1.0};
        wxBitmap bmp(16, 16);
        cache.put(key, bmp);
    }

    // Access entry 0 to promote it to MRU
    IconCacheKey key0{"icon_0", 16, 16, 0, 1.0};
    cache.get(key0);

    // Insert a 4th entry — should evict entry 1 (now the LRU)
    IconCacheKey key3{"icon_3", 16, 16, 0, 1.0};
    wxBitmap bmp(16, 16);
    cache.put(key3, bmp);

    REQUIRE(cache.size() == 3);
    REQUIRE(cache.get(key0).has_value()); // promoted — still present
    IconCacheKey key1{"icon_1", 16, 16, 0, 1.0};
    REQUIRE_FALSE(cache.get(key1).has_value()); // evicted (was LRU)
}

TEST_CASE("IconCache memory budget enforcement", "[cache][memory][v16]")
{
    IconCache cache;
    // 16x16 RGBA = 1024 bytes each. Set budget to 2500 bytes (fits ~2 entries)
    cache.set_memory_budget(2500);
    cache.set_max_entries(100); // don't limit by count

    for (int idx = 0; idx < 4; ++idx)
    {
        IconCacheKey key{"icon_" + std::to_string(idx), 16, 16, 0, 1.0};
        wxBitmap bmp(16, 16);
        cache.put(key, bmp);
    }

    // Should have evicted until within budget
    auto current_stats = cache.stats();
    REQUIRE(current_stats.total_bytes <= 2500);
    REQUIRE(current_stats.eviction_count > 0);
}

TEST_CASE("IconCache statistics tracking", "[cache][stats][v16]")
{
    IconCache cache;
    IconCacheKey key{"stat_icon", 16, 16, 0, 1.0};
    wxBitmap bmp(16, 16);

    // Miss
    cache.get(key);
    auto stat = cache.stats();
    REQUIRE(stat.miss_count == 1);
    REQUIRE(stat.hit_count == 0);

    // Put + Hit
    cache.put(key, bmp);
    cache.get(key);
    stat = cache.stats();
    REQUIRE(stat.hit_count == 1);
    REQUIRE(stat.miss_count == 1);
    REQUIRE_THAT(stat.hit_rate(), Catch::Matchers::WithinAbs(0.5, 0.01));

    // Reset
    cache.reset_stats();
    stat = cache.stats();
    REQUIRE(stat.hit_count == 0);
    REQUIRE(stat.miss_count == 0);
}

TEST_CASE("IconCache estimate_bitmap_bytes", "[cache][memory][v16]")
{
    wxBitmap bmp(32, 32);
    auto bytes = IconCache::estimate_bitmap_bytes(bmp);
    // 32 * 32 * 4 = 4096
    REQUIRE(bytes == 4096);

    wxBitmap invalid_bmp;
    REQUIRE(IconCache::estimate_bitmap_bytes(invalid_bmp) == 0);
}

TEST_CASE("IconCache clear resets everything", "[cache][v16]")
{
    IconCache cache;
    IconCacheKey key{"clear_test", 16, 16, 0, 1.0};
    wxBitmap bmp(16, 16);
    cache.put(key, bmp);

    REQUIRE(cache.size() == 1);
    cache.clear();
    REQUIRE(cache.size() == 0);
    REQUIRE(cache.stats().total_bytes == 0);
}

TEST_CASE("IconCache update existing entry", "[cache][v16]")
{
    IconCache cache;
    IconCacheKey key{"update_test", 16, 16, 0, 1.0};
    wxBitmap bmp1(16, 16);
    wxBitmap bmp2(32, 32);

    cache.put(key, bmp1);
    auto bytes_before = cache.stats().total_bytes;

    // Re-put with larger bitmap
    cache.put(key, bmp2);
    REQUIRE(cache.size() == 1);                        // same entry, not a new one
    REQUIRE(cache.stats().total_bytes > bytes_before); // larger bitmap
}

// ============================================================================
// Phase 22: IconCacheStats
// ============================================================================

TEST_CASE("IconCacheStats hit_rate and budget_usage", "[cache][stats][v16]")
{
    IconCacheStats stats;
    stats.hit_count = 7;
    stats.miss_count = 3;
    stats.total_bytes = 16384;
    stats.budget_bytes = 32768;

    REQUIRE_THAT(stats.hit_rate(), Catch::Matchers::WithinAbs(0.7, 0.01));
    REQUIRE_THAT(stats.budget_usage(), Catch::Matchers::WithinAbs(0.5, 0.01));

    // Edge case: no accesses
    IconCacheStats empty;
    REQUIRE(empty.hit_rate() == 0.0);
    REQUIRE(empty.budget_usage() == 0.0);
}

// ============================================================================
// Phase 23: Resilience — icon cache under stress
// ============================================================================

TEST_CASE("IconCache handles high-volume inserts", "[cache][resilience][v16]")
{
    IconCache cache;
    cache.set_max_entries(100);

    // Insert 500 entries — should never exceed 100
    for (int idx = 0; idx < 500; ++idx)
    {
        IconCacheKey key{"stress_" + std::to_string(idx), 8, 8, 0, 1.0};
        wxBitmap bmp(8, 8);
        cache.put(key, bmp);
    }

    REQUIRE(cache.size() <= 100);
    REQUIRE(cache.stats().eviction_count >= 400);
}

TEST_CASE("IconCache handles duplicate keys gracefully", "[cache][resilience][v16]")
{
    IconCache cache;
    IconCacheKey key{"dupe", 16, 16, 0, 1.0};

    // Insert same key 100 times
    for (int idx = 0; idx < 100; ++idx)
    {
        wxBitmap bmp(16, 16);
        cache.put(key, bmp);
    }

    REQUIRE(cache.size() == 1); // only one entry
}

TEST_CASE("IconCache color_to_rgba round-trip", "[cache][resilience][v16]")
{
    wxColour color(0xAB, 0xCD, 0xEF, 0xFF);
    auto rgba = IconCache::color_to_rgba(color);
    REQUIRE(rgba == 0xABCDEFFF);

    wxColour invalid;
    REQUIRE(IconCache::color_to_rgba(invalid) == 0);
}
