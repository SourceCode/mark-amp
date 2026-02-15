#include "SelectTool.h"

#include "canvas/CanvasInputManager.h"
#include "core/Events.h"
#include "ui/CanvasPanel.h"

#include <algorithm>
#include <cmath>

namespace markamp::canvas
{

auto SelectTool::activate(CanvasPanel& /*panel*/) -> void
{
    is_dragging_ = false;
    is_marquee_ = false;
}

auto SelectTool::deactivate(CanvasPanel& /*panel*/) -> void
{
    is_dragging_ = false;
    is_marquee_ = false;
}

auto SelectTool::on_mouse_down(CanvasPanel& /*panel*/, const CanvasInputEvent& evt) -> bool
{
    if (evt.button != MouseButton::Left)
    {
        return false;
    }

    is_dragging_ = true;
    drag_start_world_ = evt.world_pos;
    drag_current_world_ = evt.world_pos;
    is_marquee_ = false;

    return true;
}

auto SelectTool::on_mouse_up(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool
{
    if (!is_dragging_)
    {
        return false;
    }

    is_dragging_ = false;

    if (is_marquee_)
    {
        // Finish marquee: select all objects inside the rectangle.
        is_marquee_ = false;
        const auto bounds = marquee_bounds();
        if (bounds.has_value())
        {
            // Query the spatial index for objects in the marquee region.
            // Selection integration deferred to Phase 4 (SelectionManager).
            (void)panel.query_region(bounds.value());
        }
        panel.request_repaint();
        return true;
    }

    // Single click — not a marquee — handled by InputManager/SelectionManager.
    (void)evt;
    return true;
}

auto SelectTool::on_mouse_move(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool
{
    if (!is_dragging_)
    {
        return false;
    }

    drag_current_world_ = evt.world_pos;

    // If moved more than a few world-units, switch to marquee.
    const double drag_distance = std::sqrt((drag_current_world_.x - drag_start_world_.x) *
                                               (drag_current_world_.x - drag_start_world_.x) +
                                           (drag_current_world_.y - drag_start_world_.y) *
                                               (drag_current_world_.y - drag_start_world_.y));

    if (drag_distance > 3.0)
    {
        is_marquee_ = true;
    }

    if (is_marquee_)
    {
        panel.request_repaint();
    }

    return true;
}

auto SelectTool::on_mouse_scroll(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool
{
    // Zoom with scroll wheel.
    if (std::abs(evt.scroll_delta_y) > 0.01)
    {
        const double zoom_factor = (evt.scroll_delta_y > 0) ? 1.1 : 0.9;
        const double new_zoom = panel.viewport().zoom() * zoom_factor;
        panel.viewport_mut().zoom_at(evt.screen_pos.x, evt.screen_pos.y, new_zoom);
        panel.request_repaint();
        return true;
    }
    return false;
}

auto SelectTool::on_key_down(CanvasPanel& /*panel*/, int /*key_code*/, ModifierKeys /*mods*/)
    -> bool
{
    // Key handling (e.g. Delete, Escape) deferred to Phase 4 integration.
    return false;
}

auto SelectTool::on_key_up(CanvasPanel& /*panel*/, int /*key_code*/, ModifierKeys /*mods*/) -> bool
{
    return false;
}

auto SelectTool::is_marquee_active() const -> bool
{
    return is_marquee_;
}

auto SelectTool::marquee_bounds() const -> std::optional<AABB>
{
    if (!is_marquee_)
    {
        return std::nullopt;
    }

    return AABB{std::min(drag_start_world_.x, drag_current_world_.x),
                std::min(drag_start_world_.y, drag_current_world_.y),
                std::max(drag_start_world_.x, drag_current_world_.x),
                std::max(drag_start_world_.y, drag_current_world_.y)};
}

} // namespace markamp::canvas
