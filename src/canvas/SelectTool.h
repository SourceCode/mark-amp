#pragma once

#include "canvas/CanvasTool.h"
#include "canvas/CanvasTypes.h"

#include <optional>
#include <vector>

namespace markamp::canvas
{

/// Select tool: click-to-select, Shift-click additive, marquee box selection.
class SelectTool : public ICanvasTool
{
public:
    SelectTool() = default;

    [[nodiscard]] auto tool_mode() const -> ToolMode override
    {
        return ToolMode::Select;
    }
    [[nodiscard]] auto cursor_name() const -> std::string override
    {
        return "arrow";
    }

    auto activate(CanvasPanel& panel) -> void override;
    auto deactivate(CanvasPanel& panel) -> void override;

    auto on_mouse_down(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool override;
    auto on_mouse_up(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool override;
    auto on_mouse_move(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool override;
    auto on_mouse_scroll(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool override;
    auto on_key_down(CanvasPanel& panel, int key_code, ModifierKeys mods) -> bool override;
    auto on_key_up(CanvasPanel& panel, int key_code, ModifierKeys mods) -> bool override;

    /// Whether a marquee selection is currently active.
    [[nodiscard]] auto is_marquee_active() const -> bool;
    [[nodiscard]] auto marquee_bounds() const -> std::optional<AABB>;

private:
    bool is_dragging_{false};
    bool is_marquee_{false};
    Point2D drag_start_world_{0.0, 0.0};
    Point2D drag_current_world_{0.0, 0.0};
};

} // namespace markamp::canvas
