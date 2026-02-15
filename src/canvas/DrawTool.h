#pragma once

#include "canvas/CanvasTool.h"
#include "canvas/CanvasTypes.h"
#include "canvas/FreehandPath.h"

#include <memory>

namespace markamp::canvas
{

/// Draw tool: captures mouse drag as a FreehandPath, smooths on release.
class DrawTool : public ICanvasTool
{
public:
    DrawTool() = default;

    [[nodiscard]] auto tool_mode() const -> ToolMode override
    {
        return ToolMode::Draw;
    }
    [[nodiscard]] auto cursor_name() const -> std::string override
    {
        return "crosshair";
    }

    auto activate(CanvasPanel& panel) -> void override;
    auto deactivate(CanvasPanel& panel) -> void override;

    auto on_mouse_down(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool override;
    auto on_mouse_up(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool override;
    auto on_mouse_move(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool override;
    auto on_mouse_scroll(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool override;
    auto on_key_down(CanvasPanel& panel, int key_code, ModifierKeys mods) -> bool override;
    auto on_key_up(CanvasPanel& panel, int key_code, ModifierKeys mods) -> bool override;

    /// Whether a freehand stroke is currently being captured.
    [[nodiscard]] auto is_drawing() const -> bool;

    /// In-progress path (for overlay rendering). May be nullptr.
    [[nodiscard]] auto current_path() const -> const FreehandPath*;

    // ── Configurable tool state ──────────────────────────────

    [[nodiscard]] auto pen_color() const -> CanvasColor;
    auto set_pen_color(CanvasColor color) -> void;

    [[nodiscard]] auto pen_width() const -> double;
    auto set_pen_width(double width) -> void;

    [[nodiscard]] auto pen_smoothing() const -> double;
    auto set_pen_smoothing(double smoothing) -> void;

private:
    std::unique_ptr<FreehandPath> current_path_;
    bool is_drawing_{false};
    CanvasColor pen_color_{0, 0, 0, 255};
    double pen_width_{2.0};
    double pen_smoothing_{0.5};
};

} // namespace markamp::canvas
