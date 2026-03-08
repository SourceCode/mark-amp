/// @file IconCache.cpp
/// @brief V16 Phase 22 — LRU icon bitmap cache with memory budget enforcement.

#include "ui/IconCache.h"

namespace markamp::ui
{

auto IconCache::get(const IconCacheKey& key) -> std::optional<wxBitmap>
{
    auto iter = cache_.find(key);
    if (iter != cache_.end())
    {
        // Move to MRU position (front of list)
        lru_list_.splice(lru_list_.begin(), lru_list_, iter->second.lru_iter);
        ++hit_count_;
        return iter->second.bitmap;
    }
    ++miss_count_;
    return std::nullopt;
}

auto IconCache::put(const IconCacheKey& key, const wxBitmap& bmp) -> void
{
    if (!bmp.IsOk())
    {
        return;
    }

    auto existing = cache_.find(key);
    if (existing != cache_.end())
    {
        // Update existing entry: adjust memory, move to MRU
        total_bytes_ -= existing->second.estimated_bytes;
        existing->second.bitmap = bmp;
        existing->second.estimated_bytes = estimate_bitmap_bytes(bmp);
        total_bytes_ += existing->second.estimated_bytes;
        lru_list_.splice(lru_list_.begin(), lru_list_, existing->second.lru_iter);
    }
    else
    {
        // Insert new entry at MRU position
        lru_list_.push_front(key);
        CacheEntry entry;
        entry.bitmap = bmp;
        entry.lru_iter = lru_list_.begin();
        entry.estimated_bytes = estimate_bitmap_bytes(bmp);
        total_bytes_ += entry.estimated_bytes;
        cache_[key] = std::move(entry);
    }

    evict_if_needed();
}

auto IconCache::clear() -> void
{
    cache_.clear();
    lru_list_.clear();
    total_bytes_ = 0;
}

auto IconCache::size() const -> std::size_t
{
    return cache_.size();
}

auto IconCache::color_to_rgba(const wxColour& color) -> uint32_t
{
    if (!color.IsOk())
    {
        return 0;
    }
    return (static_cast<uint32_t>(color.Red()) << 24) |
           (static_cast<uint32_t>(color.Green()) << 16) |
           (static_cast<uint32_t>(color.Blue()) << 8) | (static_cast<uint32_t>(color.Alpha()));
}

// --- V16 Phase 22: Cache management ---

auto IconCache::set_max_entries(std::size_t max_entries) -> void
{
    max_entries_ = max_entries;
    evict_if_needed();
}

auto IconCache::set_memory_budget(std::size_t budget_bytes) -> void
{
    budget_bytes_ = budget_bytes;
    evict_if_needed();
}

auto IconCache::stats() const -> IconCacheStats
{
    IconCacheStats result;
    result.entry_count = cache_.size();
    result.total_bytes = total_bytes_;
    result.budget_bytes = budget_bytes_;
    result.hit_count = hit_count_;
    result.miss_count = miss_count_;
    result.eviction_count = eviction_count_;
    return result;
}

auto IconCache::reset_stats() -> void
{
    hit_count_ = 0;
    miss_count_ = 0;
    eviction_count_ = 0;
}

auto IconCache::estimate_bitmap_bytes(const wxBitmap& bmp) -> std::size_t
{
    if (!bmp.IsOk())
    {
        return 0;
    }
    // width * height * bytes_per_pixel (4 for RGBA)
    auto width = static_cast<std::size_t>(bmp.GetWidth());
    auto height = static_cast<std::size_t>(bmp.GetHeight());
    constexpr std::size_t kBytesPerPixelRgba = 4;
    return width * height * kBytesPerPixelRgba;
}

auto IconCache::evict_if_needed() -> void
{
    // Evict LRU entries until within limits
    while (!lru_list_.empty() && (cache_.size() > max_entries_ || total_bytes_ > budget_bytes_))
    {
        // Remove the LRU entry (back of list)
        const auto& lru_key = lru_list_.back();
        auto iter = cache_.find(lru_key);
        if (iter != cache_.end())
        {
            total_bytes_ -= iter->second.estimated_bytes;
            cache_.erase(iter);
        }
        lru_list_.pop_back();
        ++eviction_count_;
    }
}

} // namespace markamp::ui
