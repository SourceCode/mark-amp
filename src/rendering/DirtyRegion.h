#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>

namespace markamp::rendering
{

/// Axis-aligned rectangle for dirty region tracking.
struct Rect
{
    int32_t left{0};
    int32_t top{0};
    int32_t right{0};
    int32_t bottom{0};

    [[nodiscard]] auto width() const noexcept -> int32_t
    {
        return right - left;
    }
    [[nodiscard]] auto height() const noexcept -> int32_t
    {
        return bottom - top;
    }
    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return width() <= 0 || height() <= 0;
    }

    /// Return the union of this rect and another.
    [[nodiscard]] auto united(const Rect& other) const noexcept -> Rect
    {
        return Rect{std::min(left, other.left),
                    std::min(top, other.top),
                    std::max(right, other.right),
                    std::max(bottom, other.bottom)};
    }

    /// Check if two rects overlap.
    [[nodiscard]] auto intersects(const Rect& other) const noexcept -> bool
    {
        return left < other.right && right > other.left && top < other.bottom && bottom > other.top;
    }

    /// Check if two rects are "close enough" to merge (within proximity threshold).
    [[nodiscard]] auto is_near(const Rect& other, int32_t threshold) const noexcept -> bool
    {
        return (left - threshold) < other.right && (right + threshold) > other.left &&
               (top - threshold) < other.bottom && (bottom + threshold) > other.top;
    }
};

/// Accumulates and coalesces dirty rectangles.
///
/// When UI regions are invalidated, they are added here. Nearby
/// rects are merged to minimize the number of repaint calls.
/// Call consume() to retrieve and clear all accumulated rects.
///
/// Pattern implemented: #2 Incremental rendering — only update changed regions
class DirtyRegionAccumulator
{
public:
    static constexpr int32_t kDefaultMergeThreshold = 32; // pixels

    explicit DirtyRegionAccumulator(int32_t merge_threshold = kDefaultMergeThreshold)
        : merge_threshold_(merge_threshold)
    {
    }

    /// Mark a region as dirty.
    void invalidate(const Rect& rect)
    {
        if (rect.empty())
        {
            return;
        }

        // Try to merge with an existing rect
        for (auto& existing : rects_)
        {
            if (existing.is_near(rect, merge_threshold_))
            {
                existing = existing.united(rect);
                return;
            }
        }

        rects_.push_back(rect);
    }

    /// Retrieve all accumulated dirty regions and clear the accumulator.
    [[nodiscard]] auto consume() -> std::vector<Rect>
    {
        auto result = std::move(rects_);
        rects_.clear();
        return result;
    }

    /// Check if there are pending dirty regions.
    [[nodiscard]] auto has_dirty() const noexcept -> bool
    {
        return !rects_.empty();
    }

    /// Number of accumulated regions.
    [[nodiscard]] auto count() const noexcept -> std::size_t
    {
        return rects_.size();
    }

    /// Clear all accumulated regions without returning them.
    void clear() noexcept
    {
        rects_.clear();
    }

private:
    int32_t merge_threshold_;
    std::vector<Rect> rects_;
};

} // namespace markamp::rendering
