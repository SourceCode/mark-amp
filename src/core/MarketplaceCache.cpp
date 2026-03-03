#include "MarketplaceCache.h"

#include "Logger.h"

#include <fstream>

namespace markamp::core
{

MarketplaceCache::MarketplaceCache(const std::filesystem::path& cache_dir)
    : cache_dir_(cache_dir)
{
    std::error_code error_code;
    std::filesystem::create_directories(cache_dir_, error_code);
    if (error_code)
    {
        MARKAMP_LOG_WARN("MarketplaceCache: Failed to create cache dir: {}", error_code.message());
    }
}

auto MarketplaceCache::GetSearchResults(const std::string& query) -> std::vector<GalleryExtension>
{
    std::lock_guard lock(mutex_);

    auto search_it = search_cache_.find(query);
    if (search_it != search_cache_.end())
    {
        if (!IsMemoryExpired(search_it->second.timestamp))
        {
            MARKAMP_LOG_INFO("MarketplaceCache: memory hit for '{}'", query);
            return search_it->second.results;
        }

        // Expired — remove from memory
        search_cache_.erase(search_it);
    }

    // Future: check disk cache
    return {};
}

void MarketplaceCache::PutSearchResults(const std::string& query,
                                        const std::vector<GalleryExtension>& results)
{
    std::lock_guard lock(mutex_);

    CacheEntry entry;
    entry.results = results;
    entry.timestamp = std::chrono::steady_clock::now();
    search_cache_[query] = std::move(entry);

    // Also cache individual extensions
    for (const auto& ext : results)
    {
        extension_cache_[ext.identifier] = ext;
    }
    extension_cache_time_ = std::chrono::steady_clock::now();

    MARKAMP_LOG_INFO("MarketplaceCache: cached {} results for '{}'", results.size(), query);
}

auto MarketplaceCache::GetExtension(const std::string& extension_id) -> GalleryExtension*
{
    std::lock_guard lock(mutex_);

    auto ext_it = extension_cache_.find(extension_id);
    if (ext_it != extension_cache_.end())
    {
        return &ext_it->second;
    }
    return nullptr;
}

void MarketplaceCache::PutExtension(const GalleryExtension& extension)
{
    std::lock_guard lock(mutex_);
    extension_cache_[extension.identifier] = extension;
}

void MarketplaceCache::Clear()
{
    std::lock_guard lock(mutex_);
    search_cache_.clear();
    extension_cache_.clear();

    // Clear disk cache
    std::error_code error_code;
    std::filesystem::remove_all(cache_dir_, error_code);
    std::filesystem::create_directories(cache_dir_, error_code);

    MARKAMP_LOG_INFO("MarketplaceCache: cleared all caches");
}

void MarketplaceCache::EvictExpired()
{
    std::lock_guard lock(mutex_);

    std::erase_if(search_cache_,
                  [this](const auto& pair) { return IsMemoryExpired(pair.second.timestamp); });
}

auto MarketplaceCache::DiskCacheSize() const -> std::int64_t
{
    std::int64_t total = 0;
    std::error_code error_code;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(cache_dir_, error_code))
    {
        if (entry.is_regular_file())
        {
            total += static_cast<std::int64_t>(entry.file_size());
        }
    }
    return total;
}

auto MarketplaceCache::IsMemoryExpired(const std::chrono::steady_clock::time_point& timestamp) const
    -> bool
{
    return (std::chrono::steady_clock::now() - timestamp) > kMemoryTtl;
}

} // namespace markamp::core
