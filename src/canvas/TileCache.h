#pragma once

#include "canvas/CanvasTypes.h"

#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>

namespace markamp::canvas
{

/// Key for a tile in the tile cache: column, row, and discrete zoom level.
struct TileKey
{
    int col{0};
    int row{0};
    int zoom_level{0};

    auto operator==(const TileKey& rhs) const -> bool = default;

    // ── Round 3 Batch 8 (#71) ───────────────────────────────────

    /// (#71) Whether this tile is at the origin.
    [[nodiscard]] auto is_origin() const noexcept -> bool
    {
        return col == 0 && row == 0;
    }
};

} // namespace markamp::canvas

// Hash for TileKey so it can be used in unordered_map.
template <>
struct std::hash<markamp::canvas::TileKey>
{
    auto operator()(const markamp::canvas::TileKey& key) const noexcept -> size_t
    {
        // Simple hash combining.
        size_t seed = std::hash<int>()(key.col);
        seed ^= std::hash<int>()(key.row) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<int>()(key.zoom_level) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};

namespace markamp::canvas
{

/// Cached tile entry — tracks validity and generation for LRU eviction.
struct TileEntry
{
    TileKey key;
    bool valid{true};
    uint64_t last_access{0};
    // In a real implementation, this would hold a wxBitmap or similar.
    // For now we track metadata only; the actual pixel data is managed by
    // the CanvasPanel's wxBitmap cache.

    /// Whether this tile entry is still valid.
    [[nodiscard]] auto is_valid_entry() const noexcept -> bool
    {
        return valid;
    }

    // ── Round 3 Batch 8 (#72) ───────────────────────────────────

    /// (#72) Whether this tile is stale (invalidated).
    [[nodiscard]] auto is_stale() const noexcept -> bool
    {
        return !valid;
    }
};

/// Off-screen tile cache for the canvas rendering engine.
/// Tiles are 256×256 pixel squares that cover the world space at a given zoom level.
/// The cache uses LRU eviction when the maximum tile count is exceeded.
class TileCache
{
public:
    static constexpr int kTileSize = 256;
    static constexpr size_t kDefaultMaxTiles = 256;

    explicit TileCache(size_t max_tiles = kDefaultMaxTiles);

    /// Get a tile entry, returning std::nullopt if not cached.
    [[nodiscard]] auto get_tile(const TileKey& key) -> std::optional<TileEntry>;

    /// Insert or update a tile entry.
    auto put_tile(const TileKey& key) -> void;

    /// Invalidate all tiles whose world-space rect overlaps the given region.
    auto invalidate_region(const AABB& world_region, int zoom_level) -> void;

    /// Invalidate all tiles.
    auto invalidate_all() -> void;

    /// Compute which tile keys are needed to cover the given viewport.
    [[nodiscard]] auto tiles_for_viewport(const AABB& visible_world, int zoom_level) const
        -> std::vector<TileKey>;

    /// Evict least-recently-used tiles if over capacity.
    auto evict_if_needed() -> void;

    /// Number of tiles currently cached.
    [[nodiscard]] auto size() const -> size_t;

    /// Maximum tiles allowed.
    [[nodiscard]] auto max_tiles() const -> size_t;

    /// Convert zoom factor to discrete zoom level (for tile grid alignment).
    [[nodiscard]] static auto zoom_to_level(double zoom) -> int;

    /// Whether the cache is empty.
    [[nodiscard]] auto is_empty() const noexcept -> bool
    {
        return tiles_.empty();
    }

    /// Whether the cache is at max capacity.
    [[nodiscard]] auto is_full() const noexcept -> bool
    {
        return tiles_.size() >= max_tiles_;
    }

    // ── Round 3 Batch 8 (#73-74) ────────────────────────────────

    /// (#73) Remaining tile capacity.
    [[nodiscard]] auto remaining_capacity() const noexcept -> size_t
    {
        return tiles_.size() < max_tiles_ ? max_tiles_ - tiles_.size() : 0;
    }

    /// (#74) Cache utilization ratio (0.0 to 1.0).
    [[nodiscard]] auto utilization() const noexcept -> double
    {
        return max_tiles_ > 0 ? static_cast<double>(tiles_.size()) / static_cast<double>(max_tiles_) : 0.0;
    }

private:
    std::unordered_map<TileKey, TileEntry> tiles_;
    size_t max_tiles_;
    uint64_t access_counter_{0};
};

} // namespace markamp::canvas
