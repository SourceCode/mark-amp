#pragma once

#include "DirtyRegion.h"

#include <chrono>
#include <cstddef>
#include <cstdint>

namespace markamp::rendering
{

/// Dedicated overlay layer for caret rendering.
///
/// Manages caret position, blink state, and a minimal Rect for refresh.
/// Uses RefreshRect on the parent window to avoid full Refresh() on blink.
///
/// Pattern implemented: #36 Caret blink and animations without full redraw
class CaretOverlay
{
public:
    static constexpr auto kDefaultBlinkInterval = std::chrono::milliseconds(530);

    CaretOverlay() = default;

    /// Update the caret position in document coordinates.
    /// Rect is stored as {left, top, right, bottom}.
    void update_position(std::size_t line,
                         std::size_t col,
                         int32_t line_height,
                         int32_t char_width = 2) noexcept
    {
        line_ = line;
        col_ = col;
        auto px_left = static_cast<int32_t>(col) * char_width;
        auto px_top = static_cast<int32_t>(line) * line_height;
        caret_rect_ = Rect{px_left,
                           px_top,
                           px_left + 2, // caret width = 2px
                           px_top + line_height};
        visible_ = true; // show on position change
    }

    /// Toggle blink visibility. Called by a timer.
    void toggle_blink() noexcept
    {
        visible_ = !visible_;
    }

    /// Force the caret to be visible (e.g., after user input).
    void show() noexcept
    {
        visible_ = true;
    }

    /// Force the caret to be hidden.
    void hide() noexcept
    {
        visible_ = false;
    }

    /// Whether the caret is currently visible.
    [[nodiscard]] auto is_visible() const noexcept -> bool
    {
        return visible_;
    }

    /// Get the minimal rectangle to refresh for this caret.
    [[nodiscard]] auto refresh_rect() const noexcept -> Rect
    {
        return caret_rect_;
    }

    /// Current line position.
    [[nodiscard]] auto line() const noexcept -> std::size_t
    {
        return line_;
    }

    /// Current column position.
    [[nodiscard]] auto col() const noexcept -> std::size_t
    {
        return col_;
    }

    /// Blink interval.
    [[nodiscard]] auto blink_interval() const noexcept -> std::chrono::milliseconds
    {
        return blink_interval_;
    }

    /// Set blink interval.
    void set_blink_interval(std::chrono::milliseconds interval) noexcept
    {
        blink_interval_ = interval;
    }

private:
    std::size_t line_{0};
    std::size_t col_{0};
    Rect caret_rect_{0, 0, 2, 16}; // default: left=0, top=0, right=2, bottom=16
    bool visible_{true};
    std::chrono::milliseconds blink_interval_{kDefaultBlinkInterval};
};

} // namespace markamp::rendering
