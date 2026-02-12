#pragma once

#include "DirtyRegion.h"

#include <cstdint>
#include <functional>
#include <vector>

namespace markamp::rendering
{

/// Retained back-buffer rendering that only blits dirty regions.
///
/// Manages an off-screen bitmap. Renders only dirty regions to the
/// back buffer, then blits to screen. Eliminates flicker from
/// background erases.
///
/// Pattern implemented: #29 Minimized overdraw and background erases
class DoubleBufferedPaint
{
public:
    DoubleBufferedPaint() = default;

    /// Initialize the back buffer with the given dimensions.
    void initialize(int32_t width, int32_t height)
    {
        buffer_width_ = width;
        buffer_height_ = height;
        is_initialized_ = true;
        full_redraw_needed_ = true;
    }

    /// Mark a rectangle as needing repaint.
    void invalidate_rect(const Rect& rect)
    {
        dirty_accumulator_.invalidate(rect);
    }

    /// Mark the entire buffer as needing repaint (e.g., on resize).
    void invalidate_all()
    {
        full_redraw_needed_ = true;
    }

    /// Consume the current dirty rects and return them.
    /// After this call, the dirty region is empty.
    /// Callers should paint only these rects to the back buffer.
    [[nodiscard]] auto consume_dirty_rects() -> std::vector<Rect>
    {
        if (full_redraw_needed_)
        {
            full_redraw_needed_ = false;
            dirty_accumulator_.clear();
            return {Rect{0, 0, buffer_width_, buffer_height_}};
        }
        return dirty_accumulator_.consume();
    }

    /// Check if any region needs repaint.
    [[nodiscard]] auto needs_paint() const noexcept -> bool
    {
        return full_redraw_needed_ || dirty_accumulator_.count() > 0;
    }

    /// Check if a full redraw is needed.
    [[nodiscard]] auto needs_full_redraw() const noexcept -> bool
    {
        return full_redraw_needed_;
    }

    /// Call when the window is resized — reinitializes buffer & forces full redraw.
    void on_resize(int32_t new_width, int32_t new_height)
    {
        buffer_width_ = new_width;
        buffer_height_ = new_height;
        full_redraw_needed_ = true;
    }

    /// Buffer dimensions.
    [[nodiscard]] auto width() const noexcept -> int32_t
    {
        return buffer_width_;
    }
    [[nodiscard]] auto height() const noexcept -> int32_t
    {
        return buffer_height_;
    }

    /// Whether the back buffer has been initialized.
    [[nodiscard]] auto is_initialized() const noexcept -> bool
    {
        return is_initialized_;
    }

private:
    DirtyRegionAccumulator dirty_accumulator_{4}; // merge threshold of 4px
    int32_t buffer_width_{0};
    int32_t buffer_height_{0};
    bool is_initialized_{false};
    bool full_redraw_needed_{true};
};

} // namespace markamp::rendering
