#pragma once

#include <array>
#include <cstdint>
#include <unordered_map>

namespace markamp::rendering
{

/// Cached glyph advance width for a (codepoint, font_id) pair.
///
/// Uses a flat array for ASCII (fast path) and unordered_map for
/// non-ASCII fallback.
///
/// Pattern implemented: #28 Fast text measurement via glyph advance caching
class GlyphAdvanceCache
{
public:
    static constexpr std::size_t kAsciiSize = 128;
    static constexpr int32_t kInvalidAdvance = -1;

    GlyphAdvanceCache()
    {
        // Initialize ASCII cache to invalid
        for (auto& row : ascii_cache_)
        {
            row.fill(kInvalidAdvance);
        }
    }

    /// Get the cached advance for a codepoint + font pair.
    /// Returns kInvalidAdvance if not cached (caller should measure and put).
    [[nodiscard]] auto get(char32_t codepoint, uint16_t font_id) const noexcept -> int32_t
    {
        if (codepoint < kAsciiSize && font_id < kMaxFonts)
        {
            return ascii_cache_[font_id][codepoint];
        }
        auto key = make_key(codepoint, font_id);
        auto iter = extended_cache_.find(key);
        return (iter != extended_cache_.end()) ? iter->second : kInvalidAdvance;
    }

    /// Cache the advance for a codepoint + font pair.
    void put(char32_t codepoint, uint16_t font_id, int32_t advance) noexcept
    {
        if (codepoint < kAsciiSize && font_id < kMaxFonts)
        {
            ascii_cache_[font_id][codepoint] = advance;
        }
        else
        {
            extended_cache_[make_key(codepoint, font_id)] = advance;
        }
    }

    /// Measure a text run by accumulating cached advances.
    /// Returns the total width and the number of cache hits.
    /// Characters with kInvalidAdvance are counted as misses.
    struct MeasureResult
    {
        int32_t total_width{0};
        std::size_t hits{0};
        std::size_t misses{0};
    };

    [[nodiscard]] auto measure_run_ascii(const char* data,
                                         std::size_t length,
                                         uint16_t font_id) const noexcept -> MeasureResult
    {
        MeasureResult result;
        if (font_id >= kMaxFonts)
        {
            result.misses = length;
            return result;
        }

        for (std::size_t idx = 0; idx < length; ++idx)
        {
            auto ch = static_cast<uint8_t>(data[idx]);
            if (ch < kAsciiSize)
            {
                auto advance = ascii_cache_[font_id][ch];
                if (advance != kInvalidAdvance)
                {
                    result.total_width += advance;
                    ++result.hits;
                    continue;
                }
            }
            ++result.misses;
        }
        return result;
    }

    /// Invalidate all cached advances for a specific font
    /// (e.g., after font size change).
    void invalidate_font(uint16_t font_id)
    {
        if (font_id < kMaxFonts)
        {
            ascii_cache_[font_id].fill(kInvalidAdvance);
        }
        // Remove extended entries for this font
        for (auto iter = extended_cache_.begin(); iter != extended_cache_.end();)
        {
            if (get_font_from_key(iter->first) == font_id)
            {
                iter = extended_cache_.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }

    /// Clear all cached advances.
    void clear()
    {
        for (auto& row : ascii_cache_)
        {
            row.fill(kInvalidAdvance);
        }
        extended_cache_.clear();
    }

    /// Number of extended (non-ASCII) entries.
    [[nodiscard]] auto extended_count() const noexcept -> std::size_t
    {
        return extended_cache_.size();
    }

private:
    static constexpr uint16_t kMaxFonts = 8;

    using ExtendedKey = uint64_t;

    static auto make_key(char32_t codepoint, uint16_t font_id) noexcept -> ExtendedKey
    {
        return (static_cast<uint64_t>(font_id) << 32) | static_cast<uint64_t>(codepoint);
    }

    static auto get_font_from_key(ExtendedKey key) noexcept -> uint16_t
    {
        return static_cast<uint16_t>(key >> 32);
    }

    // Fast path: indexed by [font_id][ascii_codepoint]
    std::array<std::array<int32_t, kAsciiSize>, kMaxFonts> ascii_cache_;

    // Fallback for non-ASCII
    std::unordered_map<ExtendedKey, int32_t> extended_cache_;
};

/// Cache for common token/substring widths to avoid redundant measurement.
///
/// Pattern implemented: #28 Fast text measurement via glyph advance caching
class CommonWidthCache
{
public:
    /// Get cached width for a content hash + font combination.
    [[nodiscard]] auto get(uint64_t content_hash, uint16_t font_id) const noexcept -> int32_t
    {
        auto key = make_key(content_hash, font_id);
        auto iter = cache_.find(key);
        return (iter != cache_.end()) ? iter->second : GlyphAdvanceCache::kInvalidAdvance;
    }

    /// Cache a width for a content hash + font combination.
    void put(uint64_t content_hash, uint16_t font_id, int32_t width)
    {
        cache_[make_key(content_hash, font_id)] = width;
    }

    /// Clear all entries.
    void clear()
    {
        cache_.clear();
    }

    [[nodiscard]] auto size() const noexcept -> std::size_t
    {
        return cache_.size();
    }

private:
    using CacheKey = uint64_t;

    static auto make_key(uint64_t content_hash, uint16_t font_id) noexcept -> CacheKey
    {
        return (static_cast<uint64_t>(font_id) << 48) ^ content_hash;
    }

    std::unordered_map<CacheKey, int32_t> cache_;
};

} // namespace markamp::rendering
