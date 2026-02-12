#pragma once

#include "DirtyRegion.h"

#include <algorithm>
#include <cstdint>
#include <functional>

namespace markamp::rendering
{

/// Scroll direction for predictive prefetch cancellation.
///
/// Pattern implemented: #30 Tuned repaint strategy for scrolling
enum class ScrollDirection : uint8_t
{
    None,
    Up,
    Down,
    Left,
    Right
};

/// Pixel-shift existing content and paint only newly revealed areas
/// during scroll. Avoids repainting the entire viewport.
///
/// Pattern implemented: #30 Tuned repaint strategy for scrolling
class ScrollBlit
{
public:
    ScrollBlit() = default;

    /// Initialize with back buffer dimensions.
    void initialize(int32_t width, int32_t height) noexcept
    {
        width_ = width;
        height_ = height;
    }

    /// Compute the revealed rectangle after a vertical scroll by `dy` pixels.
    /// dy > 0 = scroll down (content moves up), revealed strip at bottom.
    /// dy < 0 = scroll up (content moves down), revealed strip at top.
    /// Returns Rect in {left, top, right, bottom} format.
    [[nodiscard]] auto compute_revealed_rect(int32_t dy) const noexcept -> Rect
    {
        if (dy == 0)
        {
            return Rect{0, 0, 0, 0};
        }

        auto abs_dy = std::abs(dy);
        if (abs_dy >= height_)
        {
            // Full repaint — scrolled more than viewport
            return Rect{0, 0, width_, height_};
        }

        if (dy > 0)
        {
            // Scrolled down: reveal at bottom
            return Rect{0, height_ - abs_dy, width_, height_};
        }
        // Scrolled up: reveal at top
        return Rect{0, 0, width_, abs_dy};
    }

    /// Compute the revealed rectangle after a horizontal scroll by `dx` pixels.
    [[nodiscard]] auto compute_revealed_rect_horizontal(int32_t dx) const noexcept -> Rect
    {
        if (dx == 0)
        {
            return Rect{0, 0, 0, 0};
        }

        auto abs_dx = std::abs(dx);
        if (abs_dx >= width_)
        {
            return Rect{0, 0, width_, height_};
        }

        if (dx > 0)
        {
            // Scrolled right: reveal at right
            return Rect{width_ - abs_dx, 0, width_, height_};
        }
        // Scrolled left: reveal at left
        return Rect{0, 0, abs_dx, height_};
    }

    /// Detect scroll direction from delta.
    [[nodiscard]] static auto detect_direction(int32_t dx, int32_t dy) noexcept -> ScrollDirection
    {
        // Prefer vertical scrolling when both are non-zero
        if (dy > 0)
            return ScrollDirection::Down;
        if (dy < 0)
            return ScrollDirection::Up;
        if (dx > 0)
            return ScrollDirection::Right;
        if (dx < 0)
            return ScrollDirection::Left;
        return ScrollDirection::None;
    }

    /// Check if the scroll magnitude exceeds the viewport, requiring a full repaint.
    [[nodiscard]] auto is_full_repaint_needed(int32_t dx, int32_t dy) const noexcept -> bool
    {
        return std::abs(dy) >= height_ || std::abs(dx) >= width_;
    }

    /// The last scroll direction (for prefetch cancellation).
    [[nodiscard]] auto last_direction() const noexcept -> ScrollDirection
    {
        return last_direction_;
    }

    /// Record a scroll event and update direction tracking.
    void record_scroll(int32_t dx, int32_t dy) noexcept
    {
        last_direction_ = detect_direction(dx, dy);
        total_dy_ += dy;
        total_dx_ += dx;
    }

    /// Reset scroll tracking (e.g., on new frame).
    void reset_tracking() noexcept
    {
        total_dy_ = 0;
        total_dx_ = 0;
    }

    /// Accumulated vertical scroll since last reset.
    [[nodiscard]] auto accumulated_dy() const noexcept -> int32_t
    {
        return total_dy_;
    }

    /// Accumulated horizontal scroll since last reset.
    [[nodiscard]] auto accumulated_dx() const noexcept -> int32_t
    {
        return total_dx_;
    }

    /// Buffer dimensions.
    [[nodiscard]] auto width() const noexcept -> int32_t
    {
        return width_;
    }
    [[nodiscard]] auto height() const noexcept -> int32_t
    {
        return height_;
    }

private:
    int32_t width_{0};
    int32_t height_{0};
    ScrollDirection last_direction_{ScrollDirection::None};
    int32_t total_dy_{0};
    int32_t total_dx_{0};
};

} // namespace markamp::rendering
