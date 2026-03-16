#pragma once

#include "canvas/CanvasTool.h"
#include "canvas/CanvasTypes.h"
#include "core/EventBus.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace markamp::canvas
{

class CanvasPanel;

/// Routes wxWidgets events to the active ICanvasTool, manages tool switching,
/// coordinate conversion, and hit-testing.
class CanvasInputManager
{
public:
    explicit CanvasInputManager(CanvasPanel& panel, std::shared_ptr<core::EventBus> event_bus);

    /// Register a tool. The manager takes ownership.
    auto register_tool(std::unique_ptr<ICanvasTool> tool) -> void;

    /// Switch to a tool by ToolMode.
    auto set_active_tool(ToolMode mode) -> bool;

    /// Get the currently active tool.
    [[nodiscard]] auto active_tool() const -> ICanvasTool*;
    [[nodiscard]] auto active_tool_mode() const -> ToolMode;

    /// Event dispatch from wxWidgets (called by CanvasPanel).
    auto handle_mouse_down(const CanvasInputEvent& evt) -> bool;
    auto handle_mouse_up(const CanvasInputEvent& evt) -> bool;
    auto handle_mouse_move(const CanvasInputEvent& evt) -> bool;
    auto handle_mouse_scroll(const CanvasInputEvent& evt) -> bool;
    auto handle_key_down(int key_code, ModifierKeys mods) -> bool;
    auto handle_key_up(int key_code, ModifierKeys mods) -> bool;

    /// Hit-test: find the topmost visible, unlocked object at a world point.
    [[nodiscard]] auto hit_test(const Point2D& world_point) const -> ObjectId;

    /// Hit-test: find all objects at a world point (sorted by z-index descending).
    [[nodiscard]] auto hit_test_all(const Point2D& world_point) const -> std::vector<ObjectId>;

    /// Space-key temporary pan support.
    auto set_space_held(bool held) -> void;
    [[nodiscard]] auto is_space_held() const -> bool;

    // ── Round 2 Batch 8 (#76-80) ──────────────────────────────────

    /// (#76) Whether an active tool is set.
    [[nodiscard]] auto has_active_tool() const noexcept -> bool
    {
        return active_tool_ != nullptr;
    }

    /// (#77) Number of registered tools.
    [[nodiscard]] auto tool_count() const noexcept -> size_t
    {
        return tools_.size();
    }

    /// (#78) Whether the current tool is Select.
    [[nodiscard]] auto is_select_mode() const noexcept -> bool
    {
        return active_tool_mode_ == ToolMode::Select;
    }

    /// (#79) Whether the current tool is Pan.
    [[nodiscard]] auto is_pan_mode() const noexcept -> bool
    {
        return active_tool_mode_ == ToolMode::Pan;
    }

    /// (#80) Whether the current tool is Draw.
    [[nodiscard]] auto is_draw_mode() const noexcept -> bool
    {
        return active_tool_mode_ == ToolMode::Draw;
    }

private:
    CanvasPanel& panel_;
    std::shared_ptr<core::EventBus> event_bus_;
    std::unordered_map<uint8_t, std::unique_ptr<ICanvasTool>> tools_;
    ICanvasTool* active_tool_{nullptr};
    ToolMode active_tool_mode_{ToolMode::Select};
    ToolMode previous_tool_mode_{ToolMode::Select}; // For space-pan restore.
    bool space_held_{false};
};

} // namespace markamp::canvas
