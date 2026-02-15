#pragma once

#include "canvas/CanvasTypes.h"

#include <cstdint>
#include <string>

namespace markamp::canvas
{

/// Tool modes available in the canvas editor.
enum class ToolMode : uint8_t
{
    Select,
    Pan,
    Draw,
    Shape,
    Text,
    Connector,
    StickyNote,
    Eraser,
    Comment
};

/// Mouse button identifiers.
enum class MouseButton : uint8_t
{
    None,
    Left,
    Middle,
    Right
};

/// Modifier key flags (bit field).
enum class ModifierKeys : uint8_t
{
    None = 0,
    Shift = 1 << 0,
    Ctrl = 1 << 1,
    Alt = 1 << 2,
    Meta = 1 << 3, // Cmd on macOS
};

/// Combine modifier-key flags.
inline auto operator|(ModifierKeys lhs, ModifierKeys rhs) -> ModifierKeys
{
    return static_cast<ModifierKeys>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
inline auto operator&(ModifierKeys lhs, ModifierKeys rhs) -> ModifierKeys
{
    return static_cast<ModifierKeys>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
inline auto has_modifier(ModifierKeys flags, ModifierKeys test) -> bool
{
    return (flags & test) == test;
}

/// Unified input event for the canvas, produced from raw wxWidgets events.
struct CanvasInputEvent
{
    Point2D screen_pos{0.0, 0.0};
    Point2D world_pos{0.0, 0.0};
    MouseButton button{MouseButton::None};
    ModifierKeys modifiers{ModifierKeys::None};
    double scroll_delta_x{0.0};
    double scroll_delta_y{0.0};
    int click_count{0};
    bool is_drag{false};
};

/// Forward declare CanvasPanel for tool interaction.
class CanvasPanel;

/// Abstract interface for canvas tools (Select, Pan, Draw, etc.).
class ICanvasTool
{
public:
    virtual ~ICanvasTool() = default;

    ICanvasTool(const ICanvasTool&) = delete;
    auto operator=(const ICanvasTool&) -> ICanvasTool& = delete;
    ICanvasTool(ICanvasTool&&) = delete;
    auto operator=(ICanvasTool&&) -> ICanvasTool& = delete;

    [[nodiscard]] virtual auto tool_mode() const -> ToolMode = 0;
    [[nodiscard]] virtual auto cursor_name() const -> std::string = 0;

    virtual auto activate(CanvasPanel& panel) -> void = 0;
    virtual auto deactivate(CanvasPanel& panel) -> void = 0;

    virtual auto on_mouse_down(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool = 0;
    virtual auto on_mouse_up(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool = 0;
    virtual auto on_mouse_move(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool = 0;
    virtual auto on_mouse_scroll(CanvasPanel& panel, const CanvasInputEvent& evt) -> bool = 0;
    virtual auto on_key_down(CanvasPanel& panel, int key_code, ModifierKeys mods) -> bool = 0;
    virtual auto on_key_up(CanvasPanel& panel, int key_code, ModifierKeys mods) -> bool = 0;

protected:
    ICanvasTool() = default;
};

} // namespace markamp::canvas
