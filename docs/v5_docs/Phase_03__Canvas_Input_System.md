# Phase 03 -- Canvas Input System

## Objective

Implement the mouse and keyboard input handling layer for the canvas: event dispatch from wxWidgets to canvas-space coordinates, hit-testing via the Quadtree, drag handling (pan, move objects), zoom gestures (scroll wheel, pinch on trackpad), and a tool-mode state machine that routes input to the active tool (Select, Pan, Draw, etc.).

## Prerequisites

- Phase 01 (CanvasTypes, CanvasObject, Quadtree, ViewportTransform)
- Phase 02 (CanvasPanel, CanvasRenderer)
- Existing ShortcutManager (`src/core/ShortcutManager.h`)

## Feature References (PRD)

- PRD #1: Infinite Canvas (pan/zoom)
- PRD #42: Hotkeys (canvas shortcuts)
- PRD #43: Bulk Select/Actions (marquee selection)

## Data Structures to Implement

### File: `src/canvas/CanvasTool.h`

```cpp
#pragma once

#include "CanvasTypes.h"

#include <string>

namespace markamp::canvas
{

/// Identifies which tool is currently active.
enum class ToolMode : uint8_t
{
    Select,     // Default: click to select, drag to move, marquee select
    Pan,        // Hand tool: drag to pan the viewport
    Draw,       // Freehand drawing tool
    Shape,      // Shape creation tool
    Text,       // Text box creation tool
    Connector,  // Line/connector creation tool
    StickyNote, // Sticky note placement tool
    Eraser,     // Eraser tool
    Comment     // Comment placement tool
};

/// Mouse button identifiers.
enum class MouseButton : uint8_t
{
    Left,
    Middle,
    Right,
    None
};

/// Input event data passed to tools.
struct CanvasInputEvent
{
    Point2D screen_pos;    // Position in screen pixels
    Point2D world_pos;     // Position in world coordinates
    MouseButton button{MouseButton::None};
    bool shift_held{false};
    bool ctrl_held{false};
    bool alt_held{false};
    bool meta_held{false};  // Cmd on macOS
    double scroll_delta{0.0}; // Positive = scroll up (zoom in)
    int click_count{1};    // 1=single, 2=double, 3=triple

    /// True if any modifier is held.
    [[nodiscard]] auto has_modifier() const -> bool;
};

/// Abstract base for canvas tools. Each tool handles a specific input mode.
class ICanvasTool
{
public:
    virtual ~ICanvasTool() = default;

    [[nodiscard]] virtual auto tool_mode() const -> ToolMode = 0;
    [[nodiscard]] virtual auto tool_name() const -> std::string = 0;
    [[nodiscard]] virtual auto cursor_name() const -> std::string = 0;

    /// Called when this tool becomes active.
    virtual auto on_activate() -> void = 0;
    /// Called when this tool is deactivated.
    virtual auto on_deactivate() -> void = 0;

    /// Mouse event handlers. Return true if handled (consumes event).
    virtual auto on_mouse_down(const CanvasInputEvent& event) -> bool = 0;
    virtual auto on_mouse_move(const CanvasInputEvent& event) -> bool = 0;
    virtual auto on_mouse_up(const CanvasInputEvent& event) -> bool = 0;
    virtual auto on_mouse_double_click(const CanvasInputEvent& event) -> bool = 0;
    virtual auto on_scroll(const CanvasInputEvent& event) -> bool = 0;

    /// Key event handlers.
    virtual auto on_key_down(int key_code, int modifiers) -> bool = 0;
    virtual auto on_key_up(int key_code, int modifiers) -> bool = 0;

    /// Draw tool-specific overlays (e.g., marquee rectangle, connector preview).
    virtual auto render_overlay(class wxGraphicsContext& gc,
                                 const ViewportTransform& viewport) -> void = 0;
};

} // namespace markamp::canvas
```

### File: `src/canvas/CanvasInputManager.h`

```cpp
#pragma once

#include "CanvasTool.h"
#include "CanvasTypes.h"
#include "ViewportTransform.h"

#include <memory>
#include <unordered_map>
#include <vector>

class wxMouseEvent;
class wxKeyEvent;

namespace markamp::core
{
class EventBus;
class ShortcutManager;
} // namespace markamp::core

namespace markamp::canvas
{

class Quadtree;

/// Central input dispatcher for the canvas. Routes wxWidgets mouse/key events
/// to the active ICanvasTool after transforming coordinates.
class CanvasInputManager
{
public:
    CanvasInputManager(core::EventBus& event_bus,
                       core::ShortcutManager& shortcut_manager,
                       ViewportTransform& viewport,
                       Quadtree& spatial_index);

    /// Register a tool. The input manager owns all tool instances.
    auto register_tool(std::unique_ptr<ICanvasTool> tool) -> void;

    /// Set the active tool by mode.
    auto set_active_tool(ToolMode mode) -> void;
    [[nodiscard]] auto active_tool_mode() const -> ToolMode;
    [[nodiscard]] auto active_tool() const -> ICanvasTool*;

    /// Process wxWidgets events. Called by CanvasPanel event handlers.
    auto handle_mouse_down(wxMouseEvent& event) -> void;
    auto handle_mouse_move(wxMouseEvent& event) -> void;
    auto handle_mouse_up(wxMouseEvent& event) -> void;
    auto handle_mouse_double_click(wxMouseEvent& event) -> void;
    auto handle_scroll(wxMouseEvent& event) -> void;
    auto handle_key_down(wxKeyEvent& event) -> void;
    auto handle_key_up(wxKeyEvent& event) -> void;

    /// Hit-test: find the topmost object at a screen point. Uses Quadtree
    /// then resolves by z-index.
    [[nodiscard]] auto hit_test(const Point2D& screen_pos,
                                 const std::unordered_map<ObjectId,
                                     std::unique_ptr<CanvasObject>>& objects) const
        -> ObjectId;

    /// Render the active tool's overlay (e.g., selection marquee).
    auto render_tool_overlay(wxGraphicsContext& gc, const ViewportTransform& viewport) -> void;

private:
    core::EventBus& event_bus_;
    core::ShortcutManager& shortcut_manager_;
    ViewportTransform& viewport_;
    Quadtree& spatial_index_;

    std::unordered_map<ToolMode, std::unique_ptr<ICanvasTool>> tools_;
    ToolMode active_mode_{ToolMode::Select};

    /// Convert a wxMouseEvent to a CanvasInputEvent with screen + world coords.
    [[nodiscard]] auto make_input_event(wxMouseEvent& event) const -> CanvasInputEvent;
};

} // namespace markamp::canvas
```

### File: `src/canvas/SelectTool.h`

```cpp
#pragma once

#include "CanvasTool.h"
#include "CanvasTypes.h"

#include <optional>
#include <vector>

namespace markamp::canvas
{

/// The default Select tool handles:
/// - Click to select/deselect objects
/// - Shift+click for additive selection
/// - Drag on empty space for marquee selection
/// - Drag on object to move (delegated to Phase 04)
/// - Escape to deselect all
class SelectTool : public ICanvasTool
{
public:
    [[nodiscard]] auto tool_mode() const -> ToolMode override { return ToolMode::Select; }
    [[nodiscard]] auto tool_name() const -> std::string override { return "Select"; }
    [[nodiscard]] auto cursor_name() const -> std::string override { return "arrow"; }

    auto on_activate() -> void override;
    auto on_deactivate() -> void override;
    auto on_mouse_down(const CanvasInputEvent& event) -> bool override;
    auto on_mouse_move(const CanvasInputEvent& event) -> bool override;
    auto on_mouse_up(const CanvasInputEvent& event) -> bool override;
    auto on_mouse_double_click(const CanvasInputEvent& event) -> bool override;
    auto on_scroll(const CanvasInputEvent& event) -> bool override;
    auto on_key_down(int key_code, int modifiers) -> bool override;
    auto on_key_up(int key_code, int modifiers) -> bool override;
    auto render_overlay(wxGraphicsContext& gc, const ViewportTransform& viewport) -> void override;

    /// Set a callback for when selection changes.
    using SelectionChangedCallback = std::function<void(const std::vector<ObjectId>&)>;
    auto set_on_selection_changed(SelectionChangedCallback cb) -> void;

    /// Set a callback for hit-test resolution.
    using HitTestCallback = std::function<ObjectId(const Point2D&)>;
    auto set_hit_test(HitTestCallback cb) -> void;

    /// Current selection.
    [[nodiscard]] auto selection() const -> const std::vector<ObjectId>&;
    auto set_selection(const std::vector<ObjectId>& ids) -> void;
    auto clear_selection() -> void;

private:
    std::vector<ObjectId> selected_ids_;
    bool marquee_active_{false};
    Point2D marquee_start_;
    Point2D marquee_end_;
    SelectionChangedCallback on_selection_changed_;
    HitTestCallback hit_test_fn_;
};

} // namespace markamp::canvas
```

### File: `src/canvas/PanTool.h`

```cpp
#pragma once

#include "CanvasTool.h"

namespace markamp::canvas
{

/// Hand/Pan tool: drag to pan the viewport. Also active when
/// middle-mouse-button or Space+drag in Select mode.
class PanTool : public ICanvasTool
{
public:
    explicit PanTool(ViewportTransform& viewport);

    [[nodiscard]] auto tool_mode() const -> ToolMode override { return ToolMode::Pan; }
    [[nodiscard]] auto tool_name() const -> std::string override { return "Pan"; }
    [[nodiscard]] auto cursor_name() const -> std::string override { return "hand"; }

    auto on_activate() -> void override;
    auto on_deactivate() -> void override;
    auto on_mouse_down(const CanvasInputEvent& event) -> bool override;
    auto on_mouse_move(const CanvasInputEvent& event) -> bool override;
    auto on_mouse_up(const CanvasInputEvent& event) -> bool override;
    auto on_mouse_double_click(const CanvasInputEvent& event) -> bool override;
    auto on_scroll(const CanvasInputEvent& event) -> bool override;
    auto on_key_down(int key_code, int modifiers) -> bool override;
    auto on_key_up(int key_code, int modifiers) -> bool override;
    auto render_overlay(wxGraphicsContext& gc, const ViewportTransform& viewport) -> void override;

private:
    ViewportTransform& viewport_;
    bool dragging_{false};
    Point2D drag_start_screen_;
    Point2D drag_start_pan_;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `CanvasInputManager::make_input_event()` -- Extract screen x/y from wxMouseEvent, convert to world via viewport_.screen_to_world(), read modifier keys, build CanvasInputEvent.

2. `CanvasInputManager::handle_scroll()` -- Compute zoom delta from scroll event. Call `viewport_.zoom_at(screen_x, screen_y, new_zoom)`. Publish CanvasViewportChangedEvent.

3. `CanvasInputManager::hit_test()` -- Call `spatial_index_.query_point(world_pos)` to get candidates. Sort by z-index descending. Return topmost non-locked visible object. Skip locked objects for selection but not for hover.

4. `SelectTool::on_mouse_down()` -- Hit-test. If hit an object: select it (or add to selection if Shift held). If no hit: start marquee selection.

5. `SelectTool::on_mouse_move()` -- If marquee active: update marquee_end_, redraw marquee overlay. If dragging an object (future Phase 04): delegate to move handler.

6. `SelectTool::render_overlay()` -- If marquee active: draw semi-transparent blue rectangle from marquee_start_ to marquee_end_.

7. `PanTool::on_mouse_down()` -- Record start position and current pan. Set dragging_ = true.

8. `PanTool::on_mouse_move()` -- If dragging: compute screen delta from start, convert to world delta, update viewport pan.

9. Space+drag in Select mode: When Space is pressed in SelectTool, temporarily delegate to PanTool behavior.

## Events to Add

```cpp
// In Events.h

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasToolChangedEvent)
uint8_t tool_mode{0}; // ToolMode as uint8_t
std::string tool_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasSelectionChangedEvent)
std::vector<uint64_t> selected_ids;
int count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasObjectHoveredEvent)
uint64_t object_id{0}; // 0 = no hover
MARKAMP_DECLARE_EVENT_END;
```

## Shortcut Registrations

| Shortcut | Action | Context |
|----------|--------|---------|
| V | Switch to Select tool | canvas |
| H | Switch to Pan tool | canvas |
| Space (hold) | Temporary pan mode | canvas |
| Escape | Deselect all / cancel tool | canvas |
| Cmd/Ctrl+A | Select all objects | canvas |
| Cmd/Ctrl+Shift+A | Deselect all | canvas |
| +/= | Zoom in | canvas |
| - | Zoom out | canvas |
| Cmd/Ctrl+0 | Reset zoom to 100% | canvas |
| Cmd/Ctrl+1 | Zoom to fit | canvas |

## Test Cases

File: `tests/unit/test_canvas_input.cpp`

1. **make_input_event coordinates** -- Verify screen and world coordinates are correctly computed.
2. **Hit-test topmost object** -- Two overlapping objects, different z-index. Verify hit returns highest z.
3. **Hit-test miss** -- Click empty space, verify kInvalidObjectId returned.
4. **Hit-test skips invisible** -- Invisible object at click point, visible object beneath. Verify visible one returned.
5. **Scroll zoom** -- Simulate scroll event, verify zoom changes and viewport event published.
6. **Zoom at cursor** -- Zoom at a specific screen point, verify the world point under cursor is preserved.
7. **Select tool -- single select** -- Click an object, verify selection contains only that object.
8. **Select tool -- shift-select** -- Select one, shift-click another, verify both selected.
9. **Select tool -- deselect** -- Click empty space, verify selection empty.
10. **Select tool -- marquee** -- Drag on empty space, verify marquee AABB computed correctly.
11. **Pan tool -- drag** -- Mouse down + move, verify viewport pan changes.
12. **Tool switching** -- Switch from Select to Pan, verify CanvasToolChangedEvent published.
13. **Escape clears selection** -- Select objects, press Escape, verify selection empty.

## Acceptance Criteria

- [ ] CanvasInputManager converts wxWidgets events to CanvasInputEvent with world coordinates
- [ ] ICanvasTool interface with on_mouse_down/move/up/scroll/key_down/key_up
- [ ] SelectTool handles single click, shift-click, marquee selection, deselect
- [ ] PanTool handles drag-to-pan with viewport update
- [ ] Scroll wheel zoom centered on cursor position
- [ ] Hit-testing resolves topmost visible object by z-index via Quadtree
- [ ] Space+drag activates temporary pan mode from Select tool
- [ ] CanvasToolChangedEvent published on tool switch
- [ ] CanvasSelectionChangedEvent published on selection change
- [ ] All 13 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/CanvasTool.h` | ToolMode, CanvasInputEvent, ICanvasTool interface |
| CREATE | `src/canvas/CanvasInputManager.h` | Central input dispatcher |
| CREATE | `src/canvas/CanvasInputManager.cpp` | Input routing, coordinate conversion, hit-test |
| CREATE | `src/canvas/SelectTool.h` | Select/marquee tool |
| CREATE | `src/canvas/SelectTool.cpp` | SelectTool implementation |
| CREATE | `src/canvas/PanTool.h` | Pan/hand tool |
| CREATE | `src/canvas/PanTool.cpp` | PanTool implementation |
| MODIFY | `src/ui/CanvasPanel.h` | Add CanvasInputManager member, wire wxWidgets events |
| MODIFY | `src/ui/CanvasPanel.cpp` | Forward mouse/key events to input manager |
| MODIFY | `src/core/Events.h` | Add CanvasToolChangedEvent, CanvasSelectionChangedEvent, CanvasObjectHoveredEvent |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_canvas_input.cpp` | 13 Catch2 tests |

## Architecture Notes

- The ICanvasTool pattern follows the Strategy pattern. Tools are registered with CanvasInputManager and swapped at runtime.
- Each tool receives CanvasInputEvent (not raw wxWidgets events), making tools testable without wxWidgets.
- The CanvasInputManager mediates between wxWidgets (platform) and tools (canvas logic).
- Hit-testing uses the Quadtree for O(log n) candidate lookup, then z-index sorting for final resolution.
- Selection state lives in SelectTool, not in CanvasPanel. Events notify other components.
- Middle mouse button always pans regardless of active tool (handled in CanvasInputManager before tool dispatch).
- Future tools (Draw, Shape, Connector, etc.) are added in their respective phases by implementing ICanvasTool and registering with the input manager.

## Estimated Complexity

**L** -- Tool state machine, coordinate mapping, hit-test integration, marquee selection geometry, multiple tool implementations, 13 tests.
