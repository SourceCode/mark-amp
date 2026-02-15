#pragma once

#include "canvas/CanvasTool.h"
#include "canvas/CanvasTypes.h"

namespace markamp::canvas
{

/// Pan (hand) tool: drag-to-pan the viewport.
class PanTool : public ICanvasTool
{
public:
    PanTool() = default;

    [[nodiscard]] auto tool_mode() const -> ToolMode override
    {
        return ToolMode::Pan;
    }
    [[nodiscard]] auto cursor_name() const -> std::string override
    {
        return "hand";
    }

    auto activate(CanvasPanel& panel) -> void override;
    auto deactivate(CanvasPanel& panel) -> void override;

    auto on_mouse_down(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool override;
    auto on_mouse_up(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool override;
    auto on_mouse_move(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool override;
    auto on_mouse_scroll(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool override;
    auto on_key_down(CanvasPanel& panel, int key_code, ModifierKeys mods) -> bool override;
    auto on_key_up(CanvasPanel& panel, int key_code, ModifierKeys mods) -> bool override;

    [[nodiscard]] auto is_panning() const -> bool;

private:
    bool is_panning_{false};
    Point2D pan_start_screen_{0.0, 0.0};
    Point2D pan_start_offset_{0.0, 0.0};
};

} // namespace markamp::canvas
