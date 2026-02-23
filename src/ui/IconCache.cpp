#include "ui/IconCache.h"

namespace markamp::ui
{

auto IconCache::get(const IconCacheKey& key) const -> std::optional<wxBitmap>
{
    auto it = cache_.find(key);
    if (it != cache_.end())
    {
        return it->second;
    }
    return std::nullopt;
}

auto IconCache::put(const IconCacheKey& key, const wxBitmap& bmp) -> void
{
    if (bmp.IsOk())
    {
        cache_[key] = bmp;
    }
}

auto IconCache::clear() -> void
{
    cache_.clear();
}

auto IconCache::size() const -> size_t
{
    return cache_.size();
}

auto IconCache::color_to_rgba(const wxColour& c) -> uint32_t
{
    if (!c.IsOk())
        return 0;
    return (static_cast<uint32_t>(c.Red()) << 24) | (static_cast<uint32_t>(c.Green()) << 16) |
           (static_cast<uint32_t>(c.Blue()) << 8) | (static_cast<uint32_t>(c.Alpha()));
}

} // namespace markamp::ui
