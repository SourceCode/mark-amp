# Phase 30: Floating Toolbars

## Overview

Implement a floating toolbar system that provides contextual tool surfaces that appear during specific interaction modes: a debug toolbar during debugging, a format bar on text selection, a selection action bar for bulk operations, a minimap hover toolbar, and an image preview toolbar. Floating toolbars are draggable, auto-hide when not needed, snap to window edges, and maintain correct z-order above all other panels.

## Prerequisites

- Phase 25 (Build and Task Runner) -- debug/run lifecycle for debug toolbar
- Phase 26 (Toolbar Redesign) -- run/debug/stop button patterns
- Phase 29 (Quick Actions) -- code action integration for selection toolbar
- Existing `FloatingFormatBar` at `/Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.h`
- Existing `ThemeAwareWindow` and theme system

## Target Files

| Action | File |
|--------|------|
| Create | `/Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/DebugToolbar.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/DebugToolbar.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/SelectionActionBar.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/SelectionActionBar.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/MinimapHoverToolbar.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/MinimapHoverToolbar.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/ImagePreviewToolbar.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/ImagePreviewToolbar.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/core/Events.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` |
| Create | `/Users/ryanrentfro/code/markamp/tests/unit/test_floating_toolbars.cpp` |

## Tasks

### Task 1: Design Floating Toolbar Base Class

**Title:** Create a shared base class for all floating toolbars

**Description:** Implement a `FloatingToolbar` base class that handles common behavior: theme-aware rendering, drag handle for repositioning, auto-hide after timeout, snap-to-edges, z-order management, and shadow rendering. All floating toolbars inherit from this base.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.h`:
```cpp
#pragma once
#include "core/EventBus.h"
#include "core/ThemeEngine.h"
#include <wx/popupwin.h>
#include <wx/timer.h>
#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

struct FloatingToolbarButton
{
    std::string id;
    std::string tooltip;
    std::string shortcut;
    PanelHeaderIconId icon;
    std::function<void()> callback;
    std::function<bool()> is_enabled;
    std::function<bool()> is_toggled;
    bool is_toggle{false};
    bool is_separator{false};
};

enum class FloatingToolbarEdge : uint8_t
{
    kNone,
    kTop,
    kBottom,
    kLeft,
    kRight,
};

class FloatingToolbar : public wxPopupTransientWindow
{
public:
    FloatingToolbar(wxWindow* parent,
                    core::ThemeEngine& theme_engine,
                    core::EventBus& event_bus,
                    const std::string& toolbar_id);

    void ShowAt(const wxPoint& position);
    void ShowAtScreenCenter();
    void Hide();
    void SetAutoHideMs(int ms); // 0 to disable auto-hide
    void SetDraggable(bool draggable);
    void SetSnapToEdges(bool snap);

    [[nodiscard]] auto toolbar_id() const -> const std::string&;
    [[nodiscard]] auto is_visible() const -> bool;
    [[nodiscard]] auto position() const -> wxPoint;

    static constexpr int kButtonSize = 28;
    static constexpr int kButtonPadding = 2;
    static constexpr int kDragHandleWidth = 12;
    static constexpr int kBorderRadius = 8;
    static constexpr int kShadowOffset = 3;
    static constexpr int kSnapDistance = 20;
    static constexpr int kDefaultAutoHideMs = 5000;

protected:
    void SetButtons(std::vector<FloatingToolbarButton> buttons);
    void RefreshButtonStates();

    core::ThemeEngine& theme_engine_;
    core::EventBus& event_bus_;

private:
    std::string toolbar_id_;
    std::vector<FloatingToolbarButton> buttons_;
    bool draggable_{true};
    bool snap_to_edges_{true};
    int auto_hide_ms_{0};

    // Drag state
    bool is_dragging_{false};
    wxPoint drag_start_;
    wxPoint window_start_;

    // Button interaction
    int hovered_button_{-1};
    int pressed_button_{-1};

    // Auto-hide
    wxTimer auto_hide_timer_;

    // Rendering
    void OnPaint(wxPaintEvent& event);
    void DrawShadow(wxGraphicsContext& gc, const wxRect& bounds);
    void DrawBackground(wxGraphicsContext& gc, const wxRect& bounds, const core::Theme& theme);
    void DrawDragHandle(wxGraphicsContext& gc, int x, int y, int height, const core::Theme& theme);
    void DrawButton(wxGraphicsContext& gc, const FloatingToolbarButton& btn,
                    const wxRect& rect, bool hovered, bool pressed, const core::Theme& theme);

    // Interaction
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);

    // Layout
    void RecalculateLayout();
    [[nodiscard]] auto CalculateSize() -> wxSize;

    // Snapping
    void SnapToNearestEdge();
    [[nodiscard]] auto FindSnapEdge(const wxPoint& pos) -> FloatingToolbarEdge;

    // Auto-hide
    void OnAutoHideTimer(wxTimerEvent& event);
    void ResetAutoHideTimer();
};

} // namespace markamp::ui
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp` (create)

**Acceptance Criteria:**
- Floating toolbar renders with rounded corners, shadow, and theme colors
- Drag handle on the left allows repositioning
- Toolbar snaps to window edges within 20px
- Auto-hide timer dismisses the toolbar after configurable delay
- Buttons render with hover and press states
- Tooltip appears on button hover
- Keyboard focus allows button navigation with arrow keys
- Z-order is above all application panels

**Dependencies:** Phase 27 Task 3 (PanelHeaderIcons)

---

### Task 2: Implement Floating Toolbar Manager

**Title:** Central manager for all floating toolbar lifecycle and z-order

**Description:** Create a `FloatingToolbarManager` that owns all floating toolbar instances, manages their visibility, handles z-order conflicts, persists last-known positions, and coordinates show/hide across toolbars.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.h`:
```cpp
#pragma once
#include "FloatingToolbar.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace markamp::ui
{

class FloatingToolbarManager
{
public:
    FloatingToolbarManager(wxWindow* parent,
                           core::ThemeEngine& theme_engine,
                           core::EventBus& event_bus);

    void RegisterToolbar(const std::string& id, std::unique_ptr<FloatingToolbar> toolbar);
    void ShowToolbar(const std::string& id, const wxPoint& position);
    void HideToolbar(const std::string& id);
    void HideAll();

    [[nodiscard]] auto GetToolbar(const std::string& id) -> FloatingToolbar*;
    [[nodiscard]] auto IsVisible(const std::string& id) const -> bool;

    // Position persistence
    void SavePositions();
    void RestorePositions();

    // Z-order
    void BringToFront(const std::string& id);

private:
    wxWindow* parent_;
    core::ThemeEngine& theme_engine_;
    core::EventBus& event_bus_;
    std::unordered_map<std::string, std::unique_ptr<FloatingToolbar>> toolbars_;
    std::unordered_map<std::string, wxPoint> saved_positions_;
    std::vector<std::string> z_order_; // front to back
};

} // namespace markamp::ui
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp` (create)

**Acceptance Criteria:**
- Manager tracks all floating toolbar instances
- `ShowToolbar()` shows at last-known position or specified position
- `HideAll()` hides all visible toolbars (used on mode change)
- Z-order is maintained: most recently shown toolbar is on top
- Positions are persisted in config and restored on restart
- Destroyed toolbars are cleaned up properly

**Dependencies:** Task 1

---

### Task 3: Implement Debug Toolbar

**Title:** Floating debug toolbar with Continue/Step Over/Step In/Step Out/Restart/Stop

**Description:** Create a floating toolbar that appears during debug sessions at the top-center of the editor area. Contains six buttons for debug control: Continue, Step Over, Step In, Step Out, Restart, and Stop.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/ui/DebugToolbar.h`:
```cpp
#pragma once
#include "FloatingToolbar.h"

namespace markamp::ui
{

class DebugToolbar : public FloatingToolbar
{
public:
    DebugToolbar(wxWindow* parent,
                 core::ThemeEngine& theme_engine,
                 core::EventBus& event_bus);

    void OnDebugStarted();
    void OnDebugPaused();
    void OnDebugContinued();
    void OnDebugStopped();

private:
    core::Subscription debug_start_sub_;
    core::Subscription debug_stop_sub_;

    void BuildButtons();

    // Icon drawing
    static void DrawContinueIcon(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawStepOverIcon(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawStepInIcon(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawStepOutIcon(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawRestartIcon(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawStopIcon(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
};

} // namespace markamp::ui
```

Button layout: `[drag] [|>] [->] [v>] [<^] [O>] [X]`

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DebugToolbar.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/DebugToolbar.cpp` (create)

**Acceptance Criteria:**
- Toolbar appears at top-center of editor when debug session starts
- Six buttons: Continue (green play), Step Over (arrow over line), Step In (arrow down), Step Out (arrow up), Restart (circular arrow), Stop (red square)
- Continue/Step buttons are disabled when not paused
- Stop is always enabled during debug
- Restart stops and restarts the debug session
- Toolbar auto-hides when debug session ends
- Toolbar is draggable to any position
- Position persists across debug sessions

**Dependencies:** Task 1, Task 2

---

### Task 4: Migrate and Enhance Floating Format Bar

**Title:** Refactor existing FloatingFormatBar to use the FloatingToolbar base

**Description:** Migrate the existing `FloatingFormatBar` to inherit from `FloatingToolbar` instead of `wxPopupTransientWindow`. Add additional actions: Strikethrough, Highlight, Footnote. Improve positioning to appear above the selection, centered.

**Implementation Details:**
Update `/Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.h`:
```cpp
class FloatingFormatBar : public FloatingToolbar
{
public:
    FloatingFormatBar(wxWindow* parent,
                      core::ThemeEngine& theme_engine,
                      core::EventBus& event_bus);

    void ShowAboveSelection(const wxPoint& selection_start, const wxPoint& selection_end);

private:
    void BuildButtons();
};
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.h`
- `/Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.cpp`

**Acceptance Criteria:**
- Format bar inherits from `FloatingToolbar` base
- All existing actions preserved: Bold, Italic, Code, Link, Quote, Heading, Table
- New actions: Strikethrough, Highlight, Footnote
- Appears above the text selection, centered horizontally
- If selection is near the top of the screen, appears below instead
- Auto-hides when selection is deselected (0.5s delay)
- Not draggable (fixed relative to selection)

**Dependencies:** Task 1

---

### Task 5: Implement Selection Action Bar

**Title:** Action bar for bulk operations on selected text

**Description:** Create a floating toolbar that appears when text is selected (separate from the format bar) with operations: Copy, Cut, Comment/Uncomment, Indent/Outdent, Transform Case (submenu), Sort Lines. This bar appears below the selection.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/ui/SelectionActionBar.h`:
```cpp
#pragma once
#include "FloatingToolbar.h"

namespace markamp::ui
{

class SelectionActionBar : public FloatingToolbar
{
public:
    SelectionActionBar(wxWindow* parent,
                       core::ThemeEngine& theme_engine,
                       core::EventBus& event_bus);

    void ShowBelowSelection(const wxPoint& selection_end);

private:
    void BuildButtons();
};

} // namespace markamp::ui
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/SelectionActionBar.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/SelectionActionBar.cpp` (create)

**Acceptance Criteria:**
- Appears below the selection end point
- Buttons: Copy, Cut, Comment, Indent, Outdent, Transform (dropdown)
- Transform dropdown includes: Uppercase, Lowercase, Title Case
- Auto-hides when selection is cleared
- Only appears for multi-line selections (single-line uses format bar)
- Not draggable (fixed relative to selection)

**Dependencies:** Task 1

---

### Task 6: Implement Minimap Hover Toolbar

**Title:** Toolbar that appears when hovering over the minimap

**Description:** When the user hovers over the editor minimap, show a small toolbar with: Zoom to Region, Navigate to Line, Toggle Minimap, Minimap Position (Left/Right).

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/ui/MinimapHoverToolbar.h`:
```cpp
#pragma once
#include "FloatingToolbar.h"

namespace markamp::ui
{

class MinimapHoverToolbar : public FloatingToolbar
{
public:
    MinimapHoverToolbar(wxWindow* parent,
                        core::ThemeEngine& theme_engine,
                        core::EventBus& event_bus);

    void ShowNearMinimap(const wxPoint& minimap_pos);

private:
    void BuildButtons();
};

} // namespace markamp::ui
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/MinimapHoverToolbar.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/MinimapHoverToolbar.cpp` (create)

**Acceptance Criteria:**
- Appears when hovering over the minimap for 500ms
- Buttons: Zoom to Fit, Navigate to Line, Toggle Minimap, Switch Side
- Auto-hides when mouse leaves the minimap area
- Small size (4 buttons only)
- Appears adjacent to the minimap, not overlapping

**Dependencies:** Task 1

---

### Task 7: Implement Image Preview Toolbar

**Title:** Toolbar for image preview actions (zoom, rotate, copy)

**Description:** When the user is viewing an image preview (in the preview panel or as a standalone image), show a floating toolbar with: Zoom In, Zoom Out, Zoom to Fit, Rotate Left, Rotate Right, Copy Image, Open Original.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/ui/ImagePreviewToolbar.h`:
```cpp
#pragma once
#include "FloatingToolbar.h"

namespace markamp::ui
{

class ImagePreviewToolbar : public FloatingToolbar
{
public:
    ImagePreviewToolbar(wxWindow* parent,
                        core::ThemeEngine& theme_engine,
                        core::EventBus& event_bus);

    void ShowForImage(const wxPoint& position);

    using ZoomCallback = std::function<void(float factor)>;
    using RotateCallback = std::function<void(int degrees)>;

    void set_on_zoom(ZoomCallback cb);
    void set_on_rotate(RotateCallback cb);

private:
    ZoomCallback on_zoom_;
    RotateCallback on_rotate_;
    float current_zoom_{1.0f};
    int current_rotation_{0};

    void BuildButtons();
};

} // namespace markamp::ui
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ImagePreviewToolbar.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ImagePreviewToolbar.cpp` (create)

**Acceptance Criteria:**
- Buttons: Zoom In (+), Zoom Out (-), Fit, Rotate Left, Rotate Right, Copy, Open
- Zoom buttons increment/decrement by 25%
- Fit button resets zoom to fit the container
- Rotate rotates by 90 degrees
- Copy puts the image on the clipboard
- Open launches the system image viewer
- Auto-hides after 5 seconds of inactivity

**Dependencies:** Task 1

---

### Task 8: Implement Drag Handle

**Title:** Drag handle for repositioning floating toolbars

**Description:** The drag handle is a 12px-wide grip area on the left edge of draggable floating toolbars. It renders as a series of small dots and changes the cursor to a grab hand when hovered.

**Implementation Details:**
```cpp
void FloatingToolbar::DrawDragHandle(wxGraphicsContext& gc, int x, int y, int height,
                                      const core::Theme& theme)
{
    auto color = theme.fg_dimmed;
    int dot_size = 2;
    int dot_spacing = 4;
    int dot_x = x + kDragHandleWidth / 2 - dot_size / 2;

    for (int dy = 8; dy < height - 8; dy += dot_spacing)
    {
        gc.SetBrush(wxBrush(color));
        gc.DrawEllipse(dot_x - 2, y + dy, dot_size, dot_size);
        gc.DrawEllipse(dot_x + 2, y + dy, dot_size, dot_size);
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp`

**Acceptance Criteria:**
- Drag handle renders as a vertical column of dot pairs
- Cursor changes to grab hand when hovering the drag handle
- Cursor changes to grabbing hand while dragging
- Dragging moves the toolbar with the mouse
- Mouse offset from the drag start point is preserved (no jumping)
- Non-draggable toolbars do not show the drag handle

**Dependencies:** Task 1

---

### Task 9: Implement Auto-Hide Behavior

**Title:** Configurable auto-hide with fade-out animation

**Description:** Floating toolbars can auto-hide after a configurable timeout. Before hiding, play a 200ms fade-out animation. Moving the mouse back over the toolbar cancels the hide and resets the timer.

**Implementation Details:**
```cpp
void FloatingToolbar::OnAutoHideTimer(wxTimerEvent&)
{
    // Start fade-out animation
    fade_out_timer_.Start(16); // 60fps
    fade_opacity_ = 1.0f;
}

void FloatingToolbar::OnFadeOut(wxTimerEvent&)
{
    fade_opacity_ -= 0.05f;
    if (fade_opacity_ <= 0.0f)
    {
        fade_out_timer_.Stop();
        Hide();
        fade_opacity_ = 1.0f;
    }
    else
    {
        SetTransparent(static_cast<int>(fade_opacity_ * 255));
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp`

**Acceptance Criteria:**
- Auto-hide timer starts when the mouse leaves the toolbar
- Timer resets when the mouse re-enters
- Fade-out animation over 200ms (opacity 1.0 to 0.0)
- After fade-out, toolbar is hidden (not just transparent)
- `SetAutoHideMs(0)` disables auto-hide
- Default auto-hide is 5 seconds for debug toolbar, 0 for format bar

**Dependencies:** Task 1

---

### Task 10: Implement Snap to Edges

**Title:** Toolbar snapping to window edges during drag

**Description:** When dragging a floating toolbar near a window edge (within 20px), snap the toolbar to align with that edge. Snapping provides visual feedback (a highlight line) before the toolbar reaches the snap point.

**Implementation Details:**
```cpp
void FloatingToolbar::SnapToNearestEdge()
{
    auto parent_rect = GetParent()->GetClientRect();
    auto pos = GetPosition();
    auto size = GetSize();

    // Snap to top
    if (pos.y < kSnapDistance)
    {
        pos.y = 0;
    }
    // Snap to bottom
    if (parent_rect.GetBottom() - (pos.y + size.GetHeight()) < kSnapDistance)
    {
        pos.y = parent_rect.GetBottom() - size.GetHeight();
    }
    // Snap to left
    if (pos.x < kSnapDistance)
    {
        pos.x = 0;
    }
    // Snap to right
    if (parent_rect.GetRight() - (pos.x + size.GetWidth()) < kSnapDistance)
    {
        pos.x = parent_rect.GetRight() - size.GetWidth();
    }

    SetPosition(pos);
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp`

**Acceptance Criteria:**
- Toolbar snaps to top/bottom/left/right window edges within 20px
- Snap preview line shows before the toolbar reaches the edge
- Snapping is smooth (no visual jumping)
- Snap can be overridden by continuing to drag past the edge
- Snap distance is configurable via constant

**Dependencies:** Task 1

---

### Task 11: Implement Z-Order Management

**Title:** Correct z-order for multiple floating toolbars

**Description:** When multiple floating toolbars are visible (e.g., debug toolbar + format bar), ensure the most recently interacted toolbar is on top. Clicking or showing a toolbar brings it to the front.

**Implementation Details:**
```cpp
void FloatingToolbarManager::BringToFront(const std::string& id)
{
    auto it = std::find(z_order_.begin(), z_order_.end(), id);
    if (it != z_order_.end())
    {
        z_order_.erase(it);
    }
    z_order_.insert(z_order_.begin(), id);

    // Reorder windows
    for (auto rit = z_order_.rbegin(); rit != z_order_.rend(); ++rit)
    {
        if (auto* tb = GetToolbar(*rit); tb != nullptr && tb->is_visible())
        {
            tb->Raise();
        }
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp`

**Acceptance Criteria:**
- Most recently shown/clicked toolbar is on top
- Z-order updates immediately on interaction
- All visible toolbars remain accessible
- Debug toolbar defaults to top z-order when active

**Dependencies:** Task 2

---

### Task 12: Define Floating Toolbar Events

**Title:** EventBus events for floating toolbar lifecycle

**Implementation Details:**
Add to Events.h:
```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FloatingToolbarShownEvent)
    std::string toolbar_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FloatingToolbarHiddenEvent)
    std::string toolbar_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FloatingToolbarMovedEvent)
    std::string toolbar_id;
    int x{0};
    int y{0};
MARKAMP_DECLARE_EVENT_END;
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/Events.h`

**Acceptance Criteria:**
- Events publish on show, hide, and move
- Events include the toolbar ID for routing
- Move event includes the new position

**Dependencies:** None

---

### Task 13: Wire Floating Toolbars to Editor Events

**Title:** Show/hide floating toolbars based on editor state changes

**Description:** Wire the format bar to show on text selection, the selection action bar to show on multi-line selection, and the debug toolbar to show on debug session start. Handle the transitions correctly when multiple states overlap.

**Implementation Details:**
In `EditorPanel`:
```cpp
void EditorPanel::OnSelectionChanged()
{
    auto has_selection = GetStyledTextCtrl()->GetSelectionStart() != GetStyledTextCtrl()->GetSelectionEnd();
    auto is_multiline = /* check if selection spans multiple lines */;

    if (has_selection && !is_multiline)
    {
        toolbar_manager_.ShowToolbar("format_bar", GetSelectionScreenPos());
        toolbar_manager_.HideToolbar("selection_bar");
    }
    else if (has_selection && is_multiline)
    {
        toolbar_manager_.ShowToolbar("selection_bar", GetSelectionEndScreenPos());
        toolbar_manager_.HideToolbar("format_bar");
    }
    else
    {
        toolbar_manager_.HideToolbar("format_bar");
        toolbar_manager_.HideToolbar("selection_bar");
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`

**Acceptance Criteria:**
- Format bar appears on single-line text selection (after 300ms delay)
- Selection action bar appears on multi-line selection
- Both hide when selection is cleared
- Debug toolbar appears on debug session start, hides on end
- No flicker when transitioning between selection types
- Toolbars do not overlap each other

**Dependencies:** Task 2, Task 4, Task 5

---

### Task 14: Implement Position Persistence

**Title:** Save and restore floating toolbar positions across sessions

**Description:** When a draggable toolbar is moved, save its position to config. On next launch, restore to the last known position.

**Implementation Details:**
```cpp
void FloatingToolbarManager::SavePositions()
{
    for (const auto& [id, toolbar] : toolbars_)
    {
        if (toolbar->is_visible())
        {
            saved_positions_[id] = toolbar->position();
        }
    }
    // Persist to config
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`

**Acceptance Criteria:**
- Toolbar positions are saved to config on hide/close
- Positions are restored on next show
- Invalid positions (off-screen) are corrected to window bounds
- Non-draggable toolbars do not persist positions
- Config key: `floating_toolbars.positions.{toolbar_id}`

**Dependencies:** Task 2

---

### Task 15: Implement Floating Toolbar Accessibility

**Title:** Keyboard accessibility for floating toolbars

**Description:** Floating toolbars must be navigable via keyboard: Tab moves focus to the toolbar, arrow keys navigate between buttons, Enter activates, Escape dismisses.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp`

**Acceptance Criteria:**
- Tab from the editor moves focus to the visible floating toolbar
- Left/Right arrows navigate between buttons
- Enter activates the focused button
- Escape dismisses the toolbar and returns focus to the editor
- Focus ring is visible on the focused button
- Screen reader accessible (button tooltip as accessible name)

**Dependencies:** Task 1

---

### Task 16: Update CMakeLists.txt

**Title:** Add all new floating toolbar source files to the build

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`

**Acceptance Criteria:**
- All new `.h` and `.cpp` files in both `add_executable()` and `source_group()`
- Build succeeds

**Dependencies:** All create tasks

---

### Task 17: Write Unit Tests for FloatingToolbar Base

**Title:** Test toolbar layout, button states, and interaction logic

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_floating_toolbars.cpp` (create)

**Acceptance Criteria:**
- Tests verify button layout calculation
- Tests verify hover/press state transitions
- Tests verify auto-hide timer behavior
- Tests verify snap-to-edge calculations
- All tests pass

**Dependencies:** Task 1

---

### Task 18: Write Unit Tests for FloatingToolbarManager

**Title:** Test toolbar lifecycle management and z-order

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_floating_toolbars.cpp`

**Acceptance Criteria:**
- Tests verify toolbar registration and retrieval
- Tests verify show/hide lifecycle
- Tests verify z-order management
- Tests verify position persistence
- All tests pass

**Dependencies:** Task 2

---

### Task 19: Write Unit Tests for Debug Toolbar

**Title:** Test debug toolbar state transitions

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_floating_toolbars.cpp`

**Acceptance Criteria:**
- Tests verify toolbar appears on debug start
- Tests verify toolbar hides on debug stop
- Tests verify button enablement during pause vs running
- All tests pass

**Dependencies:** Task 3

---

### Task 20: Write Unit Tests for Selection Bar Triggers

**Title:** Test format bar and selection bar show/hide conditions

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_floating_toolbars.cpp`

**Acceptance Criteria:**
- Tests verify format bar appears on single-line selection
- Tests verify selection bar appears on multi-line selection
- Tests verify both hide on deselection
- Tests verify mutual exclusion (only one visible at a time)
- All tests pass

**Dependencies:** Task 4, Task 5

---

### Task 21: Write Drag Behavior Tests

**Title:** Test drag-and-drop repositioning with snap-to-edge calculations

**Description:** Verify that dragging a floating toolbar correctly updates its position and that snap-to-edge logic triggers at the correct distance thresholds.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_floating_toolbars.cpp`

**Acceptance Criteria:**
- Tests verify drag offset is preserved (no position jumping on drag start)
- Tests verify snap triggers within 20px of window edges
- Tests verify snap does not trigger beyond 20px
- Tests verify all four edges (top, bottom, left, right) snap correctly
- All tests pass

**Dependencies:** Task 8, Task 10

---

### Task 22: Write Auto-Hide Timer Tests

**Title:** Test auto-hide timer and fade-out animation timing

**Description:** Verify that auto-hide timer starts on mouse leave, resets on mouse re-enter, and that the fade-out animation completes correctly.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_floating_toolbars.cpp`

**Acceptance Criteria:**
- Tests verify timer starts after mouse leaves toolbar
- Tests verify timer resets when mouse re-enters before timeout
- Tests verify toolbar hides after fade-out completes
- Tests verify SetAutoHideMs(0) disables auto-hide
- All tests pass

**Dependencies:** Task 9

---

### Task 23: Write Keyboard Navigation Tests

**Title:** Test keyboard accessibility for floating toolbar buttons

**Description:** Verify that Tab focuses the toolbar, arrow keys navigate between buttons, Enter activates, and Escape dismisses.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_floating_toolbars.cpp`

**Acceptance Criteria:**
- Tests verify Left/Right arrows move focus between buttons
- Tests verify Enter activates the focused button callback
- Tests verify Escape hides the toolbar
- Tests verify focus ring is visible on the focused button
- All tests pass

**Dependencies:** Task 15

---

### Task 24: Write Position Persistence Tests

**Title:** Test save and restore of toolbar positions across sessions

**Description:** Verify that toolbar positions are saved to config when hidden and restored correctly when shown again.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_floating_toolbars.cpp`

**Acceptance Criteria:**
- Tests verify position is saved to config on hide
- Tests verify position is restored on next show
- Tests verify off-screen positions are corrected to window bounds
- Tests verify non-draggable toolbars do not persist positions
- All tests pass

**Dependencies:** Task 14

---

### Task 25: Write Z-Order Tests with Multiple Toolbars

**Title:** Test z-order management with three simultaneous visible toolbars

**Description:** Verify that when three toolbars are visible simultaneously, clicking one brings it to the front and the z-order stack updates correctly.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_floating_toolbars.cpp`

**Acceptance Criteria:**
- Tests verify most recently shown toolbar is on top
- Tests verify clicking a toolbar brings it to front
- Tests verify z-order updates when one toolbar is hidden and re-shown
- Tests verify HideAll() hides all toolbars
- All tests pass

**Dependencies:** Task 11

---

### Task 26: Documentation

**Title:** Document the floating toolbar system

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/docs/architecture.md`

**Acceptance Criteria:**
- FloatingToolbar base class API documented
- FloatingToolbarManager usage documented
- Each toolbar type's trigger conditions documented
- Position persistence and z-order behavior described
- Keyboard accessibility documented

**Dependencies:** All previous tasks

## Testing Requirements

- Unit tests for base class layout and interaction
- Unit tests for manager lifecycle and z-order
- Unit tests for each specific toolbar's show/hide conditions
- Integration tests for editor-triggered toolbar display
- Manual testing with drag, snap, auto-hide, and theme switching

## Phase Completion Criteria

- Debug toolbar appears during debug sessions with all control buttons
- Format bar appears on text selection with formatting actions
- Selection action bar appears on multi-line selection
- Minimap hover toolbar shows on minimap hover
- Image preview toolbar shows during image preview
- All toolbars are draggable with edge snapping
- Auto-hide and fade-out work correctly
- Z-order is managed correctly with multiple visible toolbars
- Positions persist across sessions
- All unit and integration tests pass
