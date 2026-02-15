#include "PanTool.h"

#include "ui/CanvasPanel.h"

#include <cmath>

namespace markamp::canvas
{

auto PanTool::activate(CanvasPanel& /*panel*/) -> void
{
    is_panning_ = false;
}

auto PanTool::deactivate(CanvasPanel& /*panel*/) -> void
{
    is_panning_ = false;
}

auto PanTool::on_mouse_down(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool
{
    if (evt.button != MouseButton::Left && evt.button != MouseButton::Middle)
    {
        return false;
    }

    is_panning_ = true;
    pan_start_screen_ = evt.screen_pos;
    pan_start_offset_ = panel.viewport().pan();

    return true;
}

auto PanTool::on_mouse_up(CanvasPanel& /*panel*/, const CanvasInputEvent& /*evt*/) -> bool
{
    if (!is_panning_)
    {
        return false;
    }

    is_panning_ = false;
    return true;
}

auto PanTool::on_mouse_move(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool
{
    if (!is_panning_)
    {
        return false;
    }

    const double delta_x = evt.screen_pos.x - pan_start_screen_.x;
    const double delta_y = evt.screen_pos.y - pan_start_screen_.y;

    // Pan moves the viewport in the opposite direction of mouse drag.
    const double zoom = panel.viewport().zoom();
    panel.viewport_mut().set_pan(
        Point2D{pan_start_offset_.x + delta_x / zoom, pan_start_offset_.y + delta_y / zoom});

    panel.request_repaint();
    return true;
}

auto PanTool::on_mouse_scroll(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool
{
    // Zoom with scroll wheel even while in pan mode.
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

auto PanTool::on_key_down(CanvasPanel& /*panel*/, int /*key_code*/, ModifierKeys /*mods*/) -> bool
{
    return false;
}

auto PanTool::on_key_up(CanvasPanel& /*panel*/, int /*key_code*/, ModifierKeys /*mods*/) -> bool
{
    return false;
}

auto PanTool::is_panning() const -> bool
{
    return is_panning_;
}

} // namespace markamp::canvas
