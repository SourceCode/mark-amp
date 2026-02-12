#pragma once

#include "ViewportCache.h"
#include "core/StableLineId.h"

#include <cstdint>
#include <vector>

namespace markamp::rendering
{

/// Result of wrapping a single logical line into display lines.
///
/// Pattern implemented: #27 Line wrapping computed lazily and incrementally
struct WrapResult
{
    /// Byte offsets within the line where wraps occur.
    /// If empty, the line fits on a single display line.
    std::vector<uint32_t> break_offsets;

    /// The wrap width used when computing this result.
    uint32_t wrap_width{0};

    /// Number of display lines this logical line occupies.
    [[nodiscard]] auto display_line_count() const noexcept -> std::size_t
    {
        return break_offsets.size() + 1;
    }

    /// Whether the line wraps at all.
    [[nodiscard]] auto is_wrapped() const noexcept -> bool
    {
        return !break_offsets.empty();
    }
};

/// Per-line incremental wrap cache. Wraps only visible + impacted lines,
/// caching results keyed by (line content hash, wrap_width).
///
/// Full rewrap only when wrap width changes. After an edit, re-wrap the
/// edited line and propagate downward until wrap state stabilizes.
///
/// Pattern implemented: #27 Line wrapping computed lazily and incrementally
class IncrementalLineWrap
{
public:
    IncrementalLineWrap() = default;

    /// Get cached wrap result for a line. Returns nullptr if not cached
    /// or if wrap_width has changed.
    [[nodiscard]] auto get(std::size_t line, uint32_t wrap_width) const -> const WrapResult*
    {
        if (line < cache_.size() && cache_[line].wrap_width == wrap_width &&
            !cache_[line].break_offsets.empty())
        {
            return &cache_[line];
        }
        // Also return valid result for non-wrapping lines
        if (line < cache_.size() && cache_[line].wrap_width == wrap_width)
        {
            return &cache_[line];
        }
        return nullptr;
    }

    /// Store a wrap result for a line.
    void set(std::size_t line, WrapResult result)
    {
        ensure_capacity(line);
        cache_[line] = std::move(result);
    }

    /// Invalidate a single line (e.g., after edit).
    void invalidate_line(std::size_t line)
    {
        if (line < cache_.size())
        {
            cache_[line] = WrapResult{};
        }
    }

    /// Invalidate a range of lines.
    void invalidate_range(std::size_t start_line, std::size_t end_line)
    {
        for (auto line = start_line; line < end_line && line < cache_.size(); ++line)
        {
            cache_[line] = WrapResult{};
        }
    }

    /// Invalidate all cached wrap results (e.g., wrap width changed).
    void invalidate_all()
    {
        cache_.clear();
    }

    /// Total cached line count.
    [[nodiscard]] auto size() const noexcept -> std::size_t
    {
        return cache_.size();
    }

    /// Check if a line has a valid cached result for the given wrap width.
    [[nodiscard]] auto is_valid(std::size_t line, uint32_t wrap_width) const noexcept -> bool
    {
        return line < cache_.size() && cache_[line].wrap_width == wrap_width;
    }

    /// Total number of display lines across all cached logical lines for a given wrap width.
    [[nodiscard]] auto total_display_lines(uint32_t wrap_width) const noexcept -> std::size_t
    {
        std::size_t total = 0;
        for (const auto& entry : cache_)
        {
            if (entry.wrap_width == wrap_width)
            {
                total += entry.display_line_count();
            }
            else
            {
                total += 1; // assume 1 display line for uncached/mismatched
            }
        }
        return total;
    }

private:
    std::vector<WrapResult> cache_;

    void ensure_capacity(std::size_t line)
    {
        if (line >= cache_.size())
        {
            cache_.resize(line + 1);
        }
    }
};

} // namespace markamp::rendering
