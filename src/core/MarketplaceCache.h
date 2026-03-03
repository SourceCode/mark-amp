#pragma once

#include "GalleryService.h"

#include <chrono>
#include <filesystem>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Phase 20 Task 22: Multi-level marketplace cache.
/// Memory cache (5-min TTL) + disk cache (24-hour TTL) + offline mode.
class MarketplaceCache
{
public:
    explicit MarketplaceCache(const std::filesystem::path& cache_dir);

    /// Look up cached search results. Returns empty if cache miss.
    [[nodiscard]] auto GetSearchResults(const std::string& query) -> std::vector<GalleryExtension>;

    /// Store search results in memory + disk cache.
    void PutSearchResults(const std::string& query, const std::vector<GalleryExtension>& results);

    /// Get cached extension metadata by ID.
    [[nodiscard]] auto GetExtension(const std::string& extension_id) -> GalleryExtension*;

    /// Store extension metadata.
    void PutExtension(const GalleryExtension& extension);

    /// Clear all cached data (memory + disk).
    void Clear();

    /// Evict expired entries from memory cache.
    void EvictExpired();

    /// Get total disk cache size in bytes.
    [[nodiscard]] auto DiskCacheSize() const -> std::int64_t;

    /// Maximum disk cache size (50 MB).
    static constexpr std::int64_t kMaxDiskCacheBytes = 50 * 1024 * 1024;

    /// Memory TTL: 5 minutes.
    static constexpr auto kMemoryTtl = std::chrono::minutes(5);

    /// Disk TTL: 24 hours.
    static constexpr auto kDiskTtl = std::chrono::hours(24);

private:
    struct CacheEntry
    {
        std::vector<GalleryExtension> results;
        std::chrono::steady_clock::time_point timestamp;
    };

    std::filesystem::path cache_dir_;
    mutable std::mutex mutex_;

    // Memory cache: query -> results
    std::unordered_map<std::string, CacheEntry> search_cache_;

    // Extension metadata cache
    std::unordered_map<std::string, GalleryExtension> extension_cache_;
    std::chrono::steady_clock::time_point extension_cache_time_;

    [[nodiscard]] auto IsMemoryExpired(const std::chrono::steady_clock::time_point& timestamp) const
        -> bool;
};

} // namespace markamp::core
