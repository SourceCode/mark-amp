/// @file IconCache.h
/// @brief V16 Phase 22 — Rendered icon bitmap cache with LRU eviction and memory budget.

#pragma once

#include <wx/bitmap.h>
#include <wx/colour.h>
#include <wx/gdicmn.h>

#include <cstdint>
#include <list>
#include <optional>
#include <string>
#include <unordered_map>

namespace markamp::ui
{

/// Composite key uniquely identifying a rendered icon parameters.
struct IconCacheKey
{
    std::string icon_name;
    int size_x{0};
    int size_y{0};
    uint32_t color_rgba{0};
    double scale{1.0};

    bool operator==(const IconCacheKey& other) const
    {
        return icon_name == other.icon_name && size_x == other.size_x && size_y == other.size_y &&
               color_rgba == other.color_rgba && scale == other.scale;
    }
};

} // namespace markamp::ui

namespace std
{
template <>
struct hash<markamp::ui::IconCacheKey>
{
    size_t operator()(const markamp::ui::IconCacheKey& key) const
    {
        // Simple hash combination
        size_t h1 = std::hash<std::string>{}(key.icon_name);
        size_t h2 = std::hash<int>{}(key.size_x) ^ (std::hash<int>{}(key.size_y) << 1);
        size_t h3 = std::hash<uint32_t>{}(key.color_rgba);
        size_t h4 = std::hash<double>{}(key.scale);

        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
    }
};
} // namespace std

namespace markamp::ui
{

/// V16 Phase 22: Cache statistics for monitoring and diagnostics.
struct IconCacheStats
{
    std::size_t entry_count{0};    ///< Number of cached entries
    std::size_t total_bytes{0};    ///< Estimated total memory usage
    std::size_t budget_bytes{0};   ///< Configured memory budget
    std::size_t hit_count{0};      ///< Cache hits since last reset
    std::size_t miss_count{0};     ///< Cache misses since last reset
    std::size_t eviction_count{0}; ///< Total evictions performed

    [[nodiscard]] auto hit_rate() const -> double
    {
        auto total = hit_count + miss_count;
        return total > 0 ? static_cast<double>(hit_count) / static_cast<double>(total) : 0.0;
    }

    [[nodiscard]] auto budget_usage() const -> double
    {
        return budget_bytes > 0
                   ? static_cast<double>(total_bytes) / static_cast<double>(budget_bytes)
                   : 0.0;
    }
};

/// Caches fully rendered icon bitmaps with LRU eviction and memory budget.
///
/// V16 Phase 22 enhancements:
/// - LRU eviction: Least-recently-used entries are evicted when the cache exceeds
///   the max entry count or memory budget.
/// - Memory budget: Estimated bitmap memory is tracked; entries are evicted when
///   the total exceeds the configured budget.
/// - Statistics: Hit/miss/eviction counters for performance monitoring.
class IconCache
{
public:
    /// Default memory budget: 32 MB
    static constexpr std::size_t kDefaultBudgetBytes = 32ULL * 1024ULL * 1024ULL;
    /// Default max entries
    static constexpr std::size_t kDefaultMaxEntries = 2048;

    IconCache() = default;
    ~IconCache() = default;

    // Non-copyable, movable
    IconCache(const IconCache&) = delete;
    auto operator=(const IconCache&) -> IconCache& = delete;
    IconCache(IconCache&&) = default;
    auto operator=(IconCache&&) -> IconCache& = default;

    /// Retrieves an icon from the cache if it exists.
    /// Moves the entry to MRU position on hit.
    [[nodiscard]] auto get(const IconCacheKey& key) -> std::optional<wxBitmap>;

    /// Stores a new icon in the cache.
    /// May trigger eviction if budget or max entries exceeded.
    auto put(const IconCacheKey& key, const wxBitmap& bmp) -> void;

    /// Clears the entire cache.
    auto clear() -> void;

    /// Returns the number of cached items.
    [[nodiscard]] auto size() const -> std::size_t;

    /// Helper to construct a color rgba integer from wxColour
    [[nodiscard]] static auto color_to_rgba(const wxColour& color) -> uint32_t;

    // --- V16 Phase 22: Cache management ---

    /// Set the maximum number of cached entries.
    auto set_max_entries(std::size_t max_entries) -> void;

    /// Set the memory budget in bytes.
    auto set_memory_budget(std::size_t budget_bytes) -> void;

    /// Get current cache statistics.
    [[nodiscard]] auto stats() const -> IconCacheStats;

    /// Reset hit/miss/eviction counters.
    auto reset_stats() -> void;

    /// Estimate memory usage of a single bitmap.
    [[nodiscard]] static auto estimate_bitmap_bytes(const wxBitmap& bmp) -> std::size_t;

private:
    /// LRU eviction: evict least-recently-used entries until within budget.
    auto evict_if_needed() -> void;

    /// LRU list: front = MRU, back = LRU
    using LruList = std::list<IconCacheKey>;
    LruList lru_list_;

    /// Map from key to (bitmap, iterator-into-lru-list)
    struct CacheEntry
    {
        wxBitmap bitmap;
        LruList::iterator lru_iter;
        std::size_t estimated_bytes{0};
    };
    std::unordered_map<IconCacheKey, CacheEntry> cache_;

    // Budget limits
    std::size_t max_entries_{kDefaultMaxEntries};
    std::size_t budget_bytes_{kDefaultBudgetBytes};
    std::size_t total_bytes_{0};

    // Statistics
    mutable std::size_t hit_count_{0};
    mutable std::size_t miss_count_{0};
    std::size_t eviction_count_{0};
};

} // namespace markamp::ui
