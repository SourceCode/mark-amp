#include "DrawTool.h"

#include "canvas/FreehandPath.h"

#include <algorithm>

namespace markamp::canvas
{

auto DrawTool::activate(CanvasPanel& /*panel*/) -> void
{
    current_path_.reset();
    is_drawing_ = false;
}

auto DrawTool::deactivate(CanvasPanel& /*panel*/) -> void
{
    current_path_.reset();
    is_drawing_ = false;
}

auto DrawTool::on_mouse_down(CanvasPanel& /*panel*/, const CanvasInputEvent& evt) -> bool
{
    if (evt.button != MouseButton::Left)
    {
        return false;
    }

    // Start a new freehand stroke.
    current_path_ = std::make_unique<FreehandPath>();
    current_path_->set_stroke_color(pen_color_);
    current_path_->set_stroke_width(pen_width_);
    current_path_->set_smoothing_factor(pen_smoothing_);
    current_path_->add_point(evt.world_pos);
    is_drawing_ = true;

    return true;
}

auto DrawTool::on_mouse_move(CanvasPanel& /*panel*/, const CanvasInputEvent& evt) -> bool
{
    if (!is_drawing_ || !current_path_)
    {
        return false;
    }

    // Only add points that are a minimum distance apart to avoid clutter.
    constexpr double kMinPointDistance = 1.5;
    const auto& pts = current_path_->points();

    if (!pts.empty())
    {
        const double separation = pts.back().distance_to(evt.world_pos);
        if (separation < kMinPointDistance)
        {
            return true; // Still consuming the event, just skipping the point.
        }
    }

    current_path_->add_point(evt.world_pos);
    return true;
}

auto DrawTool::on_mouse_up(CanvasPanel& /*panel*/, const CanvasInputEvent& /*evt*/) -> bool
{
    if (!is_drawing_ || !current_path_)
    {
        return false;
    }

    // Finalize: smooth and simplify.
    if (current_path_->point_count() >= 3)
    {
        current_path_->smooth();
        current_path_->simplify(1.0);
    }

    // The caller (CanvasPanel) would typically add current_path_ to the board
    // via an AddObjectCommand here. We just finalize the path and release it.
    // The panel can retrieve it via current_path() before we reset.
    is_drawing_ = false;

    // Note: current_path_ is intentionally NOT reset here so the caller can
    // retrieve the finished path before the next stroke begins.
    return true;
}

auto DrawTool::on_mouse_scroll(CanvasPanel& /*panel*/, const CanvasInputEvent& /*evt*/) -> bool
{
    return false;
}

auto DrawTool::on_key_down(CanvasPanel& /*panel*/, int /*key_code*/, ModifierKeys /*mods*/) -> bool
{
    return false;
}

auto DrawTool::on_key_up(CanvasPanel& /*panel*/, int /*key_code*/, ModifierKeys /*mods*/) -> bool
{
    return false;
}

auto DrawTool::is_drawing() const -> bool
{
    return is_drawing_;
}

auto DrawTool::current_path() const -> const FreehandPath*
{
    return current_path_.get();
}

// ── Configurable pen state ──────────────────────────────────────

auto DrawTool::pen_color() const -> CanvasColor
{
    return pen_color_;
}

auto DrawTool::set_pen_color(CanvasColor color) -> void
{
    pen_color_ = color;
}

auto DrawTool::pen_width() const -> double
{
    return pen_width_;
}

auto DrawTool::set_pen_width(double width) -> void
{
    pen_width_ = std::max(0.5, width);
}

auto DrawTool::pen_smoothing() const -> double
{
    return pen_smoothing_;
}

auto DrawTool::set_pen_smoothing(double smoothing) -> void
{
    pen_smoothing_ = std::clamp(smoothing, 0.0, 1.0);
}

} // namespace markamp::canvas
