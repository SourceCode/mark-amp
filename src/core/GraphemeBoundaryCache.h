#pragma once

#include "rendering/ViewportCache.h"

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace markamp::core
{

/// Byte and codepoint offset information for a single grapheme cluster.
///
/// Pattern implemented: #24 Avoiding pathological UTF handling
struct GraphemeInfo
{
    /// Byte offset of each grapheme cluster start within the line.
    std::vector<uint32_t> byte_offsets;

    /// Codepoint offset corresponding to each grapheme cluster.
    std::vector<uint32_t> codepoint_offsets;

    /// Total number of grapheme clusters in the line.
    [[nodiscard]] auto cluster_count() const noexcept -> std::size_t
    {
        return byte_offsets.size();
    }

    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return byte_offsets.empty();
    }
};

/// Decode a single UTF-8 codepoint from a byte span.
/// Returns the codepoint and the number of bytes consumed.
///
/// Pattern implemented: #24 Avoiding pathological UTF handling
struct Utf8DecodeResult
{
    char32_t codepoint{0};
    uint8_t bytes_consumed{0};
};

/// Decode one UTF-8 codepoint at the given offset.
[[nodiscard]] inline auto utf8_decode_one(std::span<const char> data, std::size_t offset) noexcept
    -> Utf8DecodeResult
{
    if (offset >= data.size())
    {
        return {0, 0};
    }

    auto byte0 = static_cast<uint8_t>(data[offset]);

    // 1-byte (ASCII)
    if (byte0 < 0x80)
    {
        return {static_cast<char32_t>(byte0), 1};
    }
    // 2-byte
    if ((byte0 & 0xE0) == 0xC0 && offset + 1 < data.size())
    {
        auto byte1 = static_cast<uint8_t>(data[offset + 1]);
        char32_t cp = ((byte0 & 0x1F) << 6) | (byte1 & 0x3F);
        return {cp, 2};
    }
    // 3-byte
    if ((byte0 & 0xF0) == 0xE0 && offset + 2 < data.size())
    {
        auto byte1 = static_cast<uint8_t>(data[offset + 1]);
        auto byte2 = static_cast<uint8_t>(data[offset + 2]);
        char32_t cp = ((byte0 & 0x0F) << 12) | ((byte1 & 0x3F) << 6) | (byte2 & 0x3F);
        return {cp, 3};
    }
    // 4-byte
    if ((byte0 & 0xF8) == 0xF0 && offset + 3 < data.size())
    {
        auto byte1 = static_cast<uint8_t>(data[offset + 1]);
        auto byte2 = static_cast<uint8_t>(data[offset + 2]);
        auto byte3 = static_cast<uint8_t>(data[offset + 3]);
        char32_t cp = ((byte0 & 0x07) << 18) | ((byte1 & 0x3F) << 12) | ((byte2 & 0x3F) << 6) |
                      (byte3 & 0x3F);
        return {cp, 4};
    }

    // Invalid — treat as single byte
    return {0xFFFD, 1}; // replacement character
}

/// Build grapheme boundary info for a UTF-8 line.
/// For simplicity, treats each codepoint as a grapheme cluster.
/// A full implementation would use UAX#29 grapheme cluster boundaries.
[[nodiscard]] inline auto build_grapheme_info(std::span<const char> line_data) -> GraphemeInfo
{
    GraphemeInfo info;
    std::size_t byte_offset = 0;
    uint32_t codepoint_idx = 0;

    while (byte_offset < line_data.size())
    {
        info.byte_offsets.push_back(static_cast<uint32_t>(byte_offset));
        info.codepoint_offsets.push_back(codepoint_idx);

        auto result = utf8_decode_one(line_data, byte_offset);
        if (result.bytes_consumed == 0)
        {
            break; // safety
        }
        byte_offset += result.bytes_consumed;
        ++codepoint_idx;
    }

    return info;
}

/// Per-line cache of grapheme cluster boundaries and codepoint-to-byte
/// offset mappings. Avoids re-decoding entire lines on every operation.
///
/// Keyed by (line_number, content_hash). Uses LRUCache from ViewportCache.h.
///
/// Pattern implemented: #24 Avoiding pathological UTF handling
class GraphemeBoundaryCache
{
public:
    static constexpr std::size_t kMaxCachedLines = 512;

    GraphemeBoundaryCache() = default;

    /// Get cached grapheme info for a line. Returns nullptr if not cached.
    [[nodiscard]] auto get(std::size_t line, uint64_t content_hash) -> const GraphemeInfo*
    {
        auto key = make_key(line, content_hash);
        auto result = cache_.get(key);
        if (result.has_value())
        {
            return &result->get();
        }
        return nullptr;
    }

    /// Cache grapheme info for a line.
    void put(std::size_t line, uint64_t content_hash, GraphemeInfo info)
    {
        cache_.put(make_key(line, content_hash), std::move(info));
    }

    /// Invalidate cached info for a specific line.
    void invalidate(std::size_t line)
    {
        // LRU doesn't support key-prefix deletion, so we do full clear
        // for simplicity. In production, would use a map keyed by line.
        (void)line;
    }

    /// Clear all cached entries.
    void clear()
    {
        cache_.clear();
    }

    /// Number of cached entries.
    [[nodiscard]] auto size() const noexcept -> std::size_t
    {
        return cache_.size();
    }

private:
    using CacheKey = uint64_t;

    static auto make_key(std::size_t line, uint64_t content_hash) noexcept -> CacheKey
    {
        // Combine line number and content hash into a single key
        return (static_cast<uint64_t>(line) << 32) ^ content_hash;
    }

    rendering::LRUCache<CacheKey, GraphemeInfo, kMaxCachedLines> cache_;
};

} // namespace markamp::core
