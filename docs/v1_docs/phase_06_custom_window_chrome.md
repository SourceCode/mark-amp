# Phase 06: Custom Window Chrome

## Objective

Replace the default OS window frame with a custom-drawn window chrome that matches the MarkAmp retro-futuristic aesthetic. The chrome includes a custom title bar with app branding, window controls (minimize, maximize, close), and drag-to-move behavior. This must work correctly on macOS, Windows, and Linux.

## Prerequisites

- Phase 05 (Application Architecture and Module Design)

## Deliverables

1. Frameless window with custom-drawn title bar
2. Working window controls (minimize, maximize/restore, close) with hover effects
3. Drag-to-move functionality on the title bar
4. Drag-to-resize from window edges
5. Double-click title bar to maximize/restore
6. Platform-specific integration (macOS traffic lights, Windows snap layouts)
7. Beveled panel rendering system

## Tasks

### Task 6.1: Convert MainFrame to frameless window

Update `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.h` and `MainFrame.cpp`:

- Change the window style to `wxBORDER_NONE | wxRESIZE_BORDER | wxCLIP_CHILDREN`
- On macOS: additionally use `wxFULLSCREEN_NOMENUBAR` flag and handle NSWindow-level frameless
- On Windows: use `WS_POPUP` style with manual resizing
- On Linux/GTK: use `gtk_window_set_decorated(FALSE)` via platform abstraction

Remove the temporary menu bar created in Phase 04 (it will be replaced by the custom chrome toolbar).

**Acceptance criteria:**
- Window appears with no OS-provided title bar or borders
- Window is still visible and positioned correctly
- Close button or keyboard shortcut still works

### Task 6.2: Implement CustomChrome component

Create `/Users/ryanrentfro/code/markamp/src/ui/CustomChrome.h` and `CustomChrome.cpp`:

This is a wxPanel subclass that draws the custom title bar.

**Layout (matching reference App.tsx lines 102-130):**
```
[Logo dot] [MARKAMP v1.0.0]  ...  [filename.md]  ...  [Sidebar Toggle] | [_] [O] [X]
```

- Left section: gradient-filled circle (accent color), "MARKAMP" in bold uppercase, version in muted text
- Center section: current filename in monospace, muted background pill
- Right section: sidebar toggle button, separator, minimize/maximize/close buttons

**Drawing with wxGraphicsContext:**
- Use `wxGraphicsContext` for anti-aliased rendering
- Logo dot: radial gradient from accent-primary to purple
- Glow effect on logo: semi-transparent accent color circle behind the dot
- Text rendering with proper font selection

**Sizing:**
- Height: 40px (matching reference `h-10`)
- Bottom border: 2px dark border with 1px light border on top (bevel effect)

**Acceptance criteria:**
- Custom chrome renders matching the React reference design
- All elements (logo, text, buttons) are visible and correctly positioned
- Chrome redraws correctly on window resize

### Task 6.3: Implement window control buttons

Add minimize, maximize, and close buttons to `CustomChrome`:

**Button specifications (matching reference lines 126-129):**
- Size: 24x24 hit area, 12px icon
- Minimize: `_` glyph or custom-drawn line
- Maximize: `[ ]` glyph or custom-drawn rectangle (toggle to overlapping rectangles when maximized)
- Close: `X` glyph or custom-drawn cross

**Hover behavior:**
- Minimize/Maximize: `hover:bg-white/10`, text changes from muted to main
- Close: `hover:bg-red-500/80`, text changes from muted to white

**Click behavior:**
- Minimize: call `wxFrame::Iconize(true)` on parent
- Maximize: toggle between `wxFrame::Maximize(true)` and `wxFrame::Maximize(false)`
- Close: call `wxFrame::Close()`

**Implementation:**
- Use `wxPanel` subclass for each button, or draw all buttons in `CustomChrome::OnPaint`
- Track mouse hover state via `EVT_ENTER_WINDOW` / `EVT_LEAVE_WINDOW`
- Track mouse click via `EVT_LEFT_DOWN` / `EVT_LEFT_UP`

**Acceptance criteria:**
- All three buttons are visible and correctly sized
- Hover effects work smoothly
- Minimize, maximize (toggle), and close all function correctly
- Close button has red hover background

### Task 6.4: Implement drag-to-move

Add title bar drag functionality to `CustomChrome`:

**Implementation:**
- On `EVT_LEFT_DOWN` in the chrome area (not on buttons):
  - Record the mouse position relative to the window
  - Begin drag tracking
- On `EVT_MOTION` while dragging:
  - Move the window to follow the mouse
  - Use `wxFrame::Move()` or platform-specific methods
- On `EVT_LEFT_UP`:
  - End drag tracking
- On `EVT_LEFT_DCLICK` in the chrome area:
  - Toggle maximize/restore

**Platform-specific optimizations:**
- Windows: use `WM_NCHITTEST` returning `HTCAPTION` for native drag behavior (smoother)
- macOS: use `performWindowDragWithEvent` via Objective-C bridge
- Linux: use `gdk_window_begin_move_drag` via GTK

Create `/Users/ryanrentfro/code/markamp/src/platform/PlatformAbstraction.h` (interface) and platform-specific implementations:
- `MacPlatform.h/.cpp` (or `.mm` for Objective-C++)
- `WinPlatform.h/.cpp`
- `LinuxPlatform.h/.cpp`

**Acceptance criteria:**
- Window can be dragged by the title bar on all platforms
- Dragging is smooth without lag
- Double-click toggles maximize
- Buttons do not trigger drag

### Task 6.5: Implement drag-to-resize from edges

Since the OS frame is removed, implement manual resize behavior:

**Resize zones (8 edges/corners):**
- Top, bottom, left, right edges (4px hit zone)
- Top-left, top-right, bottom-left, bottom-right corners (8px hit zone)

**Implementation:**
- Override `EVT_MOTION` on `MainFrame` to detect hover over resize zones
- Change cursor to appropriate resize cursor (`wxCURSOR_SIZENS`, `wxCURSOR_SIZEWE`, `wxCURSOR_SIZENWSE`, `wxCURSOR_SIZENESW`)
- On `EVT_LEFT_DOWN` in a resize zone, begin resize operation
- On `EVT_MOTION` while resizing, update window size/position
- Respect minimum size constraints

**Platform-specific:**
- Windows: intercept `WM_NCHITTEST` to return `HTLEFT`, `HTRIGHT`, etc. for native resize
- macOS/Linux: manual implementation or use platform drag APIs

**Acceptance criteria:**
- All 8 resize directions work
- Cursor changes correctly when hovering over edges
- Minimum size is respected
- Resize is smooth without flickering

### Task 6.6: Implement the BevelPanel base component

Create `/Users/ryanrentfro/code/markamp/src/ui/BevelPanel.h` and `BevelPanel.cpp`:

The bevel effect is a key visual element of the MarkAmp aesthetic (reference App.tsx lines 200-202, 211-213):
```
border-t-black/20 border-l-black/20 border-b-white/5 border-r-white/5
```

This creates a sunken/inset appearance reminiscent of classic media player UIs.

**BevelPanel class:**
```cpp
class BevelPanel : public wxPanel
{
public:
    enum class BevelStyle { Raised, Sunken, Flat };

    BevelPanel(wxWindow* parent, BevelStyle style = BevelStyle::Sunken);

protected:
    void OnPaint(wxPaintEvent& event);

private:
    BevelStyle style_;
    // Colors derived from current theme
    wxColour highlight_color_; // border-light with alpha
    wxColour shadow_color_;    // border-dark with alpha
};
```

**Drawing the bevel:**
- Sunken: top/left edges use shadow color (black/20%), bottom/right use highlight (white/5%)
- Raised: inverse of sunken
- Flat: no bevel, just background
- Draw as a 2px inset border overlay (pointer-events-none equivalent: draw last, on top)

**Acceptance criteria:**
- BevelPanel renders the correct bevel effect for each style
- Bevel colors update when the theme changes
- Child content renders correctly inside the beveled area
- Visual comparison with the React reference shows matching appearance

### Task 6.7: Implement sidebar toggle button in chrome

Add a sidebar toggle button to `CustomChrome` (matching reference line 118-124):

- Icon: three horizontal lines (hamburger menu), 14px
- Position: right side of chrome, before the window control separator
- Hover: `bg-white/10`, color changes from muted to text-main
- Click: publishes `SidebarToggleEvent` via the event bus

**Icon rendering:**
- Draw three horizontal lines using `wxGraphicsContext`
- Or embed a small bitmap resource

**Acceptance criteria:**
- Button is visible and correctly positioned in the chrome
- Hover effect works
- Clicking publishes the event (verify via test subscriber)

### Task 6.8: Handle platform-specific window state

Ensure these window operations work correctly with custom chrome:

- **Minimize to taskbar/dock**: window restores correctly with chrome intact
- **Maximize**: window fills the screen (respecting taskbar), chrome remains visible
- **Fullscreen** (macOS): handle macOS fullscreen API, chrome adjusts or hides
- **Snap layouts** (Windows 11): support Windows snap by handling `WM_GETMINMAXINFO`
- **Window restore**: position and size are remembered and restored correctly

Store window state in the Config (Phase 05):
- Last position (x, y)
- Last size (width, height)
- Maximized state

**Acceptance criteria:**
- Window state persists across application restarts
- All platform window management features work (minimize, maximize, snap)
- No visual glitches when transitioning between states

## Files Created/Modified

| File | Action |
|---|---|
| `src/ui/MainFrame.h` | Modified (frameless style) |
| `src/ui/MainFrame.cpp` | Modified (remove menu bar, add resize handling) |
| `src/ui/CustomChrome.h` | Created |
| `src/ui/CustomChrome.cpp` | Created |
| `src/ui/BevelPanel.h` | Created |
| `src/ui/BevelPanel.cpp` | Created |
| `src/platform/PlatformAbstraction.h` | Created/Updated |
| `src/platform/MacPlatform.h` | Created |
| `src/platform/MacPlatform.mm` | Created (Objective-C++ for macOS APIs) |
| `src/platform/WinPlatform.h` | Created |
| `src/platform/WinPlatform.cpp` | Created |
| `src/platform/LinuxPlatform.h` | Created |
| `src/platform/LinuxPlatform.cpp` | Created |
| `src/CMakeLists.txt` | Modified (add new sources, platform files) |
| `tests/unit/test_bevel_panel.cpp` | Created |

## Dependencies

- wxWidgets 3.2.9 (core, aui for some window management)
- Platform-specific APIs:
  - macOS: Cocoa (NSWindow, NSView)
  - Windows: Win32 (WM_NCHITTEST, DwmExtendFrameIntoClientArea)
  - Linux: GTK3 (gdk_window functions)

## Estimated Complexity

**High** -- Custom window chrome is one of the most complex UI tasks. Platform-specific drag-to-move and resize require native API calls. Getting smooth, bug-free window management across three platforms is challenging.
