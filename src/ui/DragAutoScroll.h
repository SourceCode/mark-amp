#pragma once

#include <functional>

namespace markamp::ui
{

/// Phase 45: Auto-scroll behavior when dragging near panel edges.
class DragAutoScroll
{
public:
    using ScrollCallback = std::function<void(int delta_y)>;

    void set_scroll_callback(ScrollCallback callback)
    {
        callback_ = std::move(callback);
    }
    void set_edge_margin(int margin_px)
    {
        edge_margin_ = margin_px;
    }
    void set_scroll_speed(int pixels_per_tick)
    {
        scroll_speed_ = pixels_per_tick;
    }

    /// Called each frame with current cursor Y and viewport bounds.
    void update(int cursor_y, int viewport_top, int viewport_height);

    /// Stop auto-scrolling.
    void stop();

    [[nodiscard]] auto is_scrolling() const -> bool
    {
        return is_scrolling_;
    }
    [[nodiscard]] auto scroll_direction() const -> int
    {
        return scroll_dir_;
    }

private:
    ScrollCallback callback_;
    int edge_margin_{40};
    int scroll_speed_{8};
    int scroll_dir_{0};
    bool is_scrolling_{false};
};

} // namespace markamp::ui
