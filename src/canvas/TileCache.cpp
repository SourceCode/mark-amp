#include "TileCache.h"

#include <algorithm>
#include <cmath>

namespace markamp::canvas
{

TileCache::TileCache(size_t max_tiles)
    : max_tiles_(max_tiles)
{
}

auto TileCache::get_tile(const TileKey& key) -> std::optional<TileEntry>
{
    const auto it = tiles_.find(key);
    if (it == tiles_.end() || !it->second.valid)
    {
        return std::nullopt;
    }

    it->second.last_access = ++access_counter_;
    return it->second;
}

auto TileCache::put_tile(const TileKey& key) -> void
{
    TileEntry entry;
    entry.key = key;
    entry.valid = true;
    entry.last_access = ++access_counter_;
    tiles_[key] = entry;

    evict_if_needed();
}

auto TileCache::invalidate_region(const AABB& world_region, int zoom_level) -> void
{
    const double world_tile_size = static_cast<double>(kTileSize) / std::pow(2.0, zoom_level);

    const int min_col = static_cast<int>(std::floor(world_region.min_x / world_tile_size));
    const int max_col = static_cast<int>(std::floor(world_region.max_x / world_tile_size));
    const int min_row = static_cast<int>(std::floor(world_region.min_y / world_tile_size));
    const int max_row = static_cast<int>(std::floor(world_region.max_y / world_tile_size));

    for (int col = min_col; col <= max_col; ++col)
    {
        for (int row = min_row; row <= max_row; ++row)
        {
            TileKey key{col, row, zoom_level};
            auto it = tiles_.find(key);
            if (it != tiles_.end())
            {
                it->second.valid = false;
            }
        }
    }
}

auto TileCache::invalidate_all() -> void
{
    tiles_.clear();
}

auto TileCache::tiles_for_viewport(const AABB& visible_world, int zoom_level) const
    -> std::vector<TileKey>
{
    const double world_tile_size = static_cast<double>(kTileSize) / std::pow(2.0, zoom_level);

    const int min_col = static_cast<int>(std::floor(visible_world.min_x / world_tile_size));
    const int max_col = static_cast<int>(std::floor(visible_world.max_x / world_tile_size));
    const int min_row = static_cast<int>(std::floor(visible_world.min_y / world_tile_size));
    const int max_row = static_cast<int>(std::floor(visible_world.max_y / world_tile_size));

    std::vector<TileKey> keys;
    keys.reserve(static_cast<size_t>((max_col - min_col + 1) * (max_row - min_row + 1)));

    for (int col = min_col; col <= max_col; ++col)
    {
        for (int row = min_row; row <= max_row; ++row)
        {
            keys.push_back({col, row, zoom_level});
        }
    }

    return keys;
}

auto TileCache::evict_if_needed() -> void
{
    while (tiles_.size() > max_tiles_)
    {
        // Find the LRU tile.
        auto lru_it = tiles_.begin();
        for (auto it = tiles_.begin(); it != tiles_.end(); ++it)
        {
            if (it->second.last_access < lru_it->second.last_access)
            {
                lru_it = it;
            }
        }
        tiles_.erase(lru_it);
    }
}

auto TileCache::size() const -> size_t
{
    return tiles_.size();
}

auto TileCache::max_tiles() const -> size_t
{
    return max_tiles_;
}

auto TileCache::zoom_to_level(double zoom) -> int
{
    // Discrete zoom level: log2 of zoom clamped to reasonable range.
    return std::clamp(static_cast<int>(std::round(std::log2(zoom))), -4, 8);
}

} // namespace markamp::canvas
