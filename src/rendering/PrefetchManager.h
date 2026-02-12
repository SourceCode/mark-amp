#pragma once

#include "ViewportCache.h"

#include <cstddef>
#include <cstdint>

namespace markamp::rendering
{

/// Pre-measured, pre-styled line data ready for fast paint.
///
/// Pattern implemented: #33 Predictive prefetching (near-viewport)
struct PrefetchedLine
{
    std::size_t logical_line{0};
    int32_t height_px{0};
    int32_t width_px{0};
    bool is_valid{false};
};

/// Pre-render lines just beyond the visible viewport so scrolling feels instant.
///
/// Uses ViewportState to determine the prefetch range. Manages a
/// background task that renders N lines above/below the viewport.
///
/// Pattern implemented: #33 Predictive prefetching (near-viewport)
class PrefetchManager
{
public:
    PrefetchManager() = default;

    /// Compute the prefetch range based on current viewport state.
    /// Returns (start_line, end_line) — the lines that should be pre-rendered.
    struct PrefetchRange
    {
        std::size_t start_line{0};
        std::size_t end_line{0};

        [[nodiscard]] auto line_count() const noexcept -> std::size_t
        {
            return (end_line > start_line) ? (end_line - start_line) : 0;
        }

        [[nodiscard]] auto contains(std::size_t line) const noexcept -> bool
        {
            return line >= start_line && line < end_line;
        }
    };

    /// Compute the range of lines that should be prefetched.
    [[nodiscard]] auto compute_range(const ViewportState& viewport,
                                     std::size_t total_lines) const noexcept -> PrefetchRange
    {
        auto render_start = viewport.render_start(total_lines);
        auto render_end = viewport.render_end(total_lines);

        // Include extra margin beyond the standard ViewportState prefetch
        auto extra_above = std::min(render_start, extra_margin_);
        auto extra_below = std::min(total_lines - render_end, extra_margin_);

        return PrefetchRange{.start_line = render_start - extra_above,
                             .end_line = render_end + extra_below};
    }

    /// Check if a line is within the prefetch range.
    [[nodiscard]] auto is_prefetch_line(std::size_t line,
                                        const ViewportState& viewport,
                                        std::size_t total_lines) const noexcept -> bool
    {
        auto range = compute_range(viewport, total_lines);
        return range.contains(line) &&
               (line < viewport.first_visible_line ||
                line >= viewport.first_visible_line + viewport.visible_line_count);
    }

    /// Set the extra prefetch margin (lines beyond ViewportState's margin).
    void set_extra_margin(std::size_t margin) noexcept
    {
        extra_margin_ = margin;
    }

    /// Get the extra prefetch margin.
    [[nodiscard]] auto extra_margin() const noexcept -> std::size_t
    {
        return extra_margin_;
    }

    /// Update the last known scroll direction for directional prefetch.
    void update_scroll_direction(bool scrolling_down) noexcept
    {
        scrolling_down_ = scrolling_down;
    }

    /// Whether the user is scrolling down (for directional prefetch bias).
    [[nodiscard]] auto is_scrolling_down() const noexcept -> bool
    {
        return scrolling_down_;
    }

private:
    std::size_t extra_margin_{20}; // Extra lines beyond ViewportState prefetch
    bool scrolling_down_{true};    // Default bias
};

} // namespace markamp::rendering
